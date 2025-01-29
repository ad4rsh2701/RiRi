#ifndef RIRI_SERVER_H
#define RIRI_SERVER_H

#include <string>
#include "DataStore.h"
#include "PersistenceEngine.h"
#include "NetworkManager.h"
#include "CommandParser.h"

class RiRiServer {
    private:
        NetworkManager* networkManager;
        DataStore* dataStore;
        PersistenceEngine* persistenceEngine;
        CommandParser* commandParser;
        std::string password;
        bool running;

    public:
        RiRiServer(const std::string& password = "");
        void startServer();
        void stopServer();
        void processRequest(const std::string& request, int clientSocket);
};

#endif // RIRI_SERVER_H