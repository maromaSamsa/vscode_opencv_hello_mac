#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <vector>
#include <string>

using namespace cv;
using namespace std;

int main()
{   
    // 測試擴增函式庫是否能正常使用
    vector<string> msg {"Hello", "C++", "World", "from", "VS Code", "and the C++ extension!"};

    for (const string& word : msg)
    {
        cout << word << " ";
    }
    cout << endl;

    // 測試 opencv 能否運作
    cv::Mat img = cv::imread("W_A1_0_3.jpg");
    if(img.empty()){
        cout << "open img failed " << endl;
    }
    cv::imshow("Example", img);
    cv::waitKey(0);
    return 0;
}