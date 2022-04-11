//DLIB I HSV
#include <dlib/opencv.h>
#include <dlib/image_processing.h>
#include <opencv2/highgui.hpp>
#include "renderFace.hpp"
#include "HsvDlib.h"
#include "INIReaderV2.h"
#include <iostream>
#include <cstdio>
#include <ctime>
#include <WinUser.h>
#include "Camera.h"
#include "Cursor.h"
#include "faceBlendCommon.hpp"
#include "dirent.h"

#include <windows.h>

using namespace dlib;
using namespace std;
using namespace cv;

#define recThreshold 0.8

//// Get the face detector
frontal_face_detector faceDetector = get_frontal_face_detector();
//// The landmark detector is implemented in the shape_predictor class
//shape_predictor landmarkDetector;
//shape_predictor landmarkDetectorFaceRec;




namespace {
    double mouse_x = 0;
    double mouse_y = 0;
    double reale_x[2];
    double reale_y[2];
    int array_counter = 0;
    int timer_counter = 0;
    bool condition = true;
    double last_x_pos[1];
    double last_y_pos[1];
    bool firstTry = true;
    Mat img(600, 800, CV_8UC3);
    int last_mouse_x_pos[1];
    int last_mouse_y_pos[1];

}

#define drawCross( center, color, d )                                 \
line( img, Point( center.x - d, center.y - d ), Point( center.x + d, center.y + d ), color, 2); \
line( img, Point( center.x + d, center.y - d ), Point( center.x - d, center.y + d ), color, 2)




// Reads files, folders and symbolic links in a directory
void listdir(string dirName, std::vector<string>& folderNames, std::vector<string>& fileNames, std::vector<string>& symlinkNames) {
    DIR* dir;
    struct dirent* ent;

    if ((dir = opendir(dirName.c_str())) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir(dir)) != NULL) {
            // ignore . and ..
            if ((strcmp(ent->d_name, ".") == 0) || (strcmp(ent->d_name, "..") == 0)) {
                continue;
            }
            string temp_name = ent->d_name;
            // Read more about file types identified by dirent.h here
            // https://www.gnu.org/software/libc/manual/html_node/Directory-Entries.html
            switch (ent->d_type) {
            case DT_REG:
                fileNames.push_back(temp_name);
                break;
            case DT_DIR:
                folderNames.push_back(dirName + "/" + temp_name);
                break;
            case DT_LNK:
                symlinkNames.push_back(temp_name);
                break;
            default:
                break;
            }
            // cout << temp_name << endl;
        }
        // sort all the files
        std::sort(folderNames.begin(), folderNames.end());
        std::sort(fileNames.begin(), fileNames.end());
        std::sort(symlinkNames.begin(), symlinkNames.end());
        closedir(dir);
    }
}

// filter files having extension ext i.e. jpg
void filterFiles(string dirPath, std::vector<string>& fileNames, std::vector<string>& filteredFilePaths, string ext, std::vector<int>& imageLabels, int index) {
    for (int i = 0; i < fileNames.size(); i++) {
        string fname = fileNames[i];
        if (fname.find(ext, (fname.length() - ext.length())) != std::string::npos) {
            filteredFilePaths.push_back(dirPath + "/" + fname);
            imageLabels.push_back(index);
        }
    }
}

template<typename T>
void printVector(std::vector<T>& vec) {
    for (int i = 0; i < vec.size(); i++) {
        cout << i << " " << vec[i] << "; ";
    }
    cout << endl;
}

