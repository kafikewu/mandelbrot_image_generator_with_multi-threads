#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
struct parameters{
    char outfile[100];
	double xcenter;
	double ycenter;
	double scale;
	int    image_width;
	int    image_height;
	int    max;
    int    upper;
    int    lower;
};
void* diff_threads(struct parameters *param)
{
    struct parameters idk = *param;
    char outfile[100];
    strcpy(outfile, idk.outfile);
    double xcenter = idk.xcenter;
    double ycenter = idk.ycenter;
    double scale = idk.scale;
    int    image_width = idk.image_width;
    int    image_height = idk.image_height;
    int    max = idk.max;
    int    upper = idk.upper;
    int    lower = idk.lower;


    for(int i=lower; i<upper; i++)
    {
        char command[100] = "./mandel ";
        char name_of_file[20] = " -o ";
        strcat(name_of_file, outfile);
        char some[5];
        sprintf(some, "%d", i+1);
        strcat(name_of_file, some);
        strcat(name_of_file,".bmp");
        float scale1 = scale / (float)i;
        char X[20] = " -x ";
        char x_size[10];
        
        //sprintf(x_size, "%f", -1.4);
        sprintf(x_size, "%f", xcenter);
        strcat(X, x_size);
        char Y[20] = " -y ";
        char y_size[10];
        sprintf(y_size, "%f", ycenter);
        strcat(Y, y_size);
        char scale_char[10];
        char scale_of_file[20] = " -s ";
        sprintf(scale_char, "%f", scale1);
        char max_iter[20] = " -m ";
        char con_max[20];
        sprintf(con_max, "%d", max);

        char Width[20] = " -W ";
        char W_max[20];
        sprintf(W_max, "%d", image_width);
        strcat(Width, W_max);

        char Height[20] = " -H ";
        char H_max[20];
        sprintf(H_max, "%d", image_height);
        strcat(Height, H_max);

        strcat(max_iter, con_max);
        strcat(scale_of_file, scale_char);
        strcat(command, X);
        strcat(command, Y);
        strcat(command, scale_of_file);
        strcat(command, max_iter);
        strcat(command, Width);
        strcat(command, Height);
        strcat(command, name_of_file);
        system(command);
        //cout<<command<<endl;
        //printf("%s\n", command);
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

int main ( int argc, char *argv[]  ) 
{
    system("gcc -Wall -g -c mandel.c -o mandel.o && gcc mandel.o bitmap.o -o mandel -lpthread -lm && gcc -Wall -g -c bitmap.c -o bitmap.o ");
    //float SCALEX = ; 
    ///X = -1.5
    const char *outfile = "mandel";
	double xcenter = 0;
	double ycenter = 0;
	double scale = 4;
	int    image_width = 500;
	int    image_height = 500;
	int    max = 1000;
    int    numberot = 1;
    char c;
    while((c = getopt(argc,argv,"t:x:y:s:W:H:m:o:h"))!=-1) {
		switch(c) {
            case 't':
				numberot = atoi(optarg);
				break;
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
    int numember_of_images = 100;
    pthread_t _threads[numberot];
    struct parameters p[numberot];
    int range = numember_of_images/numberot;
    for(int i = 0; i < numberot; i++)
    {
        int lower = range * i;
        int upper = range * (i+1);
        p[i].xcenter = xcenter;
        p[i].ycenter = ycenter;
        p[i].scale = scale;
        p[i].image_width = image_width;
        p[i].image_height = image_height;
        p[i].max = max;
        p[i].upper = upper;
        p[i].lower = lower;
        strcpy(p[i].outfile, outfile);
        
        pthread_create(&_threads[i], NULL, diff_threads , &p[i]);
        
    }


    for(int i=0; i<numberot; i++)
    {
        pthread_join(_threads[i], NULL);
    }
    system("ffmpeg -f image2 -r 10 -i mandel%01d.bmp -c:v libx264 -pix_fmt yuv420p out.mp4");
    system("mplayer -vo caca out.mp4");
    return 0; 
} 
