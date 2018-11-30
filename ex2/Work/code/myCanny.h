#ifndef _MYCANNY_
#define _MYCANNY_

#include "CImg.h"
#include <string>

#define _USE_MATH_DEFINES

using namespace std;
using namespace cimg_library;

/*
step0:转成灰度图像
step1:用高斯滤波器平滑图象；
step2:用Sobel计算梯度的幅值和方向；
step3:对梯度幅值进行非极大值抑制；
step4:用双阈值算法检测, 抑制孤立弱边缘
step5:连接边缘, 消除长度小于20的边缘
*/
class canny {
private:
  CImg<double> img; // Original Image
  CImg<double> grayscaled; // Grayscale
  CImg<double> gFiltered; // Gaussian Filtered
  CImg<double> sFiltered; // Sobel Filtered
  CImg<double> xGradient; // x-gradient image
  CImg<double> yGradient; // y-gradient image
  CImg<double> nonMaxSupped; // Non-maxima supp.
  CImg<double> thresholded; // Double threshold
  CImg<double> linked; // Link adjacent edges and delete edges shorter than 20
  unsigned int width, height; // the width and height of the img
public:
  canny(string, int, double, double, double, string); //Constructor
  void toGrayScale(); // Convert to grayscale image
  void gaussianFilter(int, int, double); // gaussian filtering
  void sobelFilter(); // Sobel filtering
  void nonMaxSupp(); // Non-maxima suppression
  void threshold(double, double); // Double threshold and finalize img
  void linkEdge(); // Link adjacent edges and delete edges shorter than 20
};

#endif