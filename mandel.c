
#include "bitmap.h"

#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

int iteration_to_color( int i, int max );
int iterations_at_point( double x, double y, int max );
void compute_image( struct bitmap *bm, double xmin, double xmax, double ymin, double ymax, int max );

sem_t mut;


int NUMBER_OF_NODES;
typedef struct node {
    //DATA data;
    int i, j, iter;
    struct bitmap* bm;
    struct node* next;
} NODE;

struct node1 {
    //DATA data;
    int i, j, iter;
    struct bitmap* bm;
    struct node* next;
};

//struct node1 arr[2160][3840];
///IDK WHAT THIS IS
struct node1 **arr;
int HEIGHT=0, WIDTH=0;


NODE* head;
NODE* node;


struct Thread_Values
{
	struct bitmap *bm; 
	double xmin; 
	double xmax; 
	double ymin; 
	double ymax;
	int max;
	int width_start;
	int width_end;
	int height;
	int j;
	int width;
};

void init(NODE** head) {
    *head = NULL;
}

void print_list(NODE* head) {
    NODE * temp;
    for (temp = head; temp; temp = temp->next)
	{
		sem_wait(&mut);
		bitmap_set(temp->bm,temp->i,temp->j,temp->iter);
		sem_post(&mut);
	}
        //printf("%d %d %d ", temp->i, temp->j, temp->bm->i);
}

NODE* add(NODE* node, int x, int y, struct bitmap* SOME, int iter) {
    NODE* temp = (NODE*) malloc(sizeof (NODE));
    if (temp == NULL) {
        exit(0); // no memory available
    }
	NUMBER_OF_NODES++;
    temp->i = x;
    temp->j = y;
    temp->iter = iter;
    temp->bm = SOME;
    temp->next = node;
    node = temp;
    return node;
}

NODE *free_list(NODE *head) {
    NODE *tmpPtr = head;
    NODE *followPtr;
    while (tmpPtr != NULL) {
        followPtr = tmpPtr;
        tmpPtr = tmpPtr->next;
        free(followPtr);
    }
    return NULL;
}


void* _thread1(struct Thread_Values *para)
{
	struct Thread_Values param = *para;
	struct bitmap *bm = param.bm;
	double xmin = param.xmin;
	double xmax = param.xmax;
	double ymin = param.ymin;
	double ymax = param.ymax;
	int max = param.max;
	int width_start = param.width_start;
	int width_end = param.width_end;
	int height = param.height;
	int j = param.j;
	int width = param.width;
	
	for(int i = width_start ; i < width_end ; i++)
	{
		// Determine the point in x,y space for that pixel.
			double x = xmin + i*(xmax-xmin)/width;
			double y = ymin + j*(ymax-ymin)/height;

			// Compute the iterations at that point.
			int iters = iterations_at_point(x,y,max);

			// Set the pixel in the bitmap.
			arr[i][j].bm = bm;
			arr[i][j].i = i;
			arr[i][j].j = j;
			arr[i][j].iter = iters;
			//head = add(head, i, j, bm, iters);
			//bitmap_set(bm,i,j,iters);

			
	}
	return NULL;
	
}



void show_help()
{
	printf("Use: mandel [options]\n");
	printf("Where options are:\n");
	printf("-m <max>    The maximum number of iterations per point. (default=1000)\n");
	printf("-x <coord>  X coordinate of image center point. (default=0)\n");
	printf("-y <coord>  Y coordinate of image center point. (default=0)\n");
	printf("-s <scale>  Scale of the image in Mandlebrot coordinates. (default=4)\n");
	printf("-W <pixels> Width of the image in pixels. (default=500)\n");
	printf("-H <pixels> Height of the image in pixels. (default=500)\n");
	printf("-o <file>   Set output file. (default=mandel.bmp)\n");
	printf("-h          Show this help text.\n");
	printf("\nSome examples are:\n");
	printf("mandel -x -0.5 -y -0.5 -s 0.2\n");
	printf("mandel -x -.38 -y -.665 -s .05 -m 100\n");
	printf("mandel -x 0.286932 -y 0.014287 -s .0005 -m 1000\n\n");
}
void WHAT()
{
	for(int i=0; i<HEIGHT; i++)
	{
		for(int j=0; j<WIDTH; j++)
		{
			bitmap_set(arr[i][j].bm,arr[i][j].i,arr[i][j].j,arr[i][j].iter);
		}
	}
}

