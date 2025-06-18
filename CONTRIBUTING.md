# Contributing to RiRi

...wait really, you’re considering contributing to RiRi?  
Thanks, I guess?

**Disclaimer**: Contributing to RiRi isn’t your typical drag-and-drop web dev weekend.  
There will be **pain**, **macros**, and **me** (which is arguably the worst of the three).

That said, if you’re still here —  
you’re either brave, brilliant, a masochist, or just bored. In any case, welcome.

###### [*Click to skip to Getting Started*](#getting-started)
---

Before I lay out a *very lacking* set of contribution guidelines, you deserve a brief synopsis of myself.  
Trust me, I am **a very professional developer**, as you’re about to find out:

- Soon-to-be a 3rd year Computer Science undergrad student
- Didn’t know `.hpp` existed until a commit in this very repo
- Doesn’t know React well, but can somehow *design*
- Asks ChatGPT/Claude more than his friends (avg. 70% of the time — spikes to 100% during panic)
- Still Googles/Brave Searches his problems (~40%, rising when the LLMs disappoint)
- Doesn’t type *all* code
- Relies heavily on luck
- Still doesn't know the best practices

**TL;DR**: _Professional Developer_ (didn't get an internship this summer either).

---

Now that you know why I am possibly the worst,  
here you go — a questionably helpful guide on how to contribute to RiRi.  
(But really, thank you. I mean it.)


# Getting Started

RiRi is a moderately extensive project, with a lot of WORK IN PROGRESS, there have been virtually no builds or compilation yet (at least after the restructure, ref: 689df3c), the core is still being developed.

Even so, the compiler is already decided, due to the reasons explained later. So instead of going through all the usual contributing guidelines, I would suggest installing the compiler (or at least know which compiler we are keeping in mind for the final builds/binaries).

## The Compiler: `clang++`

This one’s non-negotiable.  
RiRi is compiled using **`clang++` only** — _and nothing else_.

**Why**?    
Because I’m using the absolute unit of a hash map from [**martinus/unordered_dense**](https://github.com/martinus/unordered_dense/blob/main/include/ankerl/unordered_dense.h) (massive thanks, Martinus), which is arguably the **fastest hashmap on Earth**.

RiRi depends on `ankerl::unordered_dense::map`, and that beast performs **best with `clang++`**, so that’s what this entire project is tuned for.

If you're thinking of using GCC or MSVC —  
well… don't. You’ll regret it (_psst.. macros will break_).

Already have Clang? [Skip to the next section](#coding-practices-philosophy)

### Installing Clang
> Follow any of the following methods to get the clang compiler:
>1. **LLVM (official Clang)**
>    - For Windows:
>       - Head over to: https://github.com/llvm/llvm-project/releases
>       - Download the appropriate LATEST RELEASED binary (`clang+llvm-2X.X.X-x86_64-pc-windows-msvc.tar.xz`).
>       - Too Lazy? I can tell. Just click this link and the download should start: [clang_llvm-20.1.7](https://github.com/llvm/llvm-project/releases/download/llvmorg-20.1.7/clang+llvm-20.1.7-x86_64-pc-windows-msvc.tar.xz)
>       - Wait for your slow internet to download it ✨
>       - While you wait, go and create a new folder called `LLVM` anywhere on your drive (a path that’s easy to remember — please.). In my case it's `C:\LLVM`.
>       - Once installed, extract the `.tar` file using **7zip** or **WinRAR** (default windows extractor doesn't do `.tar`).
>       - Once extracted, you will see another archived/zipped folder (I don't know which compression, just extract it using 7zip).
>       - After extraction you will see a few folders, namely `bin`, `include`, `lib`, etc.
>       - Cut ALL these folders and paste it in the `LLVM` folder you created before.
>       - Copy the path to the `bin` folder.
>       - Add this path value (in my case `C:\LLVM\bin`) to your **system PATH** (_If you don’t know how, now’s a good time to level up_).
>       - Open a terminal, and type: `clang++ --version`, and if there is an output like:
>           ```
>           clang version 20.1.7
>           Target: x86_64-pc-windows-msvc
>           Thread model: posix
>           InstalledDir: C:\LLVM\bin
>           ```
>       - You are good to go! Congratulations I guess.
>    - For Linux:
>       - Just go into your terminal and do: `sudo apt install clang llvm`
>       - This should work flawlessly? Or just google it, I am not developing RiRi on Linux.
>2. **MSYS2 (for Windows only):**  
>     I don’t use this setup. If you figure it out, send a PR.
>
> What? You need a guide for.. Clang on _Mac_?   
> Mac? What’s that? I’m afraid I’m too broke to know about this (_psst.._ explore the [LLVM release page](https://github.com/llvm/llvm-project/releases), I saw some "mac"s in there).  


#### Although I recommend reading everything above.. but here, a `Too Lazy;DR`:
- Only `clang++` is supported.  
- Don’t complain if it breaks under anything else.  
- If you didn’t read the full section... go fix that.

## Coding Practices/ Philosophy
_a.k.a_ "do this my way"

- **Performance-first.** Always prefer minimal allocations, `string_view`, `span`,  and `constexpr` where possible.
- **Version Independency.** Don't shy away from using the latest C++ features (latest stable C++ is `C++23`), that is if they can provide a better, faster and a cheaper solution.
- **Clarity matters.** Code should be understandable without comments — and *enhanced* with them. If a function name needs to be longer to be more clear, make it longer.
- **Internal APIs are sacred.** Only touch files under `src/` if you're modifying core logic. Public headers live in `include/riri/`.
- **Macros are not scary.** Please make sure to use the macros appropriately and their use is mandatory.

## Folder Structure
For some, it might feel massive, and for others it might feel very small, regardless, knowing the purpose of each folder is important, before you start contributing.

> NOTE  
> There are multiple seemingly empty folders and files, these folders and files were used to "scaffold" the directory structure for future expansion and compatibility. Please do not judge.

### Active Directories
| Folder | Purpose |
|--------|---------|
| `include/riri/` | Public headers (`.hpp`) for embedded/server usage |
| `src/include/` | Private headers (`.h`) only for internal use |
| `src/core/` | Internal implementation files |
| `src/core/ankerl` | External header file for `unordered_dense`
| `src/utils/` | Helpers like `RapidError.h`, etc. |
| `.notes/` | Design decisions, optimization notes, etc. |

### Inactive Directories
| Folder | Purpose |
|--------|---------|
| `src/cli` | Implementation file for the CLI interface of RiRi (for development/testing purpose only).
| `src/server` | For client-server architecture.
| `src/core/persistence` | Implementation files (empty right now) for future data persistence.
| `connectors/` | For connectors, allowing RiRi to be used in any programming language (planned for: `python`, `node.js` and `java`).
| `config/` | For storing user configured values, such as the number of key-value pairs the map should reserve space for, etc.
| `release` | Built Binaries and RiRi library go here.

Inactive directories only have empty `readme.md`s or empty header/implementation files in them, for now.

## Branching Strategy
Now that you know the coding practices, the directory structure, here's how to start contributing:

Firstly, pick an issue to work on, read it, understand it, and make it your goal. **DO NOT WORK INDEPENDENTLY**, all contributions must address an **ISSUE**.

Check out the current issues here: [RiRi Open Issues](https://github.com/ad4rsh2701/RiRi/issues)

This makes assigning goals/targets much easier, and A LOT MORE EASIER for you, the contributor.

Now that you selected an issue, and got yourself assigned for that, let's setup your RiRi workspace on your system.

> Note   
> From this point onwards, I am assuming you are working with VSCode, if that's not the case, the instructions are general enough for other editors/IDEs as well (_neoVim anyone?_).

---
### Setup (the tedious one)
Open your VSCode console (`CTRL`+`SHIFT`+`~`) in the folder you want to clone RiRi on and type:
```git
git clone https://github.com/ad4rsh2701/riri.git
```
That should add the entire RiRi directory. Now, if you do not see a `.vscode` automatically made by your VSCode, then create a `.vscode` folder and inside it, add a `c_cpp_properties.json` file, with the following configuration:

```json
{
    "configurations": [
        {
            "name": "Win32",
            "includePath": [
                "${workspaceFolder}/**",
                "C:/LLVM/include"
            ],
            "defines": [
                "_DEBUG",
                "UNICODE",
                "_UNICODE"
            ],
            "compilerPath": "C:/LLVM/bin/clang++.exe",
            "cStandard": "c23",
            "cppStandard": "c++23",
            "intelliSenseMode": "clang-x64"
        }
    ],
    "version": 4
}
```
This just sets up your intellisense and compiler path, you can even add these in your `settings.json` file, inside the same folder:
```json
    "C_Cpp.default.cppStandard": "c++23",
    "C_Cpp.default.cStandard": "c23",
    "C_Cpp.default.intelliSenseMode": "windows-clang-x64",
    "C_Cpp.workspaceSymbols": "All",
    "C_Cpp.default.compilerPath": "C:/LLVM/bin/clang++.exe"
```

Note that the compiler path must direct to the compiler path you chose while installing Clang, refer [Installing Clang](#installing-clang). Additionally, note that we are using the latest stable C++23 standard.

I know, one `make RiRi` command using `CMake` can skip all these, but I am yet to setup CMAKE for RiRi (soon, I promise).

If you are using other IDEs/Editors, just make sure you are working in C++23 and the compiler is set to Clang for hinting/intellisense.

---
Now, that the "one time setup" is done, we can go back to the branching strategy.

Just follow the below points and you should be fine:

### 1. NEVER push directly to main!

We have one **main** branch, for now, pushing changes directly to the main branch is strictly not allowed.

Instead, create feature branches:
```git
git checkout -b feat/data-manager
```
This creates a branch from the "main" branch. You can create a branch via the GitHub UI on the website too if you don't fancy the CLI method.

Just make sure the branch name follows this format:  
```
type/scope-additional-personal-comments
```
For example:    
- `feat/Parser`
- `feat/Data-Manager-please-be-nice`
- `docs/contributing-md-or-my-demise` (yes, `CONTRIBUTING.md` was made in this)

The types could be anything from the following:
- `feat`: If your branch is adding a feature.
- `fix`: If your branch is performing a fix.
- `exp`: If your branch is working on something experimental.
- `docs`: If your branch is modifying/adding/cursing in the docs/documentation.

Feel free to go wild with personal comments, just make sure the type and scope are proper.

Also, to switch to a branch, do:
```git
git checkout branch-name
```

### 2. Perform changes/commits in the branch you made
Makes changes, break code, cry, scream, ask for help, literally anything, do it here. And after you are done, commit those changes, atomically.

Remember for each individual feature/addition you add, there should be one commit (for reference check: 788638c).

Say, you are adding multiple functions to a file, then each function should have its own commit. These keep commit messages short, concise and very sharp.

> Ignore my past commit history please — just because I wasn't using proper commits doesn't mean you shouldn't either. Though I am not abandoning the commit style, and it was better than your commits at least, so zip it.

#### Use clear, structured commit messages:

`type(scope): message`  
Similar to branch name, the type should mention what are you working on, the scope should mention exactly what type you are adding (function name, class name, file name if fixing bugs), and the message should document your changes.

Examples:
- `feat(clearMap): Implemented clearMap method in DataManager`
- `Fix(deleteKey): Added noexcept to deleteKey method`

You can even go ahead and add more details in the commit like so, though its not necessary, but you will need to explain that in the Pull Request or Issue discussion.

Example of a proper commit message:
```
feat(clearMap): Implemented clearMap method in DataManager
- The `clearMap` method clears the internal MemoryMap and resets the size to 0.
- I am not checking if it succeeds or not because there is no reason for it to fail.
- We may check the size of the map, in the layer above, after calling this method to ensure it has been cleared (if needed, a very big IF).
```

### 3. Create a PULL REQUEST (PR) for your branch

After the suffering, you may open a Pull Request for your branch, so that the changes maybe be pulled over to the main branch.

I recommend creating Pull Request via the GitHub UI, since it shows the issue number and proper preview of your PR message.

This PR should be descriptive or at least be long enough to explain your questionable coding choices.

#### The PR Message should always reference the ISSUE it was meant to resolve.

Adding a line like in your PR message:
```
Resolves #5 (Issue Name; optional)
```
Automatically references the issue you have been working so tirelessly on. For example:
```
Resolves #5
OR
Resolves #5 (Add Implementation for the basic map modification functions)
```
A lack of an Issue reference will lead to instant PR rejection (exceptions always exists, more so in my case).

### 4. Discuss, improve if needed, and finally take a break.
During the entire duration of the PR, you may be required to add improvements, more commits, heck even an entire new PR, but once the PR is pulled into main successfully, take a break, relax, for there is always another issue waiting for you.

## RiRi Gotchas (FAQs)

I know you are confused, I still am too and I am not the best maintainer/coder, I know that too.

But here are some answers to clear some of your confusion:

### Q. "What are we working on at the moment? Exactly?"
- We are currently building the core functionality of RiRi, these include commands, command parser, data maps, helper functions for accessing/modifying data and command executioner.
- Immediately after this, we will work on a very basic CLI, to test RiRi internals.
- Lastly, we will work on making RiRi for embedded use (serverless architecture), which will allow other developers to directly import the RiRi module/library and be able to use RiRi in their code, just like SQLite.
- After this, the first version of RiRi will be shipped, and we can start working on improvements, extensions, connectors and of course Client-Server Architecture (networking capabilities).

### Q. "What are these macros.. `RIRI_API`, `GO_AWAY`???"
- Ahh yes, these macros server two purposes:
    - To mark which functions to be exported to the symbols table (during builds): `RIRI_API`
    - To not let users use functions available in the private headers: `GO_AWAY`
- Make sure to use these aptly and mercilessly, usually the Issue description will define which macros to use.

### Q. "Private/Public headers? `.hpp` and `.h`? What?
It's a simple bifurcation, that's all.
- The private headers (or those which end with `.h`), are part of the internal system of RiRi, they are not meant to be used by the developer who may be using RiRi as an embedded/networked system.
- Conversely, the public headers (or those which end with `.hpp`), contains methods or classes, which are intended to be used by the developer who is working with RiRi in a embedded/networked mode.


### Q. Why is there no "Build" section?
- This should be obvious, we are still in development and I have yet to setup CMAKE.
- Once we are done with core functionality of RiRi and start working on CLI for testing, there will be a Build section with proper use of CMAKE for convenience.


### Q. What is ".notes/"? I have never seen this in any repo..."
Makes sense, and for the most part, ignore it. It's a documentation lore tablet essentially, it's for future me's reference only. Tho, feel free to explore it, you are not allowed to make changes there tho (yet, at least).  
Also, if you don't understand something, chances are that [.notes](.notes/) got you covered.

---

That's all!

Thanks for keeping RiRi clean, fast, and sharp.  
Now go write something dangerously efficient.

And if you are not my future self, then I do appreciate you taking your time to read all this and considering contributing.