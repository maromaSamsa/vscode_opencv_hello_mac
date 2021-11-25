#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

using namespace cv;
using namespace std;

class Affined_Image
{
private:
    struct Flag{
        bool name = false;
        bool img = false;
        bool fromTri = false;
        bool toTri = false;
        bool outputImgSize = false;
        bool res = false;
    };
    Affined_Image::Flag flag;
    std::string name = NULL;
    cv::Mat img;
    cv::Point2f fromTri[3] = {};
    cv::Point2f toTri[3] = {};
    cv::Matx33f affineMatrix = {1.0f, 0.0f, 0.0f,
                                0.0f, 1.0f, 0.0f,
                                0.0f, 0.0f, 1.0f};
    
    cv::Size outputImgSize;
    cv::Mat res;
    void setAffineTransformMatrix();
    void generateResImg();

public:
    
    Affined_Image(const std::string &name, const cv::Mat &img) : name(name), img(img), outputImgSize(img.size()){
        flag.name = true;
        flag.img = true;
        flag.outputImgSize = true;
    }

    std::string getName() { return this->name; }
    cv::Mat getResImage(){ return this->res; }
    void setImage(const std::string &, const cv::Mat &);
    bool setSrcPt(const cv::Point2f[3]);
    bool setProjPt(const cv::Point2f[3]);
    void setOutputImgSize(cv::Size s){this->outputImgSize = s;}
    void setOutputImgSize(int, int);
    void update();
    void showResImg(const std::string &);
};

void Affined_Image::setAffineTransformMatrix()
{
    // 3x3 source 3 points matrix
    cv::Matx33f srcPtMatrix = {fromTri[0].x, fromTri[1].x, fromTri[2].x,
                               fromTri[0].y, fromTri[1].y, fromTri[2].y,
                               1.0,          1.0,          1.0          };
    // 3x3 matrix of source 3 points projection
    cv::Matx33f projPtMatrix = {toTri[0].x, toTri[1].x, toTri[2].x,
                                toTri[0].y, toTri[1].y, toTri[2].y,
                                1.0,        1.0,        1.0        };
    /*
        [projPtMatrix] = [affineMatrix]*[srcPtMatrix]
        =>  [affineMatrix] = [projPtMatrix] * Inv[srcPtMatrix]
    */
    try
    {
        // get affined matrix
        this->affineMatrix = projPtMatrix * srcPtMatrix.inv();
    }
    catch (cv::Exception &e)
    {
       throw e;
    }
}

