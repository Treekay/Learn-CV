#ifndef _HOUGHLINE_H
#define _HOUGHLINE_H

#include <iostream>
#include <vector>
#include <string>
#include <cmath>

#include "../CImg.h"
#include "../myCanny.h"

using namespace std;
using namespace cimg_library;

class houghLine {
public:
  houghLine(string, string, int, int, int);

  void houghTransform(); // hough lines transform
  void houghVote(int, int); // Vote for the point of hough space
  void getMaxHough(int, int, int); // get a maxHough of a Xsize * Ysize square
  void drawLines(); // hough draw lines with blue color
  void detectLines(); // detect the lines of the papers with red color
  void detectPoints(); // detect the intersection point with red color
private:
  CImg<double> img; // the img of canny
  CImg<double> edgeImg; // the edge img for process
  CImg<double> houghImg; // the hough space img
  vector<pair<int, int>> lines; // <k, b>
  unsigned int width, height; // the width and height of the image
  int theta, rho; // the polar coordinates axis of hough space
};

#endif