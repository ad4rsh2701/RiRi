#include <unordered_map>
#include <string>

class DataStore {

private:
    std::unordered_map<std::string, std::string> data; // Key-value store

public:

    void setKey(const std::string& key, const std::string& value){
        data[key] = value;
    }
     // Set key-value pair
    std::string getKey(const std::string& key){
        return data[key];
    }                    // Get value by key
    
    bool deleteKey(const std::string& key){
        return data.erase(key);
    }                        // Delete a key
    
    void clearData(){
        data.clear();
    }                                              // Clear all data
};