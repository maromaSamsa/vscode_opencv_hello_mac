# 仿射轉換和平方內插 #

**開發環境和套件**
- vscode on mac
- Visual Studio Code - 1.61.0
- opencv ( Install by Homebrew ) - 4.5.3
- clang++ 13.0.0

**執行**
可使用終端機進入主資料夾，並直接執行執行檔 main
```shell=
./ main ＄{ImageName}.jpg
```

執行後在根目錄會出現兩個檔案：
 - res.jpg 是輸出影像
 - res_rev.jpg 是還原影像

### 概述 ###
1. 未做其他考慮，只能讀取三通道的RGB.jpg
2. 轉換過後的圖片在沒有填色的地方有時會出現雜訊，原因未知
3. 未能實作出內差運算，相關程式碼位於 private Affined_Image::generateResImg() 中註解的部分 （line 123 - 147）



### 說明 ###
自定義 Affined_Image class:
- private   Affined_Image::flag                 標記哪一些必要的變數已經初始化，這會作為是否可以生成 affine matrix 的基準
- private   std::string name                    輸入影像名
- private   cv::Mat img                         輸入影像
- private   cv::Point2f fromTri[3]              投影點
- private   cv::Point2f toTri[3]                映射點 (T:投影點 -> 映射點)
- private   cv::Matx33f affineMatrix            轉換陣列（T）
- private   cv::Size outputImgSize              輸出影像大小
- private   cv::Mat res                         輸出影像
- private   void setAffineTransformMatrix()     生成轉換陣列
- private   void generateResImg()               生成輸出影像

- public Affined_Image()            建構子
- public std::string getName()      取得數入影像名稱
- public cv::Mat getResImage()      取得輸出影像
- public void setImage()            設定輸入影像
- public bool setSrcPt()            設定投影點
- public bool setProjPt()           設定映射點
- public void setOutputImgSize()    設定輸出影像大小
- public void update()              更新內部參數，包含生成轉換陣列和輸出影像
- public void showResImg()          顯示輸出影像
