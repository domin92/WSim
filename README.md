# WSim
Water simulator

## How to build WSim

Run 'build.sh' script.

## How to run WSimMPI

Use MS-MPI or MPICH
```
mpiexec.exe -n <number of processes> .\WSimMPI.exe <simulation size> // MS-MPI
mpiexec -np <number of processes> WSimMPI <simulation size> //MPICH
```
Where `<number of processes>` is a number of nodes + master process (n^3 + 1) e.g. 2,9 or 28.

`<simulation size>` is a size of an edge of the simulation cube e.g. 100 (60 by default).

## TO-DO

Now
- Fix gravity - weird things happen
- Fix walls on more than one node
- Fix camera initial lookAt
- Fix window - unable to close
- Level set physics
- Level set rendering

Later
- Render on nodes and send 2D images to master
- Master -> Node messages
- Load .obj files into simulation
