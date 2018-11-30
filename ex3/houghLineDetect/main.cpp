#include <iostream>
#include <cmath>
#include <vector>

#include "../CImg.h"

#include "../myCanny.cpp"
#include "houghLine.cpp"

using namespace std;
using namespace cimg_library;

int main() {
  // parameters list: file path, Xsize, Ysize, thresh
  houghLine hg1("Dataset1/paper1.bmp", "result/paper1.bmp", 39, 499, 200);
  houghLine hg2("Dataset1/paper2.bmp", "result/paper2.bmp", 39, 499, 200);
  houghLine hg3("Dataset1/paper3.bmp", "result/paper3.bmp", 39, 499, 200);
  houghLine hg4("Dataset1/paper4.bmp", "result/paper4.bmp", 39, 499, 200);
  houghLine hg5("Dataset1/paper5.bmp", "result/paper5.bmp", 39, 499, 200);
  houghLine hg6("Dataset1/paper6.bmp", "result/paper6.bmp", 39, 499, 200);
  
  return 0;
}