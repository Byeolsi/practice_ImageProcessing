#include "stdafx.h"
#include <opencv2/opencv.hpp> 

using namespace std;
using namespace cv;

double cubic_interpolation(double v1, double v2, double v3, double v4, double d) {
	double p1, p2, p3, p4;

	p1 = 2 * v2;
	p2 = -v1 + v3;
	p3 = 2 * v1 - 5 * v2 + 4 * v3 - v4;
	p4 = -v1 + 3 * v2 - 3 * v3 + v4;

	return ((p1 + d * (p2 + d * (p3 + d * p4))) / 2.0);
}
void Scaling_Cubic(Mat& imgSrc, Mat& imgDst) {
	int nw = imgDst.cols;
	int nh = imgDst.rows;

	int w = imgSrc.cols;
	int h = imgSrc.rows;
	int i, j, x1, x2, x3, x4, y1, y2, y3, y4;
	double v1, v2, v3, v4, v;
	double rx, ry, p, q;

	for (j = 0; j < nh; j++) {
		for (i = 0; i < nw; i++) {
			rx = (w - 1) * i / (double)(nw - 1);
			ry = (h - 1) * j / (double)(nh - 1);

			x2 = (int)rx;
			x1 = max(x2 - 1, 0);
			x3 = min(x2 + 1, w - 1);
			x4 = min(x2 + 2, w - 1);
			p = rx - x2;

			y2 = (int)ry;
			y1 = max(y2 - 1, 0);
			y3 = min(y2 + 1, h - 1);
			y4 = min(y2 + 2, h - 1);
			q = ry - y2;

			v1 = cubic_interpolation(imgSrc.at<uchar>(y1, x1), imgSrc.at<uchar>(y1, x2),
				imgSrc.at<uchar>(y1, x3), imgSrc.at<uchar>(y1, x4), p);
			v2 = cubic_interpolation(imgSrc.at<uchar>(y2, x1), imgSrc.at<uchar>(y2, x2),
				imgSrc.at<uchar>(y2, x3), imgSrc.at<uchar>(y2, x4), p);
			v3 = cubic_interpolation(imgSrc.at<uchar>(y3, x1), imgSrc.at<uchar>(y3, x2),
				imgSrc.at<uchar>(y3, x3), imgSrc.at<uchar>(y3, x4), p);
			v4 = cubic_interpolation(imgSrc.at<uchar>(y4, x1), imgSrc.at<uchar>(y4, x2),
				imgSrc.at<uchar>(y4, x3), imgSrc.at<uchar>(y4, x4), p);

			v = cubic_interpolation(v1, v2, v3, v4, q);
			imgDst.at<uchar>(j, i) = saturate_cast<uchar>(v + 0.5);
		}
	}
}