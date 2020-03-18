#include<mpi.h>

#include"node/node.hpp"
#include"master/master.hpp"

using namespace std;

int my_sqrt(int a){
	if(a==1){
		return 1;
	}else if(a==4){
		return 2;
	}else if(a==9){
		return 3;
	}else if(a==16){
		return 4;
	}else if(a==25){
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
	
	int grid_size = my_sqrt(proc_count-1);
	int node_size = 400 / grid_size;
	
	if(my_rank==0){
		Master master(proc_count, grid_size, node_size);
		master.main();
	}else{ 
		Node node(my_rank, grid_size, node_size);
		node.main();
	}

    MPI_Finalize();
}