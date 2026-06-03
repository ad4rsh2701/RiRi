# RiRi: A high-performance C++ in-memory key-value library

![Version](https://img.shields.io/badge/Version-0.0.1-white?style=flat-square)
![Status](https://img.shields.io/badge/Status-Miden-success?style=flat-square)
![Build](https://img.shields.io/badge/Build-passing-success?style=flat-square)
![C++23](https://img.shields.io/badge/C++-23-00599C?style=flat-square&logo=c%2B%2B&logoColor=white)
![Compiler](https://img.shields.io/badge/Compiler-Clang++-blueviolet?style=flat-square)
![Last Commit](https://img.shields.io/github/last-commit/ad4rsh2701/RiRi?style=flat-square&logo=github)
![Contributors](https://img.shields.io/github/contributors/ad4rsh2701/RiRi?style=flat-square&logo=github)


RiRi is a fast, lightweight, in-process key-value static library built in C++23.

```cpp
#include <RiRi.hpp>

RiRi::RapidNode node {
    "riri_key",
    RiRi::RapidDataType(3.14159265359) 
};

RiRi::Commands::SET(node);
```

Benchmarks coming soon ^-^

---

## Features:

- CRUD operations on typed key-value pairs
- Native support for strings, integers, booleans, and doubles
- Bulk operations across multiple keys
- Rich response system with per-operation status codes for bulk results

For usage examples, see the [examples](#Examples).

---

## Building RiRi
Requires CMake 3.20+ and Clang++ (C++23).
```bash
git clone https://github.com/ad4rsh2701/RiRi.git
cd RiRi
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=clang++
cmake --build build
```
> Clang++ is the officially supported compiler. GCC/MSVC may work but is untested.

> To build tests, add `-DRIRI_BUILD_TESTS=ON` to the CMake command.

---

## Using RiRi via CMake
Add the following to your `CMakeLists.txt`
```cmake
include(FetchContent)
FetchContent_Declare(RiRi
    GIT_REPOSITORY https://github.com/ad4rsh2701/RiRi.git
    GIT_TAG        0.0.1
)
FetchContent_MakeAvailable(RiRi)
target_link_libraries(your_target PRIVATE RiRi)
```
> Replace `0.0.1` with the [latest release](https://github.com/ad4rsh2701/RiRi/releases).

---

## Examples
> NOTE: This section is a work in progress.
> And the API DOCS ARE ON THE WAY!
> For now, reference the doc-strings of commands at `src/include/RiRi/Commands.hpp`.


1.**Data Preparation**
```cpp
#include <RiRi.hpp>

// 1. Standard Method (Recommeneded)
// Create an array of nodes of types `RapidNode` with values to insert/fetch
RiRi::RapidNode nodes[] {
    {"riri_key", RiRi::RapidDataType(3.14159265359)}
};

// 2. Lazy Method
    // Do nothing, literally. 

// this allows you to send multiple nodes to the commands.
```
> `RapidNode`: is a simple struct that contains a `string` key and a `RapidDataType` value.

> `RapidDataType`: is a varaint over the types `string`, `int64`, `double` and `boo`.

2. Inserting single key-value pairs
```cpp
using namespace RiRi::Commands;

// Standard Method
auto response = SET(nodes);
// Lazy Method
auto response_lazy = SET("_key", 3.14159265359);

// you can use the response it returned!
if (response.status == RiRi::Status::OK) {
    // do something
}
```

3. Fetching single key-value pairs
```cpp
// Standard Method
auto get_response = GET(nodes); // this is ALLOWED!!
// Lazy Method
auto get_response_lazy = GET("_key");

if (get_response.status == RiRi::Status::OK) {
    auto data = get_response.field();   // data is of type `RapidDataType`
    // do something with the data
}
```


4. Inserting bulk key-value pairs
```cpp
// There is no LAZY method for bulk operations. Span of nodes is required.
RiRi::RapidNode many_nodes[] {
    {"riri_key", RiRi::RapidDataType(3.14159265359)},
    {"riri_key2", RiRi::RapidDataType("raresy")},
    {"riri_key3", RiRi::RapidDataType(true)},
};
auto batched_error_response = SET(many_nodes, RiRi::enableErrorBatched{});
    // this returns a list of keys which failed to get inserted (capped at a fixed entry limit) and why.
    // { {"key": ERROR_CODE}, ... }
    
// if you want the uncapped version
auto batched_response = SET(many_nodes, RiRi::enableBatched{});
```

5. Fetching bulk key-value pairs

```cpp
auto get_response = GET(many_nodes, RiRi::enableBatched{});
// returns a mixed list of keys which failed to get fetched and why, or the value if fetched
// { {"key": value}, {"key2": ERROR_CODE}, ... }
```

---

## Development

- [x] Root Data Structure for the Data Store
- [x] Lower level functions/helpers for modifying the Data Store
- [x] Custom Response System for handling requests, error, and validation (ref: [#30](https://github.com/ad4rsh2701/RiRi/pull/30))
- [x] User level functions/commands for inserting/fetching the data (ref: [#32](https://github.com/ad4rsh2701/RiRi/pull/32))
- [x] First library build (ref: [#33](https://github.com/ad4rsh2701/RiRi/pull/35))
- [x] Unit Testing and Benchmarking
- [x] Pre-Release 0.0.1
- [ ] Thread Safety and Multi-threading
- [ ] Data Persistence and Recovery


---

## License

This project is licensed under the [Apache License 2.0](https://www.apache.org/licenses/LICENSE-2.0). Unless otherwise
stated, all files are licensed under the Apache License, Version 2.0. See the [LICENSE.txt](LICENSE.txt) file for details.

> Note: The `src/include/ankerl/` directory contains code licensed under the MIT License, see [here](src/include/ankerl/LICENSE).