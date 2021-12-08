# 模板偵測 #

**開發環境和套件**
- vscode on mac
- Visual Studio Code - 1.61.0
- opencv ( Install by Homebrew ) - 4.5.3
- clang++ 13.0.0

**執行**
可使用終端機進入主資料夾，並直接執行執行檔 main
```shell=
./ main ＄{srcImageName} ＄{templImageName} ＄{threshold_value}
```
終端機會顯示 opencv function 和自己設計的程式各自的執行時間

執行後在根目錄會出現：
 - implement.jpg 做完比對後的結果

### 概述 ###
1. 發現閥值設定越大，執行程式時間越短
2. 有使用圖形金字塔加速，越多層會越快，在這支程式做了兩次，理論上減少1/16的計算量
3. 有使用到 E(X*Y) - uv 加速優化
4. opencv 結果可以imshow出，但是imwrite無法寫入，原因不明

### 說明 ###
無