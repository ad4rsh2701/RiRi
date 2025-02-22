#include "../include/CommandParser.h"
#include <sstream>

CommandParser::CommandParser(DataStore* dataStore) {
    this->dataStore = dataStore;

    riricommands.emplace("SET", [dataStore](const std::vector<std::string>& args) { 
        if (args.size() != 2) return "ERROR: SET needs key and value";
        dataStore->setValue(args[0], args[1]);
        return "OK";
    });
    
    riricommands.emplace("GET", [dataStore](const std::vector<std::string>& args) { 
        if (args.size() != 1) return std::string("ERROR: GET needs key");
        return dataStore->getValue(args[0]);
    });
}

std::vector<std::string> CommandParser::parseCommand(const std::string& command) {
    std::vector<std::string> tokens;
    size_t pos = 0;

    while (pos < command.size()) {
        size_t spacePos = command.find(' ', pos);
        if (spacePos == std::string::npos) {
            spacePos = command.size();
        }

        tokens.push_back(command.substr(pos, spacePos - pos));
        pos = spacePos + 1;
    }

    return tokens;
}


std::string CommandParser::executeCommand(const std::string& command) {
    std::vector<std::string> tokens = parseCommand(command);
    if (tokens.empty()) return "ERROR: No command entered";

    std::string commandName = tokens[0];
    tokens.erase(tokens.begin());  // Remove command name from args

    auto it = riricommands.find(commandName);
    if (it == riricommands.end()) return "ERROR: Unknown command";

    return it->second(tokens);  // Call the function stored in the map
}