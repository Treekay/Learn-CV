#ifndef _IMGSEGMENT_H_
#define _IMGSEGMENT_H_

#include <string>
#include <queue>
#include <vector>
#include <iostream>

#include "CImg.h"

using namespace std;
using namespace cimg_library;

class ImgSegment {
public:
    ImgSegment(string imgPath, string _resPath);
    CImg<double> gaussianFilter();
    void clustering();
    void edgeDetect();
    void edgeDelete(int);
    void getPoints();
    void calHomography();
    void inverseProject();
  
private:
    CImg<double> sourceImg;
    CImg<double> clusterImg;
    CImg<double> edgeImg;
    CImg<double> lineImg;
    CImg<double> resultImg;
    vector<pair<int, int>> points;
    vector<pair<double, double>> sourcePoints;
    vector<pair<double, double>> targetPoints;
    double H[8];
    int width, height;
    string resPath;
};

#endif // !_IMGSEGMENTATION