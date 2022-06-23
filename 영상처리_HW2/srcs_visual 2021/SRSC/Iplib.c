/***************************************************************************
 * File: iplib.c                                                           *
 *                                                                         *
 * Desc: general purpose image processing routines                         *
 ***************************************************************************/


#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include "ip.h"

image_ptr read_pnm(char* filename, int* rows, int* cols, int* type);
int getnum(FILE* fp);

/***************************************************************************
 * Func: read_pnm                                                          *
 *                                                                         *
 * Desc: reads a portable bitmap file                                      *
 *                                                                         *
 * Params: filename - name of image file to read                           *
 *         rows - number of rows in the image                              *
 *         cols - number of columns in the image                           *
 *         type - file type                                                *
 *                                                                         *
 * Returns: pointer to the image just read into memory                     *
 ***************************************************************************/

image_ptr read_pnm(char* filename, int* rows, int* cols, int* type)
{
	int i;                     /* index variable */
	int row_size;              /* size of image row in bytes */
	int maxval;                /* maximum value of pixel */
	FILE* fp;                  /* input file pointer */
	int firstchar, secchar;    /* first 2 characters in the input file */
	image_ptr ptr;             /* pointer to image buffer */
	unsigned long offset;      /* offset into image buffer */
	unsigned long total_size;  /* size of image in bytes */
	unsigned long total_bytes; /* number of total bytes written to file */
	float scale;               /* number of bytes per pixel */

	/* open input file */
	if ((fp = fopen(filename, "rb")) == NULL)	// ���� ó��. ������ ���� �� ���α׷��� ������ ����ϰ� ����.
	{
		printf("Unable to open %s for reading\n", filename);
		exit(1);
	}

	firstchar = getc(fp);	// ������ PPM �������� Ȯ���ϱ� ����.
	secchar = getc(fp);		// ������ PBM ����, PGM ����, PPM ���� Ȯ���ϱ� ����.

	if (firstchar != 'P')	// ù���ڰ� 'P' �� �ƴϸ�, �߸��� �Է����� ����ϰ� ���α׷��� ����.
	{
		printf("You silly goof... This is not a PPM file!\n");
		exit(1);
	}

	*cols = getnum(fp);		// ���� ���̸� ����.
	*rows = getnum(fp);		// ���� ���̸� ����.
	*type = secchar - '0';	// ���� ������ char �� secchar �� type �� ������ �ٲ㼭 �ֱ� ����.

	switch (secchar)
	{
	case '4':            /* PBM */
		scale = 0.125;
		maxval = 1;				// PBM ������ 0 �� 1 �� �̱� ������.
		break;
	case '5':            /* PGM */
		scale = 1.0;
		maxval = getnum(fp);	// ����� �������� ��� ������ �ִ밪�̹Ƿ�.
		break;
	case '6':             /* PPM */
		scale = 3.0;
		maxval = getnum(fp);	// ��������.
		break;
	default:             /* Error */
		printf("read_pnm: This is not a Portable bitmap RAWBITS file\n");
		exit(1);
		break;
	}

	row_size = (*cols) * scale;
	total_size = (unsigned long)(*rows) * row_size;	// ��ü ũ�� ���.

	ptr = (image_ptr)IP_MALLOC(total_size);			// ��ü ũ�⸸ŭ �޸� �Ҵ�.

	if (ptr == NULL)		// ���� ó��. ptr == NULL �̸�, �޸� �Ҵ��� ���������� �̷������ �ʾ����� �ǹ��ϱ� ������.
	{
		printf("Unable to malloc %lu bytes\n", total_size);
		exit(1);
	}

	total_bytes = 0;
	offset = 0;
	for (i = 0; i < (*rows); i++)							// ������ ����� �ݺ�.
	{
		total_bytes += fread(ptr + offset, 1, row_size, fp);	// ptr �迭�� 1 byte (0 ~ 255) ũ���� ���� ���Ϸκ��� �� �྿ �Է¹���.
		offset += row_size;									// ���� ���� ù offset �� ������.
	}

	if (total_size != total_bytes)						// total_size �� total_bytes �� ���� �� Ȯ�������ν�, ���Ϸκ��� �����͸� �ҷ����� ���� ������ �������� Ȯ��.
	{
		printf("Failed miserably trying to read %ld bytes\nRead %ld bytes\n",
			total_size, total_bytes);
		exit(1);
	}

	fclose(fp);
	return ptr;											// ptr �� ù offset �ּ� ��ȯ.
}

/***************************************************************************
 * Func: getnum                                                            *
 *                                                                         *
 * Desc: reads an ASCII number from a portable bitmap file header          *
 *                                                                         *
 * Param: fp - pointer to file being read                                  *
 *                                                                         *
 * Returns: the number read                                                *
 ***************************************************************************/

