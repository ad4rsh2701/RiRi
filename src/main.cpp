#include "../include/PersistenceEngine.h"
#include <iostream>

int main(){
    printf("RiRi Server\n");

    DataStore *dataStore = new DataStore();

    PersistenceEngine engine = PersistenceEngine(dataStore);
    
    engine.loadData();
    
    auto data = dataStore->returnData();
    for (const auto& entry : data) {
        std::cout << entry.first << ": " << entry.second << std::endl;
    }
    engine.saveData();

    return 0;
}
