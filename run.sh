#!/bin/bash
mpirun -np 5 ./WSim
#np must be a square number+1 e.g. 5,10,17
# 1 master + 2d square of nodes