int getnum(FILE* fp)
{
	char c;               /* character read in from file */
	int i;                /* number accumulated and returned */

	do
	{
		c = getc(fp);	// ���Ϸκ��� ���� c �� �Է� ����.
	}     while ((c == ' ') || (c == '\t') || (c == '\n') || (c == '\r'));

	if ((c < '0') || (c > '9'))		// ���� ó��.
		if (c == '#')                   /* chew off comments */
		{
			while (c == '#')			// �ּ��� �����ϱ� ����.
			{
				while (c != '\n')	// �� �ٲ��� ���� ������ ��� ����.
					c = getc(fp);
				c = getc(fp);
			}
		}
		else
		{
			printf("Garbage in ASCII fields\n");	// ���� ó��. '#', "0 ~ 9" ���ڰ� �ƴ� ���, ���� ��� ��, ���α׷� ����.
			exit(1);
		}

	i = 0;
	do
	{
		i = i * 10 + (c - '0');         /* convert ASCII to int */
		c = getc(fp);
	}     while ((c >= '0') && (c <= '9'));

	return i;	// ���ڷ� ��ȯ�� i ��ȯ
}

/***************************************************************************
 * Func: write_pnm                                                         *
 *                                                                         *
 * Desc: writes out a portable bitmap file                                 *
 *                                                                         *
 * Params: ptr - pointer to image in memory                                *
 *         filename _ name of file to write image to                       *
 *         rows - number of rows in the image                              *
 *         cols - number of columns in the image                           *
 *         magic_number - number that defines what type of file it is      *
 *                                                                         *
 * Returns: nothing                                                        *
 ***************************************************************************/

void write_pnm(image_ptr ptr, char* filename, int rows,
	int cols, int magic_number)
{
	FILE* fp;             /* file pointer for output file */
	long offset;          /* current offset into image buffer */
	long total_bytes;     /* number of bytes written to output file */
	long total_size;      /* size of image buffer */
	int row_size;         /* size of row in bytes */
	int i;                /* index variable */
	float scale;          /* number of bytes per image pixel */

	switch (magic_number)
	{
	case 4:            /* PBM */
		scale = 0.125;
		break;
	case 5:            /* PGM */
		scale = 1.0;
		break;
	case 6:             /* PPM */
		scale = 3.0;
		break;
	default:             /* Error */
		printf("write_pnm: This is not a Portable bitmap RAWBITS file\n");
		exit(1);
		break;
	}

	/* open new output file */
	if ((fp = fopen(filename, "wb")) == NULL)
	{
		printf("Unable to open %s for output\n", filename);
		exit(1);
	}

	/* print out the portable bitmap header */	// ��� �ۼ�
	fprintf(fp, "P%d\n%d %d\n", magic_number, cols, rows);
	if (magic_number != 4)
		fprintf(fp, "255\n");

	row_size = cols * scale;
	total_size = (long)row_size * rows;			// ��ü ũ�� ���.
	offset = 0;
	total_bytes = 0;
	for (i = 0; i < rows; i++)
	{
		total_bytes += fwrite(ptr + offset, 1, row_size, fp);		// ptr �迭�κ��� 1 byte (0 ~ 255) ũ���� ���� ���Ͽ� �� �྿ �Է���.
		offset += row_size;										// ���� ���� ù offset �� ������.
	}

	if (total_bytes != total_size)							// total_size �� total_bytes �� ���� �� Ȯ�������ν�, ���Ͽ� �����͸� ���� ���� ������ �������� Ȯ��.
		printf("Tried to write %ld bytes...Only wrote %ld\n",
			total_size, total_bytes);

	fclose(fp);
}

/****************************************************************************
 * Func: pnm_open                                                           *
 *                                                                          *
 * Desc: opens a pnm file and determines rows, cols, and maxval             *
 *                                                                          *
 * Params: rows- pointer to number of rows in the image                     *
 *         cols - pointer number of columns in the image                    *
 *         maxval - pointer to max value                                    *
 *         filename - name of image file                                    *
 ****************************************************************************/

