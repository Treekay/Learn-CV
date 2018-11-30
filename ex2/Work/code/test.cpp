#include <iostream>
#include <cmath>
#include <vector>
#include "CImg.h"
#include "myCanny.h"

using namespace std;
using namespace cimg_library;

int main() {
  int gsize = 5; // size of Gaussian Filter
  double sigma = 1; // sigma of Gaussian Filter
  double highThresh = 60; // highThresh of Double Threshold
  double lowThresh = 30; // lowThresh of Double Threshold
  cout << "The size (Must be odd) of GaussianFilter Kernel: ";
  cin >> gsize;
  cout << "The sigma of GaussianFilter Kernel: ";
  cin >> sigma;
  cout << "The highThresh of Threshold: ";
  cin >> highThresh;
  cout << "The lowThresh of Threshold: ";
  cin >> lowThresh;

  canny cny1("../test_Data/lena.bmp", gsize, sigma, highThresh, lowThresh, "lena");
  canny cny2("../test_Data/bigben.bmp", gsize, sigma, highThresh, lowThresh, "bigben");
  canny cny3("../test_Data/stpietro.bmp", gsize, sigma, highThresh, lowThresh, "stpietro");
  canny cny4("../test_Data/twows.bmp", gsize, sigma, highThresh, lowThresh, "twows");
  return 0;
}