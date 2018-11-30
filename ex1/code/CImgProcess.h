#ifndef _CIMGProcess_
#define _CIMGProcess_

#include "CImg.h"
#include <cmath>
#include <iostream>
#define PI 3.1415926535
using namespace cimg_library;
using namespace std;

unsigned char WHITE[] = {255, 255, 255};
unsigned char BLACK[] = {0, 0, 0};
unsigned char YELLOW[] = {255, 255, 0};
unsigned char RED[] = {255, 0, 0};
unsigned char GREEN[] = {0, 255, 0};
unsigned char BLUE[] = {0, 0, 255};

class CImgProcess {
public:

  CImgProcess() {}

  /**
   * 1.load the image file, and display the image by CImg.display()
   **/
  Process1() {
    image.load("../img/1.bmp");
    image.display();
  }

  /**
   * 2.dye the image's white area into red, and black are into green
   */
  void Process2() {
    cimg_forXY(image, x, y) {
      if (image(x,y,0) == WHITE[0] && image(x,y,1) == WHITE[1] && image(x,y,2) == WHITE[2]) {
        image(x,y,0) = RED[0];
        image(x,y,1) = RED[1];
        image(x,y,2) = RED[2];
      }
      else if (image(x,y,0) == BLACK[0] && image(x,y,1) == BLACK[1] && image(x,y,2) == BLACK[2] ){
        image(x,y,0) = GREEN[0];
        image(x,y,1) = GREEN[1];
        image(x,y,2) = GREEN[2];
      }
    }
  }

  /**
   * 3.draw a circle area, coordinate(50,50), radius is 30, color is blue
   * 1) no using CImg function
   * 2) using CImg function
   */
  void Process3_drawBySelf() {
    cimg_forXY(image, x, y) {
      if (pow(x-50,2) + pow(y-50,2) <= 900) {
        image(x,y,0) = BLUE[0];
        image(x,y,1) = BLUE[1];
        image(x,y,2) = BLUE[2];
      }
    }
  }

  void Process3_drawByCImg() {
    image.draw_circle(50,50,30,BLUE,1);
  }

  /**
   * 4.draw a circle area, coordinate(50,50), radius is 3, color is yellow
   * 1) no using CImg function
   * 2) using CImg function
   */
  void Process4_drawBySelf() {
    cimg_forXY(image, x, y) {
      if (pow(x-50,2) + pow(y-50,2) <= 9) {
        image(x,y,0) = YELLOW[0];
        image(x,y,1) = YELLOW[1];
        image(x,y,2) = YELLOW[2];
      }
    }
  }

  void Process4_drawByCImg() {
    image.draw_circle(50,50,3,YELLOW,1);
  }

  /**
   * 5.draw a line, start point(0,0), length 100, angle is 35, color is blue
   * 1) no using CImg function
   * 2) using CImg function
   */
  void Process5_drawBySelf() {
    cimg_forXY(image, x, y) {
      if (pow(x,2) + pow(y,2) <= 10000 && y - x*tan(35*PI/180) <= 0.5 && x*tan(35*PI/180) - y <= 0.5){
        image(x,y,0) = BLUE[0];
        image(x,y,1) = BLUE[1];
        image(x,y,2) = BLUE[2];
      }
    }
    image.display();
  }
  void Process5_drawByCImg() {
    image.draw_line(0,0,100*cos(35*PI/180),100*sin(35*PI/180),BLUE);
    image.display();
  }

  /**
   * 6.save the image 
   */
  void Process6() {
    image.save("../img/2.bmp");
  }

private:
  CImg<unsigned char> image;
};

#endif