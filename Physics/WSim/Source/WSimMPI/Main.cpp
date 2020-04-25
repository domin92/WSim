#include<mpi.h>

#include "Node/Node.hpp"
#include "Master/Master.hpp"

using namespace std;

int my_cbrt(int a){
	if(a==1){
		return 1;
	}else if(a==8){
		return 2;
	}else if(a==27){
		return 3;
	}else if(a==64){
		return 4;
	}else if(a==125){
		return 5;
	}else{
		return 0;
	}
}

int main(int argc, char **argv) {
	
	int my_rank, proc_count;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &proc_count);
	
    int full_size = 60; // Size of the edge of the simulation cube
	int grid_size = my_cbrt(proc_count-1);
    int node_size = full_size / grid_size;
	
	if(my_rank==0){
		Master master(proc_count, grid_size, node_size);
		master.main();
	}else{ 
		Node node(my_rank, grid_size, node_size);
		node.main();
	}

    MPI_Finalize();
}