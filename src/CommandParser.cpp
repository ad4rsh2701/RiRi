// TODO: Better implementation, more commands and documentation.

// NOTE: The commented code are with due respect to the previous
// implementation, as discussed in the CommandParser.h header file.
// More details given later below.

#include "../include/CommandParser.h"

DataStore* g_dataStore = nullptr;  // initializing

// Typical functions, instead of lambda functions.
std::string setCommand(const std::vector<std::string_view>& args) {
    if (args.size() != 2) return "ERROR: SET needs key and value";
    g_dataStore->setValue(std::string(args[0]), std::string(args[1]));  // Direct global access is BETTER
    return "OK";
}
std::string getCommand(const std::vector<std::string_view>& args){
    if (args.size()!=1) return "ERROR: GET needs key";
    return g_dataStore->getValue(std::string(args[0]));  // just better
}

CommandParser::CommandParser(DataStore* dataStore) {
    g_dataStore = dataStore;
    
    // riricommands.emplace("SET", [&dataStore](const std::vector<std::string>& args) { 
    //     if (args.size() != 2) return "ERROR: SET needs key and value";
    //     g_dataStore->setValue(args[0], args[1]);
    //     return "OK";
    // });
    
    // riricommands.emplace("GET", [&dataStore](const std::vector<std::string>& args) { 
    //     if (args.size() != 1) return std::string("ERROR: GET needs key");
    //     return g_dataStore->getValue(args[0]);
    // });

    // In the above code, I was emplacing the command string and mapping it with lambda
    // functions. In this case, the dataStore was captured by references. I used lambda
    // functions as they were awfully convienent to play with and pass as std::functions
    // to other functions. I didn't consider the overhead of this appraoch at that time.

    riricommands.emplace("SET", setCommand);
    riricommands.emplace("GET", getCommand);
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

    auto it = riricommands.find(commandName);
    if (it == riricommands.end()) return "ERROR: Unknown command";
    
    return it->second(tokens);  // Call the function stored in the map
}