int enrollOpenFace(frontal_face_detector faceDetector, dlib::shape_predictor landmarkDetector, dnn::Net recModel) {
    //// Initialize face detector and face recognize
    //const std::string recModelPath = "../data/models/openface.nn4.small2.v1.t7";
    //frontal_face_detector faceDetector = get_frontal_face_detector();
    //dnn::Net recModel = dnn::readNetFromTorch(recModelPath);
    //dlib::shape_predictor landmarkDetector;
    //dlib::deserialize("../data/models/shape_predictor_5_face_landmarks.dat") >> landmarkDetector;

    // Now let's prepare our training data
    // data is organized assuming following structure
    // faces folder has subfolders.
    // each subfolder has images of a person
    string faceDatasetFolder = "data/images/faces";
    std::vector<string> subfolders, fileNames, symlinkNames;
    // fileNames and symlinkNames are useless here
    // as we are looking for sub-directories only
    listdir(faceDatasetFolder, subfolders, fileNames, symlinkNames);

    // names: vector containing names of subfolders i.e. persons
    // labels: integer labels assigned to persons
    // labelNameMap: dict containing (integer label, person name) pairs
    std::vector<string> names;
    std::vector<int> labels;
    std::map<int, string> labelNameMap;
    // add -1 integer label for un-enrolled persons
    names.push_back("unknown");
    labels.push_back(-1);

    // imagePaths: vector containing imagePaths
    // imageLabels: vector containing integer labels corresponding to imagePaths
    std::vector<string> imagePaths;
    std::vector<int> imageLabels;

    // variable to hold any subfolders within person subFolders
    std::vector<string> folderNames;
    // iterate over all subFolders within faces folder
    for (int i = 0; i < subfolders.size(); i++) {
        string personFolderName = subfolders[i];
        // remove / or \\ from end of subFolder
        std::size_t found = personFolderName.find_last_of("/\\");
        string name = personFolderName.substr(found + 1);
        // assign integer label to person subFolder
        int label = i;
        // add person name and label to vectors
        names.push_back(name);
        labels.push_back(label);
        // add (integer label, person name) pair to map
        labelNameMap[label] = name;

        // read imagePaths from each person subFolder
        // clear vectors
        folderNames.clear();
        fileNames.clear();
        symlinkNames.clear();
        // folderNames and symlinkNames are useless here
        // as we are only looking for files here
        // read all files present in subFolder
        listdir(subfolders[i], folderNames, fileNames, symlinkNames);
        // filter only jpg files
        filterFiles(subfolders[i], fileNames, imagePaths, "jpg", imageLabels, i);
    }

    // process training data
    // We will store face descriptors in vector faceDescriptors
    // and their corresponding labels in vector faceLabels
    std::vector<Mat> faceDescriptors;
    // std::vector<cv_image<bgr_pixel> > imagesFaceTrain;
    std::vector<int> faceLabels;
    Mat faceDescriptor;
    // iterate over images
    for (int i = 0; i < imagePaths.size(); i++) {
        string imagePath = imagePaths[i];
        int imageLabel = imageLabels[i];

        std::cout << "processing: " << imagePath << endl;

        // read image using OpenCV
        Mat frame = cv::imread(imagePath);

        cv_image<bgr_pixel> imDlib(frame);
        std::vector<dlib::rectangle> faceRects = faceDetector(imDlib);
        std::cout << faceRects.size() << " Face(s) Found" << endl;
        // Now process each face we found
        for (int j = 0; j < faceRects.size(); j++) {
            Mat alignedFace;
            alignFace(frame, alignedFace, faceRects[j], landmarkDetector, cv::Size(96, 96));

            cv::Mat blob = dnn::blobFromImage(alignedFace, 1.0 / 255, cv::Size(96, 96), Scalar(0, 0, 0), false, false);
            recModel.setInput(blob);
            faceDescriptor = recModel.forward();

            // add face descriptor and label for this face to
            // vectors faceDescriptors and faceLabels
            faceDescriptors.push_back(faceDescriptor.clone());

            // add label for this face to vector containing labels corresponding to
            // vector containing face descriptors
            faceLabels.push_back(imageLabel);

        }
    }
    std::cout << "number of face descriptors " << faceDescriptors.size() << endl;
    std::cout << "number of face labels " << faceLabels.size() << endl;

    // write label name map to disk
    const string labelNameFile = "label_name_openface.txt";
    ofstream of;
    of.open(labelNameFile);
    for (int m = 0; m < names.size(); m++) {
        of << names[m];
        of << ";";
        of << labels[m];
        of << "\n";
    }
    of.close();

    // write face labels and descriptor to disk
    // each row of file descriptors_openface.csv has:
    // 1st element as face label and
    // rest 128 as descriptor values
    const string descriptorsPath = "descriptors_openface.csv";
    ofstream ofs;
    ofs.open(descriptorsPath);
    // write descriptors
    for (int m = 0; m < faceDescriptors.size(); m++) {
        Mat faceDescriptorVec = faceDescriptors[m];
        ofs << faceLabels[m];
        ofs << ";";
        for (int n = 0; n < faceDescriptorVec.cols; n++) {
            ofs << std::fixed << std::setprecision(8) << faceDescriptorVec.at<float>(n);
            // cout << n << " " << faceDescriptorVec.at<float>(n) << endl;
            if (n == (faceDescriptorVec.cols - 1)) {
                ofs << "\n";  // add ; if not the last element of descriptor
            }
            else {
                ofs << ";";  // add newline character if last element of descriptor
            }
        }
    }
    ofs.close();
    return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// END ENROLMENT PART ///////////////////////////////////////////////////////////////////


// read names and labels mapping from file
void readLabelNameMap(const string& filename, std::vector<string>& names, std::vector<int>& labels,
    std::map<int, string>& labelNameMap, char separator) {
    std::ifstream file(filename.c_str(), ifstream::in);
    if (!file) {
        string error_message = "No valid input file was given, please check the given filename.";
        CV_Error(CV_StsBadArg, error_message);
    }
    string line;
    string name, labelStr;
    // read lines from file one by one
    while (getline(file, line)) {
        stringstream liness(line);
        // read first word which is person name
        getline(liness, name, separator);
        // read second word which is integer label
        getline(liness, labelStr);
        if (!name.empty() && !labelStr.empty()) {
            names.push_back(name);
            // convert label from string format to integer
            int label = atoi(labelStr.c_str());
            labels.push_back(label);
            // add (integer label, person name) pair to map
            labelNameMap[label] = name;
        }
    }
}

// read descriptors saved on disk
void readDescriptors(const string& filename, std::vector<int>& faceLabels, std::vector<Mat>& faceDescriptors, char separator) {
    std::ifstream file(filename.c_str(), ifstream::in);
    if (!file) {
        string error_message = "No valid input file was given, please check the given filename.";
        CV_Error(CV_StsBadArg, error_message);
    }
    // each line has:
    // 1st element = face label
    // rest 128 elements = descriptor elements
    string line;
    string faceLabel;
    // valueStr = one element of descriptor in string format
    // value = one element of descriptor in float
    string valueStr;
    float value;
    std::vector<float> faceDescriptorVec;
    // read lines from file one by one
    while (getline(file, line)) {
        stringstream liness(line);
        // read face label
        // read first word on a line till separator
        getline(liness, faceLabel, separator);
        if (!faceLabel.empty()) {
            faceLabels.push_back(std::atoi(faceLabel.c_str()));
        }

        faceDescriptorVec.clear();
        // read rest of the words one by one using separator
        while (getline(liness, valueStr, separator)) {
            if (!valueStr.empty()) {
                // convert descriptor element from string to float
                faceDescriptorVec.push_back(atof(valueStr.c_str()));
            }
        }

        // convert face descriptor from vector of float to Dlib's matrix format
        Mat faceDescriptor(faceDescriptorVec);
        faceDescriptors.push_back(faceDescriptor.clone());
    }
}

// find nearest face descriptor from vector of enrolled faceDescriptor
// to a query face descriptor
void nearestNeighbor(Mat& faceDescriptorQuery,
    std::vector<Mat>& faceDescriptors,
    std::vector<int>& faceLabels, int& label, float& minDistance) {
    int minDistIndex = 0;
    minDistance = 1.0;
    label = -1;
    // Calculate Euclidean distances between face descriptor calculated on face dectected
    // in current frame with all the face descriptors we calculated while enrolling faces
    // Calculate minimum distance and index of this face
    for (int i = 0; i < faceDescriptors.size(); i++) {
        double distance = cv::norm(faceDescriptors[i].t() - faceDescriptorQuery);
        if (distance < minDistance) {
            minDistance = distance;
            minDistIndex = i;
        }
    }
    // if minimum distance is greater than a threshold
    // assign integer label -1 i.e. unknown face
    if (minDistance > recThreshold) {
        label = -1;
    }
    else {
        label = faceLabels[minDistIndex];
    }
}


void leftClick_HSVDlib() {
    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
    sleep(0.1);
    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}

void detect_landmark_HSVDlib(cv::Mat face_frame) {
    cv_image<bgr_pixel> dlibIm(face_frame);
    std::vector<dlib::rectangle> faceRects = faceDetector(dlibIm);
    for (int i = 0; i < faceRects.size(); i++) {
        full_object_detection landmarks = landmarkDetector(dlibIm, faceRects[i]);
        cv::Vec3b color;
        color[0] = 0;
        color[1] = 0;
        color[2] = 255;
        cv::circle(face_frame, cv::Point(landmarks.part(30).x(), landmarks.part(30).y()), 3, color, -1);
    }
}

int tracking_HSVDlib(cv::Mat face_frame, double& x, double& y, int camera_state, int test) {
    cv::TermCriteria term_crit(cv::TermCriteria::EPS | cv::TermCriteria::COUNT, 10, 1);
    //cv::Rect track_window(0, 0, 70, 70);
    cv::Rect track_window(0, 0, 640, 480);

    cv::Mat hsv;
    cvtColor(face_frame, hsv, cv::COLOR_BGR2HSV);
    //HSV VALUES(lower, upper)
    int l_b[3] = { 0, 200, 200 };
    int u_b[3] = { 0, 255, 255 };
    /*SHOW ONLY COLORS IN RANGE*/
    cv::Mat mask;
    inRange(hsv, cv::Scalar(l_b[0], l_b[1], l_b[2]), cv::Scalar(u_b[0], u_b[1], u_b[2]), mask);
    cv::RotatedRect rot_rect = CamShift(mask, track_window, term_crit);
    cv::Point2f points[4];

    rot_rect.points(points);
    for (int i = 0; i < 4; i++) {
        //cv::line(face_frame, points[i], points[(i + 1) % 4], 255, 2);
    }
    int x1 = points[0].x;
    int x2 = points[2].x;
    int y1 = points[0].y;
    int y2 = points[2].y;
    int x_m_point = (x1 + x2) / 2;
    int y_m_point = (y1 + y2) / 2;
    //std::cout << "x_m_point: " << x_m_point << endl;
    /*std::cout << "y_m_point: " << y_m_point << endl;*/
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
        //cv::imshow("Mask", mask);
    }
    
    

    return x, y;
}



