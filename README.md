# RiRi: A rapid, low-latency, in-memory key-value data engine

![Status](https://img.shields.io/badge/Status-PRE--MVP-red)
![Stage](https://img.shields.io/badge/stage-Under_Active_Development-darkorange)

![Made with C++](https://img.shields.io/badge/Made_with-C++23-green)
![Compiler](https://img.shields.io/badge/compiler-clang++-blueviolet)
![Build](https://img.shields.io/badge/build-pending-lightgrey)  
[![License](https://img.shields.io/badge/license-RRDL_v1-informational)](./License.md)
![Last Commit](https://img.shields.io/github/last-commit/ad4rsh2701/RiRi)
![Contributors](https://img.shields.io/github/contributors/ad4rsh2701/RiRi)


RiRi is a fast, secure and lightweight key-value data engine (like a caching system) built in C++23.

## Roadmap
    [#] Root Data Structure for the Data Store: DONE (Using `ankerl::unordered_dense::map`)
    [#] Lower level functions/helpers for modifying the Data Store: DONE
    [#] Custom Response System for handiling requests, error and validation: DONE
    [ ] User level functions/commands for inserting/fetching the data: IN PROGRESS
    [ ] First library build: IN PROGRESS
    [ ] Thread Safety and Multi-threading: PLANNED NEXT
    [ ] Data Persistance and Recovery
