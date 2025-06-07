# Function Pointers over `std::function` and Lambda Functions
*Function pointers may not be cooler, but they are definitely faster when all you need is raw speed and an overall less memory usage.*

## Context
Currently, [`CommandParser.h`](../../include/CommandParser.h) defines a function pointer type as follows:
```cpp
// CommandParser.h at line: 23
using RiRiCommandFn = std::string(*)(const std::vector<std::string_view>&);
```
Which defines the function pointer type and is used as the parameter type for our `ankerl::unordered_dense:map<...> ririCommandMap`'s value type (see [`CommandParser.h`](../../include/CommandParser.h): line 46).

Functions are defined based on the parameters and return type of the function pointer and then emplaced in `ririCommandMap` with corresponding `COMMAND` strings (see [`CommandParser.cpp`](../../src/CommandParser.cpp): line 149).

Initially however, something much cooler was used.
```cpp
using RiRiCommandFn = std::function<std::string(const std::vector<std::string>&)
```
`std::function` introduced in C++11: 
This was used to give the parameter type for the same `ririCommandMap` as discussed above. And:

```cpp
riricommands.emplace("SET", [dataStore](const std::vector<std::string>& args) { 
    if (args.size() != 2) return "ERROR: SET needs key and value";
    dataStore->setValue(args[0], args[1]);
    return "OK";
});

riricommands.emplace("GET", [dataStore](const std::vector<std::string>& args) { 
    if (args.size() != 1) return std::string("ERROR: GET needs key");
    return dataStore->getValue(args[0]);
});
```
Yes, __Lambda Functions__ (more like nameless functions) were used, which allowed me to define and emplace both at the same time, it also made the implemntation look a lot cleaner and was more secure (encapsualtion, ~~though I am not so sure about the secure part~~).

This avoided defining and emplacing the functions separately (using function pointers), which requires more boilerplate when compared to inline lambdas using `std::function`.

## So, why not `std::function` anymore?

As previously stated, commands were stored using std::function and lambdas in a string-to-function map. However, profiling showed that it introduces overhead due to:

 - I do not yet know how, but potential heap allocations (likely from **type erasure** or **dynamic storage**).
 - Type erasure logic (polymorphism essentially, we don't need that)
 - Larger Objects because `std::function` is typically 32+ bytes.

Since, *we do NOT need capturing lambdas* (although they are cool), and we are mostly working with global/static functions, we are using **function pointers** instead. Oh and capturing lambdas are in itself a memory overhead as well.

### Initial Metrics
| Iterations   | `std::function` | function pointer | Difference %     |
|:------------:|:---------------:|:----------------:|:----------------:|
| 1 million    | 137.191 ms      | 134.613 ms       | ~1.88% faster    |
| 1.4 billion  | 202236 ms       | 189615 ms        | ~6.2% faster     |

Compiler Used: `Clang++` with `lld`.

Sure, this isn't a deal-breaking change — I don't expect RiRi to deal with 1.4 billion commands.
But the difference is clear. Plus, since we are not capturing lambdas memory usage is expected to be lower.

### *"Expected? Where are the metrics?"* you might ask

`TODO:`
Well, I wasn't able to benchmark the memory usage due to the limited number of commands and lack of its code/logic cleanup during the inital testing. So, as soon as the command parser is ready (which it almost is), I will add the metrics for memory usage as well.

**NOTE:** The profiling was done with all optimisation enabled (`-O3`, `-flto`, `-march=native` and `-fuse-ld=lld`) to simulate real world usage. Additionally, these metrics are without the use of `std::string_view` as I didn't use it originally with the initial implementation.

*Loosely inspired by how Redis dispatches commands via raw function pointers in C.*