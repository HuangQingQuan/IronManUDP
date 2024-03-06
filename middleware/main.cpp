#include "tcpserver.hpp"
#include "udpserver.hpp"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

vector<string> vector_ipv4;
vector<uint16_t> vector_port;
UDPServer<> udpServer;
TCPServer<> tcpServer;

int main()
{
	// UDP Server
	udpServer.Bind(36999, [](int errorCode, string errorMessage) {
        cout << "Binding Failed:" << errorCode << " : " << errorMessage << endl;
    });
	// Welcome!
	udpServer.onMessageReceived = [&](string message, string ipv4, uint16_t port) {
        cout << ipv4 << ":" << port << " => " << message << endl;
		if(message == "CC268895-C515-19A7-F127-B3464777F7DF")
		{
			vector_ipv4.push_back(ipv4);
			vector_port.push_back(port);
		}
    };
	// TCP Server
    tcpServer.onNewConnection = [&](TCPSocket<> *newClient) {
		std::string client_ip = newClient->remoteAddress();
        cout << "New client: [" << newClient->remoteAddress() << ":" << newClient->remotePort() << "]" << endl;
		if(client_ip != "127.0.0.1" || client_ip != "1.193.38.118" || client_ip != "157.122.224.92" || client_ip != "222.72.23.78")
		{
			// newClient->Close();
		}
        newClient->onMessageReceived = [newClient](string message) {
            // cout << newClient->remoteAddress() << ":" << newClient->remotePort() << " => " << message << endl;
			for(int i = 0; i < vector_ipv4.size(); i++)
			{
				udpServer.SendTo(message, vector_ipv4.at(i), vector_port.at(i));
			}
        };
        newClient->onSocketClosed = [newClient](int errorCode) {
            cout << "Socket closed:" << newClient->remoteAddress() << ":" << newClient->remotePort() << " -> " << errorCode << endl;
            cout << flush;
        };
    };
    tcpServer.Bind(36888, [](int errorCode, string errorMessage) {
        cout << "Binding Failed:" << errorCode << " : " << errorMessage << endl;
    });
    tcpServer.Listen([](int errorCode, string errorMessage) {
        cout << "Listening Failed:" << errorCode << " : " << errorMessage << endl;
    });

    // You should do an input loop, so the program won't terminate immediately
    string input;
    getline(cin, input);
    while (input != "exit")
    {
        getline(cin, input);
    }

    // Close the server before exiting the program.
    tcpServer.Close();
    udpServer.Close();

    return 0;
}
