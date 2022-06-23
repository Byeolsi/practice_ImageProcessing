#include <stdio.h>
#include <math.h>
#include "ip.h"

/****************************************************************************
 * Func: scale_pnm                                                          *
 *                                                                          *
 * Desc: scale an image using nearest neighbor interpolation                *
 *                                                                          *
 * Params: buffer - pointer to image in memory                              *
 *         fileout - name of output file                                    *
 *         rows - number of rows in image                                   *
 *         cols - number of columns in image                                *
 *         x_scale - scale factor in X direction                            *
 *         y_scale - scale factor in Y direction                            *
 *         type - graphics file type (5 = PGM    6 = PPM)                   *
 ****************************************************************************/ 
void scale_pnm(image_ptr buffer, char *fileout, int rows, int cols,
	float x_scale, float y_scale, int type) 
    {

    unsigned long x,y;          /* loop indices for columns and rows */
    unsigned long index;        /* index into line buffer */
    unsigned long source_index; /* address of source pixel in image buffer */
    unsigned char *line_buff;   /* output line buffer */
    int new_rows, new_cols;     /* values of rows and columns for new image */
    unsigned line;              /* number of pixels in one scan line */
    FILE *fp;                   /* output file pointer */
    unsigned long X_Source, Y_Source;  /* x and y address of source pixel */
    pixel_ptr color_buff;       /* pointer to a color image in memory */

	int NW, NE, SW, SE, result;
	float EWweight, NSweight, EWtop, EWbottom;

	int cubic_arr[4][4], i, j;
	int arr_x, arr_y;
	float cubic_EWweight[4], cubic_NSweight[4], cubic_intermResult[4], cubic_result, dif_tmp, a = -0.5f;

    /* open new output file */
    if((fp=fopen(fileout, "wb")) == NULL)
	{
	printf("Unable to open %s for output\n",fileout);
	exit(1); 
	}

    new_cols = cols * x_scale;
    new_rows = rows * y_scale;

    /* print out the portable bitmap header */
    fprintf(fp, "P%d\n%d %d\n255\n", type, new_cols, new_rows);

    if(type == 5)        /* PGM file */
	line = new_cols;
    else                 /* PPM file */
	{
	line = new_cols * 3;
	color_buff = (pixel_ptr) buffer;
	}

    line_buff = (unsigned char *) malloc(line);

    for(y=0; y<new_rows; y++)
	{
	index = 0;
		for(x=0; x<new_cols; x++)
	    {
			X_Source = (unsigned long)((x / x_scale) + 0.5);
			Y_Source = (unsigned long)((y / y_scale) + 0.5);
			source_index = Y_Source * cols + X_Source;

			// (511, 511) 이라면, x / x_scale = 255.5, y / y_scale = 255.5, X_Source = 255, Y_Source = 255
			/*X_Source = (unsigned long)(x / x_scale);
			Y_Source = (unsigned long)(y / y_scale);*/
			if (type == 5)      /* PGM */
			{
				/* nearest neighbor interpolation */
				line_buff[index++] = buffer[source_index];

				/* bilinear interpolation */
				/*
				NW = buffer[Y_Source * cols + X_Source];
				NE = buffer[Y_Source * cols + X_Source + 1];
				SW = buffer[(Y_Source + 1) * cols + X_Source];
				SE = buffer[(Y_Source + 1) * cols + X_Source + 1];

				// 1 / 5 - 0 = 0.2		X_Source = 0	// NW 와 가까울 때,
				// 2 / 5 - 0 = 0.4		X_Source = 0	// NW 와 가깝지만, 중앙
				// 3 / 5 - 0 = 0.6		X_Source = 0	// NE 와 가깝지만, 중앙
				// 4 / 5 - 0 = 0.8		X_Source = 0	// NE 와 가까울 때,
				// 5 / 5 - 1 = 0		X_Source = 1
				// 511 / 2 = 255.5, 255.5 - 255 = 0.5
				EWweight = (x / x_scale) - (float)X_Source;
				NSweight = (y / y_scale) - (float)Y_Source;

				EWtop = NW + EWweight * (NE - NW);
				EWbottom = SW + EWweight * (SE - SW);

				result = EWtop + NSweight * (EWbottom - EWtop);
				
				line_buff[index++] = result;
				*/

				/* cubic convolution interpolation */
				/*
				arr_y = Y_Source - 1;
				for (i = 0; i < 4; i++)
				{
					arr_x = X_Source - 1;
					for (j = 0; j < 4; j++)
					{
						if (arr_y < 0 || arr_x < 0 || arr_y >= rows || arr_x >= cols)
							cubic_arr[i][j] = 128;
						else
							cubic_arr[i][j] = buffer[arr_y * cols + arr_x];
						
						arr_x++;
					}
					arr_y++;
				}

				arr_x = X_Source - 1;
				for (i = 0; i < 4; i++)
				{
					// x = 511, x_scale = 2, (x / x_scale) = 255.5, arr_x = 255 - 1 = 254		dif = 1.5
					// (x / x_scale) = 255.5, arr_x = 255		dif = 0.5
					// (x / x_scale) = 255.5, arr_x = 256		dif = -0.5
					// (x / x_scale) = 255.5, arr_x = 257		dif = -1.5
					dif_tmp = fabsf((x / x_scale) - arr_x);

					if (0 <= dif_tmp && dif_tmp < 1)
						cubic_EWweight[i] = (a + 2) * pow(dif_tmp, 3) - (a + 3) * pow(dif_tmp, 2) + 1;
					else if (1 <= dif_tmp && dif_tmp < 2)
						cubic_EWweight[i] = a * pow(dif_tmp, 3) - 5 * a * pow(dif_tmp, 2) + 8 * a * dif_tmp - 4 * a;
					else
						cubic_EWweight[i] = 0;
					
					arr_x++;
				}

				arr_y = Y_Source - 1;
				for (i = 0; i < 4; i++)
				{
					dif_tmp = fabsf((y / y_scale - arr_y));

					if (0 <= dif_tmp && dif_tmp < 1)
						cubic_NSweight[i] = (a + 2) * pow(dif_tmp, 3) - (a + 3) * pow(dif_tmp, 2) + 1;
					else if (1 <= dif_tmp && dif_tmp < 2)
						cubic_NSweight[i] = a * pow(dif_tmp, 3) - 5 * a * pow(dif_tmp, 2) + 8 * a * dif_tmp - 4 * a;
					else
						cubic_NSweight[i] = 0;

					arr_y++;
				}

				for (i = 0; i < 4; i++)
				{
					cubic_intermResult[i] = 0;
					for (j = 0; j < 4; j++)
					{
						cubic_intermResult[i] += cubic_EWweight[j] * cubic_arr[i][j];
					}
				}

				cubic_result = 0;
				for (i = 0; i < 4; i++)
				{
					cubic_result += cubic_NSweight[i] * cubic_intermResult[i];
				}

				CLIP(cubic_result, 0, 255);
				line_buff[index++] = cubic_result;
				*/
			}
			else               /* PPM */
			{
				line_buff[index++] = color_buff[source_index].r;
				line_buff[index++] = color_buff[source_index].g;
				line_buff[index++] = color_buff[source_index].b;
			}
	    }
	fwrite(line_buff, 1, line, fp);
	}
    fclose(fp);
}

