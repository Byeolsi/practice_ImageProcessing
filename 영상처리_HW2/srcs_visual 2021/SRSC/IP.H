/****************************************************************************
 * file - ip.h                                                              *
 ****************************************************************************/

 /* typedefs */

typedef unsigned char* image_ptr;   // 배열
typedef double* double_ptr;
typedef struct
{
    unsigned char r, g, b;
} pixel;

typedef pixel* pixel_ptr;

typedef struct
{
    int width;      // 넓이
    int height;     // 높이
    float* x_data;
    float* y_data;
} mesh;

typedef struct
{
    double re;      // 실수부
    double im;      // 가수부
} COMPLEX;

typedef COMPLEX* complex_ptr;

typedef struct
{
    int x;
    int y;
} POINT;

typedef struct
{
    POINT P;
    POINT Q;
    int dx, dy;
    float length;
    long length_squared;
} LINE;

typedef struct
{
    POINT P;
    POINT Q;
} LINE_SEGMENT;

typedef struct
{
    int number;             /* number of segments to follow */
    LINE_SEGMENT line[100];
    char* filename; /* name of file holding the line list */
} LINE_LIST;

/* defines */

#define PI   3.14159265358979323846
#define CLIP(val, low, high) {if(val<low) val=low; if(val>high) val=high;}  // val 이 low 보다 작으면 val 에 low 값을 저장. val 이 high 보다 크면 val 에 high 값을 저장.
#define CLAMP(val, low, high) ((val<low) ? low : ((val>high) ? high : val))
#define MAX(A,B)        ((A) > (B) ? (A) : (B))
#define MIN(A,B)        ((A) < (B) ? (A) : (B))
#define IP_MALLOC(X) malloc(X)      // malloc 함수 - 메모리 할당
#define IP_FREE(X) free(X)          // free 함수 - 메모리 해제
#define PBM 4                       // '4' 를 PBM 형식으로 인식.
#define PGM 5                       // '5' 를 PGM 형식으로 인식.
#define PPM 6                       // '6' 을 PPM 형식으로 인식.
