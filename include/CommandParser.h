#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <string>
#include "DataStore.h"

class CommandParser {
private:
    DataStore* dataStore; // Pointer to the DataStore instance
public:
    CommandParser(DataStore* ds); // Constructor
    std::string parseCommand(const std::string& command); // Parse and validate commands
    std::string executeCommand(const std::string& command); // Execute commands on the DataStore
};

#endif // COMMAND_PARSER_H