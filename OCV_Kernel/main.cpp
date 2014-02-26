#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

int main()
{

	VideoCapture cap("../eye_test.h264");
	if(!cap.isOpened())
	{
		cout << "Failed to open video stream" << endl;
		return 1;
	}

	cout << "Streaming video at an estimated " << cap.get(CV_CAP_PROP_FPS) << " FPS" << endl;
	
	Mat frame;
	while(cap.read(frame))
	{
		Mat frame_HSV; //2 frame buffers
	
		//frame = imread("eye.jpg"); //load the frame image

		resize(frame, frame, Size(), .75, .75);
		//resize(frame, frame, Size(), 3, 3);


		//Switch to HSV view
		cvtColor(frame, frame_HSV, CV_BGR2HSV);
		//Smooth the image 
		GaussianBlur(frame_HSV, frame_HSV, Size(9,9), 2, 2);
		
		Mat brown_mask;
		//Mask out everything except the blackest parts of the image
		inRange(frame_HSV, Scalar(0, 0,0), Scalar(255,255,15), brown_mask);
	
		//Draw the contours for the isolated pupil
		vector<vector<Point> > contours;
		vector<Vec4i> hierarchy;
		findContours(brown_mask, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0,0));
		
		Mat contour_edges = Mat::zeros(brown_mask.size(), CV_8UC3);
		vector<double> areas;
		//Find the largest contour, and assume it bounds the pupil
		for(int j = 0; j < contours.size(); j++)
			areas.push_back(contourArea(contours[j]));

		cout << "Est. pupil area = " << *max_element(areas.begin(), areas.end()) << endl;
		drawContours( contour_edges, contours, -1, Scalar(255,0,0), -1,8, hierarchy, 0, Point());
		areas.clear();	

		namedWindow("W1");
		imshow("W1", contour_edges|frame);

		waitKey(0);

		destroyWindow("W1");
		
	}

	return 0;
}
