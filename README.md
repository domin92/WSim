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
Where '<number of processes>' is a cube number + 1 e.g. 2 or 9
'<simulation size>' is a size of a simulation cube e.g. 100 (60 by default).

## TO-DO

Now
- Fix gravity
- Fix walls on more than one node
- Level set physics
- Level set rendering
- Reduce share arrays size (4 Floats to 1 Float, 4 Floats to 4 Bytes?)

Later
- Render on nodes and send 2D images to master
- Master -> Node messages
- Load .obj files into simulation