void Affined_Image::generateResImg()
{   
    this->res = cv::Mat(this->outputImgSize, this->img.type());
    try
    {   
        // use for iteration all pixels
        int rows = img.rows;
        int cols = img.cols;

        // Position after Transfer
        float px, py;

        // use for interpolation
        float delta_x, delta_y;

        for(int y = 0; y < rows; ++y){
            for(int x = 0; x < cols; ++x){
                // origin point (x, y, 1), the third element don't care
                cv::Matx31f pOrg = {float(x), 
                                    float(y), 
                                    1.0f     };
                // Transfer point (x, y, 1), the third element don't care
                cv::Matx31f pTrans = this->affineMatrix * pOrg;

                // Position after Transfer
                px = pTrans(0,0); // new x
                py = pTrans(1,0); // new y
                // use for interpolation
                delta_x = px - int(px);
                delta_y = py - int(py);

                // Boudary check
                if(px < 0 || px >= outputImgSize.width || py < 0 || py >= outputImgSize.height)continue;

                // Iteration all pixel to new res
                // int yy, xx can convert py, py to int
                for(int yy = py; yy <= py+1 ;++yy){
                    for(int xx = px; xx <= px+1 ;++xx){
                        // BGR 3 channels
                        this->res.at<cv::Vec3b>(yy, xx)[0] = this->img.at<cv::Vec3b>(y, x)[0];
                        this->res.at<cv::Vec3b>(yy, xx)[1] = this->img.at<cv::Vec3b>(y, x)[1];
                        this->res.at<cv::Vec3b>(yy, xx)[2] = this->img.at<cv::Vec3b>(y, x)[2];

                        // Bilinear interpolation, but failed
                        // if(yy == py && xx == px)
                        // {
                        //     this->res.at<cv::Vec3b>(yy, xx)[0] += this->img.at<cv::Vec3b>(y, x)[0] * (1-delta_x) * (1-delta_y);
                        //     this->res.at<cv::Vec3b>(yy, xx)[1] += this->img.at<cv::Vec3b>(y, x)[1] * (1-delta_x) * (1-delta_y);
                        //     this->res.at<cv::Vec3b>(yy, xx)[2] += this->img.at<cv::Vec3b>(y, x)[2] * (1-delta_x) * (1-delta_y);
                        // }
                        // else if (yy == py && xx != px)
                        // {
                        //     this->res.at<cv::Vec3b>(yy, xx)[0] += this->img.at<cv::Vec3b>(y, x)[0] * delta_x * (1 - delta_y);
                        //     this->res.at<cv::Vec3b>(yy, xx)[1] += this->img.at<cv::Vec3b>(y, x)[1] * delta_x * (1 - delta_y);
                        //     this->res.at<cv::Vec3b>(yy, xx)[2] += this->img.at<cv::Vec3b>(y, x)[2] * delta_x * (1 - delta_y);
                        // }
                        // else if (yy != py && xx == px)
                        // {
                        //     this->res.at<cv::Vec3b>(yy, xx)[0] += this->img.at<cv::Vec3b>(y, x)[0] * delta_y * (1 - delta_x);
                        //     this->res.at<cv::Vec3b>(yy, xx)[1] += this->img.at<cv::Vec3b>(y, x)[1] * delta_y * (1 - delta_x);
                        //     this->res.at<cv::Vec3b>(yy, xx)[2] += this->img.at<cv::Vec3b>(y, x)[2] * delta_y * (1 - delta_x);
                        // }
                        // else
                        // {
                        //     this->res.at<cv::Vec3b>(yy, xx)[0] += this->img.at<cv::Vec3b>(y, x)[0] * delta_x * delta_y;
                        //     this->res.at<cv::Vec3b>(yy, xx)[1] += this->img.at<cv::Vec3b>(y, x)[1] * delta_x * delta_y;
                        //     this->res.at<cv::Vec3b>(yy, xx)[2] += this->img.at<cv::Vec3b>(y, x)[2] * delta_x * delta_y;
                        // }
                        
                    }
                }
            }
        }
    }
    catch (cv::Exception &e)
    {
        throw e;
    }
}

void Affined_Image::setImage(const std::string &name, const cv::Mat &img)
{
    this->name = name;
    this->img = img;
    this->outputImgSize = img.size();
    this->flag.name = true;
    this->flag.img = true;
    this->flag.outputImgSize = true;
}

bool Affined_Image::setSrcPt(const cv::Point2f pt[3])
{
    this->fromTri[0] = pt[0];
    this->fromTri[1] = pt[1];
    this->fromTri[2] = pt[2];
    cv::Vec2f v1 = pt[1] - pt[0];
    cv::Vec2f v2 = pt[2] - pt[0];

    // if 3 point cannot describe a 2D plane, return false
    if(cv::normalize(v1) != cv::normalize(v2) && v1 != cv::Vec2f(0.0, 0.0) && v2 != cv::Vec2f(0.0, 0.0)){
        flag.fromTri = true;
        return true;
    }else{
        return false;
    }
}

bool Affined_Image::setProjPt(const cv::Point2f pt[3])
{
    this->toTri[0] = pt[0];
    this->toTri[1] = pt[1];
    this->toTri[2] = pt[2];
    cv::Vec2f v1 = pt[1] - pt[0];
    cv::Vec2f v2 = pt[2] - pt[0];

    // if 3 point cannot describe a 2D plane, return false
    if(cv::normalize(v1) != cv::normalize(v2) && v1 != cv::Vec2f(0.0, 0.0) && v2 != cv::Vec2f(0.0, 0.0)){
        flag.toTri = true;
        return true;
    }else{
        return false;
    }
}

