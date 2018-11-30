#include "houghCircle.h"

unsigned char BLUE[] = {0, 0, 255};
unsigned char RED[] = {255, 0, 0};

houghCircle::houghCircle(string filePath, string filename, int thresh, int minR, int maxR) {
  canny cny(filePath, 5, 1, 60, 30); // 读取图片并进行canny边缘检测

  CImg<double> cannyImg = cny.edgeDelete(20);
  width = cannyImg.width();
  height = cannyImg.height();
  img = CImg<double>(width, height, 1, 3); // 用于显示的边缘图
  cimg_forXY(img, x, y) {
    img(x, y, 0) = cannyImg(x, y);
    img(x, y, 1) = cannyImg(x, y);
    img(x, y, 2) = cannyImg(x, y);
  }
  edgeImg = cny.edgeDelete(100); // 实际用于做霍夫变化的边缘图

  this->minR = minR;
  this->maxR = maxR;

  // 预先求得各角度的sin, cos函数值
  for (int angle = 0; angle < 360; angle++) {
    mySin[angle] = sin(angle * M_PI / 180);
    myCos[angle] = cos(angle * M_PI / 180);
  }

  img.display("Edge Image");

  // 霍夫圆检测
  circlesTransform(thresh); // 作圆霍夫变换
  getMaxHough(); // 得到霍夫空间中给定区域内的大于阈值的票数最高的点
  drawCircles(); // 画出霍夫变换得到的圆
  detectCircles(); // 用霍夫变换得到的圆检测原边缘图中的硬币边缘
  countCoins(); // 数出硬币的个数

  // 保存最终结果图片
  img.save(filename.c_str());
}

/*
  霍夫变换：
  根据变换规则对霍夫空间上相应的点进行投票
 */
void houghCircle::circlesTransform(int thresh) {
  /* 对每个给定半径的霍夫空间投票*/
  for (int r = minR; r <= maxR; r++) {
    houghImg = CImg<double>(width, height);
    houghImg.fill(0);
    cimg_forXY(edgeImg, x, y) {
      // 对边缘图上不为0的点(即边缘点)进行投票
      if (edgeImg(x, y) != 0) {
        houghVote(x, y, r);
      }
    }

    // 仅筛选出大于给定阈值的点
    cimg_forXY(houghImg, a, b) {
      if (houghImg(a, b) > thresh) {
        centresSet.push_back(make_pair(a, b));
        radiusSet.push_back(r);
      }
    }
  }
  cout << "Hough Circles Transfrom Done" << endl;
}

/*
  原边缘图像以左上角点为原点
  x 轴从左往右, 定义域为 0 ~ width-1
  y 轴从上往下, 定义域为 0 ~ height-1
  根据圆的方程 
    (x - a)^2 + (y - b)^2 = r^2
  变换到霍夫空间
  a 轴从左往右, 定义域为 0 ~ width-1
  b 轴从上往下, 定义域为 0 ~ height-1
 */
void houghCircle::houghVote(int x, int y, int r) {
  // 求出经过该点的所有圆的极坐标参数并投票
  for (int angle = 0; angle < 360; angle++) {
    int a = x - r * myCos[angle];
    int b = y - r * mySin[angle];
    if (a > 0 && a < width && b > 0 && b < height) {
      houghImg(a, b)++;
    }
  }
}

/*
  找出霍夫空间上给定区域内票数较高的点 
  对于距离小于 minR 的两个可能圆心, 选对应半径较大的一个
 */
void houghCircle::getMaxHough() {
  /* 筛除圆心相近的点, 每个圆心附近保留投票数最大的一个圆 */
  for (int i = 0; i < centresSet.size(); i++) {
    if (radiusSet[i] != 0) {
      int max = i; // position
      for (int j = 0; j < centresSet.size(); j++) {
        /*  判断是否在给定邻域内 */
        int maxX = centresSet[max].first;
        int maxY = centresSet[max].second;
        int curX = centresSet[j].first;
        int curY = centresSet[j].second;
        if (max != j && abs(maxX - curX) < minR && abs(maxY - curY) < minR) {
          /* 判断半径是否最大 */
          if (radiusSet[max] > radiusSet[j]) {
            radiusSet[j] = 0; // 投票数少的半径置为0
          }
          else {
            radiusSet[max] = 0; // 投票数少的半径置为0
            max = j;
          }
        }
      }
    }
  }
  for (int i = 0; i < centresSet.size(); i++) {
    if (radiusSet[i] != 0) {
      centres.push_back(centresSet[i]);
      radius.push_back(radiusSet[i]);
    }
  }
  cout << "Get Max Hough Done" << endl;
}

/*
  将获得的最大点对应的参数作为圆的方程的参数
  对每个圆扫描原边缘图
  满足圆方程的点则涂为蓝色(误差在一定范围内认为满足方程)
 */
void houghCircle::drawCircles() {
  cout << "The Circle Equation: " << endl;
  for (int i = 0; i < radius.size(); i++) {
    int a = centres[i].first;
    int b = centres[i].second;
    int r = radius[i];

    // 根据圆的大小设置误差的大小, 可以控制画出的圆的线条粗细
    int rough = r * r * 0.025;

    /* 输出圆方程 */
    cout << "(x - " << a << ")^2 + (y - " << b << ")^2 = " << r << "^2" << endl;

    /* 在边缘图上画出圆 */
    cimg_forXY(img, x, y) {
      if (abs(pow(x - a, 2) + pow(y - b, 2) - pow(r, 2)) < rough) {
        img(x, y, 0) = BLUE[0];
        img(x, y, 1) = BLUE[1];
        img(x, y, 2) = BLUE[2];
      }
    }
  }
  cout << "Hough Circles Draw Done" << endl;
  img.display("Hough Circles Draw");
}

/*
  检测边缘:
  所画出的圆的八邻域内存在实际边缘点, 则视为检测到的边
 */
void houghCircle::detectCircles() {
  cimg_forXY(img, x, y) {
    if (img(x, y, 0) == BLUE[0] && img(x, y, 1) == BLUE[1] && img(x, y, 2) == BLUE[2]) {
      for (int i = x - 1; i < x + 2; i++) {
        for (int j = y - 1; j < y + 2; j++) {
          // 搜索八邻域
          if (i >= 0 && i < width && j >= 0 && j < height) {
            if (edgeImg(i, j) != 0) {
              img(i, j, 0) = RED[0];
              img(i, j, 1) = RED[1];
              img(i, j, 2) = RED[2];
            }
          }
        }
      }
    }
  }
  cout << "Hough Circles Detect Done" << endl;
  img.display("Hough Circles Detect");
}

/*
  根据得到的圆方程数量即可输出硬币的数量
 */
void houghCircle::countCoins() {
  cout << "The number of coins is: " << centres.size() << endl;
}