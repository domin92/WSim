#include<mpi.h>
#include<cstdlib>
#include"node.hpp"

Node::Node(int rank, int grid_size, int node_size){
	this->rank = rank;
	this->grid_size = grid_size;
	this->node_size = node_size;

	share_output_column = new int[node_size];
	share_input_column = new int[node_size];

	row = (rank-1)/grid_size;
	col = (rank-1)%grid_size;

	tab[0] = new int*[node_size+2];
	for(int i=0;i<node_size+2;i++){
		tab[0][i] = new int[node_size+2];
		for(int j=0;j<node_size+2;j++){
			tab[0][i][j]=0;
		}
	}

	tab[1] = new int*[node_size+2];
	for(int i=0;i<node_size+2;i++){
		tab[1][i] = new int[node_size+2];
		for(int j=0;j<node_size+2;j++){
			tab[1][i][j]=0;
		}
	}

	for(int i=1;i<node_size-1;i++){
		for(int j=1;j<node_size-1;j++){
			int r = rank*rand()%100;
			if(r>50){
				tab[0][i][j]=1;
			}
		}
	}

	current_tab_idx = 0;

	output_send_tab = new int[node_size*node_size];

}

Node::~Node(){

	delete[] share_output_column;
	delete[] share_input_column;
	delete[] output_send_tab;

	for(int i=0;i<node_size+2;i++){
		delete[] tab[0][i];
		delete[] tab[1][i];
	}

	delete[] tab[0];
	delete[] tab[1];
}