void bilinear(image_ptr buffer, char* fileout, int rows, int cols,
	float x_scale, float y_scale, int type)
{

	unsigned long x, y;          /* loop indices for columns and rows */
	unsigned long index;        /* index into line buffer */
	unsigned long source_index; /* address of source pixel in image buffer */
	unsigned char* line_buff;   /* output line buffer */
	int new_rows, new_cols;     /* values of rows and columns for new image */
	unsigned line;              /* number of pixels in one scan line */
	FILE* fp;                   /* output file pointer */
	unsigned long X_Source, Y_Source;  /* x and y address of source pixel */
	pixel_ptr color_buff;       /* pointer to a color image in memory */

	int NW, NE, SW, SE, result;
	float EWweight, NSweight, EWtop, EWbottom;

	/* open new output file */
	if ((fp = fopen(fileout, "wb")) == NULL)
	{
		printf("Unable to open %s for output\n", fileout);
		exit(1);
	}

	new_cols = cols * x_scale;
	new_rows = rows * y_scale;

	/* print out the portable bitmap header */
	fprintf(fp, "P%d\n%d %d\n255\n", type, new_cols, new_rows);

	if (type == 5)        /* PGM file */
		line = new_cols;
	else                 /* PPM file */
	{
		line = new_cols * 3;
		color_buff = (pixel_ptr)buffer;
	}

	line_buff = (unsigned char*)malloc(line);

	for (y = 0; y < new_rows; y++)
	{
		index = 0;
		for (x = 0; x < new_cols; x++)
		{
			X_Source = (unsigned long)(x / x_scale);
			Y_Source = (unsigned long)(y / y_scale);
			if (type == 5)      /* PGM */
			{
				/* bilinear interpolation */
				// duplicate the border edges
				NW = buffer[Y_Source * cols + X_Source];
				if (X_Source + 1 >= cols && Y_Source + 1 >= rows)
				{
					NE = NW;
					SW = NW;
					SE = NW;
				}
				else if (X_Source + 1 >= cols)
				{
					NE = NW;
					SW = buffer[(Y_Source + 1) * cols + X_Source];
					SE = SW;
				}
				else if (Y_Source + 1 >= rows)
				{
					NE = buffer[Y_Source * cols + X_Source + 1];
					SW = NW;
					SE = NE;
				}
				else
				{
					NE = buffer[Y_Source * cols + X_Source + 1];
					SW = buffer[(Y_Source + 1) * cols + X_Source];
					SE = buffer[(Y_Source + 1) * cols + X_Source + 1];
				}

				EWweight = (x / x_scale) - (float)X_Source;
				NSweight = (y / y_scale) - (float)Y_Source;

				EWtop = NW + EWweight * (NE - NW);
				EWbottom = SW + EWweight * (SE - SW);

				result = EWtop + NSweight * (EWbottom - EWtop);

				line_buff[index++] = result;
			}

			else               /* PPM */
			{
				line_buff[index++] = color_buff[source_index].r;
				line_buff[index++] = color_buff[source_index].g;
				line_buff[index++] = color_buff[source_index].b;
			}
		}
		fwrite(line_buff, 1, line, fp);
	}
	fclose(fp);
}

