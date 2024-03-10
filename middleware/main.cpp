#include "udpserver.hpp"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main()
{
	vector<string> vector_ipv4;
	vector<uint16_t> vector_port;
	// UDP Server
	UDPServer<> udpServer;
	udpServer.Bind(36888, [](int errorCode, string errorMessage) {
        cout << "Binding Failed:" << errorCode << " : " << errorMessage << endl;
    });
	// Welcome!
	udpServer.onMessageReceived = [&](string message, string ipv4, uint16_t port) {
        // cout << ipv4 << ":" << port << " => " << message << endl;
		if(message == "CC268895-C515-19A7-F127-B3464777F7DF")
		{
			vector_ipv4.push_back(ipv4);
			vector_port.push_back(port);
		}
		if(ipv4 == "1.193.38.118" || ipv4 == "210.22.136.66" || ipv4 == "47.98.117.222")
		{
			for(int i = 0; i < vector_ipv4.size(); i++)
			{
				udpServer.SendTo(message, vector_ipv4.at(i), vector_port.at(i));
			}
		}
    };

    // You should do an input loop, so the program won't terminate immediately
    string input;
    getline(cin, input);
    while (input != "exit")
    {
        getline(cin, input);
    }
    // Close the server before exiting the program.
    udpServer.Close();

    return 0;
}
