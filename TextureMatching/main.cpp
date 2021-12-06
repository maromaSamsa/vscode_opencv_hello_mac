#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>
#include <string>

using namespace cv;
using namespace std;

void common_matchTemplate( Mat&, Mat&, Mat&, int method, int cn);
void textureMatching_NCC(Mat&, Mat&, Mat&);
void meanStdDev(Mat&, double&, double&);

void textureMatching_NCC(Mat& src, Mat& templ, Mat& out ){

    // inverse template pixel counts
    const double invArea = 1./((double)templ.cols * templ.rows);
    // get template mean and std
    double templMean, templStdDev;
    meanStdDev(templ, templMean, templStdDev);
    
    // for(int i = templ.rows/2; i < out.rows - templ.rows/2; ++i){
    //     uchar* out_row = out.ptr<uchar>(i);
    //     for(int j = templ.cols/2; j < out.cols - templ.cols/2; ++j){
    //         int srcSum = 0;
    //         int srcSqrSum = 0;
    //         double cov_xy = 0;
    //         for(int h = i - templ.rows/2; h < i + templ.rows/2; h++){
    //             uchar* src_row = src.ptr<uchar>(h);
    //             uchar* templ_row = templ.ptr<uchar>(h);
    //             for(int w = j - templ.cols/2; w < j + templ.cols/2; w++){
    //                 // add srcSum and srcSqrSum
    //                 srcSum += src_row[w];
    //                 srcSqrSum += src_row[w] * src_row[w];
    //                 // add to covariance(src, templ)
    //                 cov_xy += src_row[w] * templ_row[w];
    //             } 
    //         }
    //         const double srcMean = (double) srcSum * invArea;
    //         const double srcStdDev = std::sqrt(srcSqrSum * invArea - srcMean * srcMean);

            
    //         // 
    //         const double CORR = (cov_xy - (srcMean*templMean)) / (srcStdDev*templStdDev);
    //         cout<<CORR<<endl;
    //         out_row[j] = (int)127.5 * CORR + 127.5;
    //     }
    // }
    for(int i = templ.rows/2; i < out.rows - templ.rows/2; ++i){
        uchar* out_row = out.ptr<uchar>(i);
        for(int j = templ.cols/2; j < out.cols - templ.cols/2; ++j){
            int srcSum = 0;
            int srcSqrSum = 0;
            double cov_xy = 0;
            for(int h = i - templ.rows/2; h < i + templ.rows/2; h++){
                uchar* src_row = src.ptr<uchar>(h);
                for(int w = j - templ.cols/2; w < j + templ.cols/2; w++){
                    // add srcSum and srcSqrSum
                    srcSum += src_row[w];
                    srcSqrSum += src_row[w] * src_row[w];
                } 
            }
            const double srcMean = (double) srcSum * invArea;
            const double srcStdDev = std::sqrt(srcSqrSum * invArea - srcMean * srcMean);

            for(int h = i - templ.rows/2; h < i + templ.rows/2; h++){
                uchar* src_row = src.ptr<uchar>(h);
                uchar* templ_row = templ.ptr<uchar>(h);
                for(int w = j - templ.cols/2; w < j + templ.cols/2; w++){
                    // add to covariance(src, templ)
                    cov_xy += (src_row[w] - srcMean) * (templ_row[w] - templMean);
                } 
            }
            
            // 
            const double CORR = (cov_xy) / (srcStdDev*templStdDev);
            cout<<CORR<<endl;
            out_row[j] = (int)127.5 * CORR + 127.5;
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

int main()
{   
    cv::Mat src = cv::imread("src.jpg", cv::ImreadModes::IMREAD_GRAYSCALE);
    cv::Mat templ = cv::imread("temp.jpg", cv::ImreadModes::IMREAD_GRAYSCALE);
    //cv::Mat out(src.rows, src.cols, CV_8UC1, Scalar(0));
    cv::Mat out = cv::imread("src.jpg", cv::ImreadModes::IMREAD_GRAYSCALE);
    int64 e1 = cv::getTickCount();
    //common_matchTemplate(src, temp, src, 1, 1);
    textureMatching_NCC(src, templ, out);
    int64 e2 = cv::getTickCount();
    double time = (e2 - e1) / cv::getTickFrequency();
    
    std::cout << time << " sec" << std::endl;
    cv::imshow("res", out);
    cv::waitKey(0);

    return 0;
}

void common_matchTemplate( Mat& img, Mat& templ, Mat& result, int method = 1, int cn = 1)
{
    int numType = 1;
    bool isNormed = true;

    double invArea = 1./((double)templ.rows * templ.cols);

    Mat sum, sqsum;
    Scalar templMean, templSdv;
    double *q0 = 0, *q1 = 0, *q2 = 0, *q3 = 0;
    double templNorm = 0;

    

    integral(img, sum, sqsum, CV_64F);
    meanStdDev( templ, templMean, templSdv );

    templNorm = templSdv[0]*templSdv[0] + templSdv[1]*templSdv[1] + templSdv[2]*templSdv[2] + templSdv[3]*templSdv[3];
    
    templNorm = std::sqrt(templNorm);
    templNorm /= std::sqrt(invArea); // care of accuracy here

   
    q0 = (double*)sqsum.data;
    q1 = q0 + templ.cols*cn;
    q2 = (double*)(sqsum.data + templ.rows*sqsum.step);
    q3 = q2 + templ.cols*cn;


    double* p0 = (double*)sum.data;
    double* p1 = p0 + templ.cols*cn;
    double* p2 = (double*)(sum.data + templ.rows*sum.step);
    double* p3 = p2 + templ.cols*cn;

    int sumstep = sum.data ? (int)(sum.step / sizeof(double)) : 0;
    int sqstep = sqsum.data ? (int)(sqsum.step / sizeof(double)) : 0;

    int i, j, k;

    for( i = 0; i < result.rows; i++ )
    {
        float* rrow = result.ptr<float>(i);
        int idx = i * sumstep;
        int idx2 = i * sqstep;

        for( j = 0; j < result.cols; j++, idx += cn, idx2 += cn )
        {
            double num = rrow[j], t;
            double wndMean2 = 0, wndSum2 = 0;

            if( numType == 1 )
            {
                for( k = 0; k < cn; k++ )
                {
                    t = p0[idx+k] - p1[idx+k] - p2[idx+k] + p3[idx+k];
                    wndMean2 += t*t;
                    num -= t*templMean[k];
                }

                wndMean2 *= invArea;
            }

            if(isNormed)
            {
                for( k = 0; k < cn; k++ )
                {
                    t = q0[idx2+k] - q1[idx2+k] - q2[idx2+k] + q3[idx2+k];
                    wndSum2 += t;
                }
            }

            if(isNormed)
            {
                double diff2 = MAX(wndSum2 - wndMean2, 0);
                if (diff2 <= std::min(0.5, 10 * FLT_EPSILON * wndSum2))
                    t = 0; // avoid rounding errors
                else
                    t = std::sqrt(diff2)*templNorm;

                if( fabs(num) < t )
                    num /= t;
                else if( fabs(num) < t*1.125 )
                    num = num > 0 ? 1 : -1;
                else
                    num = 1;
            }

            rrow[j] = (float)num;
        }
    }
}



