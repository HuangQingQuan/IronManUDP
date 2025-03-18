#include "udpsocket.hpp"
#include <iostream>
#include <string>
#include <random>
#include <chrono>
#include <thread>

using namespace std;

// Function to generate a random floating-point number in a given range
double getRandomDouble(double min, double max) {
    static random_device rd;
    static mt19937 gen(rd());
    uniform_real_distribution<> dis(min, max);
    return dis(gen);
}

// Function to generate a random integer in a given range
int getRandomInt(int min, int max) {
    static random_device rd;
    static mt19937 gen(rd());
    uniform_int_distribution<> dis(min, max);
    return dis(gen);
}

// Function to generate the message in the specified format
string generateMessage() {
    string message;
    message += "zn2410|"; // instrumentId
    message += "EXCHANGE_ID|"; // exchangeId, you can change it to real exchange ID
    for (int i = 0; i < 5; ++i) {
        message += to_string(getRandomDouble(0.0, 1000.0)) + "|"; // lastPrice, openPrice, highestPrice, lowestPrice, upperLimitPrice
    }
    message += to_string(getRandomDouble(0.0, 1000.0)) + "|"; // lowerLimitPrice
    message += to_string(getRandomInt(0, 1000)) + "|"; // volume
    message += to_string(getRandomInt(0, 1000)) + "|"; // openInterest
    for (int i = 0; i < 5; ++i) {
        message += to_string(getRandomDouble(0.0, 1000.0)) + "|"; // bidPrice
        message += to_string(getRandomInt(0, 1000)) + "|"; // bidVolume
        message += to_string(getRandomDouble(0.0, 1000.0)) + "|"; // askPrice
        message += to_string(getRandomInt(0, 1000)) + "|"; // askVolume
    }
    message += "11:30:00|"; // updateTime
    message += to_string(getRandomInt(0, 1)); // updateMillisec
    message += "#"; // End of message
    return message;
}

int main() {
    // Our constants:
    constexpr const char* IP = "47.100.249.65";
    constexpr uint16_t PORT = 36888;

    // Initialize socket.
    UDPSocket<100> udpSocket(true); // "true" to use Connection on UDP. Default is "false".
    udpSocket.Connect(IP, PORT);

    // Send messages every 500 milliseconds
    cout << "[INFO] Starting to send messages every 500 milliseconds..." << endl;
    while (true) {
        string message = generateMessage();
        udpSocket.Send(message);
        // cout << "[INFO] Sent message: " << message << endl;

        // Wait for 500 milliseconds
        this_thread::sleep_for(chrono::milliseconds(500));
    }

    // Close the socket (this will never be reached in this loop)
    udpSocket.Close();

    return 0;
}    