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

void ReadBitmapHeader(char* filename)
{
	// ������ �а� ���µ� ����� Descriptor

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