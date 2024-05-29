#include "udpserver.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

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
		if(message == "CC268895-C515-19A7-F127-B3464777F7DD")
		{
			bool found = false;
			for (auto i = 0; i < vector_ipv4.size(); ++i) {
				if (vector_ipv4[i] == ipv4 && vector_port[i] == port)
				{
					found = true;
				}
			}
			if(found)
			{
				return;
			}
			cout << "[client]" << ipv4 << ":" << port << endl;
			vector_ipv4.push_back(ipv4);
			vector_port.push_back(port);
			return;
		}
		if(message == "QUIT")
		{
			cout << "quit before:" << vector_ipv4.size() << endl;
			for(int i = 0; i < vector_ipv4.size(); i++)
			{
				if(vector_ipv4.at(i) == ipv4 && vector_port.at(i) == port)
				{
					cout << "quit ipv4:" << ipv4 << ":" << port << endl;
					vector_ipv4.erase(vector_ipv4.begin() + i);
					vector_port.erase(vector_port.begin() + i);
					break;
				}
			}
			cout << "quit after:" << vector_ipv4.size() << endl;
			cout << endl;
			return;
		}
		if(ipv4 == "1.193.38.118" || ipv4 == "210.22.136.66" || ipv4 == "101.230.112.6" || ipv4 == "112.65.181.210" || ipv4 == "47.98.117.222" || ipv4 == "157.122.224.92" || ipv4 == "121.33.194.138")
		{
			// cout << message << endl;
			for(int i = 0; i < vector_ipv4.size(); i++)
			{
				int res = udpServer.SendTo(message, vector_ipv4.at(i), vector_port.at(i));
				if(res == -1)
				{
					vector_ipv4.erase(vector_ipv4.begin() + i);
					vector_port.erase(vector_port.begin() + i);
				}
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
