#include "../include/DataManager.h"

using namespace RiRi;

bool Internal::setValue(std::string&& key, const RapidDataType& value) noexcept {
    return Internal::MemoryMap.insert({std::move(key), value}).second;
}


const RapidDataType* Internal::getValue(std::string_view key) noexcept {
    auto it = Internal::MemoryMap.find(key);
    if (it == Internal::MemoryMap.end()) {
        return nullptr;  // key not found
    }
    return &it->second;      // key found
}


bool Internal::deleteKey(std::string_view key) noexcept {
    return Internal::MemoryMap.erase(key) > 0;  // returns true if the key was found and erased else false
}


bool Internal::updateValue(std::string_view key, const RapidDataType& newValue) noexcept {
    auto it = Internal::MemoryMap.find(key);
    if (it == Internal::MemoryMap.end()) return false;  // key not found

    it->second = newValue;      // update the value associated with the key
    return true;
}


const std::string* Internal::getKeyByValue(const RapidDataType& value) noexcept {
    for (const auto& [key, val] : Internal::MemoryMap) {
        if (val == value) {
            return &key; // Return the first key that matches
        }
    }
    return nullptr; // No match found
}


void Internal::clearMap() noexcept {
    Internal::MemoryMap.clear();  // Clear all entries from the internal memory map
}


size_t Internal::size() noexcept {
    return Internal::MemoryMap.size();  // Return the size of the internal memory map
}