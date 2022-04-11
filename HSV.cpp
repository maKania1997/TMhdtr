#include <opencv2/opencv.hpp>
#include <dlib/opencv.h>
#include <dlib/image_processing.h>
#include <opencv2/highgui.hpp>
#include <Windows.h>
#include <time.h>
#include <iostream>

#include <cstdio>
#include <ctime>
#include <WinUser.h>

#include <windows.h>


#include "HSV.h"
//#include "HsvDlib.h"
#include "INIReaderV2.h"
#include "Camera.h"
#include "Cursor.h"
#include "faceBlendCommon.hpp"
#include "dirent.h"
#include "renderFace.hpp"
#include "HsvDlib.h"

using namespace cv;
using namespace dlib;
using namespace std;

#define recThreshold 0.8

namespace {
    double mouse_x = 0;
    double mouse_y = 0;
    double filtered_mouse_x = 0;
    double filtered_mouse_y = 0;
    double reale_x[2];
    double reale_y[2];
    int array_counter = 0;
    int timer_counter = 0;
    bool condition = true;
    double last_x_pos[1];
    double last_y_pos[1];
    bool firstTry = true;
    int face_rec_counter = 0;
    Mat img(600, 800, CV_8UC3);
}


void leftClick_HSV() {
    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}


int tracking_Rect(Mat face_frame, const char* iniFile) {
    INIReader reader(iniFile);
    if (reader.ParseError() != 0) {
        std::cout << "Can't load 'ini file. check if file exists or if the name is correct'\n";
        return 1;
    }

    string l1 = reader.Get("main", "lh", "200");
    string l2 = reader.Get("main", "ls", "200");
    string l3 = reader.Get("main", "lv", "200");

    string u1 = reader.Get("main", "uh", "200");
    string u2 = reader.Get("main", "us", "200");
    string u3 = reader.Get("main", "uv", "200");

    TermCriteria term_crit(TermCriteria::EPS | TermCriteria::COUNT, 10, 1);
    Rect track_window(0, 0, 70, 70);
    Mat hsv;
    cvtColor(face_frame, hsv, COLOR_BGR2HSV);
    //HSV VALUES(lower, upper) original
    /*int l_b[3] = { 81, 118, 69 };
    int u_b[3] = { 162, 255, 255 };*/
    ////Grenn
    //int l_b[3] = { 46, 35, 75 };
    //int u_b[3] = { 90, 255, 255 };
    ////Blue
    /*int l_b[3] = { 106,72,142 };
    int u_b[3] = { 128, 255, 255 };*/
    ////Purple
    //int l_b[3] = { 31, 54, 212 };
    //int u_b[3] = { 140, 255, 255 };

    int l_b1 = atoi(l1.c_str());
    int l_b2 = atoi(l2.c_str());
    int l_b3 = atoi(l3.c_str());

    int u_b1 = atoi(u1.c_str());
    int u_b2 = atoi(u2.c_str());
    int u_b3 = atoi(u3.c_str());

    int l_b[3] = { l_b1, l_b2, l_b3 };
    int u_b[3] = { u_b1, u_b2, u_b3 };



    /*SHOW ONLY COLORS IN RANGE*/
    Mat mask;
    inRange(hsv, Scalar(l_b[0], l_b[1], l_b[2]), Scalar(u_b[0], u_b[1], u_b[2]), mask);
    
    RotatedRect rot_rect = CamShift(mask, track_window, term_crit);
    Point2f points[4];

    /*track_window.width = 20;
    track_window.height = 20;*/

    //rectangle(face_frame, track_window, Scalar(255, 255, 255), 2);

   
    
    rot_rect.points(points);
    //circle(face_frame, points[1], 10, Scalar(255, 255, 255), 1);
    for (int i = 0; i < 4; i++) {
        line(face_frame, points[i], points[(i + 1) % 4], 255, 2);
    }
    int x1 = points[0].x;
    int x2 = points[2].x;
    int y1 = points[0].y;
    int y2 = points[2].y;
    
    /*int x1 = track_window.x;
    int x2 = track_window.x + track_window.width;
    int y1 = track_window.y;
    int y2 = track_window.y + track_window.height;*/
    int x_m_point = (x1 + x2) / 2;
    int y_m_point = (y1 + y2) / 2;


    cv::Vec3b color;
    color[0] = 0;
    color[1] = 0;
    color[2] = 255;
    circle(face_frame, Point(x_m_point, y_m_point), 3, color, -1);
    cv::imshow("MASK", mask);
}


