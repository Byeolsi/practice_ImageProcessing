#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include "ip.h"

extern void write_pnm(image_ptr ptr, char filein[], int rows,
		      int cols, int magic_number);
extern image_ptr read_pnm(char *filename, int *rows, int *cols,
		      int *type);
 

/***************************************************************************
 * Func: scramble                                                          *
 *                                                                         *
 * Desc: Scrambles the input data by swapping data with data in            *
 *       element with bit reversed index                                   *
 *                                                                         *
 * Params: numpoints- number of values in the complex array                *
 *         f - complex array of data                                       *
 ***************************************************************************/
void scramble(int numpoints, COMPLEX *f)
    {
    int i, j, m;          /* loop variables */
    double temp;          /* temporary storage during swapping */ 

    j = 0;
    for (i=0;i<numpoints;i++)
	{
	if (i > j)
	    {                    /* swap f[j] and f[i] */
	    temp = f[j].re;      /* swap real */
	    f[j].re = f[i].re;
	    f[i].re = temp;
	    temp = f[j].im;      /* swap imaginary */
	    f[j].im = f[i].im;
	    f[i].im = temp;
	    }
	m = numpoints>>1;
	while ((j >= m) & (m >= 2))
	    {
	    j -= m;
	    m = m >> 1;
	    }
	j += m;
	}
 }

/***************************************************************************
 * Func: butterflies                                                       *
 *                                                                         *
 * Desc: Calculates the butterflies for data in fft                        *
 *       if a reverse fft, points are divided by numpoints                 *
 *                                                                         *
 * Params: numpoints- number of points in the complex array                *
 *         logN- power of 2 (numpoints = 2^logN)                           *
 *         dir- direction of fft (1=forward, -1=reverse)                   *
 *         f- array of data                                                *
 ***************************************************************************/
void butterflies(int numpoints, int logN, int dir, COMPLEX *f)
    {
    double angle;                  /* polar angle */
    COMPLEX w, wp, temp;           /* intermediat complex numbers */
    int i, j, k, offset;           /* loop variables */
    int N, half_N;                 /* storage for powers of 2 */
    double wtemp;                  /* temporary storage for w.re */

    N = 1;
    for(k=0; k<logN; k++)
	{
	half_N = N;
	N <<= 1;                       /* multiply N by 2 */
	angle = -2.0 * PI / N * dir;
	wtemp = sin(0.5 * angle);
	wp.re = -2.0 * wtemp * wtemp;
	wp.im = sin(angle);
	w.re = 1.0;
	w.im = 0.0;
	for(offset=0; offset<half_N; offset++)
	    {
	    for(i=offset; i<numpoints; i+=N)
		{
		j = i + half_N;
		temp.re = (w.re * f[j].re) - (w.im * f[j].im);
		temp.im = (w.im * f[j].re) + (w.re * f[j].im);
		f[j].re = f[i].re - temp.re;
		f[i].re += temp.re;
		f[j].im = f[i].im - temp.im;
		f[i].im += temp.im;
		}
	    wtemp = w.re;
	    w.re = wtemp * wp.re - w.im * wp.im + w.re;
	    w.im = w.im * wp.re + wtemp * wp.im + w.im;
	    }
	}
    if (dir == -1)                 /* if inverse fft, divide by numpoints */
	for (i=0;i<numpoints;i++)
	    {
	    f[i].re /= numpoints;
	    f[i].im /= numpoints;
	   }
    }

/***************************************************************************
 * Func: fft                                                               *
 *                                                                         *
 * Desc: performs forward and reverse Fast Fourier Transform               *
 *                                                                         *
 * Params: f- complex array of data                                        *
 *         logN- power of 2 (numpoints = 2^logN)                           *
 *         numpoints- number of elements in the data array                 *
 *         dir- direction of fft (1=forward, -1=reverse)                   *
 ***************************************************************************/

void fft(COMPLEX *f, int logN, int numpoints, int dir)
     {
     scramble(numpoints, f);
     butterflies(numpoints, logN, dir, f);
     }

/****************************************************************************
 * Func: twoD_FFT                                                           *
 *                                                                          *
 * Desc: performs an FFT on image data                                      *
 *                                                                          *
 * Params: complex_data - array of complex image data                       *
 *         rows - number of rows in source image                            *
 *         cols - number of cols in source image                            *
 *         dir - direction of FFT (1=forward  -1=reverse)                   *
 ****************************************************************************/