FILE* pnm_open(int* rows, int* cols, int* maxval, char* filename)
{
	int firstchar, secchar;
	float scale;
	unsigned long row_size;
	FILE* fp;

	if ((fp = fopen(filename, "rb")) == NULL)	// ���� ó��.
	{
		printf("Unable to open %s for reading\n", filename);
		exit(1);
	}

	firstchar = getc(fp);	// ������ PPM �������� Ȯ���ϱ� ����.
	secchar = getc(fp);		// ������ PBM ����, PGM ����, PPM ���� Ȯ���ϱ� ����.

	if (firstchar != 'P')	// ù���ڰ� 'P' �� �ƴϸ�, �߸��� �Է����� ����ϰ� ���α׷��� ����.
	{
		printf("You silly goof... This is not a PPM file!\n");
		exit(1);
	}

	*cols = getnum(fp);		// ���� ���̸� ����.
	*rows = getnum(fp);		// ���� ���̸� ����.

	switch (secchar)
	{
	case '4':            /* PBM */
		scale = 0.125;
		*maxval = 1;
		break;
	case '5':            /* PGM */
		scale = 1.0;
		*maxval = getnum(fp);
		break;
	case '6':             /* PPM */
		scale = 3.0;
		*maxval = getnum(fp);
		break;
	default:             /* Error */
		printf("read_pnm: This is not a Portable bitmap RAWBITS file\n");
		exit(1);
		break;
	}

	row_size = (*cols) * scale;		// �� ���� ��ü ����Ʈ ũ��.
	return fp;
}


/****************************************************************************
 * Func: read_mesh                                                          *
 *                                                                          *
 * Desc: reads mesh data into a mesh structure								*
 *                                                                          *
 * Params: filename - name of input mesh file                               *
 *                                                                          *
 * Returns: mesh structure storing width, height, x data  and y data        *
 ****************************************************************************/

mesh* read_mesh(char* filename)
{
	FILE* fp;
	mesh* mesh_data;
	int width, height, mesh_size;

	/* open mesh file for input */
	if ((fp = fopen(filename, "rb")) == NULL)
	{
		printf("Unable to open mesh file %s for reading\n", filename);
		exit(1);
	}

	mesh_data = malloc(sizeof(mesh));	// mesh_data �޸� �Ҵ�.
	/* read dimensions of mesh */	// ����, ���̸� ���Ϸκ��� �Է¹���.
	fread(&width, sizeof(int), 1, fp);
	fread(&height, sizeof(int), 1, fp);
	mesh_data->width = width;		// mesh_data ����ü ���� �ִ� ������� width�� read_mesh �Լ� ���� ����� width ���� ���� ����.
	mesh_data->height = height;		// mesh_data ����ü ���� �ִ� ������� height�� read_mesh �Լ� ���� ����� height ���� ���� ����.
	mesh_size = width * height;		// mesh_size �� ����.

	/* allocate memory for mesh data */
	mesh_data->x_data = malloc(sizeof(float) * mesh_size);
	mesh_data->y_data = malloc(sizeof(float) * mesh_size);

	/* mesh_data �޸� �Ҵ� ��, ���Ϸκ��� �ҷ��� ������ ����. */
	fread(mesh_data->x_data, sizeof(float), mesh_size, fp);
	fread(mesh_data->y_data, sizeof(float), mesh_size, fp);

	return(mesh_data);
}

void histogram_equalize(image_ptr buffer, unsigned long number_of_pixels)
{
	unsigned long max = 0;
	unsigned long histogram[256]; /* image histogram */
	unsigned long sum_hist[256];  /* sum of histogram elements */
	float scale_factor;           /* normalized scale factor */
	unsigned long i;              /* index variable */
	unsigned long sum;            /* variable used to increment sum of hist */

	/* clear histogram to 0 */
	for (i = 0; i < 256; i++)
		histogram[i] = 0;

	/* calculate histogram */
	for (i = 0; i < number_of_pixels; i++)
		histogram[buffer[i]]++;

	/* find a max */
	for (i = 0; i < 256; i++) {
		if (histogram[i] > max) {
			max = histogram[i];
		}
	}

	/* normalization */
	for (i = 0; i < 256; i++) {
		histogram[i] = histogram[i] / (float)max * 255;
	}

	/* make a graph */
	for (i = 0; i < number_of_pixels; i++) {
		if ((255 - i / 256) > histogram[i % 256])
			buffer[i] = 255;
		else
			buffer[i] = 0;
	}

	/* calculate normalized sum of hist */
	/*
	sum = 0;
	scale_factor = 255.0 / number_of_pixels;		// CDF �׷����� y�� �ִ밪�� 255�� ����.
	for (i = 0; i < 256; i++)
	{
		sum += histogram[i];						// ������׷��� ����.
		sum_hist[i] = (sum * scale_factor) + 0.5;	// �ø��ϱ� ���� 0.5 �� ����.
													// sum_hist �� CDF ��.
	}
	*/

	/* transform image using new sum_hist as a LUT */
	/*
	for (i = 0; i < number_of_pixels; i++)			// ������׷��� ������ �Լ��� LUT ó�� ���.
		buffer[i] = sum_hist[buffer[i]];
	*/
}
