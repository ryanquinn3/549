#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>

#define threshold_bins 30
#define INIT_THROWAWAY_FRAMES 8

using namespace std;
using namespace cv;

int main()
{

	VideoCapture cap("../andy_dark.h264");
	if(!cap.isOpened())
	{
		cout << "Failed to open video stream" << endl;
		return 1;
	}

	cout << "Streaming video at an estimated " << cap.get(CV_CAP_PROP_FPS) << " FPS" << endl;
	

	Mat frame;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	namedWindow("W1");
	namedWindow("V");
	namedWindow("contours");
	namedWindow("eroded");

	int frame_num = 0;	
	float prev_ratio, ratio;
	prev_ratio = 1.0;

	while(cap.read(frame))
	{
		if(frame_num < INIT_THROWAWAY_FRAMES)
		{ 
			frame_num++;
			continue;
		}

		Mat frame_HSV; //2 frame buffers
	
		//frame = imread("./eye3.jpg"); //load the frame image

		resize(frame, frame, Size(), .25, 0.25);
		cvtColor(frame, frame_HSV, CV_BGR2HSV);

		vector<Mat> hsv_planes;
		split(frame_HSV, hsv_planes);
		int histSize = 256;

		float range[] = {0, 256};
		const float* histRange = {range};

		Mat v_hist;
		bool uniform = 1; bool accumulate = 0;

  		calcHist( &hsv_planes[2], 1, 0, Mat(), v_hist, 1, &histSize, &histRange, uniform, accumulate );

  		int hist_w = 512; int hist_h = 400;
  		int bin_w = cvRound( (double) hist_w/histSize );
  		Mat histImage( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );

  		normalize(v_hist, v_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );

		float black_sum = 0.0;
  	for(int i = 0; i < threshold_bins; i++)
		black_sum += v_hist.at<float>(i,0);

		Scalar total_sum = sum(v_hist);
		ratio = black_sum/(float)(total_sum[0]);
		cout << "Frame " << frame_num << ": Low-value ratio over " << threshold_bins << " bins = " << ratio << "; \% change = " << (ratio-prev_ratio)/prev_ratio * 100 <<  endl;
	
	prev_ratio = ratio;
	

  	/// Draw histogram
  	for( int i = 1; i < histSize; i++ )
  	{
  	    line( histImage, Point( bin_w*(i-1), hist_h - cvRound(v_hist.at<float>(i-1)) ) ,
   	                    Point( bin_w*(i), hist_h - cvRound(v_hist.at<float>(i)) ),
   	                    Scalar( 255, 0, 0), 2, 8, 0  );
  	}


	//Trial of erosion technique /////////////////////////////////////
	Mat src_gray;
	cvtColor(frame, src_gray, CV_BGR2GRAY);

  	int erosion_type = MORPH_ELLIPSE;
	Mat src, erosion_dst;

	int erosion_size = 10;

  	Mat element = getStructuringElement( erosion_type,
                                       Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                       Point( erosion_size, erosion_size ) );

  	/// Apply the erosion operation
  	erode( src_gray, erosion_dst, element );
	
	//GaussianBlur(erosion_dst, erosion_dst, Size(5,5), 0, 0);
	medianBlur(erosion_dst, erosion_dst, 35);	


	double minVal, maxVal; Point minLoc, maxLoc;
	minMaxLoc(erosion_dst, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );
	cout << "Pixel values (hsV): min = " << (minVal/255)*100 << "\%; max = " << (maxVal/255)*100 << "\%" << endl;

	Mat black_mask;
	//Mask out everything except the blackest parts of the image
	inRange(erosion_dst, Scalar(minVal), Scalar(minVal*1.67), black_mask);

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

		//Draw the contours for the isolated pupil
		findContours(black_mask, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0,0));
		
		Mat contour_edges = Mat::zeros(black_mask.size(), CV_8UC3);
		vector<double> areas;
		//Find the largest contour, and assume it bounds the pupil
		for(int j = 0; j < contours.size(); j++)
		{
			areas.push_back(contourArea(contours[j]));
			drawContours( contour_edges, contours, j, Scalar(255,0,0), 1,8, hierarchy, 0, Point());
		}

	imshow("eroded", erosion_dst);
	///////////////////////////////////////////////////////////////////


		imshow("W1", histImage);
		imshow("contours", frame|contour_edges);
		imshow("V", hsv_planes[2]);
		imwrite("V_out.jpg", erosion_dst);
		waitKey(0);
		frame_num++;

		
	}
	destroyWindow("W1");
	destroyWindow("W2");
	return 0;
}
