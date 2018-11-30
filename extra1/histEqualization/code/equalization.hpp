#ifndef EQUALIZATION_H
#define EQUALIZATION_H

#include <iostream>
#include <string>

#include "CImg.h"

using std::string;
using std::cout;
using namespace cimg_library;

class Equalization {
public:
    Equalization(string, string);           // construction function
    void toGrayScale();                     // convert the color image to gray image
    void grayEqualization();                // do histogram equalization for gray image
    void colorEqualization();               // do histogram equalization for color image
  private:
    CImg<unsigned char> grayImg;            // gray image
    CImg<unsigned char> colorImg;           // color image
    int width, height;                      // the width and height of the origin image
    int pixelNum;                           // the total number of pixels of origin image
};

#endif // !EQUALIZATION_H