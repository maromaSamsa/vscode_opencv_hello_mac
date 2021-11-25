#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

using namespace cv;
using namespace std;


cv::Mat to_grayScale(cv::Mat src){
    // return Mat
    cv::Mat grayImage(src.rows, src.cols, CV_8UC1, Scalar(255));
    
    // traversal all pixel and turn the RBG value to gray scale
    for(int i = 0; i < src.rows; ++i){
        // row pointer, is faster than Mat.at()
        uchar* orgImageRow = src.ptr(i);
        uchar* grayImageRow = grayImage.ptr(i);
        
        for(int j = 0; j < src.cols; ++j){
            // actially RGB in opencv is BRG
            uchar blue = orgImageRow[j * 3];
            uchar red = orgImageRow[j * 3 + 1];
            uchar green = orgImageRow[j * 3 + 2];
            
            // parameter based on human vision
            grayImageRow[j] = 0.299 * red + 0.587 * green + 0.114 * blue;
        }
    }
    return grayImage;
}

cv::Mat dilation(cv::Mat src, int kernel_size = 3){
    // init a return Mat, whole black image
    cv::Mat outputImage(src.rows, src.cols, CV_8UC1, Scalar(0));
    
    // first and second for loop: iteration src image pixel, considering kernel(mask) size, piexel on outer ring have not to used
    for(int i = kernel_size/2; i < src.rows - kernel_size/2; ++i){
        // output image data pointer
        uchar* inOutputImage = outputImage.ptr<uchar>(i);
        
        for(int j = kernel_size/2; j < src.cols - kernel_size/2; ++j){
            
            // third and fourth loop, to move a 2D kernel, work as a sliding window
            for(int kernel_row = i - kernel_size/2; kernel_row <= i + kernel_size/2; ++kernel_row){
                
                // src image in kernel range, data pointer
                uchar* inSrcImage = src.ptr<uchar>(kernel_row);
                
                for(int kernel_col = j - kernel_size/2; kernel_col <= j + kernel_size/2; ++kernel_col){
                    
                    // if curr pixel value on output image is less than any other pixel value on curr kernel, fill it up
                    if(inOutputImage[j] < inSrcImage[kernel_col]){
                        inOutputImage[j] = inSrcImage[kernel_col];
                    }
                }
            }
        }
    }
    return outputImage;
}

cv::Mat erosion(cv::Mat src, int kernel_size = 3){
    cv::Mat outputImage(src.rows, src.cols, CV_8UC1, Scalar(255));
    for(int i = kernel_size/2; i < src.rows - kernel_size/2; ++i){
        uchar* inOutputImage = outputImage.ptr<uchar>(i);
        for(int j = kernel_size/2; j < src.cols - kernel_size/2; ++j){
            for(int kernel_row = i - kernel_size/2; kernel_row <= i + kernel_size/2; ++kernel_row){
                uchar* inSrcImage = src.ptr<uchar>(kernel_row);
                for(int kernel_col = j - kernel_size/2; kernel_col <= j + kernel_size/2; ++kernel_col){
                    
                    // if curr pixel value on output image is less than any other pixel value on curr kernel, clean it up
                    if(inOutputImage[j] > inSrcImage[kernel_col]){
                        inOutputImage[j] = inSrcImage[kernel_col];
                    }   

                }
            }
        }
    }
    return outputImage;
}

cv::Mat subtraction(cv::Mat src1, cv::Mat src2){
    for(int i = 0; i < src1.rows; ++i){
        uchar* inSrc1 = src1.ptr(i);
        uchar* inSrc2 = src2.ptr(i);
        for(int j = 0; j < src1.cols; ++j){
            int pixel = inSrc1[j] - inSrc2[j] + 255;
            if(pixel < 0){
                inSrc2[j] = 0;
            }else if(pixel > 255){
                inSrc2[j] = 255;
            }else{
                inSrc2[j] = uchar(pixel);
            }
        }
    }
    return src2;
}

cv::Mat blur(cv::Mat src, int kernel_size = 3){
    cv::Mat outputImage(src.rows, src.cols, CV_8UC1, Scalar(0));
    for(int i = kernel_size/2; i < src.rows - kernel_size/2; ++i){
        uchar* inOutputImage = outputImage.ptr<uchar>(i);
        for(int j = kernel_size/2; j < src.cols - kernel_size/2; ++j){
            for(int kernel_row = i - kernel_size/2; kernel_row <= i + kernel_size/2; ++kernel_row){
                uchar* inSrcImage = src.ptr<uchar>(kernel_row);
                for(int kernel_col = j - kernel_size/2; kernel_col <= j + kernel_size/2; ++kernel_col){
                    // curr pixel on output image is average of curr kernel
                    inOutputImage[j] += inSrcImage[kernel_col]/(kernel_size*2); 
                }
            }
        }
    }
    return outputImage;
}

int main(int argc, char **argv)
{   
    char* imgPath = argv[1];
    cv::Mat org = cv::imread(imgPath); // type 16: CV_8UC3

    // turn into gray scale
    cv::Mat grayImage = to_grayScale(org);
    org.release();

    cv::Mat tempImage = dilation(grayImage, 5);
    tempImage = erosion(tempImage, 3);
    tempImage = subtraction(grayImage, tempImage);
    tempImage = blur(tempImage, 3);
    tempImage = subtraction(grayImage, tempImage);
    
    cv::Mat outputImage(tempImage.rows, tempImage.cols, CV_8UC1, Scalar(255));
    cv::adaptiveThreshold(tempImage, outputImage, 255, 0, 0, 7, 10);
    outputImage = erosion(outputImage, 5);


    cv::imwrite("res.jpg", outputImage);

    return 0;
}
