#!/bin/bash
mpic++ -c -o node.o node/node.cpp -lX11
mpic++ -c -o master.o master/master.cpp -lX11
mpic++ -c -o main.o main.cpp -lX11
mpic++ -o WSim main.o node.o master.o -lX11