void cubic_convolution(image_ptr buffer, char* fileout, int rows, int cols,
	float x_scale, float y_scale, int type)
{

	unsigned long x, y;          /* loop indices for columns and rows */
	unsigned long index;        /* index into line buffer */
	unsigned long source_index; /* address of source pixel in image buffer */
	unsigned char* line_buff;   /* output line buffer */
	int new_rows, new_cols;     /* values of rows and columns for new image */
	unsigned line;              /* number of pixels in one scan line */
	FILE* fp;                   /* output file pointer */
	unsigned long X_Source, Y_Source;  /* x and y address of source pixel */
	pixel_ptr color_buff;       /* pointer to a color image in memory */

	int cubic_arr[4][4], i, j;
	int arr_x, arr_y;
	float cubic_EWweight[4], cubic_NSweight[4], cubic_intermResult[4], cubic_result, dif_tmp, a = -0.5f;	// a (계수) 의 값 : -0.5f

	/* open new output file */
	if ((fp = fopen(fileout, "wb")) == NULL)
	{
		printf("Unable to open %s for output\n", fileout);
		exit(1);
	}

	new_cols = cols * x_scale;
	new_rows = rows * y_scale;

	/* print out the portable bitmap header */
	fprintf(fp, "P%d\n%d %d\n255\n", type, new_cols, new_rows);

	if (type == 5)        /* PGM file */
		line = new_cols;
	else                 /* PPM file */
	{
		line = new_cols * 3;
		color_buff = (pixel_ptr)buffer;
	}

	line_buff = (unsigned char*)malloc(line);

	for (y = 0; y < new_rows; y++)
	{
		index = 0;
		for (x = 0; x < new_cols; x++)
		{
			X_Source = (unsigned long)(x / x_scale);
			Y_Source = (unsigned long)(y / y_scale);
			if (type == 5)      /* PGM */
			{
				/* cubic convolution interpolation */
				// 배열에 픽셀 값을 배치.
				arr_y = Y_Source - 1;
				for (i = 0; i < 4; i++)
				{
					arr_x = X_Source - 1;
					for (j = 0; j < 4; j++)
					{
						// zero padding
						if (arr_y < 0 || arr_x < 0 || arr_y >= rows || arr_x >= cols)
							cubic_arr[i][j] = 0;
						else
							cubic_arr[i][j] = buffer[arr_y * cols + arr_x];

						arr_x++;
					}
					arr_y++;
				}

				// 가로 가중치 계산.
				arr_x = X_Source - 1;
				for (i = 0; i < 4; i++)
				{
					dif_tmp = fabsf((x / x_scale) - arr_x);

					if (0 <= dif_tmp && dif_tmp < 1)
						cubic_EWweight[i] = (a + 2) * pow(dif_tmp, 3) - (a + 3) * pow(dif_tmp, 2) + 1;
					else if (1 <= dif_tmp && dif_tmp < 2)
						cubic_EWweight[i] = a * pow(dif_tmp, 3) - 5 * a * pow(dif_tmp, 2) + 8 * a * dif_tmp - 4 * a;
					else
						cubic_EWweight[i] = 0;

					arr_x++;
				}

				// 세로 가중치 계산.
				arr_y = Y_Source - 1;
				for (i = 0; i < 4; i++)
				{
					dif_tmp = fabsf((y / y_scale - arr_y));

					if (0 <= dif_tmp && dif_tmp < 1)
						cubic_NSweight[i] = (a + 2) * pow(dif_tmp, 3) - (a + 3) * pow(dif_tmp, 2) + 1;
					else if (1 <= dif_tmp && dif_tmp < 2)
						cubic_NSweight[i] = a * pow(dif_tmp, 3) - 5 * a * pow(dif_tmp, 2) + 8 * a * dif_tmp - 4 * a;
					else
						cubic_NSweight[i] = 0;

					arr_y++;
				}

				// 가로 가중치를 이용하여 중간 결과값 계산.
				for (i = 0; i < 4; i++)
				{
					cubic_intermResult[i] = 0;
					for (j = 0; j < 4; j++)
					{
						cubic_intermResult[i] += cubic_EWweight[j] * cubic_arr[i][j];
					}
				}

				// 세로 가중치를 이용하여 최종 결과값 계산.
				cubic_result = 0;
				for (i = 0; i < 4; i++)
				{
					cubic_result += cubic_NSweight[i] * cubic_intermResult[i];
				}

				// float 형식의 특성 상 계산 상의 약간의 오차가 발생하는데 이 때문에 오버플로우, 언더플로우가 발생.
				// 이를 CLIP 함수를 이용하여 조정.
				CLIP(cubic_result, 0, 255);
				line_buff[index++] = cubic_result;
			}
			else               /* PPM */
			{
				line_buff[index++] = color_buff[source_index].r;
				line_buff[index++] = color_buff[source_index].g;
				line_buff[index++] = color_buff[source_index].b;
			}
		}
		fwrite(line_buff, 1, line, fp);
	}
	fclose(fp);
}

