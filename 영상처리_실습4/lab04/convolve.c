#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "ip.h"
 
 
/***************************************************************************
 * Func: convolve                                                           * 
 *                                                                         *
 * Desc: convolves an image with the floating point kernel passed  and     *
 *       writes out new image one line at a time                           *
 *                                                                         *
 * Params: source - pointer to image in memory                             *
 *         cols - number of columns in image                               *
 *         rows - number of rows in image                                  *
 *         kwidth - width of convolution kernel                            *
 *         kheight - height of convolution kernel                          *
 *         kernel - pointer to convolution kernel                          *
 *         bias - value to add to convolution sum                          *
 *         filename - name of output file                                  * 
 ***************************************************************************/

void convolve(image_ptr source, int cols, int rows, int kwidth, int kheight,
	      float *kernel, int bias, char *filename)
    {
    int x, y, i;           /* image loop variables */
    int kernx, kerny;      /* kernel loop variables */
    int index;             /* image index */
    int xextra, yextra;       /* size of boundary */
    int conv_line;            /* size of output line */
    float sum;                /* accumulator used during convolution */
    unsigned long destadd;    /* destination image address */
    unsigned long sourceadd;  /* index into source image */
    unsigned long sourcebase; /* address of line */
    unsigned char *dest;      /* destination image line */
    FILE *fp;                 /* output file pointer */
    unsigned char left[25];   /* storage of left pixel for duplication */
    unsigned char right[25];  /* storage of right pixel for duplication */
    int xpad, ypad;           /* number of pixels to duplicate at edges */
    int last_line;            /* last line to process */

    yextra = (kheight/2)*2;
    ypad = yextra/2;
    xextra = (kwidth/2)*2;
    xpad = xextra/2;
    conv_line = cols - xextra;
    last_line = rows - yextra;
    dest = malloc(cols);

    if((fp=fopen(filename, "wb")) == NULL)
	{
	printf("Unable to open %s for output\n",filename);
	exit(1);
	}
    fprintf(fp, "P5\n%d %d\n255\n", cols, rows); /* print out header */

    for(y=0; y<last_line; y++)
	{
	sourcebase=(unsigned long) cols * y;
	destadd=0;
	for(x=xextra/2; x< (cols-xpad); x++)
	   {
	   sum=0.0;
	   index=0;
	   for(kerny=0; kerny<kheight; kerny++)
	       for(kernx=0; kernx<kwidth; kernx++)
		  {
		  sourceadd = sourcebase + kernx + kerny * cols;
		  sum += (source[sourceadd] * kernel[index++]);
		  }

	   sum += bias;
	   CLIP(sum,0.0,255.0);

	   dest[destadd++]=(unsigned char) sum;
	   sourcebase++;
	   } /* for x */
	 for(i=0; i<xpad; i++)
	   left[i]=dest[0];
	 for(i=0; i<xpad; i++)
	   right[i]=dest[conv_line-1];
	 if(y==0)
	     for(i=0; i<ypad; i++)
	     {
	     fwrite(left, 1, xpad, fp);
	     fwrite(dest, 1, conv_line, fp);
	     fwrite(right, 1, xpad, fp);
	     }
	 fwrite(left, 1, xpad, fp);
	 fwrite(dest, 1, conv_line, fp);
	 fwrite(right, 1, xpad, fp);
	 if(y==(last_line-1))
	     for(i=0; i<ypad; i++)
		 {
		 fwrite(left, 1, xpad, fp);
		 fwrite(dest, 1, conv_line, fp);
		 fwrite(right, 1, xpad, fp);
		 }
    } /* for y */

}