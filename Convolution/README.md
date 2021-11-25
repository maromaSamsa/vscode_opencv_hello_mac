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

### 注意 ###
未做其他考慮，因此只能讀取.jpg

### Main topic ###
1. familiar with Operations with images, [ref](https://docs.opencv.org/3.4/d5/d98/tutorial_mat_operations.html)
2. Image type configuration [ref](http://ninghang.blogspot.com/2012/11/list-of-mat-type-in-opencv.html)

3. Convolution concept

### Program explanation ###

 - to_grayScale(): RGB image to gray scale iamge
 - dilation(): A morphological method
 - erosion(): A morphological method
 - subtraction(): Subtract two image and make sure all pixel values is in range 0 - 255
 - blur(): Averaging blurring implementation
