#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>
#include <string>

using namespace cv;
using namespace std;

void textureMatching_NCC(Mat&, Mat&, Mat&, const int&, Mat&);
void meanStdDev(Mat&, double&, double&);
void draw(const Mat&, const Point&, Mat&);

void textureMatching_NCC(Mat& src, Mat& templ, Mat& out, const int& thres,  Mat& mask){
    // inverse template pixel counts
    const double invArea = 1./((double)templ.cols * templ.rows);
    // get template mean and std
    double templMean, templStdDev;
    meanStdDev(templ, templMean, templStdDev);
    
    for(int i = templ.rows/2; i < out.rows - templ.rows/2; ++i){
        uchar* out_row = out.ptr<uchar>(i);
        uchar* mask_row = mask.ptr<uchar>(i);
        for(int j = templ.cols/2; j < out.cols - templ.cols/2; ++j){
            if(mask_row[j] == 0){
                continue;
            }
            int srcSum = 0;
            int srcSqrSum = 0;
            double cov_xy = 0;
            
            int th = 0;
            for(int h = i - templ.rows/2; h < i + templ.rows/2 + templ.rows%2; ++h){
                int tw = 0;
                uchar* src_row = src.ptr<uchar>(h);
                uchar* templ_row = templ.ptr<uchar>(th);
                for(int w = j - templ.cols/2; w < j + templ.cols/2 + templ.cols%2; ++w){
                    // add srcSum and srcSqrSum
                    srcSum += src_row[w];
                    srcSqrSum += src_row[w] * src_row[w];
                    // add to covariance(src, templ)
                    cov_xy += src_row[w] * templ_row[tw] * invArea;
                    ++tw;
                } 
                ++th;
            }
            
            const double srcMean = (double) srcSum * invArea;
            const double srcStdDev = std::sqrt(srcSqrSum * invArea - srcMean * srcMean);

            // corr
            const double CORR = (cov_xy - (srcMean*templMean)) / (srcStdDev*templStdDev);
            uchar p = ((int)127.5 * CORR + 127.5);
            if(thres < p){
                out_row[j] = p;
                mask_row[j] = 255;
            }else{
                out_row[j] = 0;
                mask_row[j] = 0;
            }
        }
    }
}

void meanStdDev(Mat& img, double& mean, double& stdDev){
    const double invArea = 1./((double)img.cols * img.rows);
    int sum = 0;
    int sqrtSum = 0;
    for(int y = 0; y < img.rows; ++y){
        uchar* rrow = img.ptr<uchar>(y);
        for(int x = 0; x < img.cols; ++x){
            sum += rrow[x];
            sqrtSum += rrow[x] * rrow[x];
        }
    }
    mean = sum * invArea;
    stdDev = std::sqrt(sqrtSum * invArea - mean * mean);
}

void draw(const Mat& templ, const Point2i& center, Mat& out){
    Point2i p1 = Point2i(center.x - templ.cols/2, center.y - templ.rows/2);
    Point2i p2 = Point2i(center.x + templ.cols/2, center.y + templ.rows/2);
    cv::Rect rect(p1, p2);
    cv::rectangle(out, rect, cv::Scalar(255, 0, 255), 3);
}

int main(int argc, char **argv)
{   
    char* srcPath = argv[1];
    char* templPath = argv[2];
    int thres = atoi(argv[3]);
    
    // init basic Mat
    cv::Mat src = cv::imread(srcPath, cv::ImreadModes::IMREAD_GRAYSCALE);
    cv::Mat templ = cv::imread(templPath, cv::ImreadModes::IMREAD_GRAYSCALE);
    cv::Mat out(src.rows, src.cols, CV_8UC1, Scalar(0));
    cv::Mat mask(src.rows, src.cols, CV_8UC1, Scalar(255));

    // built in opencv method
    cout << " ========== built in opencv method ========== "<< endl;
    Mat out_cv = out.clone();

    int64 e1 = cv::getTickCount();

    cv::matchTemplate(src, templ, out_cv, TM_CCOEFF_NORMED);

    int64 e2 = cv::getTickCount();
    double time = (e2 - e1) / cv::getTickFrequency();
    std::cout << time << " sec" << std::endl;
    //cv::imshow("opencv", out_cv);
    //cv::waitKey(0);
    //cv::imwrite("opencv.jpg", out_cv);
    cout << " ========== built in opencv method end ========== "<< endl;

    // implement method
    cout << " ========== implement method ========== "<< endl;

    // clone basic Mat to do Image Pyramids
    cv::Mat src_py = src.clone();
    cv::Mat templ_py = templ.clone();

    // start time
    e1 = cv::getTickCount();
    cv::pyrDown(src_py, src_py, cv::Size(src_py.cols/2, src_py.rows/2));
    cv::pyrDown(templ_py, templ_py, cv::Size(templ_py.cols/2, templ_py.rows/2));
    cv::pyrDown(mask, mask, cv::Size(mask.cols/2, mask.rows/2));
    cv::pyrDown(src_py, src_py, cv::Size(src_py.cols/2, src_py.rows/2));
    cv::pyrDown(templ_py, templ_py, cv::Size(templ_py.cols/2, templ_py.rows/2));
    cv::pyrDown(mask, mask, cv::Size(mask.cols/2, mask.rows/2));
    
    textureMatching_NCC(src_py, templ_py, mask, thres, mask);
    
    cv::pyrUp(mask, mask, cv::Size(mask.cols*2, mask.rows*2));
    cv::pyrUp(mask, mask, cv::Size(mask.cols*2, mask.rows*2));

    textureMatching_NCC(src, templ, out, thres, mask);

    // end time
    e2 = cv::getTickCount();
    time = (e2 - e1) / cv::getTickFrequency();
    std::cout << time << " sec" << std::endl;
    cv::imwrite("implement.jpg", out);
    cout << " ========== implement method end ========== "<< endl;

    
    

    return 0;
}