// #include <unordered_map>
#include "../include/ankerl/unordered_dense.h"
#include <string>
#include <stdexcept>
#include "../include/DataStore.h" // Include the header file

// CRUD operations
void DataStore::setValue(const std::string& key, const std::string& value){
    data[key] = value;
} // CREATE

std::string DataStore::getValue(const std::string& key) const {
    auto it = data.find(key);
    if (it == data.end()) {
        throw std::out_of_range("Key not found");
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