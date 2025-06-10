#pragma once

#include <string>
class NetworkManager {

    private:
        std::string host;   // IP address (default: "127.0.0.1")
        int port;           // Port number
        int serverSocket;   // The server socket descriptor
    public:
        NetworkManager(const std::string& host = "127.0.0.1", int port = 6379); // Constructor
        void startListener();                  // Initialize socket and start listening
        int acceptClient();                    // Accept client connections
        std::string receiveRequest(int clientSocket); // Receive client requests
        void sendResponse(int clientSocket, const std::string& response); // Send response to client

};
// NETWORK_MANAGER_H