void Affined_Image::setOutputImgSize(int cols, int rows){
    this->outputImgSize = cv::Size(cols, rows);
    this->flag.outputImgSize = true;
}

void Affined_Image::update(){
    // Active when all necessary varibles are ready
    bool is_ready_for_update = (this->flag.name & this->flag.img & this->flag.fromTri & this->flag.toTri & this->flag.outputImgSize);
    if (true)
    {
        try {
            setAffineTransformMatrix();
        }catch(cv::Exception &e){
            cout << "Function setAffineTransformMatrix() failed" << endl;
            return;
        }
        try{
            generateResImg();
        }catch(cv::Exception &e){
            cout << "Function generateResImg() failed" << endl;
        }
        this->flag.res = true;
    }
    else
    {
        cout << "Func update() failed: Update after finish set up" << endl;
        return;
    }
}

void Affined_Image::showResImg(const std::string & name){
    if(this->flag.res){
        imshow(name, this->res);
        cv::waitKey(0);
    }else{
        cout << "No res Image" << endl;
    }
}

// for clicking interface
int posX = 0;
int posY = 0;
int click = 0;
bool _lock = true;
void mouseCall(int event, int x, int y, int flags, void *param)
{
    if (event == cv::EVENT_LBUTTONDOWN)
    {
        ++click;
        posX = x;
        posY = y;
        _lock = false;
    }
}

int main(int argc, char **argv)
{
    char* imgPath = argv[1];
    cv::Mat img = cv::imread(imgPath); // type 16: CV_8UC3

    // from left to right: right eye, left eye, nose
    cv::Point2f from[3]{{651, 267}, {793, 356}, {657, 405}};
    cv::Point2f to[3] = {{65, 90}, {95, 90}, {80, 120}};

    // create affine transform class
    cout << "===============basic==================" << endl;
    Affined_Image AI("Image", img);

    // creat the clicking window to let user set 3 points by mouse
    cout << "Set 3 point by clicking on the image: " << endl; 
    cv::namedWindow(AI.getName());
    cv::imshow(AI.getName(), img);
    cv::setMouseCallback(AI.getName(), mouseCall);
    //show the image
    cv::imshow(AI.getName(), img);
    // Wait until user click mouse
    while (char(cv::waitKey(1) != 'q') && click < 4)
    {
        if (!_lock)
        {
            std::cout << "set position ( " << posX << ", " << posY << " )" << endl;
            from[click - 1].x = posX;
            from[click - 1].y = posY;
            _lock = true;
        }
    }

    // set three point to generate transform matrix
    cout << "From position: " << from[0] << ", " << from[1] << ", " << from[1] << endl;
    cout << "To position: " << to[0] << ", " << to[1] << ", " << to[1] << endl;
    AI.setSrcPt(from);
    AI.setProjPt(to);

    // set output image size
    AI.setOutputImgSize(160, 190);

    // Generating output image
    cout << "Generation transform matrix" << endl;
    AI.update();

    // store result image to current directory
    cout << "Store result image" << endl;
    cv::imwrite("res.jpg", AI.getResImage());


    cout << "===========reverse===================" << endl;
    Affined_Image REV("rev", AI.getResImage());

    cout << "From position: " << to[0] << ", " << to[1] << ", " << to[1] << endl;
    cout << "To position: " << from[0] << ", " << from[1] << ", " << from[1] << endl;
    
    REV.setSrcPt(to);
    REV.setProjPt(from);

    // set output image size
    REV.setOutputImgSize(img.size());

    // Generating output image
    cout << "Generation transform matrix" << endl;
    REV.update();

    // store result image to current directory
    cout << "Store result image" << endl;
    cv::imwrite("res_rev.jpg", REV.getResImage());
    return 0;
}

