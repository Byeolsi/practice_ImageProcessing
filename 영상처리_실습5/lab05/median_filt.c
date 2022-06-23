
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "ip.h"

/***************************************************************************
 * Func: swap                                                              *
 *                                                                         *
 * Desc: swaps two elements in an array                                    *
 *                                                                         *
 * Params: array - array of element (two of which will be swapped)         *
 *         i - first element in array to swap                              *
 *         j - second element in array to swap                             *
 ***************************************************************************/
void swap(int *array, int i, int j) 
    {
    int temp; 

    temp = array[i];
    array[i] = array[j];
    array[j] = temp;
    }

 
/***************************************************************************
 * Func: quicksort                                                        *
 *                                                                         *
 * Desc: sorts an array of integers using the quicksort algorithm         *
 *                                                                         *
 * Params: array - the array of integers                                  *
 *         left - leftmost entry in array                                 *
 *         right - rightmost entry in array                               *
 ***************************************************************************/

void quicksort(int *array, int left, int right)
    {
    int i, last;

    if(left >= right)
	return;
    swap(array, left, (left + right)/2);
    last = left;
    for( i = left + 1; i<= right; i++)
	if(array[i] < array[left])
	    swap(array, ++last, i);
    swap(array, left, last);
    quicksort(array, left, last-1);
    quicksort(array, last+1, right);
    }

/****************************************************************************
 * Func: median                                                             *
 *                                                                          *
 * Desc: finds median value of an image block                               *
 *                                                                          *
 * Param: window - 3x3 window of source image                               *
 *                                                                          *
 * Returns: the median value                                                *
 ****************************************************************************/

int median(int *window, int wsize)
    {
    quicksort(window, 0, wsize);
    return window[wsize/2];
    }


/***************************************************************************
 * Func: median_filt                                                       *
 *                                                                         *
 * Desc: median filters an image using a block mask                        *
 *                                                                         *
 * Params: source - pointer to image in memory                             *
 *         cols - number of columns in image                               *
 *         rows - number of rows in image                                  *
 *         filename - name of output file                                  *
 *         side - width of block                                           *
 ***************************************************************************/

void median_filt(image_ptr source, int cols, int rows, char *filename, int side)
    {
    int x, y, i;             /* image loop variables */
    int index;               /* image index */
    int winx, winy;          /* index into the samples window */
    int xextra, yextra;      /* size of boundary */
    int conv_line;           /* size of output line */
    unsigned long destadd;   /* destination image address */
    unsigned long sourceadd, sourcebase; /* source addressing */
    char dest[1024];         /* destination image line */
    FILE *fp;                /* output file pointer */
    int *window;             /* window for block samples */
    unsigned char left[25];  /* storage for left edge pixel duplication */
    unsigned char right[25]; /* storage for right edge pixel duplication */
    int xpad, ypad;          /* size of padding */
    int last_line;           /* last line to process */
    int new_pixel;           /* newly processed pixel */
    int wsize;               /* number of elements in block */

    wsize = side * side;
    yextra = (side/2)*2;
    ypad = yextra/2;
    xextra = yextra;
    xpad = ypad;
    conv_line = cols - xextra;

    window = malloc(wsize * sizeof(int));
    if((fp=fopen(filename, "wb")) == NULL)
	{
	printf("Unable to open %s for output\n",filename);
	exit(1);
	}
    fprintf(fp, "P5\n%d %d\n255\n", cols, rows); /* print out header */
    last_line = rows - yextra;

    for(y=0; y<last_line; y++)
	{
	sourcebase=(unsigned long) cols * y;
	destadd=0;
	for(x=xpad; x<(cols - xpad); x++)
	   {
	   index=0;
	   for(winy=0; winy<side; winy++)
	       for(winx=0; winx<side; winx++)
		  {
		  sourceadd=sourcebase+winx+winy*cols;
		  window[index++] = (int) source[sourceadd];
		  }
	   new_pixel = median(window, wsize);
	   dest[destadd++]=(unsigned char) new_pixel;
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

    //free(window);
    }