int main( int argc, char *argv[] )
{
	char c;
	init(&head);

	// These are the default configuration values used
	// if no command line arguments are given.
	sem_init(&mut, 0, 1);
	const char *outfile = "mandel.bmp";
	double xcenter = 0;
	double ycenter = 0;
	double scale = 4;
	int    image_width = 500;
	int    image_height = 500;
	int    max = 1000;

	// For each command line argument given,
	// override the appropriate configuration value.

	while((c = getopt(argc,argv,"x:y:s:W:H:m:o:h"))!=-1) {
		switch(c) {
			case 'x':
				xcenter = atof(optarg);
				break;
			case 'y':
				ycenter = atof(optarg);
				break;
			case 's':
				scale = atof(optarg);
				break;
			case 'W':
				image_width = atoi(optarg);
				break;
			case 'H':
				image_height = atoi(optarg);
				break;
			case 'm':
				max = atoi(optarg);
				break;
			case 'o':
				outfile = optarg;
				break;
			case 'h':
				show_help();
				exit(1);
				break;
		}
	}
	HEIGHT = image_height;
	WIDTH = image_width;
	arr = (struct node1 **)malloc(image_height * sizeof(struct node1 *)); 
    for (int i=0; i<image_height; i++) 
	{
		arr[i] = (struct node1 *)malloc(image_width * sizeof(struct node1)); 
	}

	// Display the configuration of the image.
	printf("mandel: x=%lf y=%lf scale=%lf max=%d outfile=%s\n",xcenter,ycenter,scale,max,outfile);

	// Create a bitmap of the appropriate size.
	struct bitmap *bm = bitmap_create(image_width,image_height);

	// Fill it with a dark blue, for debugging
	bitmap_reset(bm,MAKE_RGBA(0,0,255,0));

	// Compute the Mandelbrot image
	compute_image(bm,xcenter-scale,xcenter+scale,ycenter-scale,ycenter+scale,max);
	//print_list(head);
	WHAT();
	printf("%d", NUMBER_OF_NODES);
	// Save the image in the stated file.
	if(!bitmap_save(bm,outfile)) {
		fprintf(stderr,"mandel: couldn't write to %s: %s\n",outfile,strerror(errno));
		return 1;
	}
	free_list(head);
	return 0;
}

/*
Compute an entire Mandelbrot image, writing each point to the given bitmap.
Scale the image to the range (xmin-xmax,ymin-ymax), limiting iterations to "max"
*/

void compute_image( struct bitmap *bm, double xmin, double xmax, double ymin, double ymax, int max )
{
	int i,j;

	int width = bitmap_width(bm);
	int height = bitmap_height(bm);

	// For every pixel in the image...
		int range = width/4;

	struct Thread_Values values[4];
	
	for(int i=0; i<4; i++)
	{
		values[i].bm = bm;
		values[i].xmin = xmin; 
		values[i].xmax = xmax; 
		values[i].ymin = ymin; 
		values[i].ymax = ymax;
		values[i].max = max;
		values[i].height = height;
		values[i].width = width;
	}
	values[0].width_start = 0;
	values[0].width_end = range;

	values[1].width_start = range;
	values[1].width_end = range * 2;

	values[2].width_start = range * 2;
	values[2].width_end = range * 3;

	values[3].width_start = range * 3;
	values[3].width_end = width;

	for(j=0;j<height;j++) {
		
		
		pthread_t temp[4];
		for(i=0;i<4;i++) {
			
			values[i].j = j;
			//pthread_t temp;
			pthread_create(&temp[i], NULL, _thread1, &values[i]);

			/*
			// Determine the point in x,y space for that pixel.
			double x = xmin + i*(xmax-xmin)/width;
			double y = ymin + j*(ymax-ymin)/height;

			// Compute the iterations at that point.
			int iters = iterations_at_point(x,y,max);

			// Set the pixel in the bitmap.
			bitmap_set(bm,i,j,iters); */
		}
		for(int i=0; i< 4; i++)
		{
			pthread_join(temp[i], NULL);
		}
	}
}

