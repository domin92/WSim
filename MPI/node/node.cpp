#include<mpi.h>
#include<cstdlib>
#include"node.hpp"

Node::Node(int rank, int grid_size, int node_size){
	this->rank = rank;
	this->grid_size = grid_size;
	this->node_size = node_size;

	this->share_thickness = 1;

	sh_horizontal_size = node_size * share_thickness;
	sh_horizontal_L_in = new char[sh_horizontal_size];
	sh_horizontal_L_out = new char[sh_horizontal_size];
	sh_horizontal_R_in = new char[sh_horizontal_size];
	sh_horizontal_R_out = new char[sh_horizontal_size];

	sh_vertical_size = node_size * share_thickness;
	sh_vertical_U_in = new char[sh_vertical_size];
	sh_vertical_U_out = new char[sh_vertical_size];
	sh_vertical_D_in = new char[sh_vertical_size];
	sh_vertical_D_out = new char[sh_vertical_size];

	sh_corner_size = share_thickness * share_thickness;
	sh_corner_UL_in = new char[sh_corner_size];
	sh_corner_UL_out = new char[sh_corner_size];
	sh_corner_UR_in = new char[sh_corner_size];
	sh_corner_UR_out = new char[sh_corner_size];
	sh_corner_DL_in = new char[sh_corner_size];
	sh_corner_DL_out = new char[sh_corner_size];
	sh_corner_DR_in = new char[sh_corner_size];
	sh_corner_DR_out = new char[sh_corner_size];

	row = (rank-1)/grid_size;
	col = (rank-1)%grid_size;

	tab[0] = new char*[node_size+2];
	for(int i=0;i<node_size+2;i++){
		tab[0][i] = new char[node_size+2];
		for(int j=0;j<node_size+2;j++){
			tab[0][i][j]=0;
		}
	}

	tab[1] = new char*[node_size+2];
	for(int i=0;i<node_size+2;i++){
		tab[1][i] = new char[node_size+2];
		for(int j=0;j<node_size+2;j++){
			tab[1][i][j]=0;
		}
	}

	// Fill random pixels
	/*for(int i=1;i<node_size-1;i++){
		for(int j=1;j<node_size-1;j++){
			int r = rank*rand()%100;
			if(r>50){
				tab[0][i][j]=1;
			}
		}
	}*/

	for(int i=1;i<node_size+1;i++){
		for(int j=1;j<node_size+1;j++){
			if(i%2==j%2){
				tab[0][i][j]=1;
			}
		}
	}

	current_tab_idx = 0;

	output_send_tab = new char[node_size*node_size];

}

Node::~Node(){

	delete[] sh_horizontal_L_in;
	delete[] sh_horizontal_L_out;
	delete[] sh_horizontal_R_in;
	delete[] sh_horizontal_R_out;
	delete[] sh_vertical_U_in;
	delete[] sh_vertical_U_out;
	delete[] sh_vertical_D_in;
	delete[] sh_vertical_D_out;
	delete[] sh_corner_UL_in;
	delete[] sh_corner_UL_out;
	delete[] sh_corner_UR_in;
	delete[] sh_corner_UR_out;
	delete[] sh_corner_DL_in;
	delete[] sh_corner_DL_out;
	delete[] sh_corner_DR_in;
	delete[] sh_corner_DR_out;

	delete[] output_send_tab;

	for(int i=0;i<node_size+2*(share_thickness);i++){
		delete[] tab[0][i];
		delete[] tab[1][i];
	}

	delete[] tab[0];
	delete[] tab[1];
}

