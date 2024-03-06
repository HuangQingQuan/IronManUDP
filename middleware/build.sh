#!/bin/sh

g++ --std=c++11 main.cpp -I./async-sockets/include -I. -lpthread -L. -Wl,-rpath "."  -o Middleware -O3
