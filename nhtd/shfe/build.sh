#!/bin/sh

g++ --std=c++11 main.cpp -I./async-sockets/include -I. -lthostmduserapi_se -lpthread -L. -Wl,-rpath "."  -o Sender -O3