void Node::share(){
	
	int** share_tab = tab[current_tab_idx];
	
	MPI_Status status;
	
	// Horizontal
	if(row%2==0){
		if(row - 1 >= 0){
			MPI_Recv(share_tab[0] + 1, node_size, MPI_INT, rank - grid_size, 1, MPI_COMM_WORLD, &status);
		}
	}else{
		if(row + 1 < grid_size){
			MPI_Send(share_tab[node_size] + 1, node_size, MPI_INT, rank + grid_size, 1, MPI_COMM_WORLD);
		}
	}
	
	if(row%2==0){
		if(row + 1 < grid_size){
			MPI_Recv(share_tab[node_size + 1] + 1, node_size, MPI_INT, rank + grid_size, 1, MPI_COMM_WORLD, &status);
		}
	}else{
		if(row - 1 >= 0){
			MPI_Send(share_tab[1] + 1, node_size, MPI_INT, rank - grid_size, 1, MPI_COMM_WORLD);
		}
	}
	
	if(row%2==1){
		if(row - 1 >= 0){
			MPI_Recv(share_tab[0] + 1, node_size, MPI_INT, rank - grid_size, 1, MPI_COMM_WORLD, &status);
		}
	}else{
		if(row + 1 < grid_size){
			MPI_Send(share_tab[node_size] + 1, node_size, MPI_INT, rank + grid_size, 1, MPI_COMM_WORLD);
		}
	}
	
	if(row%2==1){
		if(row + 1 < grid_size){
			MPI_Recv(share_tab[node_size + 1] + 1, node_size, MPI_INT, rank + grid_size, 1, MPI_COMM_WORLD, &status);
		}
	}else{
		if(row - 1 >= 0){
			MPI_Send(share_tab[1] + 1, node_size, MPI_INT, rank - grid_size, 1, MPI_COMM_WORLD);
		}
	}
	
	// Vertical
	
	if(col%2==0){
		if(col - 1 >= 0){
			MPI_Recv(share_input_column, node_size, MPI_INT, rank - 1, 1, MPI_COMM_WORLD, &status);
			
			for(int i=0;i<node_size;i++){
				share_tab[i + 1][0] = share_input_column[i];
			}
		}
	}else{
		if(col + 1 < grid_size){
			for(int i=0;i<node_size;i++){
				share_output_column[i] = share_tab[i + 1][node_size];
			}
			MPI_Send(share_output_column, node_size, MPI_INT, rank + 1, 1, MPI_COMM_WORLD);
		}
	}
	
	if(col%2==0){
		if(col + 1 < grid_size){
			MPI_Recv(share_input_column, node_size, MPI_INT, rank + 1, 1, MPI_COMM_WORLD, &status);
			
			for(int i=0;i<node_size;i++){
				share_tab[i + 1][node_size + 1] = share_input_column[i];
			}
		}
	}else{
		if(col - 1 >= 0){
			for(int i=0;i<node_size;i++){
				share_output_column[i] = share_tab[i + 1][1];
			}
			
			MPI_Send(share_output_column, node_size, MPI_INT, rank - 1, 1, MPI_COMM_WORLD);
		}
	}
	
	if(col%2==1){
		if(col - 1 >= 0){
			MPI_Recv(share_input_column, node_size, MPI_INT, rank - 1, 1, MPI_COMM_WORLD, &status);
			
			for(int i=0;i<node_size;i++){
				share_tab[i + 1][0] = share_input_column[i];
			}
		}
	}else{
		if(col + 1 < grid_size){
			for(int i=0;i<node_size;i++){
				share_output_column[i] = share_tab[i + 1][node_size];
			}
			MPI_Send(share_output_column, node_size, MPI_INT, rank + 1, 1, MPI_COMM_WORLD);
		}
	}
	
	if(col%2==1){
		if(col + 1 < grid_size){
			MPI_Recv(share_input_column, node_size, MPI_INT, rank + 1, 1, MPI_COMM_WORLD, &status);
			
			for(int i=0;i<node_size;i++){
				share_tab[i + 1][node_size + 1] = share_input_column[i];
			}
		}
	}else{
		if(col - 1 >= 0){
			for(int i=0;i<node_size;i++){
				share_output_column[i] = share_tab[i + 1][1];
			}
			
			MPI_Send(share_output_column, node_size, MPI_INT, rank - 1, 1, MPI_COMM_WORLD);
		}
	}
	
	// Corners
	if(row%2==0){ // Top Left
		if(row - 1 >= 0 && col - 1 >= 0){
			MPI_Recv(&share_tab[0][0], 1, MPI_INT, rank - grid_size - 1, 1, MPI_COMM_WORLD, &status);
		}
	}else{
		if(row + 1 < grid_size && col + 1 < grid_size){
			MPI_Send(&share_tab[node_size][node_size], 1, MPI_INT, rank + grid_size + 1, 1, MPI_COMM_WORLD);
		}
	}
	
	if(row%2==0){ // Top Right
		if(row - 1 >= 0 && col + 1 < grid_size){
			MPI_Recv(&share_tab[0][node_size + 1], 1, MPI_INT, rank - grid_size + 1, 1, MPI_COMM_WORLD, &status);
		}
	}else{
		if(row + 1 < grid_size && col - 1 >= 0){
			MPI_Send(&share_tab[node_size][1], 1, MPI_INT, rank + grid_size - 1, 1, MPI_COMM_WORLD);
		}
	}
	
	if(row%2==0){ // Bottom Left
		if(row + 1 < grid_size && col - 1 >= 0){
			MPI_Recv(&share_tab[node_size + 1][0], 1, MPI_INT, rank + grid_size - 1, 1, MPI_COMM_WORLD, &status);
		}
	}else{
		if(row - 1 >= 0 && col + 1 < grid_size){
			MPI_Send(&share_tab[1][node_size], 1, MPI_INT, rank - grid_size + 1, 1, MPI_COMM_WORLD);
		}
	}
	
	if(row%2==0){ // Bottom Right
		if(row + 1 < grid_size && col + 1 < grid_size){
			MPI_Recv(&share_tab[node_size + 1][node_size + 1], 1, MPI_INT, rank + grid_size + 1, 1, MPI_COMM_WORLD, &status);
		}
	}else{
		if(row - 1 >= 0 && col - 1 >= 0){
			MPI_Send(&share_tab[1][1], 1, MPI_INT, rank - grid_size - 1, 1, MPI_COMM_WORLD);
		}
	}
	
	if(row%2==1){ // Top Left
		if(row - 1 >= 0 && col - 1 >= 0){
			MPI_Recv(&share_tab[0][0], 1, MPI_INT, rank - grid_size - 1, 1, MPI_COMM_WORLD, &status);
		}
	}else{
		if(row + 1 < grid_size && col + 1 < grid_size){
			MPI_Send(&share_tab[node_size][node_size], 1, MPI_INT, rank + grid_size + 1, 1, MPI_COMM_WORLD);
		}
	}
	
	if(row%2==1){ // Top Right
		if(row - 1 >= 0 && col + 1 < grid_size){
			MPI_Recv(&share_tab[0][node_size + 1], 1, MPI_INT, rank - grid_size + 1, 1, MPI_COMM_WORLD, &status);
		}
	}else{
		if(row + 1 < grid_size && col - 1 >= 0){
			MPI_Send(&share_tab[node_size][1], 1, MPI_INT, rank + grid_size - 1, 1, MPI_COMM_WORLD);
		}
	}
	
	if(row%2==1){ // Bottom Left
		if(row + 1 < grid_size && col - 1 >= 0){
			MPI_Recv(&share_tab[node_size + 1][0], 1, MPI_INT, rank + grid_size - 1, 1, MPI_COMM_WORLD, &status);
		}
	}else{
		if(row - 1 >= 0 && col + 1 < grid_size){
			MPI_Send(&share_tab[1][node_size], 1, MPI_INT, rank - grid_size + 1, 1, MPI_COMM_WORLD);
		}
	}
	
	if(row%2==1){ // Bottom Right
		if(row + 1 < grid_size && col + 1 < grid_size){
			MPI_Recv(&share_tab[node_size + 1][node_size + 1], 1, MPI_INT, rank + grid_size + 1, 1, MPI_COMM_WORLD, &status);
		}
	}else{
		if(row - 1 >= 0 && col - 1 >= 0){
			MPI_Send(&share_tab[1][1], 1, MPI_INT, rank - grid_size - 1, 1, MPI_COMM_WORLD);
		}
	}
	
}

void Node::iter() {

	int** input_tab = tab[current_tab_idx];
	int** output_tab = tab[current_tab_idx ^ 1];

	for (int y = 1; y < node_size+1; y++) {
		for (int x = 1; x < node_size+1; x++) {
			
			int val = 0;

			// sum 8 neighbours
			for (int i = -1; i <= 1; i++) {
				for (int j = -1; j <= 1; j++) {

					if (i==0 && j==0) {
						continue;
					}
					
					val += input_tab[y+i][x+j];
					
				}
			}

			// game of life rules
			if (input_tab[y][x] == 0) {
				if (val == 3) {
					output_tab[y][x] = 1;
				}
				else {
					output_tab[y][x] = 0;
				}
			}
			else {
				if (val == 2 || val == 3) {
					output_tab[y][x] = 1;
				}
				else {
					output_tab[y][x] = 0;
				}
			}
			
			
		}
	}

	current_tab_idx ^= 1;
}

void Node::send_to_master(){
	for(int y=0;y<node_size;y++){
		for(int x=0;x<node_size;x++){
			output_send_tab[y*node_size+x]=tab[current_tab_idx][y+1][x+1];
		}
	}
	
	MPI_Send(output_send_tab, node_size*node_size,MPI_INT,0,1,MPI_COMM_WORLD);
}

void Node::main(){
	while (true) {
		share();
		iter();
		send_to_master();	
	}
}