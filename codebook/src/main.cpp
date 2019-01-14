#include <iostream>

#include <opencv2/opencv.hpp>

int main(int argc, char* argv[])
{
  std::cout << "Codebook Foreground-Background Subtraction" << std::endl;

  cv::String w_name = cv::String("Codebook Foreground-Background Subtraction");

  cv::namedWindow(w_name);

  cv::waitKey(0); // Wait for any keystroke in the window

  cv::destroyWindow(w_name);

  return 0;
}