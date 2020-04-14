#!/bin/bash
mpirun -np 9 ./WSim
#np must be a cube number+1 e.g. 2,9,28
# 1 master + cube of nodes