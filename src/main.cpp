// TODO: EVERYTHING

#include <iostream>
#include <string>
#include "CommandParser.h"
#include "DataStore.h"
#include "PersistenceEngine.h"


int main() {
    DataStore store;
    PersistenceEngine engine(&store);
    CommandParser parser(&store);
    g_dataStore = &store; // Just for pointer access in command funcs

    engine.loadData();

    std::string input;

    std::cout << "RiRi CLI is now active. Type commands (type 'exit' to quit)\n";

    while (true) {
        std::cout << "RiRi> ";
        if (!std::getline(std::cin, input)) break;

        if (input == "exit" || input == "quit" || input == "bye") {
            std::cout << "Shutting down RiRi\n";
            break;
        }

        std::string output = parser.executeCommand(input);
        std::cout << output << "\n";
    }
    engine.saveData();

    return 0;
}
