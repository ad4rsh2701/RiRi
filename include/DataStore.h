// DataStore.h
#ifndef DATA_STORE_H
#define DATA_STORE_H

#include <unordered_map>
#include <string>
#include "ankerl/unordered_dense.h"

class DataStore {
private:
    //std::unordered_map<std::string, std::string> data; // Key-value store
    ankerl::unordered_dense::map<std::string, std::string> data;
public:
    void setValue(const std::string& key, const std::string& value);    // Set key-value pair
    
    std::string getValue(const std::string& key) const;                       // Get value by key
    
    bool deleteValue(const std::string& key);                           // Delete a key
    bool updateValue(const std::string& key, const std::string& value); // Update value by key

    // std::unordered_map<std::string, std::string> returnData();          // Return all data
    ankerl::unordered_dense::map<std::string, std::string> returnData();
    
    std::string getKey(const std::string& value) const;
    
    void clearData();                                                   // Clear all data
};

#endif // DATA_STORE_H