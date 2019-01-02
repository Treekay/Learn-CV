#include "houghLine.h"

unsigned char BLUE[] = {0, 0, 255};
unsigned char RED[] = {255, 0, 0};

houghLine::houghLine(CImg<double> _img, int Xsize, int Ysize, int thresh) {
  edgeImg = _img;
  width = edgeImg.width();
  height = edgeImg.height();
  img = CImg<double>(width, height, 1, 3); // 用于显示的边缘图
  cimg_forXY(img, x, y) {
    img(x, y, 0) = edgeImg(x, y);
    img(x, y, 1) = edgeImg(x, y);
    img(x, y, 2) = edgeImg(x, y);
  }

  // 霍夫直线检测
  houghTransform(); // 作直线霍夫变换
  getMaxHough(Xsize, Ysize, thresh); // 得到霍夫空间中给定区域内的大于阈值的票数最高的点
  drawLines(); // 画出霍夫变换得到的直线
  detectPoints(); // 画出霍夫变换检测的
}

/*
  霍夫变换：
  根据变换规则对霍夫空间上相应的点进行投票
 */
void houghLine::houghTransform() {
  /* 进行霍夫空间极坐标变换 */
  theta = 180; // 横轴
  rho = (width + height) * 2; // 纵轴

  // 初始化霍夫空间
  houghImg = CImg<double>(theta, rho);
  houghImg.fill(0);
  cimg_forXY(edgeImg, x, y) {
    // 对边缘图上不为0的点(即边缘点)进行投票
    if (edgeImg(x, y) != 0) {
      houghVote(x, y);
    }
  }
  // cout << "Hough Line Transfrom Done" << endl;
}

/* 
  原边缘图像以左上角点为原点
  x 轴从左往右, 定义域为 0 ~ width-1
  y 轴从上往下, 定义域为 0 ~ height-1
  根据直线的极坐标方程 x * cos(angle) + y * sin(angle) = r
  变换到霍夫空间
  theta轴从左往右, 定义域为 -90 ~ 90
  r 轴从上往下, 定义域为 0 ~ (width + height)*2
 */
void houghLine::houghVote(int x, int y) {
  for (int i = -90; i < 90; i++) {
    // 求出经过该点的所有直线的极坐标参数
    double angle = M_PI * i / 180;
    double r = x * cos(angle) + y * sin(angle);
    if (r > -rho / 2 && r < rho / 2) {
      // 因为实际下标不能为负, 需要转换为正数, 读出时再转为对应的真实值
      houghImg(i + theta / 2, rho / 2 + r)++;
    }
  }
}

/* 
  找出霍夫空间上给定区域内票数较高的点 
  给出适当的参数 Xsize, Ysize, thresh
  搜索每个大小为 Xsize * Ysize 的方形区域内,投票数最高的点
  若该点投票数高于给定阈值, 则认为该点对应的参数可以作为检测到的直线参数
 */
void houghLine::getMaxHough(int Xsize, int Ysize, int thresh) {
  for (int x = 0; x < theta; x += Xsize) {
    for (int y = 0; y < rho; y += Ysize) {
      pair<int, int> max = make_pair(x, y);
      // 确定搜索范围
      int i_max = (x + Xsize < theta) ? (x + Xsize ) : theta;
      int j_max = (y + Ysize < rho) ? (y + Ysize ) : rho;
      // 寻找最大值
      for (int i = x; i < i_max; i++) {
        for (int j = y; j < j_max; j++) {
          if (houghImg(i, j) > houghImg(max.first, max.second)) {
            max = make_pair(i, j);
          }
        }
      }
      // 若最大值大于给定阈值则保存用于画线
      if (houghImg(max.first, max.second) > thresh) {
        lines.push_back(make_pair(max.first, max.second));
      }
    }
  }
  // cout << "Get Max Hough Done" << endl;
}

/* 
  将获得的最大点对应的参数作为直线的极坐标方程的参数
  对每条直线扫描原边缘图
  满足直线极坐标方程的点则涂为蓝色(误差在一定范围内认为满足方程)
 */
void houghLine::drawLines() {
  /* 画出投票数较高的直线 */
  // cout << "The Line Equation: " << endl;
  for (int i = 0; i < lines.size(); i++) {
    double angle = M_PI * (lines[i].first - theta / 2) / 180;
    double r = lines[i].second - rho / 2;

    /* 输出直线方程 */
    // if(sin(angle) == 0) {
    //   printf("x = %.0f \t%d\t%d \n", r / cos(angle), lines[i].first, lines[i].second);
    // }
    // else if (cos(angle) == 0) {
    //   printf("y = %.0f \t%d\t%d \n", r / sin(angle), lines[i].first, lines[i].second);
    // }
    // else {
    //   printf("y = %fx%+.0f \t%d\t%d \n", -cos(angle) / sin(angle), r / sin(angle), lines[i].first, lines[i].second);
    // }

    /* 在边缘图上画出直线 */
    cimg_forXY(img, x, y) {
      if (abs(r - x * cos(angle) - y * sin(angle)) <= 1) {
        img(x, y, 0) = BLUE[0];
        img(x, y, 1) = BLUE[1];
        img(x, y, 2) = BLUE[2];
      }
    }
  }
  cout << "Hough lines Draw Done" << endl;
  // img.display("Hough lines Draw");
}

/* 
  检测直线交点:
  对每两条直线, 判断边缘图上的点是否同时满足两条直线方程,
  若满足则该点为交点, 以该点为圆心画圆
 */
void houghLine::detectPoints() {
  // 比较每两条直线
  for (int i = 0; i < lines.size(); i++) {
    for (int j = i + 1; j < lines.size(); j++) {
      // 求出对应的极坐标参数
      double angle1 = M_PI * (lines[i].first - theta / 2) / 180;
      double r1 = lines[i].second - rho / 2;
      double angle2 = M_PI * (lines[j].first - theta / 2) / 180;
      double r2 = lines[j].second - rho / 2;

      double x = (sin(angle1) * r2 - sin(angle2) * r1) / 
                (cos(angle2) * sin(angle1) - cos(angle1) * sin(angle2));
      double y;
      if (lines[i].first != 90) {
        y = (r1 - x * cos(angle1)) / sin(angle1);
      } else {
        y = (r2 - x * cos(angle2)) / sin(angle2);
      }
      if ((int)x < 0 || (int)x > width - 1 || (int)y < 0 || (int)y > height - 1)
        continue;

      // cout << "x: " << (int)x << endl;
      // cout << "y: " << (int)y << endl;
      img.draw_circle((int)x, (int)y, 5, RED);
      points.push_back(make_pair((int)x, (int)y));
    }
  }
  cout << "Hough Point Detect Done" << endl;
  img.display("Hough Point Detect");
}

vector<pair<int, int>> houghLine::getPoints() {
  return points;
}