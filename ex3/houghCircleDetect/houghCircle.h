#ifndef _HOUGHCIRCLE_H
#define _HOUGHCIRCLE_H

#include <iostream>
#include <vector>
#include <string>
#include <cmath>

#include "../CImg.h"
#include "../myCanny.h"

using namespace std;
using namespace cimg_library;

class houghCircle {
public:
  houghCircle(string, string, int, int, int);

  void circlesTransform(int); // hough circles transform
  void houghVote(int, int, int); // Vote for the point of hough space
  void getMaxHough(); // get a maxHough of a Xsize * Ysize square
  void drawCircles(); // hough draw the circle with blue color
  void detectCircles(); // hough detect the circle with red color
  void countCoins(); // counts the num of coins
private:
  CImg<double> img; // the img of canny
  CImg<double> edgeImg; // the edge img for process
  CImg<double> houghImg; // the hough space img
  vector<pair<int, int>> centresSet; // all the possible centres
  vector<int> radiusSet; // all the possible radius
  vector<pair<int, int>> centres; // the maxhough centres in given area
  vector<int> radius; // the maxhough radius in given area
  unsigned int width, height; // the width and height of the image
  int minR, maxR; // the radius range of detecting
  double mySin[360];
  double myCos[360];
};

#endif