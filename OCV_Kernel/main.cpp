#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

int main()
{
	namedWindow("W1");
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
		inRange(frame_gray, Scalar(0, 0,0), Scalar(255,255,20), brown_mask);
	
		//Draw the contours for the isolated pupil
		vector<vector<Point> > contours;
		vector<Vec4i> hierarchy;
		findContours(brown_mask, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0,0));
		
		Mat contour_edges = Mat::zeros(brown_mask.size(), CV_8UC3);
		for(int j = 0; j < contours.size(); j++)
		{
			drawContours( contour_edges, contours, j, Scalar(255,0,0), 2,8, hierarchy, 0, Point());
		cout << "Est. pupil area = " << contourArea(contours[j]) << endl;
		}



	imshow("W1", brown_mask);



		namedWindow("W2");
		imshow("W2", contour_edges);
		


		waitKey(0);

		destroyWindow("W1");
		destroyWindow("W2");
		
	}


	return 0;
}
