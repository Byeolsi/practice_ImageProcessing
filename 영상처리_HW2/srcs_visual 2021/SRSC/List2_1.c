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
        gets(filein);                               // ���� ���� �̸��� �Է� ����.
        printf("\nInput name of output file\n");
        gets(fileout);                              // ���� ������� ���ε� ���� ���� �̸��� �Է� ����.
        printf("\n");
    }

    buffer = read_pnm(filein, &rows, &cols, &type); // �Էµ� ������ �д� �Լ��� ����.

    /* initialize Look-up table */
    for (i = 0; i < 256; i++)
    {
        /* preserve an original image or y is equal to x */
        temp = i;

        /* operation */
        // temp = operation(i);     // temp �ӽ� ������ i * 1.9 ���� ����. temp �� �������̱� ������ �Ҽ��� ������ ���� ����.
        
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

        CLIP(temp, 0, 255);     // ���ε� ���� ��� ����(L) ���� 0 ���� �۰ų�, 255 ���� ũ�� �ȵǱ� ������.
        LUT[i] = temp;          // LUT[i] �� temp ���� �ʱ�ȭ.
    }

    /* determine bytes_per_pixel, 3 for color, 1 for gray-scale */
    if (type == PPM)         // �� �ȼ��� bytes ũ�⸦ ������. PPM �� 3 bytes �� �̿ܿ��� 1 byte �� ������.
        bytes_per_pixel = 3;
    else
        bytes_per_pixel = 1;

    number_of_pixels = bytes_per_pixel * rows * cols;   // PPM �� ���, �� �ȼ� �� ��⺸�ٴ� �� ����Ʈ ũ��� ����.

    /* process image via the Look-up table */
    for (i = 0; i < number_of_pixels; i++) {   // ��� �ȼ��� ���� Look-up Table �� �̿��Ͽ� ��� ����(L)�� ���Ӱ� ����.

        /* �ǽ� A - make 0 */
        // buffer[i] = 0;

        /* �ǽ� A - make 255 */
        // buffer[i] = 255;

        /* �ǽ� A - cols */
        // buffer[i] = i % cols;

        /* �ǽ� A - rows */
        // buffer[i] = i / cols;

        /* �ǽ� A - rows + cols */
        // buffer[i] = ((i / cols) + (i % cols)) % cols;

        /* �ǽ� B */

        /*
        if (i % cols == 100 || i % cols == 146 || i / cols == 100 || i / cols == 146)
            buffer[i] = 255;
        */

        /* �ǽ� C */

        /*
        // �̹��� ũ�Ⱑ 255 * 255 �� �����Ͽ�.
        if ((255 - i / cols) > LUT[i % cols])   // �̱��� �������� �������ֽŴ�� i / cols �� row �� �ǹ�. i & cols �� col �ϳ� �ϳ��� �ǹ�.
            buffer[i] = 0;
        else
            buffer[i] = 255;
        */

        // buffer[i] = LUT[buffer[i]];
    }

    // histogram_equalize(buffer, number_of_pixels);

    scale_pnm(buffer, fileout, rows, cols, 2, 2, type);

    // write_pnm(buffer, fileout, rows, cols, type);
    IP_FREE(buffer);                    // �޸� ����
    return 0;
}


