#include <fstream>
#include <string>
#include <sstream>
#include <DataStore.h>

class PersistenceEngine {
    private:
        DataStore* dataStore;
    public:
        PersistenceEngine(DataStore* dataStore) {
            this->dataStore = dataStore;
        }
    
        void saveData() {
            std::ofstream outputFile("data/store.ridb");

            for (const auto& entry : dataStore->returnData()) {
                outputFile << entry.first << " " << entry.second << std::endl;
            }
            outputFile.close();
        }

        void loadData() {
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
};