#include<mpi.h>
#include<cstdlib>
#include<X11/Xlib.h>

#include"master.hpp"

Master::Master(int proc_count, int grid_size, int node_size){

	this->proc_count = proc_count;
	this->grid_size = grid_size;
	this->node_size = node_size;

	all_tab = new char[(proc_count-1) * node_size * node_size * node_size];

	tab = new char*[proc_count-1];

	for(int i=0;i<proc_count-1;i++){
		tab[i] = all_tab + i * node_size * node_size * node_size;
	}

}

Master::~Master(){
	delete[] tab;
	delete[] all_tab;
}

void drawpixel(Display* di, Window wi, GC gc, int x, int y, int color){
	XSetForeground(di, gc, color);
	XDrawPoint(di, wi, gc, x, y);
}

void Master::receive_from_nodes(){
	MPI_Gather(NULL, 0, MPI_CHAR, all_tab - node_size * node_size * node_size, node_size * node_size * node_size, MPI_CHAR, 0, MPI_COMM_WORLD);
}

void Master::main(){
	
	// X11
	Display *di = XOpenDisplay("");	
	int const x = 0, y = 0, width = 800, height = 800, border_width = 1;
	int sc    = DefaultScreen(di);
	Window ro = DefaultRootWindow(di);
	Window wi = XCreateSimpleWindow(di, ro, x, y, width, height, border_width, BlackPixel(di, sc), WhitePixel(di, sc));
	XMapRaised(di, wi); //Make window visible
	XStoreName(di, wi, "WSim"); // Set window title
	GC gc = XCreateGC(di, ro, 0, NULL); //Prepare the window for drawing
	//

	while (true) {
		
		receive_from_nodes();
		
		for(int i=0;i<grid_size;i++){
			for(int j=0;j<grid_size;j++){
				for(int k=0;k<grid_size;k++){

					for (int y = 0; y < node_size; y++) {
						for (int x = 0; x < node_size; x++) {

							int idx = i * grid_size * grid_size + j * grid_size + k;

							float total_power = 0;
							for (int z = 0; z < node_size; z++) {
								int power = tab[idx][z * node_size * node_size + y * node_size + x];
								if(power>0){
									total_power+=1.0f / (float)node_size;
								}

							}

							drawpixel(di, wi, gc, (j*node_size+y)*2, (k*node_size+x)*2, 0x110000 * total_power);
							drawpixel(di, wi, gc, (j*node_size+y)*2, (k*node_size+x)*2+1, 0x110000 * total_power);
							drawpixel(di, wi, gc, (j*node_size+y)*2+1, (k*node_size+x)*2, 0x110000 * total_power);
							drawpixel(di, wi, gc, (j*node_size+y)*2+1, (k*node_size+x)*2+1, 0x110000 * total_power);
						
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