double mouthRatio(cv::Mat face_frame, int left_landmark, int right_landmark, int top_landmark, int bottom_landmark, bool eye = false) {
    cv_image<bgr_pixel> dlibIm(face_frame);
    std::vector<dlib::rectangle> faceRects = faceDetector(dlibIm);
    for (int i = 0; i < faceRects.size(); i++) {
        full_object_detection landmarks = landmarkDetector(dlibIm, faceRects[i]);
        cv::Vec3b color;
        color[0] = 255;
        color[1] = 0;
        color[2] = 0;
        /*cv::circle(face_frame, cv::Point(landmarks.part(60).x(), landmarks.part(60).y()), 3, color, -1);
        cv::circle(face_frame, cv::Point(landmarks.part(64).x(), landmarks.part(64).y()), 3, color, -1);*/
        cv::Point left_side = cv::Point(landmarks.part(left_landmark).x(), landmarks.part(left_landmark).y());
        cv::Point right_side = cv::Point(landmarks.part(right_landmark).x(), landmarks.part(right_landmark).y());
        cv::Point top = cv::Point(landmarks.part(top_landmark).x(), landmarks.part(top_landmark).y());
        cv::Point bottom = cv::Point(landmarks.part(bottom_landmark).x(), landmarks.part(bottom_landmark).y());
        cv::Point top2 = cv::Point(landmarks.part(top_landmark + 1).x(), landmarks.part(top_landmark + 1).y());
        cv::Point bottom2 = cv::Point(landmarks.part(bottom_landmark + 1).x(), landmarks.part(bottom_landmark + 1).y());

        int mid_x = (top.x + top2.x) / 2;
        int mid_y = (top.y + top2.y) / 2;
        int mid_x2 = (bottom.x + bottom2.x) / 2;
        int mid_y2 = (bottom.y + bottom2.y) / 2;
        cv::line(face_frame, left_side, right_side, color, 2);
        cv::line(face_frame, cv::Point(mid_x, mid_y), cv::Point(mid_x2, mid_y2), color, 2);

        double horizontal_height = hypot((landmarks.part(left_landmark).x() - landmarks.part(right_landmark).x()),
            (landmarks.part(left_landmark).y() - landmarks.part(right_landmark).y()));
        double vertical_height;

        if (eye == true) {
            vertical_height = hypot((mid_x - mid_x2),
                (mid_y - mid_y2));
        }
        else {
            vertical_height = hypot((landmarks.part(top_landmark).x() - landmarks.part(bottom_landmark).x()),
                (landmarks.part(top_landmark).y() - landmarks.part(bottom_landmark).y()));
        }


        double ratio = horizontal_height / vertical_height;
        return ratio;
    }
}

