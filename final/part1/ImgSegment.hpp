#ifndef _IMGSEGMENT_H_
#define _IMGSEGMENT_H_

#include <queue>
#include <vector>
#include <iostream>
#include <string>
#include <fstream>

#include "CImg.h"

using namespace std;
using namespace cimg_library;

class ImgSegment {
public:
    ImgSegment(string, string, string);
    CImg<double> gaussianFilter();              // 高斯模糊
    void clustering();                          // KMeans聚类
    void edgeDetect();                          // 边缘检测
    void edgeDelete(int);                       // 边缘处理
    void getPoints();                           // 特征点获取
    void calHomography();                       // 计算变换矩阵
    void inverseProject();                      // 逆映射和双线性差值
    void numberEdge();                          // 数字边缘
    void numberExpand();
    CImg<double> Expand4(CImg<double>);         // 4领域膨胀
    CImg<double> Expand8(CImg<double>);         // 8领域膨胀
    CImg<double> Corros8(CImg<double>);         // 4领域腐蚀
    void numberSegment();                       // 切割数字
    void numberStandard();                      // 规格化数字图片
    vector<CImg<double>> getImages(); 
  
private:
    CImg<double> sourceImg;                     // 原图像
    CImg<double> clusterImg;                    // 聚类图像
    CImg<double> edgeImg;                       // 边缘图像
    CImg<double> expandImg;                     // 膨胀图像
    CImg<double> segmentImg;                    // 切割后的图像
    vector<pair<int, int>> points;               
    vector<pair<double, double>> sourcePoints;  // 原图像A4纸的四个角点
    vector<pair<double, double>> targetPoints;  // 矫正图像A4纸的四个角点
    double H[8];                                // 从矫正图像变换到原图像的逆变换矩阵
    int width, height;                          // 原图像宽高
    string resPath;                             // 文件保存路径
    string fileName;                            // 文件名

    vector<CImg<double>> digitImgs;
    vector<CImg<double>> stdImages;
};

#endif // !_IMGSEGMENTATION