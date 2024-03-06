#!/bin/bash
#guofuhui

g++ example.cpp -lnhmdapi -lpthread  -L./api --std=c++11  -Wl,-rpath  "./api"  -o testnhmdapi
