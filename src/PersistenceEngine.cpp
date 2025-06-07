#include <fstream>
#include <string>
#include <sstream>
#include "DataStore.h"
#include "PersistenceEngine.h"

PersistenceEngine::PersistenceEngine(DataStore* dataStore) {
    this->dataStore = dataStore;
}

void PersistenceEngine::saveData() {
    std::ofstream outputFile("data/store.ridb");

    for (const auto& entry : dataStore->returnData()) {
        outputFile << entry.first << " " << entry.second << std::endl;
    }
    outputFile.close();
}

void PersistenceEngine::loadData() {
    std::ifstream inputFile("data/store.ridb");
    std::string line;
    while (std::getline(inputFile, line)) {
        std::string key, value;
        std::istringstream iss(line);
        iss >> key >> value;
        dataStore->setValue(key, value);
    }
    inputFile.close();
}