#include "udpserver.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <ctime>

using namespace std;

struct CClient {
	string ipv4;
	uint16_t port;
	std::chrono::time_point<std::chrono::system_clock> last_seen;
};

int main()
{
	vector<CClient> clients;
	std::chrono::seconds timeout(300);
	// UDP Server
	UDPServer<> udpServer;
	udpServer.Bind(36888, [](int errorCode, string errorMessage) {
        cout << "Binding Failed:" << errorCode << " : " << errorMessage << endl;
    });
	// Welcome!
	udpServer.onMessageReceived = [&](string message, string ipv4, uint16_t port) {
		auto now = std::chrono::system_clock::now();
		if(message == "CC268895-C515-19A7-F127-B3464777F7DD")
		{
			time_t tt = std::chrono::system_clock::to_time_t(now);
			// cout << ctime(&tt) << ipv4 << ":" << port << endl;
			// clear the dropped client
			for(auto it = clients.begin(); it != clients.end();)
			{
				if(now - it->last_seen > timeout)
				{
					it = clients.erase(it);
				}
				else
				{
					++it;
				}
			}
			// query the disconnected client
			bool found = false;
			for(auto& client:clients)
			{
				if(client.ipv4 == ipv4 && client.port == port)
				{
					client.last_seen = now;
					found = true;
					break;
				}
			}
			if(!found)
			{
				CClient client_ = {ipv4, port, now};
				clients.push_back(client_);
			}
		}
		if(ipv4 == "1.193.38.118" || ipv4 == "210.22.136.66" || ipv4 == "101.230.112.6" || ipv4 == "112.65.181.210" || ipv4 == "47.98.117.222" || ipv4 == "157.122.224.92" || ipv4 == "121.33.194.138")
		{
			for(auto& client:clients)
			{
				udpServer.SendTo(message, client.ipv4, client.port);
				// cout << client.ipv4 << ":" << client.port << endl;
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
