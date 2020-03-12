#!/bin/bash
mpic++ -c -o node.o node/node.cpp -lX11
mpic++ -c -o main.o main.cpp -lX11
mpic++ -o WSim main.o node.o -lX11
