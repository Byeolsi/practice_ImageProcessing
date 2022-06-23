/***************************************************************************
 * File: get_bmphead.c                                                     *
 *                                                                         *
 * Desc: This program reads header part of bmp format image                *
 ***************************************************************************/
 
 
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "ip.h"

typedef unsigned short int USHORT;
typedef unsigned long int  ULONG; 

struct BITMAPHEADER {
  USHORT bmpType;                       // BMP ������ ������ ��Ÿ���� �ĺ���
  ULONG  bmpSize;                       // BMP ������ ũ��(����Ʈ ����)
  USHORT bmpReserved1;                  // ����
  USHORT bmpReserved2;                  // ����
  ULONG  bmpOffset;                     // �̹��� ������ ���۵Ǵ� ��ġ(Offset)
  ULONG  bmpHeaderSize;                 // Header�� ũ��(����Ʈ ����)
  ULONG  bmpWidth;                      // ���� ũ��(�ȼ� ����)
  ULONG  bmpHeight;                     // ���� ũ��(�ȼ� ����)
  USHORT bmpPlanes;                     // Color Plane�� ��(1�� ����)
  USHORT bmpBitCount;                   // �� �ȼ��� �Ҵ�� ��Ʈ��
  ULONG  bmpCompression;                // ���� ��� ��뿩��
  ULONG  bmpBitmapSize;                 // ��Ʈ�� �̹����� ũ��
  ULONG  bmpXPelsPerMeter;              // ���� �ػ�
  ULONG  bmpYPelsPerMeter;              // ���� �ػ�
  ULONG  bmpColors;                     // ���� �����
  ULONG  bmpClrImportant;
}; 

struct BITMAPHEADER bmp_header;                // Bitmap ������ Header ����ü


void ReadBitmapHeader(char *filename) {
int          read_fd;                              // ������ �а� ���µ� ����� Descriptor

  read_fd = open(filename, O_RDONLY);     // bmp������ open�Ѵ�.
  if (read_fd == -1) {
    printf("Can't open file %s\n", filename);
    exit(1);
  }

  read(read_fd, &bmp_header.bmpType, sizeof(bmp_header.bmpType));
  read(read_fd, &bmp_header.bmpSize, sizeof(bmp_header.bmpSize));
  read(read_fd, &bmp_header.bmpReserved1, sizeof(bmp_header.bmpReserved1));
  read(read_fd, &bmp_header.bmpReserved2, sizeof(bmp_header.bmpReserved2));
  read(read_fd, &bmp_header.bmpOffset, sizeof(bmp_header.bmpOffset));
  read(read_fd, &bmp_header.bmpHeaderSize, sizeof(bmp_header.bmpHeaderSize));
  read(read_fd, &bmp_header.bmpWidth, sizeof(bmp_header.bmpWidth));
  read(read_fd, &bmp_header.bmpHeight, sizeof(bmp_header.bmpHeight));
  read(read_fd, &bmp_header.bmpPlanes, sizeof(bmp_header.bmpPlanes));
  read(read_fd, &bmp_header.bmpBitCount, sizeof(bmp_header.bmpBitCount));
  read(read_fd, &bmp_header.bmpCompression, sizeof(bmp_header.bmpCompression));
  read(read_fd, &bmp_header.bmpBitmapSize, sizeof(bmp_header.bmpBitmapSize));
  read(read_fd, &bmp_header.bmpXPelsPerMeter, sizeof(bmp_header.bmpXPelsPerMeter));
  read(read_fd, &bmp_header.bmpYPelsPerMeter, sizeof(bmp_header.bmpYPelsPerMeter));
  read(read_fd, &bmp_header.bmpColors, sizeof(bmp_header.bmpColors));
  read(read_fd, &bmp_header.bmpClrImportant, sizeof(bmp_header.bmpClrImportant));

  close(read_fd);
}

image_ptr read_bmp(char* filename, int rows, int cols, int type, ULONG startOffset)
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

    fseek(fp, startOffset, SEEK_SET);

    scale = 1.0;
	row_size = cols * scale;
	total_size = (unsigned long)rows * row_size;	// ��ü ũ�� ���.

	ptr = (image_ptr)IP_MALLOC(total_size);			// ��ü ũ�⸸ŭ �޸� �Ҵ�.

	if (ptr == NULL)		// ���� ó��. ptr == NULL �̸�, �޸� �Ҵ��� ���������� �̷������ �ʾ����� �ǹ��ϱ� ������.
	{
		printf("Unable to malloc %lu bytes\n", total_size);
		exit(1);
	}

	total_bytes = 0;
    offset = 0;
	for (i = 0; i < rows; i++)							// ������ ����� �ݺ�.
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

void convert_bmp_to_pnm(image_ptr ptr, char* filename, int rows,
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
	offset = total_size - row_size;				// ���� write_pnm() �Լ����� ������ �κ�.
	total_bytes = 0;
	for (i = 0; i < rows; i++)
	{
		total_bytes += fwrite(ptr + offset, 1, row_size, fp);		// ptr �迭�κ��� 1 byte (0 ~ 255) ũ���� ���� ���Ͽ� �� �྿ �Է���.
		offset -= row_size;						// ���� write_pnm() �Լ����� ������ �κ�. ���� ���� ù offset �� ������.
	}

	if (total_bytes != total_size)							// total_size �� total_bytes �� ���� �� Ȯ�������ν�, ���Ͽ� �����͸� ���� ���� ������ �������� Ȯ��.
		printf("Tried to write %ld bytes...Only wrote %ld\n",
			total_size, total_bytes);

	fclose(fp);
}

int main(int argc, char* argv[])
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

    /* set input filename and output file name */
    if (argc == 3)
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

    ReadBitmapHeader(filein);
    cols = bmp_header.bmpWidth;
    rows = bmp_header.bmpHeight;
    type = PGM;
    buffer = read_bmp(filein, cols, rows, type, bmp_header.bmpOffset);
    
    /* determine bytes_per_pixel, 3 for color, 1 for gray-scale */
    if (type == PPM)
        bytes_per_pixel = 3;
    else
        bytes_per_pixel = 1;

    number_of_pixels = bytes_per_pixel * rows * cols;

    convert_bmp_to_pnm(buffer, fileout, rows, cols, type);

    IP_FREE(buffer);
    return 0;
}