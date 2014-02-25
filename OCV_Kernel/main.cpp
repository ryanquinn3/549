#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

int main()
{
	int num_frames = 1;
	for(int i = 0; i < num_frames; i++)
	{
		Mat frame, frame_gray; //2 frame buffers
	
		frame = imread("eye.jpg"); //load the frame image

		//resize(frame, frame, Size(), .25, .25);
		resize(frame, frame, Size(), 3, 3);


		//Switch to HSV view
		cvtColor(frame, frame_gray, CV_BGR2HSV);
		//Smooth the image 
		GaussianBlur(frame_gray, frame_gray, Size(9,9), 2, 2);
		
		Mat brown_mask;

		//Mask out everything except the blackest parts of the image
		inRange(frame_gray, Scalar(0, 0,0), Scalar(255,255,25), brown_mask);


		vector<Vec3f> circles;
		HoughCircles(brown_mask, circles, CV_HOUGH_GRADIENT, 1, 1, 200, 100, 0, 0);
		cout << circles.size() << endl;
		for(int j = 0; j < circles.size(); j++)
		{
			Point center(cvRound(circles[j][0]), cvRound(circles[j][1]));
			int radius = cvRound(circles[j][2]);

			circle(frame, center, 3, Scalar(0,255,0), -1, 8, 0);
			circle(frame, center, radius, Scalar(70,255,0), 3, 8, 0);	

		}
 	
	



		namedWindow("W1");
		imshow("W1", frame);
		namedWindow("W2");
		imshow("W2", brown_mask);
		


		waitKey(0);

		destroyWindow("W1");
		destroyWindow("W2");
		
	}


	return 0;
}
