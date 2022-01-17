#include <iostream>
#include <vector>
#include <string>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/video/tracking.hpp>


using namespace cv;
using namespace std;

int main(int argc, char **argv)
{   
    // step 1 load image
    char* path = argv[1];
    cv::Mat src1 = cv::imread(path);
    cv::Mat gray1;
    cv::cvtColor(src1, gray1, COLOR_BGR2GRAY);
    path = argv[2];
    cv::Mat src2 = cv::imread(path);
    cv::Mat gray2;
    cv::cvtColor(src2, gray2, COLOR_BGR2GRAY);

    // step 2 find feature point
    const int ptCount = 10;
    vector<cv::Scalar> colors;
    cv::Scalar blue(255, 0, 0);
    cv::Scalar yellow(0, 255, 255);
    cv::Scalar purple(255, 0, 255);
    cv::Scalar red(0, 0, 255);

    vector<cv::Point2f> p0, p1;
    cv::goodFeaturesToTrack(gray1, p0, ptCount, 0.3, 7);


    // step 3 optical flow
    vector<uchar> status;
    vector<float> err;
    cv::TermCriteria criteria = cv::TermCriteria((cv::TermCriteria::COUNT) + (cv::TermCriteria::EPS), 10, 0.03);
    cv::calcOpticalFlowPyrLK(gray1, gray2, p0, p1, status, err, Size(15, 15), 2, criteria);
    
    for(int i = 0; i < p0.size(); ++i){
        if(status[i] == 1){
            cv::line(src2, p0[i], p1[i], purple, 2);
            cv::circle(src2, p0[i], 1, blue, 2);
            cv::circle(src2, p1[i], 1, yellow, 2);
        }
        
    }
    cv::imwrite("res.jpg", src2);
    return 0;
}