int tracking_HSV(Mat face_frame, double& x, double& y, int camera_state) {
    TermCriteria term_crit(TermCriteria::EPS | TermCriteria::COUNT, 10, 1);
    Rect track_window(0, 0, 70, 70);
    Mat hsv;
    cvtColor(face_frame, hsv, COLOR_BGR2HSV);
    //HSV VALUES(lower, upper) original
    int l_b[3] = { 0, 200, 200 };
    int u_b[3] = { 0, 255, 255 };

    /*SHOW ONLY COLORS IN RANGE*/
    Mat mask;
    inRange(hsv, Scalar(l_b[0], l_b[1], l_b[2]), Scalar(u_b[0], u_b[1], u_b[2]), mask);
    RotatedRect rot_rect = CamShift(mask, track_window, term_crit);
    //track_window.width = 10;
    //track_window.height = 10;

    //rectangle(face_frame, track_window, Scalar(0, 0, 0), 2);
    Point2f points[4];

    rot_rect.points(points);
    for (int i = 0; i < 4; i++) {
        line(face_frame, points[i], points[(i + 1) % 4], 255, 2);
    }
    int x1 = points[0].x;
    int x2 = points[2].x;
    int y1 = points[0].y;
    int y2 = points[2].y;

    /*int x1 = track_window.x;
    int x2 = track_window.x + track_window.width;
    int y1 = track_window.y;
    int y2 = track_window.y + track_window.height;*/
    int x_m_point = (x1 + x2) / 2;
    int y_m_point = (y1 + y2) / 2;
    
    if (x_m_point < 1 && y_m_point < 1) {
        x = 1920;
        y = 0;
        //std::cout << "not found" << endl;
    }
    else {
        x = x_m_point;
        y = y_m_point;
    }
    if (camera_state == 1) {
        //imshow("mask", mask);

    }

    return x, y;
}





