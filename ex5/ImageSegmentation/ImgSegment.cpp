#include "ImgSegment.hpp"

ImgSegment::ImgSegment(string imgPath){
    // 读取图片
    sourceImg.load(imgPath.c_str());

}

/*
  将图像转为灰度图以进行Canny边缘检测
  img -> grayscaled
*/
void ImgSegment::toGrayScale(CImg<double> img) {
    CImg<double> grayscaled = CImg<double>(width, height); // To one channel
    cimg_forXY(img, x, y) {
        grayscaled(x, y) = img(x, y, 0) * 0.2126 + img(x, y, 1) * 0.7152 + img(x, y, 2) * 0.0722;
    }
}