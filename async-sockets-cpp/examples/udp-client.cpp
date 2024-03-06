#include "udpsocket.hpp"
#include <iostream>

using namespace std;

int main()
{
    // Our constants:
    constexpr const char* IP = "47.98.117.222";
    constexpr uint16_t PORT = 36888;

    // Initialize socket.
    UDPSocket<100> udpSocket(true); // "true" to use Connection on UDP. Default is "false".
    udpSocket.Connect(IP, PORT);

    // Send String:
    udpSocket.Send("ABCDEFGH");
    //udpSocket.SendTo("ABCDEFGH", IP, PORT); // If you want to connectless

    udpSocket.onRawMessageReceived = [&](const char* message, int length, string ipv4, uint16_t port) {
        cout << ipv4 << ":" << port << " => " << message << "(" << length << ")" << endl;
    };
    
    // If you want to use std::string:
    /*
    udpSocket.onMessageReceived = [&](string message, string ipv4, uint16_t port) {
        cout << ipv4 << ":" << port << " => " << message << endl;
    };
    */

    // You should do an input loop, so the program won't terminate immediately
    string input;
    getline(cin, input);
    while (input != "exit")
    {
        udpSocket.Send(input);
        getline(cin, input);
    }

    // Close the socket.
    udpSocket.Close();

    return 0;
}
