#ifndef _IMGSEGMENT_H_
#define _IMGSEGMENT_H_

#include <string>
#include <iostream>

#include "CImg.h"

using namespace std;
using namespace cimg_library;

class ImgSegment {
public:
    ImgSegment(string imgPath);
    CImg<double> ToGrayScale(CImg<double>);
    
private:
    CImg<double> sourceImg;
    CImg<double> resultImg;
};

#endif // !_IMGSEGMENTATION