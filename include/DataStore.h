// DataStore.h
#ifndef DATA_STORE_H
#define DATA_STORE_H

#include <unordered_map>
#include <string>

class DataStore {
private:
    std::unordered_map<std::string, std::string> data; // Key-value store
public:
    void setValue(const std::string& key, const std::string& value);    // Set key-value pair
    std::string getValue(const std::string& key);                       // Get value by key
    bool deleteValue(const std::string& key);                           // Delete a key
    bool updateValue(const std::string& key, const std::string& value); // Update value by key
    std::unordered_map<std::string, std::string> returnData();          // Return all data
    void clearData();                                                   // Clear all data
};

#endif // DATA_STORE_H