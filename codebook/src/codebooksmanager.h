#ifndef CODEBOOK_MANAGER_H
#define CODEBOOK_MANAGER_H

#include <vector>
#include "codebook.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"

// RGB ERROR
#define RGB_ERROR1 10.f
#define RGB_ERROR2 10.f

// LAB ERROR
#define LAB_MAX_DISTANCE_1 5.f
#define LAB_MAX_DISTANCE_2 6.f

// YCrCb ERROR
#define YCRCB_ERROR1_1 7.f
#define YCRCB_ERROR1_2 7.f
#define YCRCB_ERROR2_1 15.f
#define YCRCB_ERROR2_2 50.f //25.f

// HSV ERROR
#define HSV_ERROR1_1 7.f
#define HSV_ERROR1_2 7.f
#define HSV_ERROR2_1 30.f
#define HSV_ERROR2_2 25.f

// HLS ERROR
#define HLS_ERROR1 10.f
#define HLS_ERROR2 20.f

//#define CODEBOOK_SHOW_MASK
#define CODEBOOK_MASK_OPENING_OPERATION
//#define CODEBOOK_FILL_HOLES_MASK

class CodeBookManager
{
public:
  void CbImage ();
  void CbVideo ();
  void CbVideoStreaming ();

  void TrainFrame (cv::Mat frame, int n);
  cv::Mat GetSubtractionMask (cv::Mat frame);
  cv::Mat BackGroundSubtraction (cv::Mat frame, cv::Mat mask);
  
  void WrapAroundLambda (int N);    
  void RemoveUnderutilizedCodeWords (int N);

  void InitializeCodeBooks (cv::Mat frame);

  void Construct (cv::Mat cap);
  void Construct (int frameSpacing, cv::VideoCapture cap);
	void Construct (int NFrames, double delayPerFrame, cv::VideoCapture cap);

public:
  CodeBookManager (CodeBookColorSpace csp);
  CodeBookManager (int _im_cols, int _im_rows);
  ~CodeBookManager ();

  void AddCodebooks ();
  cv::Mat TransformFromBGR (cv::Mat frame);
  void GetColorAndThresholdFromSource (int tmode, cv::Mat_<cv::Vec3b> frame, int i, int j,
                                       float* px_colors, float* threshold);


  int im_cols, im_rows;
private:
  std::vector<CodeBook*> codebooks;
  CodeBookColorSpace cbook_colorspace;

  std::vector<std::string> GetPaths (const char* nfile);
};

#endif