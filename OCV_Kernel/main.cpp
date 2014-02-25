#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

using namespace cv;

int main()
{
	int num_frames = 1;
	for(int i = 0; i < num_frames; i++)
	{
		Mat frame, frame_gray; //2 frame buffers
	
		frame = imread("eye2.jpg"); //load the frame image

		resize(frame, frame, Size(), .25, .25);
		//resize(frame, frame, Size(), 3, 3);


		//Switch to HSV view
		cvtColor(frame, frame_gray, CV_BGR2HSV);
		//Smooth the image and 
		GaussianBlur(frame_gray, frame_gray, Size(9,9), 2, 2);
		//threshold(frame_gray, frame_gray, 20, 255, 1);
		inRangeS();

		namedWindow("W1");
		imshow("W1", frame);
		namedWindow("W2");
		imshow("W2", frame_gray);


		waitKey(0);

		destroyWindow("W1");
		destroyWindow("W2");
	}


	return 0;
}
