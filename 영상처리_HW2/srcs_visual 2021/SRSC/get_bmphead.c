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
  USHORT bmpType;                       // BMP 파일의 종류를 나타내는 식별자
  ULONG  bmpSize;                       // BMP 파일의 크기(바이트 단위)
  USHORT bmpReserved1;                  // 예약
  USHORT bmpReserved2;                  // 예약
  ULONG  bmpOffset;                     // 이미지 정보가 시작되는 위치(Offset)
  ULONG  bmpHeaderSize;                 // Header의 크기(바이트 단위)
  ULONG  bmpWidth;                      // 가로 크기(픽셀 단위)
  ULONG  bmpHeight;                     // 세로 크기(픽셀 단위)
  USHORT bmpPlanes;                     // Color Plane의 수(1로 셋팅)
  USHORT bmpBitCount;                   // 한 픽셀에 할당된 비트수
  ULONG  bmpCompression;                // 압축 기법 사용여부
  ULONG  bmpBitmapSize;                 // 비트맵 이미지의 크기
  ULONG  bmpXPelsPerMeter;              // 수평 해상도
  ULONG  bmpYPelsPerMeter;              // 수직 해상도
  ULONG  bmpColors;                     // 사용된 색상수
  ULONG  bmpClrImportant;
}; 

struct BITMAPHEADER bmp_header;                // Bitmap 파일의 Header 구조체


void ReadBitmapHeader(char *filename) {
int          read_fd;                              // 파일을 읽고 쓰는데 사용할 Descriptor

  read_fd = open(filename, O_RDONLY);     // bmp파일을 open한다.
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
	if ((fp = fopen(filename, "rb")) == NULL)	// 예외 처리. 문제가 생길 시 프로그램이 에러를 출력하고 종료.
	{
		printf("Unable to open %s for reading\n", filename);
		exit(1);
	}

    fseek(fp, startOffset, SEEK_SET);

    scale = 1.0;
	row_size = cols * scale;
	total_size = (unsigned long)rows * row_size;	// 전체 크기 계산.

	ptr = (image_ptr)IP_MALLOC(total_size);			// 전체 크기만큼 메모리 할당.

	if (ptr == NULL)		// 예외 처리. ptr == NULL 이면, 메모리 할당이 정상적으로 이루어지지 않았음을 의미하기 때문에.
	{
		printf("Unable to malloc %lu bytes\n", total_size);
		exit(1);
	}

	total_bytes = 0;
    offset = 0;
	for (i = 0; i < rows; i++)							// 마지막 행까지 반복.
	{
		total_bytes += fread(ptr + offset, 1, row_size, fp);	// ptr 배열에 1 byte (0 ~ 255) 크기의 값을 파일로부터 한 행씩 입력받음.
		offset += row_size;									// 다음 행의 첫 offset 을 지정함.
	}

	if (total_size != total_bytes)						// total_size 와 total_bytes 가 같은 지 확인함으로써, 파일로부터 데이터를 불러오는 데에 문제가 없었는지 확인.
	{
		printf("Failed miserably trying to read %ld bytes\nRead %ld bytes\n",
			total_size, total_bytes);
		exit(1);
	}

	fclose(fp);
	return ptr;											// ptr 의 첫 offset 주소 반환.
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

	/* print out the portable bitmap header */	// 헤더 작성
	fprintf(fp, "P%d\n%d %d\n", magic_number, cols, rows);
	if (magic_number != 4)
		fprintf(fp, "255\n");

	row_size = cols * scale;
	total_size = (long)row_size * rows;			// 전체 크기 계산.
	offset = total_size - row_size;				// 원본 write_pnm() 함수에서 수정된 부분.
	total_bytes = 0;
	for (i = 0; i < rows; i++)
	{
		total_bytes += fwrite(ptr + offset, 1, row_size, fp);		// ptr 배열로부터 1 byte (0 ~ 255) 크기의 값을 파일에 한 행씩 입력함.
		offset -= row_size;						// 원본 write_pnm() 함수에서 수정된 부분. 다음 행의 첫 offset 을 지정함.
	}

	if (total_bytes != total_size)							// total_size 와 total_bytes 가 같은 지 확인함으로써, 파일에 데이터를 쓰는 데에 문제가 없었는지 확인.
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