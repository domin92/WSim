#include<iostream>
#include<cstdlib>
#include<unistd.h>
#include<mpi.h>
#include<cmath>
#include<X11/Xlib.h>

#include"node/node.hpp"

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
	
	int size = 100;
	
	if(myrank==0){ // Master
		
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
		
	}else{ // Node
		
		Node node(myrank, side_size, size);

		node.main();

	}

    // Shut down MPI
    MPI_Finalize();
}