void mouthMouseClick(cv::Mat face_frame, double ratio, double ratioToClick, int& counter, int& double_click_counter, bool eye = false) {
    if (eye == true) {
        if (ratio > ratioToClick) {
            cv::putText(face_frame, "BLINK", cv::Point(0, 150), cv::FONT_ITALIC, 5, (255, 0, 0), 5);
            counter++;
            double_click_counter++;
            std::cout << counter << "\n";
            std::cout << double_click_counter << "\n";
            if (double_click_counter == 10) {
                std::cout << "DOUBLE CLICK" << "\n";
                /*leftClick_HSVDlib();
                leftClick_HSVDlib();*/
            }
            if (counter == 5) {
                std::cout << "CLICK" << "\n";
                /*leftClick_HSVDlib();*/
            }
            if (counter == 50) {
                exit(3);
            }
        }
        else {
            double_click_counter = 0;
            counter = 0;
        }
    }
    else {
        if (ratio < ratioToClick) {
            cv::putText(face_frame, "BLINK", cv::Point(0, 150), cv::FONT_ITALIC, 5, (255, 0, 0), 5);
            counter++;
            double_click_counter++;
            std::cout << counter << "\n";
            std::cout << double_click_counter << "\n";
            if (double_click_counter == 10) {
                std::cout << "MOUTH DOUBLE CLICK" << "\n";
                leftClick_HSVDlib();
                leftClick_HSVDlib();
            }
            if (counter == 5) {
                std::cout << "MOUTH CLICK" << "\n";
                leftClick_HSVDlib();
            }
            if (counter == 50) {
                exit(3);
            }
        }
        else {
            double_click_counter = 0;
            counter = 0;
        }
    }

}