void Node::share_vertical(){

	if(row%2==0){
		if(row - 1 >= 0){
			MPI_Recv(sh_vertical_U_in, sh_vertical_size, MPI_CHAR, rank - grid_size, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	}else{
		if(row + 1 < grid_size){
			MPI_Send(sh_vertical_D_out, sh_vertical_size, MPI_CHAR, rank + grid_size, 1, MPI_COMM_WORLD);
		}
	}

	if(row%2==0){
		if(row + 1 < grid_size){
			MPI_Recv(sh_vertical_D_in, sh_vertical_size, MPI_CHAR, rank + grid_size, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	}else{
		if(row - 1 >= 0){
			MPI_Send(sh_vertical_U_out, sh_vertical_size, MPI_CHAR, rank - grid_size, 1, MPI_COMM_WORLD);
		}
	}
	
	if(row%2==1){
		if(row - 1 >= 0){
			MPI_Recv(sh_vertical_U_in, sh_vertical_size, MPI_CHAR, rank - grid_size, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	}else{
		if(row + 1 < grid_size){
			MPI_Send(sh_vertical_D_out, sh_vertical_size, MPI_CHAR, rank + grid_size, 1, MPI_COMM_WORLD);
		}
	}
	
	if(row%2==1){
		if(row + 1 < grid_size){
			MPI_Recv(sh_vertical_D_in, sh_vertical_size, MPI_CHAR, rank + grid_size, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	}else{
		if(row - 1 >= 0){
			MPI_Send(sh_vertical_U_out, sh_vertical_size, MPI_CHAR, rank - grid_size, 1, MPI_COMM_WORLD);
		}
	}

}

void Node::share_horizontal(){

	if(col%2==0){
		if(col - 1 >= 0){
			MPI_Recv(sh_horizontal_L_in, sh_horizontal_size, MPI_CHAR, rank - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	}else{
		if(col + 1 < grid_size){
			MPI_Send(sh_horizontal_R_out, sh_horizontal_size, MPI_CHAR, rank + 1, 1, MPI_COMM_WORLD);
		}
	}
	
	if(col%2==0){
		if(col + 1 < grid_size){
			MPI_Recv(sh_horizontal_R_in, sh_horizontal_size, MPI_CHAR, rank + 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	}else{
		if(col - 1 >= 0){
			MPI_Send(sh_horizontal_L_out, sh_horizontal_size, MPI_CHAR, rank - 1, 1, MPI_COMM_WORLD);
		}
	}
	
	if(col%2==1){
		if(col - 1 >= 0){
			MPI_Recv(sh_horizontal_L_in, sh_horizontal_size, MPI_CHAR, rank - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	}else{
		if(col + 1 < grid_size){
			MPI_Send(sh_horizontal_R_out, sh_horizontal_size, MPI_CHAR, rank + 1, 1, MPI_COMM_WORLD);
		}
	}
	
	if(col%2==1){
		if(col + 1 < grid_size){
			MPI_Recv(sh_horizontal_R_in, sh_horizontal_size, MPI_CHAR, rank + 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	}else{
		if(col - 1 >= 0){
			MPI_Send(sh_horizontal_L_out, sh_horizontal_size, MPI_CHAR, rank - 1, 1, MPI_COMM_WORLD);
		}
	}

}

void Node::share_depth(){
	// Nothing to share
}

void Node::share_corners(){

	if(row%2==0){ // Upper Left
		if(row - 1 >= 0 && col - 1 >= 0){
			MPI_Recv(sh_corner_UL_in, 1, MPI_CHAR, rank - grid_size - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	}else{
		if(row + 1 < grid_size && col + 1 < grid_size){
			MPI_Send(sh_corner_DR_out, 1, MPI_CHAR, rank + grid_size + 1, 1, MPI_COMM_WORLD);
		}
	}
	
	if(row%2==0){ // Upper Right
		if(row - 1 >= 0 && col + 1 < grid_size){
			MPI_Recv(sh_corner_UR_in, 1, MPI_CHAR, rank - grid_size + 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	}else{
		if(row + 1 < grid_size && col - 1 >= 0){
			MPI_Send(sh_corner_DL_out, 1, MPI_CHAR, rank + grid_size - 1, 1, MPI_COMM_WORLD);
		}
	}
	
	if(row%2==0){ // Bottom Left
		if(row + 1 < grid_size && col - 1 >= 0){
			MPI_Recv(sh_corner_DL_in, 1, MPI_CHAR, rank + grid_size - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	}else{
		if(row - 1 >= 0 && col + 1 < grid_size){
			MPI_Send(sh_corner_UR_out, 1, MPI_CHAR, rank - grid_size + 1, 1, MPI_COMM_WORLD);
		}
	}
	
	if(row%2==0){ // Bottom Right
		if(row + 1 < grid_size && col + 1 < grid_size){
			MPI_Recv(sh_corner_DR_in, 1, MPI_CHAR, rank + grid_size + 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	}else{
		if(row - 1 >= 0 && col - 1 >= 0){
			MPI_Send(sh_corner_UL_out, 1, MPI_CHAR, rank - grid_size - 1, 1, MPI_COMM_WORLD);
		}
	}

	if(row%2==1){ // Upper Left
		if(row - 1 >= 0 && col - 1 >= 0){
			MPI_Recv(sh_corner_UL_in, 1, MPI_CHAR, rank - grid_size - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	}else{
		if(row + 1 < grid_size && col + 1 < grid_size){
			MPI_Send(sh_corner_DR_out, 1, MPI_CHAR, rank + grid_size + 1, 1, MPI_COMM_WORLD);
		}
	}
	
	if(row%2==1){ // Upper Right
		if(row - 1 >= 0 && col + 1 < grid_size){
			MPI_Recv(sh_corner_UR_in, 1, MPI_CHAR, rank - grid_size + 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	}else{
		if(row + 1 < grid_size && col - 1 >= 0){
			MPI_Send(sh_corner_DL_out, 1, MPI_CHAR, rank + grid_size - 1, 1, MPI_COMM_WORLD);
		}
	}
	
	if(row%2==1){ // Bottom Left
		if(row + 1 < grid_size && col - 1 >= 0){
			MPI_Recv(sh_corner_DL_in, 1, MPI_CHAR, rank + grid_size - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	}else{
		if(row - 1 >= 0 && col + 1 < grid_size){
			MPI_Send(sh_corner_UR_out, 1, MPI_CHAR, rank - grid_size + 1, 1, MPI_COMM_WORLD);
		}
	}
	
	if(row%2==1){ // Bottom Right
		if(row + 1 < grid_size && col + 1 < grid_size){
			MPI_Recv(sh_corner_DR_in, 1, MPI_CHAR, rank + grid_size + 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	}else{
		if(row - 1 >= 0 && col - 1 >= 0){
			MPI_Send(sh_corner_UL_out, 1, MPI_CHAR, rank - grid_size - 1, 1, MPI_COMM_WORLD);
		}
	}
}

void Node::share(){
	share_vertical();
	share_horizontal();
	share_depth();
	share_corners();
}

void Node::pre_share_copy(){

	char** share_tab = tab[current_tab_idx];

	// Vertical
	if(row + 1 < grid_size){
		for(int i=0;i<node_size;i++){
			sh_vertical_D_out[i] = share_tab[node_size][i+1];
		}
	}

	if(row - 1 >= 0){
		for(int i=0;i<node_size;i++){
			sh_vertical_U_out[i] = share_tab[1][i+1];
		}
	}

	// Horizontal
	if(col + 1 < grid_size){
		for(int i=0;i<node_size;i++){
			sh_horizontal_R_out[i] = share_tab[i + 1][node_size];
		}
	}

	if(col - 1 >= 0){
		for(int i=0;i<node_size;i++){
			sh_horizontal_L_out[i] = share_tab[i + 1][1];
		}
	}

	// Corners
	if(row + 1 < grid_size && col + 1 < grid_size){
		sh_corner_DR_out[0] = share_tab[node_size][node_size];
	}

	if(row + 1 < grid_size && col - 1 >= 0){
		sh_corner_DL_out[0] = share_tab[node_size][1];
	}

	if(row - 1 >= 0 && col + 1 < grid_size){
		sh_corner_UR_out[0] = share_tab[1][node_size];
	}

	if(row - 1 >= 0 && col - 1 >= 0){
		sh_corner_UL_out[0] = share_tab[1][1];
	}

}

void Node::post_share_copy(){

	char** share_tab = tab[current_tab_idx];

	// Vertical
	if(row - 1 >= 0){
		for(int i=0;i<node_size;i++){
			share_tab[0][i+1] = sh_vertical_U_in[i];
		}
	}

	if(row + 1 < grid_size){
		for(int i=0;i<node_size;i++){
			share_tab[node_size + 1][i+1] = sh_vertical_D_in[i];
		}
	}

	// Horizontal
	if(col - 1 >= 0){
		for(int i=0;i<node_size;i++){
			share_tab[i + 1][0] = sh_horizontal_L_in[i];
		}
	}	

	if(col + 1 < grid_size){
		for(int i=0;i<node_size;i++){
			share_tab[i + 1][node_size + 1] = sh_horizontal_R_in[i];
		}
	}

	// Corners
	if(row - 1 >= 0 && col - 1 >= 0){
		share_tab[0][0] = sh_corner_UL_in[0];
	}

	if(row - 1 >= 0 && col + 1 < grid_size){
		share_tab[0][node_size + 1] = sh_corner_UR_in[0];
	}

	if(row + 1 < grid_size && col - 1 >= 0){
		share_tab[node_size + 1][0] = sh_corner_DL_in[0];
	}

	if(row + 1 < grid_size && col + 1 < grid_size){
		share_tab[node_size + 1][node_size + 1] = sh_corner_DR_in[0];
	}

}

void Node::iter() {

	char** input_tab = tab[current_tab_idx];
	char** output_tab = tab[current_tab_idx ^ 1];

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

			//output_tab[y][x] = input_tab[y][x];

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
	
	MPI_Gather(output_send_tab, node_size*node_size, MPI_CHAR, NULL, node_size*node_size, MPI_CHAR, 0, MPI_COMM_WORLD);
}

void Node::main(){
	while (true) {
		pre_share_copy();
		share();
		post_share_copy();
		iter();
		send_to_master();	
	}
}