#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <string>
#include <unordered_map>
#include <functional>
#include "DataStore.h"

class CommandParser {
private:
    DataStore* dataStore; // Pointer to the DataStore instance
    std::unordered_map<std::string, std::function<std::string(const std::vector<std::string>&)>> riricommands; // Command map
public:
    CommandParser(DataStore* ds); // Constructor
    std::vector<std::string> parseCommand(const std::string& command); // Parse and validate commands
    std::string executeCommand(const std::string& command); // Execute commands on the DataStore
};

#endif // COMMAND_PARSER_H
