#!/bin/sh

g++ --std=c++11 main.cpp -I./async-sockets/include -I. -lqdmdapi -lpthread -L. -Wl,-rpath "."  -o Sender -O3