int HSV(int camera_state, int mouseClickMethod, int camera_number, int face_rec_method) {

    ////////////////////////////////////////////////////////////////////////////FACE REC PART///////////////////////////////////////////////////////////

    // Initialize face detector, facial landmarks detector and face recognizer
    const std::string recModelPath = "data/models/openface.nn4.small2.v1.t7";
    frontal_face_detector faceDetectorHSV = get_frontal_face_detector();
    dnn::Net recModel = dnn::readNetFromTorch(recModelPath);
    dlib::shape_predictor landmarkDetectorFaceRec;
    dlib::deserialize("data/models/shape_predictor_5_face_landmarks.dat") >> landmarkDetectorFaceRec;

    

    // read names, labels and labels-name-mapping from file
    std::map<int, string> labelNameMap;
    std::vector<string> names;
    std::vector<int> labels;
    const string labelNameFile = "label_name_openface.txt";
    readLabelNameMap(labelNameFile, names, labels, labelNameMap);

    // read descriptors of enrolled faces from file
    const string faceDescriptorFile = "descriptors_openface.csv";
    std::vector<int> faceLabels;
    std::vector<Mat> faceDescriptors;
    char sign = ';';
    readDescriptors(faceDescriptorFile, faceLabels, faceDescriptors, sign);
    //printVector(faceDescriptors);
    
    ////////////////////////////////////////////////////////////////////////////FACE REC PART///////////////////////////////////////////////////////////
    std::cout << "TEST001HSV";
    cv::VideoCapture cap(camera_number);
    if (!cap.isOpened())
        return -1;

   /* HCURSOR def_arrow_cur = CopyCursor(LoadCursor(0, IDC_ARROW));
    cursor_Image("mouse.cur", 32512, def_arrow_cur, true);*/

    int camera_window_pos_x = 0;
    int camera_window_pos_y = 0;
    camera_window_position(camera_window_pos_x, camera_window_pos_y);
    int face_rec_counter = 0;
    cv::Mat face_frame;
    cv::Mat rect_tracking_frame;
    int face_frame_x = 0;
    int face_frame_y = 0;
    int face_frame_width = 1;
    int face_frame_height = 1;
    int tracking_frame_y = 0;
    int tracking_frame_height = 1;
    int tracking_frame_x = 0;
    int tracking_frame_width = 1;
    int no_face_detected_counter = 0;
    int face_lost = 0;
    int set_first_face_frame = 0;
    std::vector<dlib::rectangle> faceRects;

    KalmanFilter KF(4, 2, 0);
    POINT mousePos;
    GetCursorPos(&mousePos);
    // intialization of KF...
    KF.transitionMatrix = (Mat_<float>(4, 4) << 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1);
    Mat_<float> measurement(2, 1); measurement.setTo(Scalar(0));
    KF.statePost.at<float>(0) = mouse_x;
    KF.statePost.at<float>(1) = mouse_y;
    KF.statePre.at<float>(2) = 0;
    KF.statePre.at<float>(3) = 0;
    setIdentity(KF.measurementMatrix);
    setIdentity(KF.processNoiseCov, Scalar::all(1e-3));
    setIdentity(KF.measurementNoiseCov, Scalar::all(5));
    setIdentity(KF.errorCovPost, Scalar::all(.1));
    // Image to show mouse tracking

    std::vector<Point> mousev, kalmanv;
    mousev.clear();
    kalmanv.clear();

    /*std::cout << "\nPress Enter to procced\n";
    cin.ignore();*/
    for (;;) {
        int blink_counter = 0;
        int double_blink_counter = 0;
        //std::cout << "TEST006";
        Mat frame;
        cap >> frame;
        /*Mat rect_tracking_frame = frame(Rect(280, 200, 70, 50));
        cv::Mat face_frame = frame(cv::Rect(245, 150, 180, 250));*/
        cv::Mat face_frame = frame(cv::Rect(face_frame_x, face_frame_y, face_frame_width, face_frame_height));
        //cout << "FRAME SIZE: " << frame.size << endl;
       /* cout << "FIRST TRY" << endl;

        cout << "FACE X: " << face_frame_x << endl;
        cout << "FACE Y: " << face_frame_y << endl;
        cout << "FACE WIDTH: " << face_frame_width << endl;
        cout << "FACE HEIGHT: " << face_frame_height << endl;*/
        //
        ////cout << "TRACKING X: " << tracking_frame_height << endl;
        //cout << "TRACKING Y: " << face_frame_y << endl;
        //cout << "TRACKING WIDTH: " << face_frame_width << endl;
        //cout << "TRACKING HEIGHT: " << tracking_frame_height << endl;
        tracking_frame_y = (face_frame_y * 1 / 100);
        tracking_frame_height = face_frame_height - tracking_frame_y - (face_frame_height * 70 / 100);
        tracking_frame_x = (face_frame_x * 10 / 100);
        tracking_frame_width = face_frame_width - tracking_frame_x - (face_frame_width * 15 / 100);
        ////////////////////////////////////////////////////////////////////////////FACE REC PART///////////////////////////////////////////////////////////
        //cout << "rec meth: " << face_rec_method << endl;
        if (set_first_face_frame != 0) {
            //tracking_frame = face_frame(cv::Rect(0, 20, face_frame_width, 50));
            //tracking_frame = face_frame(cv::Rect(0, tracking_frame_y, face_frame_width, tracking_frame_height));
            rect_tracking_frame = face_frame(cv::Rect(tracking_frame_x, tracking_frame_y, tracking_frame_width, tracking_frame_height));
        }
        else {
            rect_tracking_frame = face_frame(cv::Rect(0, 0, face_frame_width, face_frame_height));

        }
        cv_image<bgr_pixel> imDlib(frame);

        // detect faces in image
        std::vector<dlib::rectangle> faceRects = faceDetectorHSV(imDlib);
        // Now process each face we found
        if (face_rec_counter < 40 && face_rec_method == 1)
        {
            for (int i = 0; i < faceRects.size(); i++) {
                /*cout << faceRects.size() << " Face(s) Found" << endl;*/

                Mat alignedFace;
                alignFace(face_frame, alignedFace, faceRects[i], landmarkDetectorFaceRec, cv::Size(96, 96));
                cv::Mat blob = dnn::blobFromImage(alignedFace, 1.0 / 255, cv::Size(96, 96), Scalar(0, 0, 0), false, false);
                recModel.setInput(blob);
                Mat faceDescriptorQuery = recModel.forward();

                // Find closest face enrolled to face found in frame
                int label;
                float minDistance;
                nearestNeighbor(faceDescriptorQuery, faceDescriptors, faceLabels, label, minDistance);
                // Name of recognized person from map
                string name = labelNameMap[label];


                //// Draw a rectangle for detected face
                //Point2d p1 = Point2d(faceRects[i].left(), faceRects[i].top());
                //Point2d p2 = Point2d(faceRects[i].right(), faceRects[i].bottom());
                //cv::rectangle(face_frame, p1, p2, Scalar(0, 0, 255), 1, LINE_8);

                //// Draw circle for face recognition
                //Point2d center = Point((faceRects[i].left() + faceRects[i].right()) / 2.0,
                //    (faceRects[i].top() + faceRects[i].bottom()) / 2.0);
                //int radius = static_cast<int> ((faceRects[i].bottom() - faceRects[i].top()) / 2.0);
                //cv::circle(face_frame, center, radius, Scalar(0, 255, 0), 1, LINE_8);

                // Write text on image specifying identified person and minimum distance
                stringstream stream;
                stream << "Hallo " << name << " ";
                //stream << fixed << setprecision(4) << minDistance;
                string text = stream.str();
                cv::putText(face_frame, text, cv::Point2d(0, 20), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 255, 255), 2);
            }
            face_rec_counter++;
        }
        ////////////////////////////////////////////////////////////////////////////FACE REC PART///////////////////////////////////////////////////////////
        if (set_first_face_frame == 0) {
            //std::cout << "\nSetting up first frame\n";
            cv_image<bgr_pixel> imDlib(frame);

            // detect faces in image
            faceRects = faceDetectorHSV(imDlib);
        }
        else {
            //std::cout << "\nFrame setted\n";

            cv_image<bgr_pixel> imDlib(face_frame);

            // detect faces in image
            faceRects = faceDetectorHSV(imDlib);
        }

        //take cordinates from faceRects for face_frame
        if (faceRects.size() == 1 && set_first_face_frame == 0) {
            faceRects = faceDetectorHSV(imDlib);

            try {
                if (faceRects.size() == 0 || faceRects[0].left() > 460 || faceRects[0].top() > 230) {
                    //cout << "Faces num: " << faceRects.size() << endl;
                    /*cout << "Left: " << faceRects[0].left() << endl;
                    cout << "Top: " << faceRects[0].top() << endl;*/

                    throw 99;
                }
                else {
                    face_frame_x = faceRects[0].left();
                    face_frame_y = faceRects[0].top();
                    face_frame_width = faceRects[0].right() - faceRects[0].left();
                    face_frame_height = faceRects[0].bottom() - faceRects[0].top();
                }
            }
            catch (int x) {
                cout << "ASSERTION ERROR, ERROR NUMBER: " << x << endl;
                no_face_detected_counter = 0;
            }


            face_lost++;
            set_first_face_frame++;
            //std::cout << "\nFirst frame\n";

        }
        //     cout << faceRects.size();
        // 
             //when face lost
        if (faceRects.size() == 0) {
            no_face_detected_counter++;
            cout << no_face_detected_counter;

            if (no_face_detected_counter == 40) {
                //SetCursorPos(GetSystemMetrics(0) / 2, GetSystemMetrics(1) / 2);
                //cout << faceRects.size();
                //cv::putText(frame, "Kein Gesicht", cv::Point2d(0, 20), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 255, 255), 2);
                //cv_image<bgr_pixel> imDlibNoFace(frame);

                // detect faces in image
                face_lost++;
                try {
                    faceRects = faceDetectorHSV(imDlib);
                    //face cant be cut outside off the frame boudnaries
                    if (faceRects.size() == 0 || faceRects[0].left() > 460 || faceRects[0].top() > 230) {
                        throw 99;
                    }
                    else {
                        face_frame_x = faceRects[0].left();
                        face_frame_y = faceRects[0].top();
                        face_frame_width = faceRects[0].right() - faceRects[0].left();
                        face_frame_height = faceRects[0].bottom() - faceRects[0].top();
                        no_face_detected_counter = 0;
                    }

                }
                catch (int x) {
                    cout << "ASSERTION ERROR, ERROR NUMBER: " << x << endl;
                    no_face_detected_counter = 0;
                }
                //Draw a rectangle for detected face
                //cv::circle(frame, cv::Point(faceRects[0].left(), faceRects[0].top()), 3, 255, -1);
                //imshow("Found Face frame", found_face_frame);
                /*Point2d p1 = Point2d(faceRects[0].left(), faceRects[0].top());
                Point2d p2 = Point2d(faceRects[0].right(), faceRects[0].bottom());
                cv::rectangle(frame, p1, p2, Scalar(0, 0, 255), 1, LINE_8);*/
            }

        }
        else {
            no_face_detected_counter = 0;
        }


        tracking_Rect(rect_tracking_frame, ".\\TMheadtrack_color.ini");
        tracking_HSV(rect_tracking_frame, mouse_x, mouse_y, camera_state);

        //if (mouseClickMethod == 2) {
        //    double ratio = mouthRatio(face_frame2, 60, 64, 51, 57, false); //mouth
        //    mouthMouseClick(face_frame2, ratio, 1.4, blink_counter, double_blink_counter, false); //click for mouth
        //}

         ////////////////////////////////////////////////////////////////////////////////KALMAN//////////////////////////////////////////


    // First predict, to update the internal statePre variable
        Mat prediction = KF.predict();
        Point predictPt(prediction.at<float>(0), prediction.at<float>(1));

        // Get mouse point
        GetCursorPos(&mousePos);
        measurement(0) = mouse_x;
        measurement(1) = mouse_y;

        // The update phase 
        Mat estimated = KF.correct(measurement);

        Point statePt(estimated.at<float>(0), estimated.at<float>(1));
        Point measPt(measurement(0), measurement(1));
        // plot points
        img = Scalar::all(0);

        mousev.push_back(measPt);
        kalmanv.push_back(statePt);
        /*cout << "statePt: " << statePt << endl;
        cout << "meastPt: " << measPt << endl;*/

        circle(rect_tracking_frame, statePt, 6, Scalar(255, 255, 0), 1); //blue
        circle(rect_tracking_frame, measPt, 10, Scalar(255, 255, 255), 1); //yellow

        /*drawCross(statePt, Scalar(255, 255, 255), 5);
        drawCross(measPt, Scalar(0, 0, 255), 5);*/
        //putText(img, statePt,)
        /*for (int i = 0; i < mousev.size() - 1; i++)
            line(face_frame, mousev[i], mousev[i + 1], Scalar(255, 255, 0), 1);

        for (int i = 0; i < kalmanv.size() - 1; i++)
            line(face_frame, kalmanv[i], kalmanv[i + 1], Scalar(0, 155, 255), 1);*/

            ////////////////////////////////////////////////////////////////////////////////KALMAN//////////////////////////////////////////



        /*double real_x = (((double)mouse_x / rect_tracking_frame.size().width) * GetSystemMetrics(0));
        double real_y = (((double)mouse_y / rect_tracking_frame.size().height) * GetSystemMetrics(1));
        double reversed_x = GetSystemMetrics(0) - real_x;*/
        

        double estimated_x = statePt.x;
        double estimated_y = statePt.y;
        /*cout << "Est x: " << estimated_x << endl;
        cout << "Est y: " << estimated_y << endl;*/

        double real_x = (((double)estimated_x / rect_tracking_frame.size().width) * GetSystemMetrics(0));
        double real_y = (((double)estimated_y / rect_tracking_frame.size().height) * GetSystemMetrics(1));
        double reversed_x = GetSystemMetrics(0) - real_x;

        SetCursorPos(reversed_x, real_y);

    //    /*cout << "X: " << real_x << '\n';*/
    //    //cout << "Y: " << real_y << '\n';
    //    /*cout << "MonitorX: " << GetSystemMetrics(0) << '\n';
    //    cout << "MonitorY: " << GetSystemMetrics(1) << '\n';
    //    cout << "FrameX: " << face_frame.size().width << '\n';
    //    cout << "FrameY: " << face_frame.size().height << '\n';*/

        if (array_counter == 0) {
            reale_x[0] = reversed_x;
            reale_y[0] = real_y;
            array_counter += 1;
        }
        else if (array_counter != 0) {
            reale_x[1] = reale_x[0];
            reale_x[0] = reversed_x;
            reale_y[1] = reale_y[0];
            reale_y[0] = real_y;
            array_counter = 0;
        }
        double difference_x = abs(reale_x[0] - reale_x[1]);
        double difference_y = abs(reale_y[0] - reale_y[1]);

        //cout << "DIFF: " << difference_x << '\n';
        /*cout << "DIFF: " << difference_y << '\n';*/

        /*cout << "0: " << reale_x[0] << '\n';
        cout << "1: " << reale_x[1] << '\n';*/
        //cout << "Timer: " << timer_counter << '\n';

        if (difference_x < 27.5 && difference_y < 21.65) {
            timer_counter += 1;
            if (20 < timer_counter) {
                if (firstTry) {
                    last_x_pos[0] = reversed_x;
                    last_y_pos[0] = real_y;

                    firstTry = false;
                }
                SetCursorPos(last_x_pos[0], last_y_pos[0]);
            }

            if (timer_counter == 30 && mouseClickMethod == 1) {
                //cout << "CLICK" << '\n';
                //leftClick_HSV();
            }
        }
        else if (difference_x > 27.5 || difference_y > 21.65) {
            timer_counter = 0;
            firstTry = true;
            SetCursorPos(reversed_x, real_y);
        }

        if (camera_state == 1) {
            //imshow("frame", frame);
            //imshow("Tracking frame", rect_tracking_frame);
            imshow("Face frame", face_frame);
        }
        
        
        //camera_InFront("Face frame", camera_window_pos_x, camera_window_pos_y);

        if (waitKey(30) >= 0)
            break;
    }
    //cursor_Image("mouse.cur", 32512, def_arrow_cur, false);

    return 0;
}