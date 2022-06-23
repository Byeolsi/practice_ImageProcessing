/***************************************************************************
 * File: arithlut.c                                                        *
 *                                                                         *
 * Desc: This program performs arithmetic point operations via LUTs        *
 ***************************************************************************/


#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include "ip.h"

#define operation(VALUE)        ((float) VALUE * 1.9)
#define plus(VALUE)             ((float) VALUE + 50)
#define minus(VALUE)            ((float) VALUE - 50)
#define multiply(VALUE)         ((float) VALUE * 1.5)
#define divide(VALUE)           ((float) VALUE / 1.5)
#define complexOper(VALUE)      ((float) VALUE / 1.5 + 40)
#define convert_0(VALUE)        ((120 < VALUE && VALUE < 128) ? 0 : VALUE)
#define gamma(VALUE)            (255 * pow((float) VALUE / 255, 2))

extern void write_pnm(image_ptr ptr, char filein[], int rows,
    int cols, int magic_number);
extern image_ptr read_pnm(char* filename, int* rows, int* cols,
    int* type);

int main(int argc, char* argv[])
{
    char filein[100];                   /* name of input file */
    char fileout[100];                  /* name of output file */
    int rows, cols;                     /* image rows and columns */
    unsigned long i;                    /* counting index */
    unsigned long bytes_per_pixel;      /* number of bytes per image pixel */
    unsigned char LUT[256];             /* array for Look-up table */
    image_ptr buffer;                   /* pointer to image buffer */
    unsigned long number_of_pixels;     /* total number of pixels in image */
    int temp;                           /* temporary variable */
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
        gets(filein);                               // 영상 파일 이름을 입력 받음.
        printf("\nInput name of output file\n");
        gets(fileout);                              // 새로 만들어질 맵핑될 영상 파일 이름을 입력 받음.
        printf("\n");
    }

    buffer = read_pnm(filein, &rows, &cols, &type); // 입력된 영상을 읽는 함수를 수행.

    /* initialize Look-up table */
    for (i = 0; i < 256; i++)
    {
        /* preserve an original image or y is equal to x */
        temp = i;

        /* operation */
        // temp = operation(i);     // temp 임시 변수에 i * 1.9 값을 저장. temp 는 정수형이기 때문에 소수점 이하의 값을 버림.
        
        /* plus */
        // temp = plus(i);          // + 50
        
        /* multiply */
        // temp = multiply(i);      // * 1.5
       
        /* complex operation */
        // temp = complexOper(i);   // * 1.5 + 40

        /* convert 0 */
        // temp = convert_0(i);     // 120 < i < 128 => temp

        /* gamma */
        // temp = gamma(i);         // gamma 2

        /* Negative image */
        // temp = 255 - i
        
        /* Contrast Stretching */
        // temp = (float)i * 2 - 128;

        /* Compression */
        // temp = (float)i / 2 + 128;

        /* Posterized */
        // temp = (i / 32) * 32;

        /* Threshold */
        /*
        if (i > 128) temp = 255;
        else temp = 0;
        */

        /* Bounded Threshold */
        /*
        if (128 < i && i <= 200) temp = 200;
        else if (50 < i && i <= 128) temp = 50;
        else temp = i;
        */

        /* Iso-intensity contouring */
        /*
        if (60 < i && i < 80) temp = 0;
        else if (100 < i && i < 120) temp = 0;
        else if (140 < i && i < 160) temp = 0;
        else if (180 < i && i < 200) temp = 0;
        else temp = i;
        */

        /* Solarize */
        /*
        if (i > 128) temp = 255 - i;
        else temp = i;
        */

        CLIP(temp, 0, 255);     // 맵핑된 후의 밝기 레벨(L) 값이 0 보다 작거나, 255 보다 크면 안되기 때문에.
        LUT[i] = temp;          // LUT[i] 에 temp 값을 초기화.
    }

    /* determine bytes_per_pixel, 3 for color, 1 for gray-scale */
    if (type == PPM)         // 한 픽셀당 bytes 크기를 지정함. PPM 은 3 bytes 그 이외에는 1 byte 로 설정함.
        bytes_per_pixel = 3;
    else
        bytes_per_pixel = 1;

    number_of_pixels = bytes_per_pixel * rows * cols;   // PPM 의 경우, 총 픽셀 수 라기보다는 총 바이트 크기와 같음.

    /* process image via the Look-up table */
    for (i = 0; i < number_of_pixels; i++) {   // 모든 픽셀에 대해 Look-up Table 을 이용하여 밝기 레벨(L)을 새롭게 맵핑.

        /* 실습 A - make 0 */
        // buffer[i] = 0;

        /* 실습 A - make 255 */
        // buffer[i] = 255;

        /* 실습 A - cols */
        // buffer[i] = i % cols;

        /* 실습 A - rows */
        // buffer[i] = i / cols;

        /* 실습 A - rows + cols */
        // buffer[i] = ((i / cols) + (i % cols)) % cols;

        /* 실습 B */

        /*
        if (i % cols == 100 || i % cols == 146 || i / cols == 100 || i / cols == 146)
            buffer[i] = 255;
        */

        /* 실습 C */

        /*
        // 이미지 크기가 255 * 255 인 가정하에.
        if ((255 - i / cols) > LUT[i % cols])   // 이기현 교수님이 설명해주신대로 i / cols 는 row 를 의미. i & cols 는 col 하나 하나를 의미.
            buffer[i] = 0;
        else
            buffer[i] = 255;
        */

        // buffer[i] = LUT[buffer[i]];
    }

    // histogram_equalize(buffer, number_of_pixels);

    scale_pnm(buffer, fileout, rows, cols, 2, 2, type);

    // write_pnm(buffer, fileout, rows, cols, type);
    IP_FREE(buffer);                    // 메모리 해제
    return 0;
}


