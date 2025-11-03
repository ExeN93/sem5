/* 
c program:
--------------------------------
1. draws Mandelbrot set for Fc(z)=z*z +c
using Mandelbrot algorithm ( boolean escape time )
-------------------------------         
2. technique of creating ppm file is  based on the code of Claudio Rocchini
http://en.wikipedia.org/wiki/Image:Color_complex_plot.jpg
create 24 bit color graphic file ,  portable pixmap file = PPM 
see http://en.wikipedia.org/wiki/Portable_pixmap
to see the file use external application ( graphic viewer)
*/
#include <stdio.h>
#include <math.h>
#include <thread>
#include <chrono>
#include <iostream>

static const int num_threads = 8;

const int iXmax = 10240; 
const int iYmax = 10240;
const double CxMin=-2.5;
const double CxMax=1.5;
const double CyMin=-2.0;
const double CyMax=2.0;
const int IterationMax=200;
const int MaxColorComponentValue=255;

unsigned char ppm[iYmax][iXmax][3];
/* bail-out value , radius of circle ;  */
const double EscapeRadius=2;

 void call_from_thread(int tid) {
    int i, j, k, lb, ub;
 
    lb = (iYmax/num_threads) * tid; // N ma byc podzielne przez liczbe watkow
    ub = lb + (iYmax/num_threads) - 1;

		int iX,iY;
        
		/* world ( double) coordinate = parameter plane*/
		double Cx,Cy;
		
		/* */
		double PixelWidth=(CxMax-CxMin)/iXmax;
		double PixelHeight=(CyMax-CyMin)/iYmax;
		/* Z=Zx+Zy*i  ;   Z0 = 0 */
		double Zx, Zy;
		double Zx2, Zy2; /* Zx2=Zx*Zx;  Zy2=Zy*Zy  */
		/*  */
		int Iteration;

		double ER2=EscapeRadius*EscapeRadius;
 
    for(iY=lb; iY<=ub; iY++){
        Cy=CyMin + iY*PixelHeight;
				if (fabs(Cy)< PixelHeight/2) Cy=0.0; /* Main antenna */
				for(iX=0;iX<iXmax;iX++){         
						Cx=CxMin + iX*PixelWidth;
						/* initial value of orbit = critical point Z= 0 */
						Zx=0.0;
						Zy=0.0;
						Zx2=Zx*Zx;
						Zy2=Zy*Zy;
						/* */
						for (Iteration=0;Iteration<IterationMax && ((Zx2+Zy2)<ER2);Iteration++)
						{
								Zy=2*Zx*Zy + Cy;
								Zx=Zx2-Zy2 +Cx;
								Zx2=Zx*Zx;
								Zy2=Zy*Zy;
						};
						/* compute  pixel color (24 bit = 3 bytes) */
						if (Iteration==IterationMax)
						{ /*  interior of Mandelbrot set = black */
								ppm[iY][iX][0] = 0;
                ppm[iY][iX][1] = 0;
                ppm[iY][iX][2] = 0;                         
						}
					else 
						{ /* exterior of Mandelbrot set = white */
									ppm[iY][iX][0]=255; /* Red*/
									ppm[iY][iX][1]=255;  /* Green */ 
									ppm[iY][iX][2]=255;/* Blue */
						};
						/*write color to the file*/
				}
    }
}

 int main()
 {
			/* screen ( integer) coordinate */
		
		/* color component ( R or G or B) is coded from 0 to 255 */
		/* it is 24 bit color RGB file */
		const auto start(std::chrono::steady_clock::now()); 
		FILE * fp;
		char *filename="new2.ppm";
		char *comment="# ";/* comment should start with # */
		std::thread t[num_threads];
		for (int i = 0; i < num_threads; ++i) {
			t[i] = std::thread(call_from_thread, i);
		}
		for (int i = 0; i < num_threads; ++i) {
			t[i].join();
		}
		
		/*create new file,give it a name and open it in binary mode  */
		fp= fopen(filename,"wb"); /* b -  binary mode */
		/*write ASCII header to the file*/
		fprintf(fp,"P6\n %s\n %d\n %d\n %d\n",comment,iXmax,iYmax,MaxColorComponentValue);
		/* compute and write image data bytes to the file*/
		fwrite(ppm, 1, 3 * iXmax * iYmax, fp);
		fclose(fp);

		const auto end(std::chrono::steady_clock::now());
    const std::chrono::duration<double> elapsed(end - start);
    std::cout << elapsed.count() << std::endl;
 
    return 0;
 }