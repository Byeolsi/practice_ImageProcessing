/***************************************************************************
 * File: filter operation test                                             *
 *                                                                         *
 * Desc: This program performs filtering operation with zone plate image   * 
 ***************************************************************************/ 
 

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include "ip.h"

//#define operation(VALUE)        ((float) VALUE * 1.0) // original image

extern void write_pnm(image_ptr ptr, char filein[], int rows,
		      int cols, int magic_number);
extern image_ptr read_pnm(char *filename, int *rows, int *cols,
		      int *type);
extern void scale_pnm(image_ptr buffer, char* fileout, int rows, int cols,
    float x_scale, float y_scale, int type);
extern void bilinear(image_ptr buffer, char* fileout, int rows, int cols,
    float x_scale, float y_scale, int type);
extern void cubic_convolution(image_ptr buffer, char* fileout, int rows, int cols,
    float x_scale, float y_scale, int type);
extern void bilinear_rotation(image_ptr buffer, char* fileout, int rows, int cols,
    float angle, int type);

int main(int argc, char *argv[])
    {
    char filein[100];                   /* name of input file */
    char fileout[100];                  /* name of output file */
    int rows, cols;                     /* image rows and columns */
    unsigned long i;                    /* counting index */
    unsigned long bytes_per_pixel;      /* number of bytes per image pixel */
    image_ptr buffer;                   /* pointer to image buffer */
	// image_ptr obuffer;                   /* pointer to image buffer */
    unsigned long number_of_pixels;     /* total number of pixels in image */
    int type;                           /* what type of image data */
 
//////////////////////////////////////////////////////////////////////////////////////////
    // Convolution 계수 저장
    // float mask[3][3] = { {-0.111, -0.111, -0.111},{-0.111, 1.889, -0.111},{-0.111, -0.111, -0.111}};
    // float mask[3][3] = { {(float)1 / 9, (float)1 / 9, (float)1 / 9}, {(float)1 / 9, (float)1 / 9, (float)1 / 9}, {(float)1 / 9, (float)1 / 9, (float)1 / 9 } };
    // float mask[3][3] = { {(float)-1 / 9, (float)-1 / 9, (float)-1 / 9}, {(float)-1 / 9, (float)8 / 9, (float)-1 / 9}, {(float)-1 / 9, (float)-1 / 9, (float)-1 / 9 } };
    // float mask[3][3] = { {-1, -1, -1}, {-1, 9, -1}, {-1, -1, -1} };
    
    /* Prewitt */
    float maskGx_Prewitt[3][3] = { {-1, -1, -1}, {0, 0, 0}, {1, 1, 1} };
    float maskGy_Prewitt[3][3] = { {-1, 0, 1}, {-1, 0, 1}, {-1, 0, 1} };

    /* Sobel */
    float maskGx_Sobel[3][3] = { {-1, -2, -1}, {0, 0, 0}, {1, 2, 1} };
    float maskGy_Sobel[3][3] = { {-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1} };

    /* Compass Prewitt */
    float mask_compassPrewitt_W[3][3] = { {-1, 1, 1}, {-1, -2, 1}, {-1, 1, 1} };
    float mask_compassPrewitt_NW[3][3] = { {-1, -1, 1}, {-1, -2, 1}, {1, 1, 1} };
    float mask_compassPrewitt_N[3][3] = { {-1, -1, -1}, {1, -2, 1}, {1, 1, 1} };

    /* Compass Kirsh */
    float mask_compassKirsh_NE[3][3] = { {-3, -3, -3}, {5, 0, -3}, {5, 5, -3} };
    float mask_compassKirsh_E[3][3] = { {5, -3, -3}, {5, 0, -3}, {5, -3, -3} };
    float mask_compassKirsh_SE[3][3] = { {5, 5, -3}, {5, 0, -3}, {-3, -3, -3} };

    /* Compass Robinson 3 */
    float mask_compassRobinson3_S[3][3] = { {1, 1, 1}, {0, 0, 0}, {-1, -1, -1} };
    float mask_compassRobinson3_SW[3][3] = { {0, 1, 1}, {-1, 0, 1}, {-1, -1, 0} };
    float mask_compassRobinson3_W[3][3] = { {-1, 0, 1}, {-1, 0, 1}, {-1, 0, 1} };

    /* Compass Robinson 5 */
    float mask_compassRobinson5_S[3][3] = { {1, 2, 1}, {0, 0, 0}, {-1, -2, -1} };
    float mask_compassRobinson5_SW[3][3] = { {0, 1, 2}, {-1, 0, 1}, {-2, -1, 0} };
    float mask_compassRobinson5_W[3][3] = { {-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1} };

    /* LoG */
    float mask_LoG[5][5] = { {0, 0, -1, 0, 0}, {0, -1, -2, -1, 0}, {-1, -2, 16, -2, -1}, {0, -1, -2, -1, 0}, {0, 0, -1, 0, 0} };

    /* read bmp header */


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

    /* read PNM file */
    buffer = read_pnm(filein, &rows, &cols, &type);

    /* determine bytes_per_pixel, 3 for color, 1 for gray-scale */
    if(type == PPM)
	bytes_per_pixel = 3;
    else
	bytes_per_pixel = 1;

    number_of_pixels = bytes_per_pixel * rows * cols;

////////////////////////////////////////////////////////////////
 // convolve 함수 call을 이용한 convolution 기능 수행
    /* original convolve */
    // convolve(buffer, cols, rows, 3, 3, mask, 0, fileout);

    /* detect edge Gx using Prewitt */
    // convolve(buffer, cols, rows, 3, 3, maskGx_Prewitt, 0, fileout);

    /* detect edge Gy using Prewitt */
    // convolve(buffer, cols, rows, 3, 3, maskGy_Prewitt, 0, fileout);

    /* detect edge Gx using Sobel */
    // convolve(buffer, cols, rows, 3, 3, maskGx_Sobel, 0, fileout);

    /* detect edge Gy using Sobel */
    // convolve(buffer, cols, rows, 3, 3, maskGy_Sobel, 0, fileout);

    /* detect edge West using Prewitt */
    // convolve(buffer, cols, rows, 3, 3, mask_compassPrewitt_W, 0, fileout);

    /* detect edge NorthWest using Prewitt */
    // convolve(buffer, cols, rows, 3, 3, mask_compassPrewitt_NW, 0, fileout);

    /* detect edge North using Prewitt */
    // convolve(buffer, cols, rows, 3, 3, mask_compassPrewitt_N, 0, fileout);

    /* detect edge NorthEast using Kirsh */
    // convolve(buffer, cols, rows, 3, 3, mask_compassKirsh_NE, 0, fileout);

    /* detect edge East using Kirsh */
    // convolve(buffer, cols, rows, 3, 3, mask_compassKirsh_E, 0, fileout);

    /* detect edge SouthEast using Kirsh */
    // convolve(buffer, cols, rows, 3, 3, mask_compassKirsh_SE, 0, fileout);

    /* detect edge South using Robinson 3 */
    // convolve(buffer, cols, rows, 3, 3, mask_compassRobinson3_S, 0, fileout);

    /* detect edge SouthWest using Robinson 3 */
    // convolve(buffer, cols, rows, 3, 3, mask_compassRobinson3_SW, 0, fileout);

    /* detect edge West using Robinson 3 */
    // convolve(buffer, cols, rows, 3, 3, mask_compassRobinson3_W, 0, fileout);

    /* detect edge South using Robinson 5 */
    // convolve(buffer, cols, rows, 3, 3, mask_compassRobinson5_S, 0, fileout);

    /* detect edge SouthWest using Robinson 5 */
    // convolve(buffer, cols, rows, 3, 3, mask_compassRobinson5_SW, 0, fileout);

    /* detect edge West using Robinson 5 */
    // convolve(buffer, cols, rows, 3, 3, mask_compassRobinson5_W, 0, fileout);

    /* detect edge using LoG */
    // convolve(buffer, cols, rows, 5, 5, mask_LoG, 0, fileout);

    /* median and mean filter */
    // median_filt(buffer, cols, rows, fileout, 3);

////////////////////////////////////////////////////////////////
 // makeczp 함수 call을 이용한 zone plate 영상 생성
    // makeczp(buffer, rows, cols, 350, 350);

    // write_pnm(buffer, fileout, rows, cols, type);

    // scale_pnm(buffer, fileout, rows, cols, 2.0f, 2.0f, type);
    // bilinear(buffer, fileout, rows, cols, 2.0f, 2.0f, type);
    // cubic_convolution(buffer, fileout, rows, cols, 2.0f, 2.0f, type);
    bilinear_rotation(buffer, fileout, rows, cols, 60, type);

    IP_FREE(buffer);
    return 0;
}