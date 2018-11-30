#ifndef _COLORTRANSFER_H
#define _COLORTRANSFER_H

#include <cmath>
#include <string>

#include "CImg.h"

using std::string;
using namespace cimg_library;

class colorTransfer {
public:
    colorTransfer(string, string, string); // construct function
    CImg<double> rgbTolab(CImg<double>);    // convert RGB to lab
    CImg<double> labToRGB(CImg<double>);    // convert lab to RGB
    void transferlab();                    // transfer lab
private:
    CImg<double> sourceImg;          // source image
    CImg<double> targetImg;          // target image
};

#endif