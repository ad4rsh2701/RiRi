# Rapid In-Memory Redis Interface
or simply **RiRi** :)

![Status](https://img.shields.io/badge/Status-PRE--MVP-red)
![Stage](https://img.shields.io/badge/stage-Under_Active_Development-darkorange)

![Made with C++](https://img.shields.io/badge/Made_with-C++23-green)
![Compiler](https://img.shields.io/badge/compiler-clang++-blueviolet)
![Build](https://img.shields.io/badge/build-pending-lightgrey)  
[![License](https://img.shields.io/badge/license-RRDL_v1-informational)](./LICENSE)
![Last Commit](https://img.shields.io/github/last-commit/ad4rsh2701/RiRi)
![Contributors](https://img.shields.io/github/contributors/ad4rsh2701/RiRi)

RiRi, or **Rapid In-Memory Redis Interface**, is a high-performance, multi-modal system for managing in-memory key-value data. It’s designed to be used as a lightweight C++ **library**, a drop-in **embedded** data engine, or a standalone **backend server** — depending on your use case.

Built entirely from _scratch_ with performance and simplicity in mind, RiRi is crafted to be **fast**, **lightweight**, and **memory-efficient**. It leverages modern _C++23 features_ and relies on _one of the fastest hash maps available_ for its internal map structure.

## What is RiRi?
The description above already defines RiRi.  
But when I describe it _like that_ (you know, polished and professional), it almost sounds like I landed a summer internship in 2025 — I didn’t. So here, a much _simpler_ explanation of what RiRi actually is.

Firstly, let's clarify what a "key-value data store/map" is:

Imagine a map, where each node/point has two dimensions (_dumber version: parameters_), a `key` and a `value`. The keys are like the address to these nodes.

When fetching values, we usually search the `key` associated with the value we need (99.99% of the time, you're searching by key). Hence, a key is generally of a fixed datatype, but it should be able to support a large number of values. In C++, keys are generally of the type `std::string`.

The `value` is the "data" we are concerned with, it can be user input, phone number, your GitHub password, or anything else. Values are generally of multiple types (`int`, `string`, `char`, `bool`, `arrays[]`, `vectors<>`, etc.), usually we use a custom datatype to be able to support multiple data types.

In memory, abstractly speaking, the data is structured like this — think of it like JSON:
```json
{
  "name": "RiRi",
  "complete": false,
  "builds": 0,
}
```
(Benefits of storing like this? — should be obvious, if not... _it’s a great little homework_)

> Inside RiRi
>- `keys` are of the type `std::string`.
>- `values` are of `RapidDataType` (via `std::variant` (C++23)), and currently supports boolean, integer, floating, string and character types.

That's essentially what a key-value map or a ‘k-v map’ is. With this clarified, we can finally explain what RiRi is, in a much better way:

- RiRi is a bunch of code compiled into a binary, which _at your mercy_ modifies the key-value map however you command (add, search, delete, etc.).
- To achieve this, RiRi can be embedded directly into your code — acting as your own _temporary data holder/engine_. However with one difference... it's **extremely fast**.
- Alternatively, RiRi can run as a standalone backend — in a traditional client-server setup. RiRi becomes the **server**, your program(s) the **client(s)**.

The general idea of RiRi should be much clear now, as for the C++23 features and the _one of the fastest hash maps_; these are just tools, used to implement RiRi. These will be discussed in the upcoming sections with great detail (especially that map).

Reiterating again, RiRi is made in **pure C++** and uses no external dependencies (except one, the hash map, which has no overhead, included with RiRi). Future scaling may involve python, node.js and java into RiRi (connector implementation).

Oh and RiRi is made to be compiled in C++23 and above versions only. Why the latest C++ and not standard C++17? Because RiRi requires better and faster features, with minimal overhead, not standards.

---
This readme is incomplete