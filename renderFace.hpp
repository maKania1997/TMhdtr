#ifndef BIGVISION_renderFace_H_
#define BIGVISION_renderFace_H_
#include <dlib/image_processing/frontal_face_detector.h>
#include <opencv2/opencv.hpp>


// Draw an open or closed polygon between
// start and end indices of full_object_detection
static void drawPolyline
(
  cv::Mat &img,
  const dlib::full_object_detection& landmarks,
  const int start,
  const int end,
  bool isClosed = false
)
{
    std::vector <cv::Point> points;
    for (int i = start; i <= end; ++i)
    {
        points.push_back(cv::Point(landmarks.part(i).x(), landmarks.part(i).y()));
    }
    cv::polylines(img, points, isClosed, cv::Scalar(255, 200,0), 2, 16);

}

// Draw face for the 68-point model.
static void renderFace(cv::Mat& img, const dlib::full_object_detection& landmarks)
{
    drawPolyline(img, landmarks, 27, 30);           // Jaw line

}

// Draw points on an image.
// Works for any number of points.
static void renderFace
(
  cv::Mat &img, // Image to draw the points on
  const std::vector<cv::Point2f> &points, // Vector of points
  cv::Scalar color, // color points
  int radius = 3) // Radius of points.
{

  for (int i = 0; i < points.size(); i++)
  {
    cv::circle(img, points[i], radius, color, -1);
  }

}


#endif // BIGVISION_renderFace_H_
