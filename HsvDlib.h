#pragma once
#include <dlib/opencv.h>
#include <dlib/image_processing.h>
#include <opencv2/highgui.hpp>
#include <string>
#include "renderFace.hpp"
#include "faceBlendCommon.hpp"
#include "dirent.h"
#include <dlib/image_processing/frontal_face_detector.h>


// The landmark detector is implemented in the shape_predictor class
static dlib::shape_predictor landmarkDetector;
static dlib::shape_predictor landmarkDetectorFaceRec;
void leftClick_HSVDlib();
void detect_landmark_HSVDlib(cv::Mat);
int tracking_HSVDlib(cv::Mat, double&, double&, int);
double mouthRatio(cv::Mat, int, int, int, int, bool);
void mouthMouseClick(cv::Mat, double, double, int&, int&, bool);
int HsvDlib(int, int, int, int);
static void listdir(std::string , std::vector<std::string>& , std::vector<std::string>& , std::vector<std::string>& );
static void filterFiles(std::string , std::vector<std::string>& , std::vector<std::string>& , std::string , std::vector<int>& , int );
template<typename T>
static void printVector(std::vector<T>& );
static int enrollOpenFace(dlib::frontal_face_detector , dlib::shape_predictor , dnn::Net );
void readLabelNameMap(const std::string& , std::vector<std::string>& , std::vector<int>& ,
    std::map<int, string>& , char = ';');
void readDescriptors(const std::string& , std::vector<int>& , std::vector<Mat>& , char = ';');
void nearestNeighbor(Mat& ,
    std::vector<Mat>& ,
    std::vector<int>& , int& , float& );
