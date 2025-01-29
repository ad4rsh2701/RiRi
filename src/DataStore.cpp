#include <unordered_map>
#include <string>
#include <stdexcept>

class DataStore {

private:
    std::unordered_map<std::string, std::string> data; // Key-value store

public:
    // CRUD operations
    void setValue(const std::string& key, const std::string& value){
        data[key] = value;
    } // CREATE
    
    std::string getValue(const std::string& key) const {
        auto it = data.find(key);
        if (it == data.end()) {
            throw std::out_of_range("Key not found");
        }
        return it->second;
    } // READ

    bool updateValue(const std::string& key, const std::string& value){
        auto it = data.find(key);
        if (it != data.end()) {
            it->second = value;
            return true;
        }
        return false;
    } // UPDATE
    
    bool deleteValue(const std::string& key){
        return data.erase(key) > 0;
    } // DELETE

    std::unordered_map<std::string, std::string> returnData() {
        return data;
    }
    
    void clearData() {
        data.clear();
    } // CLEAR
};