void twoD_FFT(complex_ptr complex_data, int rows, int cols, int dir)
    {
    unsigned long x,y;           /* x and y indices to source image */
    unsigned long index;         /* index to output line buffer */
    COMPLEX *col_data;           /* storage for the columns */
    COMPLEX *row_data;           /* storage for the rows */
    int M, N;                    /* power of 2 for each dimension */
    int num;                     /* temporary variable */

    /* compute power of 2s */
    num = cols;
    M = 0;
    while(num >= 2)
	{
	num >>= 1;
	M++;
	}

    num = rows;
    N = 0;
    while(num >= 2)
	{
	num >>= 1;
	N++;
	}

    /* allocate memory for storage */
    col_data = (COMPLEX *) malloc(sizeof(COMPLEX)*rows);
    row_data = (COMPLEX *) malloc(sizeof(COMPLEX)*cols);
    if((col_data == NULL) || (row_data == NULL)) {
	exit(1);
	}

    for(y=0; y<rows; y++)                    /* FFT on all rows */
	{
	printf("Processing row %lu of %d\n",y+1, rows);
	index = y * cols;
	for(x=0; x<cols; x++)                 /* copy row data into array */
	    {
	    row_data[x].re = complex_data[index].re;
	    row_data[x].im = complex_data[index++].im;
	    }

	fft(row_data, M, cols, dir);           /* compute forward FFT */

	index = y * cols;
	for(x=0; x<cols; x++)                  /* copy row back */
	    {
	    complex_data[index].re = row_data[x].re;
	    complex_data[index++].im = row_data[x].im;
	    }
	}

    for(x=0; x<cols; x++)
	{
	printf("Processing column %lu of %d \n", x+1, cols);
	index = x;
	for(y=0; y<rows; y++)                  /* copy cols into array */
	    {
	    col_data[y].re = complex_data[index].re;
	    col_data[y].im = complex_data[index].im;
	    index += cols;
	    }
	fft(col_data, N, rows, dir);             /* perform forward FFT */
	index = x;
	for(y=0; y<rows; y++)
	    {
	    complex_data[index].re = col_data[y].re;
	    complex_data[index].im = col_data[y].im;
	    index += cols;
	    }
	}
    free(col_data);
		
    }



int main(int argc, char *argv[])
    {
    char filein[100];                   /* name of input file */
    char fileout[100];                  /* name of output file */
    int rows, cols;                     /* image rows and columns */
    unsigned long i, j;                    /* counting index */
    unsigned long bytes_per_pixel;      /* number of bytes per image pixel */
    image_ptr buffer;                   /* pointer to image buffer */
    COMPLEX *complex_data;
    /* center 보정용 버퍼 */
    image_ptr obuffer;                   /* pointer to image buffer */
    unsigned long number_of_pixels;     /* total number of pixels in image */
    int type, tmp, max;                           /* what type of image data */
    
    /* find minimum and maximum value of Fourier Spectrum */
	double ftmp, fmax;
	double imax, imin;
	int x, y;


    /* set input filename and output file name */
    if(argc == 3)
	{
	strcpy(filein, argv[1]);
	strcpy(fileout, argv[2]);
	}
    else
	{
	printf("Input name of input file\n");
	gets(filein);
	printf("\nInput name of output file\n");
	gets(fileout);
	printf("\n");
	}
 
    buffer = read_pnm(filein, &rows, &cols, &type);

    /* determine bytes_per_pixel, 3 for color, 1 for gray-scale */
    if(type == PPM)
	bytes_per_pixel = 3;
    else
	bytes_per_pixel = 1;

    number_of_pixels = bytes_per_pixel * rows * cols;

	obuffer = (image_ptr) IP_MALLOC(number_of_pixels);
	complex_data = (COMPLEX *) IP_MALLOC(sizeof(COMPLEX) * number_of_pixels);

    /* make complex image data */
	// 기본값으로 초기화.
	for(i=0; i< number_of_pixels; i++) {
		complex_data[i].re = (double)buffer[i];
		complex_data[i].im = 0.0;
	}

	// 이미지에 2D_FFT 변환을 수행.
	twoD_FFT(complex_data, rows, cols, 1);

	/* find minimum and maximum value of fourier spectrum */
	fmax = 0; // stores maximum Fourier spectrum log value, that is maximum log(1.0 + ftmp) value
	imax = 0.; // stores maximum Fourier spectrum value
	imin = 1000.; // stores minimum Fourier spectrum value
    	for(i=0; i< number_of_pixels; i++) {
			ftmp = sqrt(complex_data[i].re * complex_data[i].re
				 + complex_data[i].im * complex_data[i].im);
			
			if (ftmp > imax) imax = ftmp;
			if (ftmp < imin) imin = ftmp;

			ftmp = log(1.0 + ftmp);
			if (ftmp > fmax) fmax = ftmp;
    		//
          	// Coding is required !!!!
          	// Calculate Fourier Spectrum!
          	//
	}
	printf("fmax = %f  \n",fmax);
	printf("imax = %f, imin = %f  \n",imax, imin);

	/* make FFT image */
	for(i=0; i< number_of_pixels; i++) {
		ftmp = sqrt(complex_data[i].re * complex_data[i].re
			 + complex_data[i].im * complex_data[i].im);
		ftmp = log(1.0 + ftmp);
		
		// 상수 C 를 곱함.
		buffer[i] = (int)(255. * ftmp / fmax);
    		//
          	// Coding is required !!!!
          	// Use log operation!
          	//
	}

	for(i=0; i<rows; i++) {
		for(j=0; j<cols; j++) {
			x = (rows / 2 - i) % rows;
			y = (cols / 2 - j) % cols;

			obuffer[i * cols + j] = buffer[x * cols + y];
		/*영상의 중앙 부분을 원점으로 만들도록  x,y의 좌표를 재계산한다.*/
          	// Coding is required !!!!
          	// Move center position!
		}
	} 


  write_pnm(obuffer, fileout, rows, cols, type);

	IP_FREE(buffer);
	IP_FREE(obuffer);
	IP_FREE(complex_data);

    return 0;
    }

