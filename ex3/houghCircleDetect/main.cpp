#include <iostream>
#include <cmath>
#include <vector>

#include "../CImg.h"

#include "../myCanny.cpp"
#include "houghCircle.cpp"

using namespace std;
using namespace cimg_library;

int main() {
  /* parameters list: file path, filename, thresh, minR, maxR*/
  houghCircle hg1("Dataset2/coin1.bmp", "result/coin1.bmp", 140, 190, 235);
  houghCircle hg2("Dataset2/coin2.bmp", "result/coin2.bmp", 130, 105, 130);
  houghCircle hg3("Dataset2/coin3.bmp", "result/coin3.bmp", 90, 100, 210);
  houghCircle hg4("Dataset2/coin4.bmp", "result/coin4.bmp", 160, 35, 65);
  houghCircle hg5("Dataset2/coin5.bmp", "result/coin5.bmp", 80, 280, 350);
  houghCircle hg6("Dataset2/coin6.bmp", "result/coin6.bmp", 150, 150, 160);

  return 0;
}