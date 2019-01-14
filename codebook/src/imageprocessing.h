#ifndef IMAGE_PROCESSING_H
#define IMAGE_PROCESSING_H

/*OpenCV includes*/
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
/*OpenCV includes*/

#include <stack>
#include <queue>

#define N_FLOODFILLS 5000
#define NOT_CALLED 1
#define CALLED 0
#define MASKFLOODFILL_NOT_CALLED 0
#define MASKFLOODFILL_CALLED 1

cv::Mat erosion(cv::Mat frame, int sSize);

cv::Mat dilation(cv::Mat frame, int sSize);

cv::Mat Closing(cv::Mat frame, int sSize);

cv::Mat Opening(cv::Mat frame, int sSize);

cv::Mat histogramEqualization(cv::Mat frame);

cv::Mat CannyEdgeDetector(cv::Mat frame, int lowThreshold, int highThreshold);

cv::Mat histogramEqualWithCannyEdgeDetector(cv::Mat frame, int lowThreshold, int highThreshold);

cv::Mat fillTheHolesMask(cv::Mat mask);

cv::Mat detectForegroundByEdgesPendente(cv::Mat frame, int h, int w, cv::Mat mask);
                                                                                  
cv::Mat statisticalMask(cv::Mat frame, cv::Mat mask, int range);

#endif