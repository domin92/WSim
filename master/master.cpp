#include<mpi.h>
#include<cstdlib>
#include<X11/Xlib.h>

#include"master.hpp"

Master::Master(int proc_count, int grid_size, int node_size){

	this->proc_count = proc_count;
	this->grid_size = grid_size;
	this->node_size = node_size;

	tab = new int*[proc_count-1];
	for(int i=0;i<proc_count-1;i++){
		tab[i] = new int[node_size*node_size];
	}

}

Master::~Master(){

	for(int i=0;i<proc_count-1;i++){
		delete[] tab[i];
	}
	delete[] tab;

}

void drawpixel(Display* di, Window wi, GC gc, int x, int y, int color)
{
	XSetForeground(di, gc, color);
	XDrawPoint(di, wi, gc, x, y);
}

void Master::receive_from_nodes(){

	MPI_Status status[proc_count-1];
	MPI_Request request[proc_count-1];

	for(int i=1;i<proc_count;i++){
		MPI_Irecv(tab[i-1], node_size*node_size,MPI_INT,i,1,MPI_COMM_WORLD,&request[i]);
	}

	for(int i=1;i<proc_count;i++){
		MPI_Wait(&request[i], &status[i]);
	}
	
}

void Master::main(){
	
	// X11
	Display *di = XOpenDisplay("");	
	int const x = 0, y = 0, width = 800, height = 800, border_width = 1;
	int sc    = DefaultScreen(di);
	Window ro = DefaultRootWindow(di);
	Window wi = XCreateSimpleWindow(di, ro, x, y, width, height, border_width, BlackPixel(di, sc), WhitePixel(di, sc));
	XMapRaised(di, wi); //Make window visible
	XStoreName(di, wi, "Game of Life"); // Set window title
	GC gc = XCreateGC(di, ro, 0, NULL); //Prepare the window for drawing
	//

	while (true) {
		
		receive_from_nodes();
		
		for(int i=0;i<grid_size;i++){
			for(int j=0;j<grid_size;j++){

				for (int y = 0; y < node_size; y++) {
					for (int x = 0; x < node_size; x++) {

						int idx = i*grid_size+j;

						if(tab[idx][y*node_size+x]==1){
							drawpixel(di, wi, gc, (i*node_size+y)*2, (j*node_size+x)*2, 0xff0000);
							drawpixel(di, wi, gc, (i*node_size+y)*2, (j*node_size+x)*2+1, 0xff0000);
							drawpixel(di, wi, gc, (i*node_size+y)*2+1, (j*node_size+x)*2, 0xff0000);
							drawpixel(di, wi, gc, (i*node_size+y)*2+1, (j*node_size+x)*2+1, 0xff0000);
						}else{
							drawpixel(di, wi, gc, (i*node_size+y)*2, (j*node_size+x)*2, 0x000000);
							drawpixel(di, wi, gc, (i*node_size+y)*2, (j*node_size+x)*2+1, 0x000000);
							drawpixel(di, wi, gc, (i*node_size+y)*2+1, (j*node_size+x)*2, 0x000000);
							drawpixel(di, wi, gc, (i*node_size+y)*2+1, (j*node_size+x)*2+1, 0x000000);
						}
					}
				}
			}
		}

	}
	
	// X11
	XFreeGC(di, gc);
	XDestroyWindow(di, wi);
	XCloseDisplay(di);
	//

}

/*void display(int** tab, int size, int side_size) { // Old display on console
	
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
}*/