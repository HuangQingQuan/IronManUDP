#!/bin/bash
#HuangQingQuan

g++ main.cpp -I./async-sockets/include -lnhmdapi -lpthread  -L./api --std=c++11  -Wl,-rpath  "./api"  -o Sender
