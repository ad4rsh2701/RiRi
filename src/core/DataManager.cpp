#include "DataManager.h"
#include "MemoryMaps.h"


namespace RiRi::Internal {


    bool setValue(std::string&& key, RapidDataType&& value) noexcept {
        return MemoryMap.try_emplace(std::move(key), std::move(value)).second;
    }


    const RapidDataType* getValue(const std::string_view key) noexcept {
        const auto it = MemoryMap.find(key);
        if (it == MemoryMap.end()) {
            return nullptr;         // key not found
        }
        return &it->second;         // key found
    }


    bool deleteKey(const std::string_view key) noexcept {
        return MemoryMap.erase(key) > 0;            // returns true if the key was found and erased else false
    }


    bool updateValue(const std::string_view key, RapidDataType&& newValue) noexcept {
        const auto it = MemoryMap.find(key);
        if (it == MemoryMap.end()) return false;    // key not found

        it->second = std::move(newValue);           // update the value associated with the key
        return true;
    }


    const std::string* getKeyByValue(const RapidDataType& value) noexcept {
        for (const auto& [key, val] : MemoryMap) {
            if (val == value) {
                return &key;    // Return the first key that matches
            }
        }
        return nullptr;         // No match found
    }


    void clearMap() noexcept {
        MemoryMap.clear();          // Clear all entries from the internal memory map
    }


    size_t size() noexcept {
        return MemoryMap.size();    // Return the size of the internal memory map
    }

} // namespace RiRi::Internal