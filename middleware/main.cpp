#include "udpserver.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;

int main()
{
	vector<string> vector_ipv4;
	vector<uint16_t> vector_port;
	map<string, uint16_t> mm;
	// UDP Server
	UDPServer<> udpServer;
	udpServer.Bind(36888, [](int errorCode, string errorMessage) {
        cout << "Binding Failed:" << errorCode << " : " << errorMessage << endl;
    });
	// Welcome!
	udpServer.onMessageReceived = [&](string message, string ipv4, uint16_t port) {
		if(message == "CC268895-C515-19A7-F127-B3464777F7DF")
		{
			cout << ipv4 << ":" << port << endl;
			// mm.erase(ipv4);
			// mm.insert(pair<string, uint16_t>(ipv4, port));
			vector_ipv4.push_back(ipv4);
			vector_port.push_back(port);
			udpServer.SendTo("OK!", ipv4, port);
		}
		if(ipv4 == "1.193.38.118" || ipv4 == "210.22.136.66" || ipv4 == "101.230.112.6" || ipv4 == "112.65.181.210" || ipv4 == "47.98.117.222" || ipv4 == "157.122.224.92" || ipv4 == "121.33.194.138")
		{
			/*
			map<string, uint16_t>::iterator it;
			for(it = mm.begin(); it != mm.end(); it++)
			{
				udpServer.SendTo(message, it->first, it->second);
			}
			*/
			for(int i = 0; i < vector_ipv4.size(); i++)
			{
				int res = udpServer.SendTo(message, vector_ipv4.at(i), vector_port.at(i));
				if(res == -1)
				{// the client not exist
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
