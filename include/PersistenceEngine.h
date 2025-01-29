#ifndef PERSISTENCE_ENGINE_H
#define PERSISTENCE_ENGINE_H

#include <fstream>
#include <string>
#include <sstream>
#include "DataStore.h"

class PersistenceEngine {
private:
    DataStore* dataStore; // Pointer to the DataStore instance
public:
    PersistenceEngine(DataStore* dataStore); // Constructor
    void saveData(); // Save data to disk
    void loadData(); // Load data from disk
};

#endif // PERSISTENCE_ENGINE_H
