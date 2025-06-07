// DataStore.h

#pragma once
#include <unordered_map>
#include <string>
#include "ankerl/unordered_dense.h"

class DataStore {
private:
    //std::unordered_map<std::string, std::string> data; 
    ankerl::unordered_dense::map<std::string, std::string> data;            // Key-value store
    
public:
    void setValue(const std::string& key, const std::string& value);        // Set key-value pair
    
    std::string getValue(const std::string& key) const;                     // Get value by key
    
    bool deleteValue(const std::string& key);                               // Delete a key
    
    bool updateValue(const std::string& key, const std::string& value);     // Update value by key

    // std::unordered_map<std::string, std::string> returnData();
    ankerl::unordered_dense::map<std::string, std::string> returnData();    // Return all data
    
    std::string getKey(const std::string& value) const;
    
    void clearData();                                                       // Clear all data
};