/*
Return the number of iterations at point x, y
in the Mandelbrot space, up to a maximum of max.
*/

int iterations_at_point( double x, double y, int max )
{
	double x0 = x;
	double y0 = y;

	int iter = 0;

	while( (x*x + y*y <= 4) && iter < max ) {

		double xt = x*x - y*y + x0;
		double yt = 2*x*y + y0;

		x = xt;
		y = yt;

		iter++;
	}

	return iteration_to_color(iter,max);
}

/*
Convert a iteration number to an RGBA color.
Here, we just scale to gray with a maximum of imax.
Modify this function to make more interesting colors.
*/

int iteration_to_color( int i, int max )
{
	
	int gray = 255*i/max;
	return MAKE_RGBA((int)(i * sin(i))%256,(int)(i * cos(i))%256,(int)(i * cos(i))%256,0);
	return MAKE_RGBA((int)(i * sin(i))%256,(int)(i * sin(i))%256,(int)(i * sin(i))%256,0);
	return MAKE_RGBA((int)(i * sin(i))%256,(i*3 % 256),(i % 256),0);
	printf("%d %d\n", i, gray);
	if(gray >= 0 && gray <= 10)
	{
		return MAKE_RGBA(21,12,255,0);
	}
	if(gray > 10 && gray <= 20)
	{
		return MAKE_RGBA(22,132,154,0);
	}
	if(gray > 20 && gray <= 30)
	{
		return MAKE_RGBA(22,12,154,0);
	}
	if(gray > 30 && gray <= 40)
	{
		return MAKE_RGBA(221,122,154,0);
	}
	if(gray > 40 && gray <= 50)
	{
		return MAKE_RGBA(231,22,154,0);
	}
	if(gray > 50 && gray <= 60)
	{
		return MAKE_RGBA(141,11,98,0);
	}
	if(gray > 60 && gray <= 70)
	{
		return MAKE_RGBA(211,72,184,0);
	}
	if(gray > 70 && gray <= 80)
	{
		return MAKE_RGBA(87,221,32,0);
	}
	if(gray > 80 && gray <= 90)
	{
		return MAKE_RGBA(259,123,167,0);
	}
	if(gray > 90 && gray <= 100)
	{
		return MAKE_RGBA(223,198,124,0);
	}
	if(gray > 100 && gray <= 110)
	{
		return MAKE_RGBA(221,92,184,0);
	}
	if(gray > 110 && gray <= 120)
	{
		return MAKE_RGBA(242,57,15,0);
	}
	if(gray > 120 && gray <= 130)
	{
		return MAKE_RGBA(29,45,234,0);
	}
	if(gray > 130 && gray <= 140)
	{
		return MAKE_RGBA(98,47,124,0);
	}
	if(gray > 140 && gray <= 150)
	{
		return MAKE_RGBA(251,129,219,0);
	}
	if(gray > 150 && gray <= 160)
	{
		return MAKE_RGBA(228,19,53,0);
	}
	if(gray > 160 && gray <= 170)
	{
		return MAKE_RGBA(28,156,154,0);
	}
	if(gray > 170 && gray <= 180)
	{
		return MAKE_RGBA(210,129,187,0);
	}
	if(gray > 180 && gray <= 190)
	{
		return MAKE_RGBA(212,231,23,0);
	}
	if(gray > 190 && gray <= 200)
	{
		return MAKE_RGBA(198,98,65,0);
	}
	if(gray > 200 && gray <= 210)
	{
		return MAKE_RGBA(231,142,214,0);
	}
	if(gray > 210 && gray <= 220)
	{
		return MAKE_RGBA(211,177,122,0);
	}
	if(gray > 220 && gray <= 230)
	{
		return MAKE_RGBA(111,222,54,0);
	}
	if(gray > 230 && gray <= 240)
	{
		return MAKE_RGBA(222,112,43,0);
	}
	if(gray > 240 && gray <= 250)
	{
		return MAKE_RGBA(12,145,98,0);
	}
	if(gray > 250 && gray <= 260)
	{
		return MAKE_RGBA(0,255,54,0);
	}
	return MAKE_RGBA(gray,gray,gray,0);
}




