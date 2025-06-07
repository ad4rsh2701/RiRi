// TODO:
// - Finalize documentation for each command
// - Implement AUTOSET once hashing scheme is chosen
// - Possibly integrate performance metrics per command


// NOTE: The commented code are with due respect to the previous
// implementation of string-to-function map, as discussed in the
// CommandParser.h header file. More details given later below.

#include "CommandParser.h"

// Global pointer to the DataStore instance used by all command functions.
DataStore* g_dataStore = nullptr;

// We will use typical functions, instead of lambda functions.


std::string setCommand(const std::vector<std::string_view>& args) {
    if (args.size() == 2) {
        g_dataStore->setValue(std::string(args[0]), std::string(args[1]));
        return "OK";
    } else if (args.size() > 2 && args.size() % 2 == 0) {
        for (size_t i = 0; i < args.size(); i += 2) {
            g_dataStore->setValue(std::string(args[i]), std::string(args[i + 1]));
        }
        return "OK (" + std::to_string(args.size() / 2) + " keys)";
    } else if (args.size() < 2) {
        return "ERROR: SET needs at least KEY and VALUE";
    } else {
        return "ERROR: SET needs even number of arguments";
    }
}


std::string getCommand(const std::vector<std::string_view>& args){
    if (args.size() == 1){ return g_dataStore -> getValue(std::string(args[0])); }
    else if (args.size() > 1){
        std::string result;
        for (auto& key : args){
            result+=std::string(key) + ": " + g_dataStore->getValue(std::string(key)) + "\n";
        }
        return result;
    } else {
        return "ERROR: GET needs at least one KEY";
    }
}


std::string updateCommand(const std::vector<std::string_view>& args){
    if (args.size() == 2) {
        return g_dataStore->updateValue(std::string(args[0]), std::string(args[1])) ? "OK" : "ERROR: Key not found";    // I am so sorry
    } else if (args.size() > 2 && args.size() % 2 == 0) {
        int updated = 0;
        for (size_t i = 0; i < args.size(); i += 2) {
            if (g_dataStore->updateValue(std::string(args[i]), std::string(args[i + 1]))) {
                ++updated;
            }
        }
        return "OK (" + std::to_string(updated) + " updated)";
    } else if (args.size() < 2) {
        return "ERROR: UPDATE needs KEY and VALUE";
    } else {
        return "ERROR: UPDATE needs even number of arguments";
    }
}


std::string deleteCommand(const std::vector<std::string_view>& args) {
    if (args.size()==1) {
        return g_dataStore->deleteValue(std::string(args[0])) ? "OK (1 deleted)" : "ERROR: Key not found";
    } else if (args.size() > 1){
        int deleted = 0;
        for (auto& key : args) {
            if (g_dataStore->deleteValue(std::string(key))) {
                ++deleted;
            }
        }
        return "OK ("+ std::to_string(deleted) + "deleted)";
    } else { return "ERROR: DELETE needs at least one KEY"; }

}



std::string getAllCommand(const std::vector<std::string_view>& args) {
    if (!args.empty()) return "ERROR: GET_ALL does not accept arguments";

    auto allData = g_dataStore->returnData();
    std::string result;

    for (const auto& [key, value] : allData) {
        result += key + ": " + value + "\n";
    }

    return result.empty() ? "EMPTY" : result;
}


// CLEAR fn()
std::string clearCommand(const std::vector<std::string_view>& args) {
    if (!args.empty()) return "ERROR: CLEAR does not accept arguments";

    g_dataStore->clearData();
    return "OK (cleared)";
}

// AUTOSET fn() : TODO
std::string autoSetCommand(const std::vector<std::string_view>& args) {
    return "ERROR: AUTOSET not yet implemented.";
}


// SEARCH fn()
std::string searchCommand(const std::vector<std::string_view>& args) {
    if (args.size() == 1){ return g_dataStore -> getKey(std::string(args[0])); }
    else if (args.size() > 1){
        std::string result;
        for (auto& value : args){
            result+=std::string(value) + ": " + g_dataStore->getKey(std::string(value)) + "\n";
        }
        return result;
    } else {
        return "ERROR: SEARCH needs at least one VALUE";
    }    
}


CommandParser::CommandParser(DataStore* dataStore) {
    g_dataStore = dataStore;
    
    // ririCommandMap.emplace("SET", [&dataStore](const std::vector<std::string>& args) { 
    //     if (args.size() != 2) return "ERROR: SET needs key and value";
    //     g_dataStore->setValue(args[0], args[1]);
    //     return "OK";
    // });
    
    // ririCommandMap.emplace("GET", [&dataStore](const std::vector<std::string>& args) { 
    //     if (args.size() != 1) return std::string("ERROR: GET needs key");
    //     return g_dataStore->getValue(args[0]);
    // });

    // In the above code, I was emplacing the command string and mapping it with lambda
    // functions. In this case, the dataStore was captured by references. I used lambda
    // functions as they were awfully convienent to play with and pass as std::functions
    // to other functions. I didn't consider the overhead of this appraoch at that time.

    // Commands
    ririCommandMap.emplace("SET", setCommand);
    ririCommandMap.emplace("GET", getCommand);
    ririCommandMap.emplace("UPDATE", updateCommand);
    ririCommandMap.emplace("DELETE", deleteCommand);
    ririCommandMap.emplace("GET_ALL", getAllCommand);
    ririCommandMap.emplace("CLEAR", clearCommand);
    ririCommandMap.emplace("AUTOSET", autoSetCommand);
    ririCommandMap.emplace("SEARCH", searchCommand);
    
    // Command Aliases
    ririCommandMap.emplace("DEL", ririCommandMap.at("DELETE"));
    ririCommandMap.emplace("DUMP", ririCommandMap.at("GET_ALL"));
    ririCommandMap.emplace("CLR", ririCommandMap.at("CLEAR"));
    ririCommandMap.emplace("YEET", ririCommandMap.at("CLEAR"));
    ririCommandMap.emplace("HASH_SET", ririCommandMap.at("AUTOSET"));
    ririCommandMap.emplace("FIND_BY_VALUE", ririCommandMap.at("SEARCH"));

}


// Parsing the command by tokenizing it.
std::vector<std::string_view> CommandParser::parseCommand(std::string_view command) const {
    std::vector<std::string_view> tokens;
    size_t pos = 0;

    while (pos < command.size()) {
        size_t spacePos = command.find(' ', pos);
        if (spacePos == std::string::npos) {
            spacePos = command.size();
        }

        tokens.emplace_back(command.substr(pos, spacePos - pos));
        pos = spacePos + 1;
    }

    return tokens;
}
// I am aware that using a struct like:
//  struct ParseResult {
//      const char* command;
//      const char* arg1;
//      const char* arg2;
//      size_t arg1_len;
//      size_t arg2_len;
//  };
// would have been the fastest method, but this method is extremely error prone
// and I would like to avoid that, additionally I cannot think of a simple
// way to parse commands like "SET key1 value1 key2 value2" using this.
// Bonus: Tokenization doesn't modify the original string.


// Executing the command based on the first token.
std::string CommandParser::executeCommand(std::string_view command) const {
    std::vector<std::string_view> tokens = parseCommand(command);
    if (tokens.empty()) return "ERROR: No command entered";

    std::string_view commandName = tokens[0];
    tokens.erase(tokens.begin());  // Remove command name from args

    auto it = ririCommandMap.find(commandName);
    if (it == ririCommandMap.end()) return "ERROR: Unknown command";

    return it->second(tokens);  // Call the function stored in the map
}