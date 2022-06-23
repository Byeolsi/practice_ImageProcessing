/***************************************************************************
 * File: get_bmpimage.c                                                    *
 *                                                                         *
 * Desc: This program converts bmp format image to pgm format              *
 ***************************************************************************/


#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include "ip.h"
#include <fcntl.h>
#include <string.h>

extern image_ptr read_pnm(char* filename, int* rows, int* cols, int* type);

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

void ReadBitmapHeader(char* filename)
{
	// 파일을 읽고 쓰는데 사용할 Descriptor

	FILE* read_fd;
	if ((read_fd = fopen(filename, "rb")) == NULL)
	{
		printf("Can't open file %s\n", filename);
		exit(1);
	}

	fread(&bmp_header.bmpType, sizeof(bmp_header.bmpType), 1, read_fd);
	fread(&bmp_header.bmpSize, sizeof(bmp_header.bmpSize), 1, read_fd);
	fread(&bmp_header.bmpReserved1, sizeof(bmp_header.bmpReserved1), 1, read_fd);
	fread(&bmp_header.bmpReserved2, sizeof(bmp_header.bmpReserved2), 1, read_fd);
	fread(&bmp_header.bmpOffset, sizeof(bmp_header.bmpOffset), 1, read_fd);
	fread(&bmp_header.bmpHeaderSize, sizeof(bmp_header.bmpHeaderSize), 1, read_fd);
	fread(&bmp_header.bmpWidth, sizeof(bmp_header.bmpWidth), 1, read_fd);
	fread(&bmp_header.bmpHeight, sizeof(bmp_header.bmpHeight), 1, read_fd);
	fread(&bmp_header.bmpPlanes, sizeof(bmp_header.bmpPlanes), 1, read_fd);
	fread(&bmp_header.bmpBitCount, sizeof(bmp_header.bmpBitCount), 1, read_fd);
	fread(&bmp_header.bmpCompression, sizeof(bmp_header.bmpCompression), 1, read_fd);
	fread(&bmp_header.bmpBitmapSize, sizeof(bmp_header.bmpBitmapSize), 1, read_fd);
	fread(&bmp_header.bmpXPelsPerMeter, sizeof(bmp_header.bmpXPelsPerMeter), 1, read_fd);
	fread(&bmp_header.bmpYPelsPerMeter, sizeof(bmp_header.bmpYPelsPerMeter), 1, read_fd);
	fread(&bmp_header.bmpColors, sizeof(bmp_header.bmpColors), 1, read_fd);
	fread(&bmp_header.bmpClrImportant, sizeof(bmp_header.bmpClrImportant), 1, read_fd);

	fclose(read_fd);
}

int main(int argc, char* argv[])
{
	char filein[100];
	char fileout[100];
	char* buff;
	FILE* write_fp, * read_fp;
	int i, j;

	if (argc == 3)
	{
		strcpy(filein, argv[0]);
		strcpy(fileout, argv[1]);
	}
	else
	{
		printf("Input name of file input\n");
		gets(filein);
		printf("Input name of file output\n");
		gets(fileout);
	}

	if ((read_fp = fopen(filein, "rb")) == NULL)
	{
		printf("Unable to open %s for input\n", filein);
		exit(1);
	}
	if ((write_fp = fopen(fileout, "wb")) == NULL)
	{
		printf("Unable to open %s for output\n", fileout);
		exit(1);
	}
	ReadBitmapHeader(filein);

	fprintf(write_fp, "P%d\n%d %d\n255\n", 5, bmp_header.bmpHeight, bmp_header.bmpWidth);
	fseek(read_fp, -(long)(bmp_header.bmpWidth), SEEK_END);

	buff = (char*)malloc(bmp_header.bmpWidth);
	for (i = 0; i < bmp_header.bmpHeight; i++)
	{
		fread(buff, bmp_header.bmpWidth, 1, read_fp);
		fwrite(buff, bmp_header.bmpWidth, 1, write_fp);
		fseek(read_fp, -(long)(bmp_header.bmpWidth * 2), SEEK_CUR);
	}
	return 0;
}