// #include <unordered_map>
#include "ankerl/unordered_dense.h"
#include <string>
#include "DataStore.h" // Include the header file


void DataStore::setValue(const std::string& key, const std::string& value){
    data[key] = value;
} // CREATE

std::string DataStore::getValue(const std::string& key) const {
    auto it = data.find(key);
    if (it == data.end()) {
        return std::string(key) + ": (not found)\n";
    }
    return it->second;
} // READ

bool DataStore::updateValue(const std::string& key, const std::string& value){
    auto it = data.find(key);
    if (it != data.end()) {
        it->second = value;
        return true;
    }
    return false;
} // UPDATE

bool DataStore::deleteValue(const std::string& key){
    return data.erase(key) > 0;
} // DELETE

ankerl::unordered_dense::map<std::string, std::string> DataStore::returnData() {
    return data;
}

void DataStore::clearData() {
    data.clear();
} // CLEAR

std::string DataStore::getKey(const std::string& value) const {
    for (const auto& [key, val] : data) {
        if (val == value) {
            return key;  // Return first match
        }
    }
    return std::string(value) + ": (not found)\n";
} // SLOW_READ
