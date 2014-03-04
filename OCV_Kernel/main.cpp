#include "opencv2/opencv.hpp"
#include "opencv2/opencv_modules.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <fstream>

#define threshold_bins 13
#define INIT_THROWAWAY_FRAMES 25
#define TEST_LENGTH_SECONDS 10
#define SLIDING_WINDOW_SIZE 20

//#define VISUAL_DEBUG

using namespace std;
using namespace cv;

//Call this program with the filename of the video file to analyze
int main(int argc, char** argv)
{
	if(argc != 2)
	{
		cout << "Error: Need to Provide a Video File!" << endl;
		return 1;
	}
	
	vector<float> sliding_area_average;
	vector<float> sliding_ratio_average;
	int sliding_index = 0;

	VideoCapture cap(argv[1]);
	int FPS = cap.get(CAP_PROP_FPS);

	if(!cap.isOpened())
	{
		cout << "Error: Failed to open video stream" << endl;
		return 1;
	}

	cout << "Streaming video at an estimated " << FPS << " FPS" << endl;
	
	Mat frame;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	#ifdef VISUAL_DEBUG
	namedWindow("contours");
	#endif

	ofstream frame_output("OCV_pupil_measurements.txt");

	int frame_num = 0;
	float R0 = 0;

	while(cap.read(frame))
	{
		

		frame_num++;
		cout << frame_num << endl;	
		if(frame_num < INIT_THROWAWAY_FRAMES)
		{
			//	frame_num++;
			//continue;
		}

		Mat frame_HSV; //2 frame buffers
	
		//This factor of 4x shrinking applies to video images at the 
		//RasPi resolution only - changing this factor will invalidate
		//the rest of the smoothing calibration
		resize(frame, frame, Size(), .25, .25);


		///////////FEATURE/EROSION DETECTION: METHOD 1 //////////////////
		Mat frame_gray;
		Mat src, erosion_dst;
		cvtColor(frame, frame_gray, COLOR_BGR2GRAY);

  		const int erosion_type = MORPH_ELLIPSE;
		const int erosion_size = 5;

  		Mat element = getStructuringElement( erosion_type,
                                       Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                       Point( erosion_size, erosion_size ) );

  		/// Apply the erosion operation
  		erode( frame_gray, erosion_dst, element );
	
		GaussianBlur(erosion_dst, erosion_dst, Size(5,5), 0, 0);
		//Don't blur too much, or the dark border of the eyelid will
		//mix with the pupil
		medianBlur(erosion_dst, erosion_dst, 11);

		double minVal, maxVal; Point minLoc, maxLoc;
		minMaxLoc(erosion_dst, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );

		Mat black_mask;
		//Mask out everything except the blackest parts of the image
		inRange(erosion_dst, Scalar(minVal), Scalar(minVal*1.25), black_mask);

		vector<vector<Point> > contours;
		vector<Vec4i> hierarchy;

		//Draw the contours for the isolated pupil
		findContours(black_mask, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0,0));
		
		Mat contour_edges = Mat::zeros(black_mask.size(), CV_8UC3);
		vector<double> areas;
		//Find the largest contour, and assume it bounds the pupil
		for(int j = 0; j < contours.size(); j++)
		{
			areas.push_back(contourArea(contours[j]));
			drawContours( contour_edges, contours, j, Scalar(255,0,0), 1,8, hierarchy, 0, Point());
		}

		double max_area = 0; 
		int max_loc = distance(areas.begin(), max_element(areas.begin(), areas.end()));
		if(areas.size() != 0)
			max_area = *max_element(areas.begin(), areas.end());
		continue;
		//////////////////////////////////////////////////////////////////


		/////////////////HISTOGRAM DETECTION - METHOD 2////////////////////
		cvtColor(frame, frame_HSV, COLOR_BGR2HSV);

		//1) Look only @ the V part of the image
		//2) Look only @ the ROI surrounding the contours found above
		vector<Mat> hsv_planes;
		split(frame_HSV, hsv_planes);

		//Find the circle that encompasses the contour above;
		//this is probably (most of) the pupil
		Point2f ROI_center; float ROI_radius;
		minEnclosingCircle( contours[0], ROI_center, ROI_radius);

		//Expand the circle to get a wider ROI, 
		//BUT keep a constant size after the first iteration
		if(frame_num - INIT_THROWAWAY_FRAMES == 0)
			R0 = ROI_radius;

		Mat circle_masking_frame =  255*Mat::ones(frame.size(), CV_8UC1);
		circle(circle_masking_frame, ROI_center, R0, Scalar(0), -1,8,8);
		//Isolate part of the V channel to make our final ROI
		hsv_planes[2] |= circle_masking_frame; 

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
  		for(int i = (int)minVal; i < (int)minVal+threshold_bins; i++)
			black_sum += v_hist.at<float>(i,0);

		Scalar total_sum = sum(v_hist);
		float ratio = black_sum/(float)(total_sum[0]);
		///////////////////////////////////////////////////////////////

		//Calculate a sliding window average to smooth out noise-per-frame
		if(sliding_area_average.size() < SLIDING_WINDOW_SIZE)
		{
			//sliding_area_average.push_back(max_area);
			sliding_ratio_average.push_back(ratio);
		}
		else
		{
			float area_avg = 0; float ratio_avg = 0;

			//sliding_area_average[sliding_index] = max_area;
			sliding_ratio_average[sliding_index] = ratio;
			for(int q = 0; q < SLIDING_WINDOW_SIZE; q++)
			{
				//area_avg += sliding_area_average[q];
				ratio_avg += sliding_ratio_average[q];
			}

			sliding_index++;
			if(sliding_index == SLIDING_WINDOW_SIZE) sliding_index = 0;

			//area_avg /= SLIDING_WINDOW_SIZE;
			ratio_avg /= SLIDING_WINDOW_SIZE;

			#ifdef VISUAL_DEBUG
			cout << "Frame " << frame_num << ": Sliding Area Average = " << area_avg << endl;
			cout << "Sliding Ratio Average = " << ratio_avg << endl << endl;
			#endif

			//Write output in form FRAME;AREA;RATIO\n	
			//frame_output << frame_num-INIT_THROWAWAY_FRAMES << ";" 
				//<< area_avg << ";" << ratio_avg << endl;
		}
			
		#ifdef VISUAL_DEBUG
		imshow("contours", frame|contour_edges);
		waitKey(0);
		#endif

		frame_num++;
		if(frame_num >= (TEST_LENGTH_SECONDS*FPS)) break;
	}

	#ifdef VISUAL_DEBUG
	destroyWindow("contours");
	#endif
	frame_output.close();	
	return 0;
}
