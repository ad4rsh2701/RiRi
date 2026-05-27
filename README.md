# RiRi: A rapid, low-latency, in-memory key-value data engine

<!-- status & build -->
![Status](https://img.shields.io/badge/Status-MVP-success?style=flat-square)
![Build](https://img.shields.io/badge/Build-pending-lightgrey?style=flat-square)

<!-- our tech stack and clang biasness -->
![C++23](https://img.shields.io/badge/C++-23-00599C?style=flat-square&logo=c%2B%2B&logoColor=white)
![Compiler](https://img.shields.io/badge/Compiler-Clang++-blueviolet?style=flat-square)

<!-- repo info -->
[![License](https://img.shields.io/badge/License-Apache_2.0-informational?style=flat-square&logo=apache)](LICENSE.txt)
![Last Commit](https://img.shields.io/github/last-commit/ad4rsh2701/RiRi?style=flat-square&logo=github)  
![Contributors](https://img.shields.io/github/contributors/ad4rsh2701/RiRi?style=flat-square&logo=github)


RiRi is a fast, lightweight and an in-process key-value data engine (like a caching system) built in C++23.

## Roadmap

- [x] Root Data Structure for the Data Store: `DONE` (Using `ankerl::unordered_dense::map`)
- [x] Lower level functions/helpers for modifying the Data Store: `DONE`
- [x] Custom Response System for handling requests, error and validation: `DONE` (ref: [#30](https://github.com/ad4rsh2701/RiRi/pull/30))
- [x] User level functions/commands for inserting/fetching the data: `DONE` (ref: [#32](https://github.com/ad4rsh2701/RiRi/pull/32))
- [ ] First library build: `IN PROGRESS`
- [ ] Thread Safety and Multi-threading: `IDEATING`
- [ ] Data Persistence and Recovery


## License

This project is licensed under the [Apache License 2.0](https://www.apache.org/licenses/LICENSE-2.0). Unless otherwise
stated, all files are licensed under the Apache License, Version 2.0. See the [LICENSE.txt](LICENSE.txt) file for details.

> Note: The `src/include/ankerl/` directory contains code licensed under the MIT License, see [here](src/include/ankerl/LICENSE).