double getRadian(float angle) {
	return angle * (PI / 180);
}

void bilinear_rotation(image_ptr buffer, char* fileout, int rows, int cols,
	float angle, int type)
{

	unsigned long x, y;          /* loop indices for columns and rows */
	unsigned long index;        /* index into line buffer */
	unsigned long source_index; /* address of source pixel in image buffer */
	unsigned char* line_buff;   /* output line buffer */
	int new_rows, new_cols;     /* values of rows and columns for new image */
	unsigned line;              /* number of pixels in one scan line */
	FILE* fp;                   /* output file pointer */
	unsigned long X_Source, Y_Source;  /* x and y address of source pixel */
	pixel_ptr color_buff;       /* pointer to a color image in memory */

	int NW, NE, SW, SE, result;
	float EWweight, NSweight, EWtop, EWbottom;

	double radian = getRadian(angle);		// radian 값을 받을 변수
	unsigned long x_center, y_center;		// 이미지의 중앙 좌표를 받을 변수

	/* open new output file */
	if ((fp = fopen(fileout, "wb")) == NULL)
	{
		printf("Unable to open %s for output\n", fileout);
		exit(1);
	}

	// 중앙 부분의 좌표
	x_center = cols / 2;
	y_center = rows / 2;

	/* print out the portable bitmap header */
	// fprintf(fp, "P%d\n%d %d\n255\n", type, new_cols, new_rows);
	fprintf(fp, "P%d\n%d %d\n255\n", type, cols, rows);

	if (type == 5)        /* PGM file */
		line = cols;
	else                 /* PPM file */
	{
		line = cols * 3;
		color_buff = (pixel_ptr)buffer;
	}

	line_buff = (unsigned char*)malloc(line);

	for (y = 0; y < rows; y++)
	{
		index = 0;
		for (x = 0; x < cols; x++)
		{
			// 원본 파일의 좌표
			X_Source = (unsigned long)(cos(radian) * ((double)x - x_center) + sin(radian) * ((double)y - y_center) + (double)x_center);
			Y_Source = (unsigned long)((-1) * sin(radian) * ((double)x - x_center) + cos(radian) * ((double)y - y_center) + (double)y_center);
			if (type == 5)      /* PGM */
			{
				/* bilinear interpolation */
				// duplicate the border edges
				// X_Source, Y_Source 가 원본 이미지의 가로, 세로 크기를 넘지 않는다면,
				if (0 <= X_Source && X_Source < cols && 0 <= Y_Source && Y_Source < rows)
				{
					NW = buffer[Y_Source * cols + X_Source];
					if (X_Source + 1 >= cols && Y_Source + 1 >= rows)
					{
						NE = NW;
						SW = NW;
						SE = NW;
					}
					else if (X_Source + 1 >= cols)
					{
						NE = NW;
						SW = buffer[(Y_Source + 1) * cols + X_Source];
						SE = SW;
					}
					else if (Y_Source + 1 >= rows)
					{
						NE = buffer[Y_Source * cols + X_Source + 1];
						SW = NW;
						SE = NE;
					}
					else
					{
						NE = buffer[Y_Source * cols + X_Source + 1];
						SW = buffer[(Y_Source + 1) * cols + X_Source];
						SE = buffer[(Y_Source + 1) * cols + X_Source + 1];
					}

					// 가중치를 계산할 때에도,
					EWweight = (cos(radian) * ((double)x - x_center) + sin(radian) * ((double)y - y_center) + (double)x_center) - (float)X_Source;
					NSweight = ((-1) * sin(radian) * ((double)x - x_center) + cos(radian) * ((double)y - y_center) + (double)y_center) - (float)Y_Source;

					EWtop = NW + EWweight * (NE - NW);
					EWbottom = SW + EWweight * (SE - SW);

					result = EWtop + NSweight * (EWbottom - EWtop);

					line_buff[index++] = result;
				}
				// X_Source, Y_Source 가 원본 이미지의 가로, 세로 크기를 넘는다면,
				else
				{
					// 흰색으로 표현함.
					line_buff[index++] = 255;
				}
			}

			else               /* PPM */
			{
				line_buff[index++] = color_buff[source_index].r;
				line_buff[index++] = color_buff[source_index].g;
				line_buff[index++] = color_buff[source_index].b;
			}
		}
		fwrite(line_buff, 1, line, fp);
	}
	fclose(fp);
}