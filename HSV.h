#pragma once
#include <opencv2/opencv.hpp>
void leftClick_HSV();
int tracking_Rect(cv::Mat, double&, double&, int);
int tracking_HSV(cv::Mat, double&, double&, int);
int HSV(int, int, int, int);