#include "udpserver.hpp"
#include "tcpserver.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;
vector<TCPSocket<> *> clients;

int main()
{
	
	TCPServer<> tcpServer;
    // When a new client connected:
    tcpServer.onNewConnection = [&](TCPSocket<> *newClient) {
        cout << "New client: [";
        cout << newClient->remoteAddress() << ":" << newClient->remotePort() << "]" << endl;

        newClient->onMessageReceived = [newClient](string message) {
            cout << newClient->remoteAddress() << ":" << newClient->remotePort() << " => " << message << endl;
            // newClient->Send("OK!");
			if(message == "CC268895-C515-19A7-F127-B3464777F7DD")
			{
				clients.emplace_back(newClient);
			}
        };
		
        newClient->onSocketClosed = [newClient](int errorCode) {
			int k = -1;
			cout << clients.size() << endl;
			for(int i=0; i< clients.size(); i++)
			{
				if(clients[i]->remoteAddress() == newClient->remoteAddress() && clients[i]->remotePort() == newClient->remotePort())
				{
					k = i;
					break;
				}
			}
			cout << "k: " << k << endl;
			if(k != -1)
			{
				auto it = clients.begin();
				clients.erase(it + k);
			}
			cout << clients.size() << endl;
            cout << "Socket closed:" << newClient->remoteAddress() << ":" << newClient->remotePort() << " -> " << errorCode << endl;
            cout << flush;
        };
    };

    // Bind the server to a port.
    tcpServer.Bind(36999, [](int errorCode, string errorMessage) {
        // BINDING FAILED:
        cout << errorCode << " : " << errorMessage << endl;
    });

    // Start Listening the server.
    tcpServer.Listen([](int errorCode, string errorMessage) {
        // LISTENING FAILED:
        cout << errorCode << " : " << errorMessage << endl;
    });
	
	// UDP Server
	UDPServer<> udpServer;
	udpServer.Bind(36888, [](int errorCode, string errorMessage) {
        cout << "Binding Failed:" << errorCode << " : " << errorMessage << endl;
    });
	// Welcome!
	udpServer.onMessageReceived = [&](string message, string ipv4, uint16_t port) {
		if(ipv4 == "1.193.38.118" || ipv4 == "210.22.136.66" || ipv4 == "101.230.112.6" || ipv4 == "112.65.181.210" || ipv4 == "47.98.117.222" || ipv4 == "157.122.224.92" || ipv4 == "121.33.194.138")
		{
			for(int i=0; i< clients.size(); i++)
			{
				clients[i]->Send(message);
			}
		}
    };

    // You should do an input loop, so the program won't terminate immediately
	/*
    string input;
    getline(cin, input);
    while (input != "exit")
    {
        getline(cin, input);
    }
    // Close the server before exiting the program.
    udpServer.Close();
	*/
	pause();
    return 0;
}
