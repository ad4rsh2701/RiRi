# `std::string_view` over `std::string`: A surprisingly tricky optimization

> ## OUTDATED & INCORRECT FACTS
> This optimisation note is OUTDATED and presents some wrong facts, brought to light by external validation.
>
> However, the general opinion on `string_view` is still valid.
> An update to this note is in the works.

*I am still not sure whether my current version is the most optimal, that's how much there is to this optimization.*

## Context: The problem with `std::string`

Let's assume that we have a simple function which takes a single parameter of the type `std::string` named arg:

```cpp
void simpleFunction(std::string arg) const;
```
Also, let's assume the following implementation of that function:
```cpp
void simpleFunction(std::string arg) const {
    
    // Assigning the size of the string to i.
    int i = arg.size();

    // Printing a substring of the arg.
    std::cout << arg.substr(i, i-2);
}
```
Now, when this function is called and, suppose, a string "very short string" is passed as the parameter, like so:
```cpp
std::string str = "very short string";
simpleFunction(str);
```
>### Note
>Note that I am passing an `std::string` value and not `"very short string"` directly, as that would be taken as a string literal of type `const char*`, which would require the compiler to do this:
>```cpp
>std::string temp = std::string("very short string");
>```
>And then it will pass `temp` to the function. This creates unnecessary allocations, reducing performance for multiple calls.
>
> It is always better to pass the expected type directly (unless in very unique cases) instead of making the compiler do the conversion *(should be obvious, but still I didn't knew that before)*.

Now that the `simpleFunction` is called, let's look at what happens behind the scenes:

#### → `arg` is passed by value, which means copy again.

Because arg is passed by value, it creates another full copy of that string like so:
```cpp
std::string arg = temp;  // ← another copy + another allocation
```
***"But we can always just pass by reference?"***
I hear that, and yes, we can! It will eliminate that unnecessary allocation and will be the best option. So, let's modify the function to pass by reference instead!
```cpp
void simpleFunction(std::string& arg) const;
// We will now pass by reference only.

// Implementation
void simpleFunction(std::string& arg) const {
    
    // Assigning the size of the string to i.
    int i = arg.size();

    // Printing a substring of the arg.
    std::cout << arg.substr(0, i-2);
}
```
> ### Note (again)
> Note that again, you must still pass a `std::string` object and not a string literal, because if you did, the compiler wouldn't have a `std::string` object to pass by reference, so it will just create one, like previously noted:
> ```cpp
> std::string temp = std::string("very short string");  // Compiler Allocates
> simpleFunction(temp);  // passed by reference
> ```
> The function doesn’t make the copy itself, but the call site does it on the spot. Thanks the compiler for that.

*You might wonder why am I going over all this, but stick with me, it will make a lot of sense later*

Continuing with what happens behind the scenes when you call the function `simpleFunction` and pass a `std::string` object:

#### → `arg.size()` reads the size; trivial
#### → `arg.substr(...)` creates yet another `std::string`
This allocates a new heap buffer and copies a part of the original string into it. That's just how `substr(...)` works, it creates a string which holds a part of the original string.

So, that is one allocation per function call (I am ignoring `std::cout` for now, because: A, I haven't used it in my original code and B, It's an entire different rabbit hole).

Doesn't seem like a lot does it? But hold onto that for now, and let's first see how `std::string_view` works and how it will make it so that there are essentially no allocations.

## What is `std::string_view`?
It's a relatively known bitch in the town, it was introduced in C++17 and provides "a lightweight, non-owning view of a string or a part of a string, allowing for efficient processing of string data without the overhead of memory management".

I am gonna assume you either already knew that, or it just completely bounced off your head (the latter was me, when I read that). So, I will just do a very bad explanation of `std::string_view` in hopes of making it a bit more clear.

Under the hood `std::string_view` is just a `struct`:
```cpp
struct string_view {
    const char* ptr;
    size_t length;
};

// I avoided template for simplicity, however the actual implementation is something similar to:

// template<typename CharT>
// struct basic_string_view {
//     const CharT* _data;   // Pointer to beginning of string
//     size_t _size;         // Number of characters

//     // member functions: .substr(), .data(), .size(), etc.
// };
```
So, if you define a variable `view` as such:
```cpp
std::string str = "some string";
std::string_view view = str;
```
The `ptr` points to the start of the `str`'s location, and `length` stores the length of `str`. That's it! The entire string `str` is NEVER copied.

> ### Note (final one?)
> Oh and if you ask, like I did, whether a direct allocation is possible like so:
> ```cpp
> std::string_view view = "some string again";
> ```
> Because it looks like that a `std::string` object is required to exist in the memory before allocating a `std::string_view` object.
>
> *Then, YES a direct allocation of `std:string_view` is possible!* And you absolutely yeeted the above two NOTES about "string literals". So, let me explain that as well:
>
> In the above case:
> - `"some string again"` is a string literal and it's of type `const char[12]`.
> - It's already getting stored (in the **static memory** not the heap memory, unlike how `std::string` does it) and `std::string_view` binds directly to that literal's memory location without a need for a `std::string` allocation.
>
> If all that is making little sense, then just know that when you directly assign a `std::string_view` object (to a string literal), the compiler is first **sneakily storing** `"some string again"` somewhere for you in the **static memory**, called `.rodata` (which is is much faster to store on than on *Dynamic Memory* or heap). Then the `std::string_view` object points towards the start of that string literal.

This is why, `std::string_view` is often called a non-owning view, which views the object it is pointed to. Also, a very very very important thing about `std::string_view`:

#### → `std::string_view` **does not extend the lifetime** of the object it views.

*The reason why I called it a bitch.*

This is what makes working with `std::string_view` very risky. Essentially, the `std::string_view` object represents the view of the object it points to **as long as the object exists**. 

If the object is destroyed, the ``std::string`` will now point to an invalid memory, which is undesirable. An example will make it much clear:


```cpp
std::string_view view;
// a block
{
    std::string str = "I don't feel so good Mr. Stark";
    view = str;
    cout << view;
}
cout << view;
```
In the above, example, we define a `std::string str` inside a block `{}`, which means `str` only lives until the end of the block. But, then we assigned it to `view` (which we defined as a `std::string_view` outside the block) inside the block.

And then we tried printing `view` inside the block, which is okay! The `str` is still alive, as the block hasn't ended, so the `view` still points to `str`, hence we can print it!

However, when the block ends, `view` no longer points to `str`, rather it points to an invalid memory and printing it will result in an error.

That is, `std::string_view` doesn't extend the lifetime of the object it points to, rather it wholly depends on it.

>`std::string_view` is only valid as long as the source string exists. It doesn’t copy, and it doesn’t keep things alive.

> ### NOTE (definitely the last one)
> Due to this, storing `std::string_view` is also very risky, as it depends directly on the lifetime of the object it points to, so ensure that you store the object directly rather than a `std::string_view` pointer to that object.

Here, a couple of points you can keep in mind while using `std::string_view`:
1. It is totally okay to pass a `std::string_view` parameter through a function, as long as you are NOT STORING IT (unless you are absolutely sure you want to).

2. It is not necessary to pass a `std::string_view` as **reference**, or more sternly, **I do not recommend passing a `std::string_view` type parameter as a reference to a function**, since it is already a pointer, you are wasting your processes creating another pointer to it.

3. A `std::string_view` pointer/object lives as long as the function's execution frame, if it is passed to or defined in a function.

4. Returning a `std::string_view` pointer/object is not *impossible*, you may return it, as long as you instantly use it and not store it.


## Why is `std::string_view` better to use than `std::string`?

Let's take *a very similar function* from [CommandParser.cpp](../../src/CommandParser.cpp) as an example:

```cpp
std::vector<std::string> CommandParser::parseCommand(std::string& command) const {
    std::vector<std::string> tokens;
    size_t pos = 0;

    while (pos < command.size()) {
        size_t spacePos = command.find(' ', pos);
        if (spacePos == std::string::npos) {
            spacePos = command.size();
        }

        tokens.push_back(command.substr(pos, spacePos - pos));
        pos = spacePos + 1;
    }
    return tokens;
}
```
The above code is essentially a command parser, it parses the `command` input (using `' '` as the separator) into separate `string`s and returns tokens of type `vector<string>`.

Though, I am gonna disgustingly simplify the above code for clarity:
```cpp
using namespace std;

vector<string> parseCommand(string command) const {
    
    vector<string> tokens;
    int i = 0;

    while (i < command.size()){
        tokens.push_back(command.substr(2));
        i++;
    }

    return tokens;
}
```
The key common key concept behind the original code and the simplified code is that the following line:
```cpp
tokens.push_back(command.substr(...))
```
Is called repetitively, under a `while` loop. And, we already discussed previously that `.substr()` adds one extra allocation.

This time however, it's being called not once, but as many times as the number of tokens (or the length of commands in the simplified code).

This makes the number of extra allocations shoot up way higher. Consider the following input:

```cpp
"SET key value key2 value2 key3 value3"
```
That as the `command` input will eventually add at least 7 allocations (I am ignoring the minor allocations from `.push_back`).

So, every time `parseCommand(...)` is called, there will be 7 allocations for a simple tokenizer.

However, if you see the current version of that said code from [`CommandParser.cpp`](../../src/CommandParser.cpp):
```cpp
std::vector<std::string_view> CommandParser::parseCommand(std::string_view command) const {
    std::vector<std::string_view> tokens;
    size_t pos = 0;

    while (pos < command.size()) {
        size_t spacePos = command.find(' ', pos);
        if (spacePos == std::string::npos) {
            spacePos = command.size();
        }

        tokens.emplace_back(command.substr(pos, spacePos - pos));
        pos = spacePos + 1;
    }
    return tokens;
}
```
or the dumber version of that code:
```cpp
using namespace std;

vector<string_view> parseCommand(string_view command) const {
    
    vector<string_view> tokens;
    int i = 0;

    while (i < command.size()){
        tokens.emplace_back(command.substr(2));
        i++;
    }

    return tokens;
}
```

You'd see that `std::string_view` is used instead and that's done so to avoid those extra allocations.

***How?***

The concerned line, which caused those allocations previously, is now:
```cpp
tokens.emplace_back(command.substr(...))
// "command" is of the type std::string_view
```
> ### NOTE (final one, I swear)
> Member functions `.push_back` and `emplace_back` are the same functions, doing the same job, under different name for different types.

This time, instead of creating a new `std::string` object, a `std:string_view` object is created, which is just:
```cpp
struct string_view {
    const char* ptr;
    size_t length;
}; // simplified
```
Which means no heap/dynamic allocation, no copying of the actual string, all because that newly created object is just a `pointer` + `length`.

Hence, instead of having minimum 7 allocation we will have 0 allocations just by switching to `std::string_view`. Which, admittedly, doesn't seem much, but over a multiple calls/iterations:
| Number of Calls/Iteration | Number of allocations: `std::string` | Number of allocations:  `std::string_view` |
| :---: | :---: | :---:|
| 1 | 7  | 0 |
| 10 | 70  | 0 |
| 1,000 | 7,000  | still 0 |
| 1,000,000 | 7,000,000,00 | STILL 0|

So, as you can tell, as the number of function calls increase, the number of extra allocations will keep on increasing when using `std::string`, whereas while using `std::string_view` the number of extra allocations effectively remain 0, irrespective of the number of allocations.

You may find this as *a very oddly specific case*, but it's relevant everywhere, every time you call a member function of `std::string` which either modifies or updates the `std::string` object, you create a `std::string` copy of it, which is heavy, *very heavy*, whereas, a copy of `std::string_view` object is literally just a pointer with an extra length value.

## Don't trust me? Here, metrics:

Even if you don't agree with my reasons, here are the metrics which I benchmarked over the same two codes we discussed, which showcase my point:

| Iterations | Time taken (ms) `std::string` | Time Taken (ms) `std::string_view`| Difference % |
| :---: | :---: | :---: | :---: |
| 100,000 | 35 | 17 | 105.882 % Faster |
| 1,000,000 | 225 | 146 | 59.2857 % Faster |
| 10,000,000 | 3944 | 1404 | 180.912 % Faster |

The difference % is not consistent over iterations (optimizations probably), but one thing is clear, `std::string_view` is significantly faster.

Oh and that was with -O3 optimization, allow me to run one benchmark with -O0 instead.
```
Benchmarking parsing for 1,000,000 iterations

Results
std::string      : 1622 ms
string_view      : 466 ms

string_view improvement: 248.069% Faster
```

That's just how much performance can be improved by a very simple looking optimization (just using `std::string_view` instead of `std::string`).


## So, is `std::string_view` ALWAYS better?
Depending on your application, *yes*. But, allow me to show you how bad it can affect performance if used unwisely.

Consider this code from [`CommandParser.cpp`](../../src/CommandParser.cpp):

```cpp
std::string setCommand(std::vector<std::string_view> args) {
    if (args.size() == 2) {
        g_dataStore->setValue(std::string(args[0]), std::string(args[1]));
        return "OK";
    } else if (args.size() > 2 && args.size() % 2 == 0) {
        for (size_t i = 0; i < args.size(); i += 2) {
            g_dataStore->setValue(std::string(args[i]), std::string(args[i + 1]));
        }
        return "OK (" + std::to_string(args.size() / 2) + " keys)";
    } else if (args.size() < 2) {
        return "ERROR: SET needs at least KEY and VALUE";
    } else {
        return "ERROR: SET needs even number of arguments";
    }
}
```

Note that, we are passing a `std::string_view` object in this code. It goes through all the conditions, and if it fits one, the condition block is replayed.

Our bottleneck code is when the function from the `g_dataStore` is called:
```cpp
g_dataStore->setValue(std::string(args[i]), std::string(args[i + 1]));
```
See how we are converting `args` (a `std::string_view` object) to a `std::string` object (why am I doing this? Because I need the return value in `std::string`, it's a need).

Now, this conversion, to `std::string` creates a heavy bulky `std::string` object, the very thing we wanted to avoid, and not just once, but twice. So, now we have got 2 extra allocations from a function instead of the expected 0 extra allocations.

Now, look at the same code, but without using `std::string_view`:

```cpp
std::string setCommand_string(const std::vector<std::string>& args) {
    if (args.size() == 2) {
        g_dataStore->setValue(args[0], args[1]);
        return "OK";
    } else if (args.size() > 2 && args.size() % 2 == 0) {
        for (size_t i = 0; i < args.size(); i += 2) {
            g_dataStore->setValue(args[i], args[i + 1]);
        }
        return "OK (" + std::to_string(args.size() / 2) + " keys)";
    } else if (args.size() < 2) {
        return "ERROR: SET needs at least KEY and VALUE";
    } else {
        return "ERROR: SET needs even number of arguments";
    }
}
```
Here, we passed a pointer to the `std::string` as the parameter (similar to what happened in `std::string_view`). But look at the bottleneck line:
```cpp
g_dataStore->setValue(args[i], args[i + 1]);
```
No conversion means no extra allocations hence no copy created which equals faster execution. And, also, the benchmark results that made me realize that blindly using `std::string_view` is just as bad (*I initially benchmarked `setCommand` function instead of `parseCommand` and I was definitely surprised*):

```
<!-- With -O3 -->
=== Benchmarking 1000000 iterations ===
std::string     : 1 ms
string_view     : 4 ms
Performance Gain: -75%

<!-- With -O0 -->
=== Benchmarking 1000000 iterations ===
std::string     : 16 ms
string_view     : 51 ms
Performance Gain: -68.6275%
```

So, the question itself is invalid, there is no *better*. But based on the context, requirement, return type and need, one can be significantly better than the other.

And, yes yes, I will change the command functions in [`CommandParser.cpp`](../../src/CommandParser.cpp) to accept `std::string` type instead of the `std::string_view` soon.


Compiler Used: `Clang++` with `lld`.
> The profiling was done with all optimization enabled (`-O3` or `O0`, `-flto`, `-march=native` and `-fuse-ld=lld`) to simulate real world usage.

*Why did I write all this? Because I did exactly what I wrote above.*
