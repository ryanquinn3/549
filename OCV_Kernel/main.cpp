#include "opencv2/highgui/highgui.hpp"
#include <iostream>

using namespace cv;

int main()
{
	Mat img = imread("eye.jpg");

	namedWindow("W1", CV_WINDOW_AUTOSIZE);
	imshow("W1", img);

	waitKey(0);

	destroyWindow("W1");



	return 0;
}