int HsvDlib(int camera_state, int mouseClickMethod, int camera_number, int face_rec_method) {
    


    cout << "TEST007";
    cv::VideoCapture cap(camera_number);
    if (!cap.isOpened()) {
        return -1;
    }
    cout << "TEST008";

    int blink_counter = 0;
    int double_blink_counter = 0;


    HCURSOR def_arrow_cur = CopyCursor(LoadCursor(0, IDC_ARROW));
    cursor_Image("mouse.cur", 32512, def_arrow_cur, true);

    cout << "Face rec method: " << face_rec_method << endl;
    //ShellExecute(NULL, "open", ".\\TMhdtr-mouthClick.exe", NULL, NULL, SW_SHOWDEFAULT);

    const std::string recModelPath = "data/models/openface.nn4.small2.v1.t7";
    //frontal_face_detector faceDetector = get_frontal_face_detector();
    dnn::Net recModel = dnn::readNetFromTorch(recModelPath);
    // Load the landmark model  
    deserialize("TMhdtr_face_tracking_02.dat") >> landmarkDetector;
    //dlib::shape_predictor landmarkDetector;
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
    readDescriptors(faceDescriptorFile, faceLabels, faceDescriptors);
    //printVector(faceDescriptors);

    /*enrollOpenFace(faceDetector, landmarkDetector, recModel);*/

    int camera_window_pos_x = 0;
    int camera_window_pos_y = 0;
    int face_rec_counter = 0;
    int no_face_detected_counter = 0;
    int face_lost = 0;
    int set_first_face_frame = 0;
    camera_window_position(camera_window_pos_x, camera_window_pos_y);
    cv::Mat face_frame;
    cv::Mat tracking_frame;
    int face_frame_x = 0;
    int face_frame_y = 0;
    int face_frame_width = 1;
    int face_frame_height = 1;
    int tracking_frame_y = 0;
    int tracking_frame_height = 1;
    int tracking_frame_x = 0;
    int tracking_frame_width = 1;
    double estimated_x;
    double estimated_y;

    std::vector<dlib::rectangle> faceRects;
    cv::Mat test_frame;
    int test = 0;


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
    

    for (;;) {
        //cout << "TEST011";

        cv::Mat frame;
        cap >> frame;


        //set first test face_frame
        face_frame = frame(cv::Rect(face_frame_x, face_frame_y, face_frame_width, face_frame_height));
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
        tracking_frame_y = (face_frame_y * 20 / 100);
        tracking_frame_height = face_frame_height - tracking_frame_y - (face_frame_height * 20 / 100);
        tracking_frame_x = (face_frame_x * 10 / 100);
        tracking_frame_width = face_frame_width - tracking_frame_x - (face_frame_width * 15 / 100);
        /* cout << "TRACKING X: " << tracking_frame_x << endl;
        cout << "TRACKING WIDTH: " << tracking_frame_width << endl;*/


       /* cout << "TRACKING Y: " << tracking_frame_y << endl;
        cout << "TRACKING HEIGHT: " << tracking_frame_height << endl;*/

        if (set_first_face_frame != 0){
            //tracking_frame = face_frame(cv::Rect(0, 20, face_frame_width, 50));
            //tracking_frame = face_frame(cv::Rect(0, tracking_frame_y, face_frame_width, tracking_frame_height));
            tracking_frame = face_frame(cv::Rect(tracking_frame_x, tracking_frame_y, tracking_frame_width, tracking_frame_height));
        }
        else {
            tracking_frame = face_frame(cv::Rect(0, 0, face_frame_width, face_frame_height));

        }
        

        /////////////////////////////////////////////////////////////////////////////////////////FACE REC OPENFACE PART///////////////////////////////////////////////////////////
        cv_image<bgr_pixel> imDlib(frame);

        // detect faces in image
        std::vector<dlib::rectangle> faceRects = faceDetector(imDlib);
        
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
                cv::putText(face_frame, text, cv::Point2d(0,20), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 255, 255), 2);
            }
            face_rec_counter++;
        }
        /////////////////////////////////////////////////////////////////////////////////////////FACE REC OPENFACE PART///////////////////////////////////////////////////////////
        //cv_image<bgr_pixel> imDlib(frame);
        //cv_image<bgr_pixel> imDlib(face_frame);
        //faceRects = faceDetector(imDlib);
        //cout << "Left: " << faceRects[0].left() << endl;
        //cout << "Top: " << faceRects[0].top() << endl;
         //Draw a rectangle for detected face
        /*Point2d p1 = Point2d(faceRects[0].left(), faceRects[0].top());
        Point2d p2 = Point2d(faceRects[0].right(), faceRects[0].bottom());
        cv::rectangle(frame, p1, p2, Scalar(0, 255, 255), 1, LINE_8);*/

        if (set_first_face_frame == 0) {
            //std::cout << "\nSetting up first frame\n";
            cv_image<bgr_pixel> imDlib(frame);

            // detect faces in image
            faceRects = faceDetector(imDlib);
        }
        else {
            //std::cout << "\nFrame setted\n";

            cv_image<bgr_pixel> imDlib(face_frame);

            // detect faces in image
            faceRects = faceDetector(imDlib);
        }

        //take cordinates from faceRects for face_frame
        if (faceRects.size() == 1 && set_first_face_frame == 0) {
            faceRects = faceDetector(imDlib);

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
            //cout << no_face_detected_counter;

			if (no_face_detected_counter == 40) {
				//SetCursorPos(GetSystemMetrics(0) / 2, GetSystemMetrics(1) / 2);
			    //cout << faceRects.size();
			    //cv::putText(frame, "Kein Gesicht", cv::Point2d(0, 20), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 255, 255), 2);
			    //cv_image<bgr_pixel> imDlibNoFace(frame);

			    // detect faces in image
                face_lost++;
                try {
                    faceRects = faceDetector(imDlib);
                    //face cant be cut outside off the frame boudnaries
                    if (faceRects.size() == 0 || faceRects[0].left() > 460 || faceRects[0].top() > 230) {
                        throw 99;
                    }
                    else {
                        face_frame_x = faceRects[0].left();
                        face_frame_y = faceRects[0].top();
                        face_frame_width = faceRects[0].right() - faceRects[0].left ();
                        face_frame_height = faceRects[0].bottom() - faceRects[0].top();
                        no_face_detected_counter = 0;
                    }
                    
                }
                catch(int x){
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
        
		detect_landmark_HSVDlib(frame);
		tracking_HSVDlib(tracking_frame, mouse_x, mouse_y, camera_state, test);
        //tracking_HSVDlib(frame, mouse_x, mouse_y, camera_state, test);

        cout << "Tracking frame size" << tracking_frame.size << endl;

        //tracking_HSVDlib(face_frame, mouse_x, mouse_y, camera_state);
        /*cout << "Mouse x: " << mouse_x << '\n';
        cout << "Mouse y: " << mouse_y << '\n';*/
		//MOUTH CLICK
		if (mouseClickMethod == 2) {
			double ratio = mouthRatio(face_frame, 60, 64, 51, 57, false); //mouth
			mouthMouseClick(face_frame, ratio, 1.4, blink_counter, double_blink_counter, false); //click for mouth
		}

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
        //cout << "statePt: " << statePt << endl;
        //cout << "meastPt: " << measPt << endl;

        circle(tracking_frame, statePt, 6, Scalar(255, 255, 0), 1); //blue
        circle(tracking_frame, measPt, 10, Scalar(0, 255, 255), 1); //yellow
        

        /// zrobic to samo ale dla punktu z funkcji detect landmark hsv dlib////////////////////////////////////////////////////////////////////////
        /*if (test == 0) {
            last_mouse_x_pos[0] = measPt.x;
            last_mouse_y_pos[0] = measPt.y;

        }
        if (measPt.x > tracking_frame.size().width || measPt.x < tracking_frame.size().width) {
            statePt.x = last_mouse_x_pos[0];
            test++;
        }
        else {
            test = 0;
        }
        
        cout << "MeastPT: " << measPt << endl;*/
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



        /*drawCross(statePt, Scalar(255, 255, 255), 5);
        drawCross(measPt, Scalar(0, 0, 255), 5);*/
        //putText(img, statePt,)
        /*for (int i = 0; i < mousev.size() - 1; i++)
            line(face_frame, mousev[i], mousev[i + 1], Scalar(255, 255, 0), 1);

        for (int i = 0; i < kalmanv.size() - 1; i++)
            line(face_frame, kalmanv[i], kalmanv[i + 1], Scalar(0, 155, 255), 1);*/

            ////////////////////////////////////////////////////////////////////////////////KALMAN//////////////////////////////////////////

	/*	double real_x = (((double)mouse_x / tracking_frame.size().width) * GetSystemMetrics(0));
		double real_y = (((double)mouse_y / tracking_frame.size().height) * GetSystemMetrics(1));*/

        int prediciton_x_difference = measPt.x - statePt.x;

        if (prediciton_x_difference > -40 && prediciton_x_difference < 40) {
            estimated_x = statePt.x;
            estimated_y = statePt.y;
        }
        else {
            statePt = measPt;
            estimated_x = measPt.x;
            estimated_y = measPt.y;
        }


       
        double real_x = (((double)estimated_x / tracking_frame.size().width) * GetSystemMetrics(0));
        double real_y = (((double)estimated_y / tracking_frame.size().height) * GetSystemMetrics(1));

		double reversed_x = GetSystemMetrics(0) - real_x;

        SetCursorPos(reversed_x, real_y);


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
		//cout << "DIFF: " << difference_y << '\n';
		/*cout << "0: " << reale_x[0] << '\n';
		cout << "1: " << reale_x[1] << '\n';*/
		/*cout << "Timer: " << timer_counter << '\n';*/

		/*if (difference_x < 21.4 && difference_y < 30.9) {
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
				cout << "INTERVAL CLICK" << '\n';
                leftClick_HSVDlib();

			}
		}
		else if (difference_x > 21.4 || difference_y > 30.9) {
			timer_counter = 0;
			firstTry = true;
			SetCursorPos(reversed_x, real_y);
		}*/
        
        



        if (camera_state == 1) {

            if (face_lost != 0) {
                //std::cout << "\nloading face frame\n";
                //imshow("Face frame", face_frame);
            }
            //imshow("Face frame", face_frame);
            imshow("Frame", frame);
            //imshow("Tracking Frame", tracking_frame);
            //imshow("kalman", img);


        }



        camera_InFront("Face frame", camera_window_pos_x, camera_window_pos_y);


        if (cv::waitKey(30) >= 0)
            break;
    }

    cursor_Image("mouse.cur", 32512, def_arrow_cur, false);

    return 0;
}