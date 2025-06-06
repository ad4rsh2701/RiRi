// TODO:
// Add more commands

#pragma once
#include <string>
#include <vector>
#include "DataStore.h"
#include "ankerl/unordered_dense.h"

// Global Pointer to the DataStore instance
extern DataStore* g_dataStore;

// Function pointer type for commands
using RiRiCommandFn = std::string(*)(const std::vector<std::string_view>&);

class CommandParser {
private:

    // Previous Implementation(the cool Lambdas):
    // ankerl::unordered_dense::map<
    //     std::string_view,
    //     std::function<std::string(const std::vector<std::string>&)>
    // > riricommands;

    // Current Mapping using simple plain function pointers
    ankerl::unordered_dense::map<std::string_view, RiRiCommandFn> riricommands;
    std::vector<std::string_view> parseCommand(std::string_view command) const; // Parse and validate commands

public:
    explicit CommandParser(DataStore* ds); // Constructor
    std::string executeCommand(std::string_view command) const; // Execute commands on the DataStore
};

std::string setCommand(const std::vector<std::string_view>& args);
std::string getCommand(const std::vector<std::string_view>& args);


// So, why not std::function?

// Originally, commands were stored using std::function and lambdas in a string-to-function map.
// However, profiling showed that it introduces overhead due to:
// 
//  - I do not know how, but potential heap allocations (likely from type erasure or dynamic storage).
//  - Type erasure logic (polymorphism essentially, we don't need that)
//  - Larger Objects because std::function is typical 32+ bytes.
//
// Since we do NOT need capturing lambdas (although they are cool), and we are mostly working
// with global/static functions, we are using function pointers instead.
//
// Metrics:
// Iterations   std::function   function pointer    % Difference
// 1 million    137.191 ms      134.613 ms          ~1.88% faster
// 1.4 billion  202,236 ms      189,615 ms          ~6.2% faster
//
// Sure, this isn't a deal-breaking change — I don't expect RiRi to deal with 1.4 billion commands.
// But the difference is clear. Plus, since we are not capturing lambdas memory usage is expected
// to be lower.

// Expected? Where are the metrics?
// I wasn't able to benchmark the memory usage due to the limited number of commands and lack of
// its code/logic cleanup. So, as soon as the command parser is ready, I will add the metrics for
// memory usage as well.
//
// Compiler Used: Clang++
// NOTE: The profiling was done with all optimisation enabled ("-O3", "-flto", "-march=native"
// and "-fuse-ld=lld") to simulate real world usage. Additionally, these metrics are without the
// use of `std::string_view` as I have yet to implement it properly.
//
// Loosely inspired by how Redis dispatches commands via raw function pointers in C.

// Also TODO: After getting the updated metrics, move it to readme.