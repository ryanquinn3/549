#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

using namespace cv;

int main()
{
	Mat img = imread("eye2.jpg", CV_LOAD_IMAGE_GRAYSCALE);

	namedWindow("W1");
	threshold(img,img,220,255, THRESH_BINARY);
	imshow("W1", ~img);


	waitKey(0);

	destroyWindow("W1");



	return 0;
}
