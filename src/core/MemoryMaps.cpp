constexpr size_t DEFAULT_MEMORY_CAPACITY = 100;
constexpr size_t DEFAULT_COMMAND_CAPACITY = 16;

#include "MemoryMaps.h"

using namespace RiRi;

ankerl::unordered_dense::map<
    std::string_view,
    RapidDataType,
    ankerl::unordered_dense::hash<std::string_view>,
    std::equal_to<>
> Internal::MemoryMap = [] {
    ankerl::unordered_dense::map<
        std::string_view,
        RapidDataType,
        ankerl::unordered_dense::hash<std::string_view>,
        std::equal_to<>
    > map;
    map.reserve(DEFAULT_MEMORY_CAPACITY);
    // NOTE: The size is reserved to avoid rehashing during runtime.
    // This is a small size, for development purposes.
    // Adjust the size based on the expected number of entries
    // which you can set in `riri.config` (TODO: Implement this).
    return map;
}();

ankerl::unordered_dense::map<
    std::string_view,
    RapidCommandFn,
    ankerl::unordered_dense::hash<std::string_view>,
    std::equal_to<>
> Internal::AuxCommandMap = [] {
    ankerl::unordered_dense::map<
        std::string_view,
        RapidCommandFn,
        ankerl::unordered_dense::hash<std::string_view>,
        std::equal_to<>
    > map;
    map.reserve(DEFAULT_COMMAND_CAPACITY);
    // NOTE: The reserved size is small because we expect a limited number of commands.
    // This can be adjusted based on the expected number of commands.
    // Make sure to adjust the size if you add more commands.
    return map;
}();
