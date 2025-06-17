#include "src/include/DataManager.h"
#include "src/include/MemoryMaps.h"

using namespace RiRi;

bool Internal::setValue(std::string_view key, const RapidDataType& value) noexcept {
    return Internal::MemoryMap.insert({key, value}).second;
}


std::optional<RapidDataType> Internal::getValue(std::string_view key) noexcept {
    auto it = Internal::MemoryMap.find(key);
    if (it == Internal::MemoryMap.end()) {
        return std::nullopt;  // key not found
    }
    return it->second;        // key found
}


bool Internal::deleteKey(std::string_view key) {
    return Internal::MemoryMap.erase(key) > 0;  // returns true if the key was found and erased else false
}