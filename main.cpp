#include<iostream>
#include<cstdlib>
#include<unistd.h>
#include<mpi.h>
#include<cmath>
#include<X11/Xlib.h>

using namespace std;

int my_sqrt(int a){
	if(a==4){
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

void share(int** tab, int size /* width/height of the group */, int grid_size /* width/height of the grid */, int rank){
	
	int row = (rank-1)/grid_size;
	int col = (rank-1)%grid_size;
	
	MPI_Status status;
	
	// Horizontal
	if(row%2==0){
		if(row - 1 >= 0){
			MPI_Recv(tab[0] + 1, size, MPI_INT, rank - grid_size, 1, MPI_COMM_WORLD, &status);
		}
	}else{
		if(row + 1 < grid_size){
			MPI_Send(tab[size] + 1, size, MPI_INT, rank + grid_size, 1, MPI_COMM_WORLD);
		}
	}
	
	if(row%2==0){
		if(row + 1 < grid_size){
			MPI_Recv(tab[size + 1] + 1, size, MPI_INT, rank + grid_size, 1, MPI_COMM_WORLD, &status);
		}
	}else{
		if(row - 1 >= 0){
			MPI_Send(tab[1] + 1, size, MPI_INT, rank - grid_size, 1, MPI_COMM_WORLD);
		}
	}
	
	if(row%2==1){
		if(row - 1 >= 0){
			MPI_Recv(tab[0] + 1, size, MPI_INT, rank - grid_size, 1, MPI_COMM_WORLD, &status);
		}
	}else{
		if(row + 1 < grid_size){
			MPI_Send(tab[size] + 1, size, MPI_INT, rank + grid_size, 1, MPI_COMM_WORLD);
		}
	}
	
	if(row%2==1){
		if(row + 1 < grid_size){
			MPI_Recv(tab[size + 1] + 1, size, MPI_INT, rank + grid_size, 1, MPI_COMM_WORLD, &status);
		}
	}else{
		if(row - 1 >= 0){
			MPI_Send(tab[1] + 1, size, MPI_INT, rank - grid_size, 1, MPI_COMM_WORLD);
		}
	}
	
	// Vertical
	int *output_column = new int[size];
	int *input_column = new int[size];
	
	if(col%2==0){
		if(col - 1 >= 0){
			MPI_Recv(input_column, size, MPI_INT, rank - 1, 1, MPI_COMM_WORLD, &status);
			
			for(int i=0;i<size;i++){
				tab[i + 1][0] = input_column[i];
			}
		}
	}else{
		if(col + 1 < grid_size){
			for(int i=0;i<size;i++){
				output_column[i] = tab[i + 1][size];
			}
			MPI_Send(output_column, size, MPI_INT, rank + 1, 1, MPI_COMM_WORLD);
		}
	}
	
	if(col%2==0){
		if(col + 1 < grid_size){
			MPI_Recv(input_column, size, MPI_INT, rank + 1, 1, MPI_COMM_WORLD, &status);
			
			for(int i=0;i<size;i++){
				tab[i + 1][size + 1] = input_column[i];
			}
		}
	}else{
		if(col - 1 >= 0){
			for(int i=0;i<size;i++){
				output_column[i] = tab[i + 1][1];
			}
			
			MPI_Send(output_column, size, MPI_INT, rank - 1, 1, MPI_COMM_WORLD);
		}
	}
	
	if(col%2==1){
		if(col - 1 >= 0){
			MPI_Recv(input_column, size, MPI_INT, rank - 1, 1, MPI_COMM_WORLD, &status);
			
			for(int i=0;i<size;i++){
				tab[i + 1][0] = input_column[i];
			}
		}
	}else{
		if(col + 1 < grid_size){
			for(int i=0;i<size;i++){
				output_column[i] = tab[i + 1][size];
			}
			MPI_Send(output_column, size, MPI_INT, rank + 1, 1, MPI_COMM_WORLD);
		}
	}
	
	if(col%2==1){
		if(col + 1 < grid_size){
			MPI_Recv(input_column, size, MPI_INT, rank + 1, 1, MPI_COMM_WORLD, &status);
			
			for(int i=0;i<size;i++){
				tab[i + 1][size + 1] = input_column[i];
			}
		}
	}else{
		if(col - 1 >= 0){
			for(int i=0;i<size;i++){
				output_column[i] = tab[i + 1][1];
			}
			
			MPI_Send(output_column, size, MPI_INT, rank - 1, 1, MPI_COMM_WORLD);
		}
	}
	
	delete output_column;
	delete input_column;
	
	// Corners
	if(row%2==0){ // Top Left
		if(row - 1 >= 0 && col - 1 >= 0){
			MPI_Recv(&tab[0][0], 1, MPI_INT, rank - grid_size - 1, 1, MPI_COMM_WORLD, &status);
		}
	}else{
		if(row + 1 < grid_size && col + 1 < grid_size){
			MPI_Send(&tab[size][size], 1, MPI_INT, rank + grid_size + 1, 1, MPI_COMM_WORLD);
		}
	}
	
	if(row%2==0){ // Top Right
		if(row - 1 >= 0 && col + 1 < grid_size){
			MPI_Recv(&tab[0][size + 1], 1, MPI_INT, rank - grid_size + 1, 1, MPI_COMM_WORLD, &status);
		}
	}else{
		if(row + 1 < grid_size && col - 1 >= 0){
			MPI_Send(&tab[size][1], 1, MPI_INT, rank + grid_size - 1, 1, MPI_COMM_WORLD);
		}
	}
	
	if(row%2==0){ // Bottom Left
		if(row + 1 < grid_size && col - 1 >= 0){
			MPI_Recv(&tab[size + 1][0], 1, MPI_INT, rank + grid_size - 1, 1, MPI_COMM_WORLD, &status);
		}
	}else{
		if(row - 1 >= 0 && col + 1 < grid_size){
			MPI_Send(&tab[1][size], 1, MPI_INT, rank - grid_size + 1, 1, MPI_COMM_WORLD);
		}
	}
	
	if(row%2==0){ // Bottom Right
		if(row + 1 < grid_size && col + 1 < grid_size){
			MPI_Recv(&tab[size + 1][size + 1], 1, MPI_INT, rank + grid_size + 1, 1, MPI_COMM_WORLD, &status);
		}
	}else{
		if(row - 1 >= 0 && col - 1 >= 0){
			MPI_Send(&tab[1][1], 1, MPI_INT, rank - grid_size - 1, 1, MPI_COMM_WORLD);
		}
	}
	
	if(row%2==1){ // Top Left
		if(row - 1 >= 0 && col - 1 >= 0){
			MPI_Recv(&tab[0][0], 1, MPI_INT, rank - grid_size - 1, 1, MPI_COMM_WORLD, &status);
		}
	}else{
		if(row + 1 < grid_size && col + 1 < grid_size){
			MPI_Send(&tab[size][size], 1, MPI_INT, rank + grid_size + 1, 1, MPI_COMM_WORLD);
		}
	}
	
	if(row%2==1){ // Top Right
		if(row - 1 >= 0 && col + 1 < grid_size){
			MPI_Recv(&tab[0][size + 1], 1, MPI_INT, rank - grid_size + 1, 1, MPI_COMM_WORLD, &status);
		}
	}else{
		if(row + 1 < grid_size && col - 1 >= 0){
			MPI_Send(&tab[size][1], 1, MPI_INT, rank + grid_size - 1, 1, MPI_COMM_WORLD);
		}
	}
	
	if(row%2==1){ // Bottom Left
		if(row + 1 < grid_size && col - 1 >= 0){
			MPI_Recv(&tab[size + 1][0], 1, MPI_INT, rank + grid_size - 1, 1, MPI_COMM_WORLD, &status);
		}
	}else{
		if(row - 1 >= 0 && col + 1 < grid_size){
			MPI_Send(&tab[1][size], 1, MPI_INT, rank - grid_size + 1, 1, MPI_COMM_WORLD);
		}
	}
	
	if(row%2==1){ // Bottom Right
		if(row + 1 < grid_size && col + 1 < grid_size){
			MPI_Recv(&tab[size + 1][size + 1], 1, MPI_INT, rank + grid_size + 1, 1, MPI_COMM_WORLD, &status);
		}
	}else{
		if(row - 1 >= 0 && col - 1 >= 0){
			MPI_Send(&tab[1][1], 1, MPI_INT, rank - grid_size - 1, 1, MPI_COMM_WORLD);
		}
	}
	
}

void iter(int ***tab, int size, int &current_tab_idx) {

	int** input_tab = tab[current_tab_idx];
	int** output_tab = tab[current_tab_idx ^ 1];

	for (int y = 1; y < size+1; y++) {
		for (int x = 1; x < size+1; x++) {
			
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

void display(int** tab, int size, int side_size) { // Old display on console
	
	for(int i=0;i<side_size;i++){
		for(int j=0;j<side_size;j++){
			
			for (int y = 0; y < size; y++) {
				for (int x = 0; x < size; x++) {

					int idx = i*side_size+j;
					
					printf("\033[%d;%dH",(i*size+y)+1,(j*size+x)+1);// Move cursor
					
					if(tab[idx][y*size+x]==1){
						cout << idx;
					}else{
						cout << " ";
					}
				}
			}
			
		}
	}
}

void drawpixel(Display* di, Window wi, GC gc, int x, int y, int color)
{
	XSetForeground(di, gc, color);
	XDrawPoint(di, wi, gc, x, y);
}

int main(int argc, char **argv) {
	
	int myrank, proccount;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &proccount);
	
	int side_size=my_sqrt(proccount-1);
	
	int size = 200;
	
	if(myrank==0){ //Display
		
		int** tab = new int*[proccount-1];
		for(int i=0;i<proccount-1;i++){
			tab[i] = new int[size*size];
		}
		
		
		Display *di = XOpenDisplay("");
		
		int const x = 0, y = 0, width = 800, height = 800, border_width = 1;
		int sc    = DefaultScreen(di);
		Window ro = DefaultRootWindow(di);
		Window wi = XCreateSimpleWindow(di, ro, x, y, width, height, border_width, BlackPixel(di, sc), WhitePixel(di, sc));
		
		XMapRaised(di, wi); //Make window visible
		XStoreName(di, wi, "Game of Life"); // Set window title

		//Prepare the window for drawing
		GC gc = XCreateGC(di, ro, 0, NULL);
		
		while (true) {
			
			for(int i=1;i<proccount;i++){
				MPI_Status status;
				MPI_Recv(tab[i-1], size*size,MPI_INT,i,1,MPI_COMM_WORLD,&status);
			}
			
			for(int i=0;i<side_size;i++){
				for(int j=0;j<side_size;j++){

					for (int y = 0; y < size; y++) {
						for (int x = 0; x < size; x++) {

							int idx = i*side_size+j;

							if(tab[idx][y*size+x]==1){
								drawpixel(di, wi, gc, (i*size+y)*2, (j*size+x)*2, 0xff0000);
								drawpixel(di, wi, gc, (i*size+y)*2, (j*size+x)*2+1, 0xff0000);
								drawpixel(di, wi, gc, (i*size+y)*2+1, (j*size+x)*2, 0xff0000);
								drawpixel(di, wi, gc, (i*size+y)*2+1, (j*size+x)*2+1, 0xff0000);
							}else{
								drawpixel(di, wi, gc, (i*size+y)*2, (j*size+x)*2, 0x000000);
								drawpixel(di, wi, gc, (i*size+y)*2, (j*size+x)*2+1, 0x000000);
								drawpixel(di, wi, gc, (i*size+y)*2+1, (j*size+x)*2, 0x000000);
								drawpixel(di, wi, gc, (i*size+y)*2+1, (j*size+x)*2+1, 0x000000);
							}
						}
					}
				}
			}
		}
		
		XFreeGC(di, gc);
		XDestroyWindow(di, wi);
		XCloseDisplay(di);
		
	}else{ //Worker
		
		int*** tab;
		tab = new int**[2];

		tab[0] = new int*[size+2];
		for(int i=0;i<size+2;i++){
			tab[0][i] = new int[size+2];
			for(int j=0;j<size+2;j++){
				tab[0][i][j]=0;
			}
		}

		tab[1] = new int*[size+2];
		for(int i=0;i<size+2;i++){
			tab[1][i] = new int[size+2];
			for(int j=0;j<size+2;j++){
				tab[1][i][j]=0;
			}
		}

		for(int i=1;i<size-1;i++){
			for(int j=1;j<size-1;j++){
				int r = myrank*rand()%100;
				if(r>50){
					tab[0][i][j]=1;
				}
			}
		}
		
		int *output_send_tab = new int[size*size];
		
		int current_tab_idx = 0;
		
		while (true) {
			share(tab[current_tab_idx], size, side_size, myrank);
			
			iter(tab, size, current_tab_idx);
			
			for(int y=0;y<size;y++){
				for(int x=0;x<size;x++){
					output_send_tab[y*size+x]=tab[current_tab_idx][y+1][x+1];
				}
			}
			
			MPI_Send(output_send_tab, size*size,MPI_INT,0,1,MPI_COMM_WORLD);
			
		}
	}

    // Shut down MPI
    MPI_Finalize();
}