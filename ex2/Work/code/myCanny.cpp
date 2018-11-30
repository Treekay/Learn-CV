#include <iostream>
#include <cmath>
#include <queue>
#include "CImg.h"
#include "myCanny.h"

#define _USE_MATH_DEFINES

using namespace std;
using namespace cimg_library;

canny::canny(string filename, int gsize, double sigma, double highThresh, double lowThresh, string file) {
  // load the image
  img.load(filename.c_str());

  // Check for invalid input
  if (!img.data()) {
    cout << "Could not open or find the image" << endl;
  }
  else {
    width = img.width();
    height = img.height();

    // canny processing the image
    toGrayScale(); // Grayscale the image
    gaussianFilter(gsize, gsize, sigma); // Gaussian Filtering
    sobelFilter(); // Sobel Filtering
    nonMaxSupp(); //Non-Maxima Suppression
    threshold(highThresh, lowThresh); // Double Threshold and Finalize
    linkEdge(); // Link adjacent edges and delete edges shorter than 20

    // saving the processing image
    string str = "../result_Data/";
    grayscaled.save((str + file + string("_GrayScaled.bmp")).c_str());
    gFiltered.save((str + file + string("_GaussianBlur.bmp")).c_str());
    sFiltered.save((str + file + string("_SobelFiltered.bmp")).c_str());
    nonMaxSupped.save((str + file + string("_NonMaxSupped.bmp")).c_str());
    thresholded.save((str + file + string("_DoubleThreshold.bmp")).c_str());
    linked.save((str + file + string("_linkEdge.bmp")).c_str());

    // display the procesing image
    img.display("Original");
    grayscaled.display("GrayScaled");
    gFiltered.display("Gaussian Blur");
    sFiltered.display("Sobel Filtered");
    nonMaxSupped.display("Non-Maxima Suppression");
    thresholded.display("Double Threshold");
    linked.display("Link Edge");
  }
}

/*
  将图像转为灰度图以进行Canny边缘检测
  img -> grayscaled
*/
void canny::toGrayScale() {
  grayscaled = CImg<double>(width, height); // To one channel
  cimg_forXY(img, x, y) {
    double r = img(x, y, 0);
    double g = img(x, y, 1);
    double b = img(x, y, 2);

    double newValue = (r * 0.2126 + g * 0.7152 + b * 0.0722);
    grayscaled(x, y) = newValue;
  }
}

/*
  使用高斯滤波器并对图像进行卷积，以平滑图像，消除噪声
  grayscaled -> gFiltered
  @param rows: 高斯滤波器行数
  @param cols: 高斯滤波器列数
  @param sigmaIn: 滤波器 σ 值
*/
void canny::gaussianFilter(int rows, int cols, double sigmaIn) {
  CImg<double> filter(rows, cols);
  double coordSum;
  double constant = 2.0 * sigmaIn * sigmaIn;
  // Sum is for normalization
  double sum = 0.0;
  for (int x = -rows / 2; x <= rows / 2; x++) {
    for (int y = -cols / 2; y <= cols / 2; y++) {
      coordSum = x * x + y * y;
      filter(x + rows / 2, y + cols / 2) = exp(-(coordSum) / constant) / (M_PI * constant);
      sum += filter(x + rows / 2, y + cols / 2);
    }
  }
  // Normalize the Filter
  cimg_forXY(filter, x, y) {
    filter(x, y) /= sum;
  }
  // Use the Filter
  gFiltered = grayscaled.get_convolve(filter);
}

/*
  用Sobel算子计算梯度强度和方向
  gFiltered -> (sFiltered, angles)
*/
void canny::sobelFilter() {
  sFiltered = CImg<double>(width, height); // 梯度强度

  // Return image gradient. list
  // \param axes Axes considered for the gradient computation, as a C-string (e.g "xy").
  // \param scheme = Numerical scheme used for the gradient computation:
  // 2 - Using Sobel kernels
  auto xyGradient = gFiltered.get_gradient("xy", 2);
  xGradient = xyGradient[0];
  yGradient = xyGradient[1];

  cimg_forXY(sFiltered, x, y) {
    double Gx = xGradient(x, y);
    double Gy = yGradient(x, y);
    sFiltered(x, y) = sqrt(Gx * Gx + Gy * Gy);
  }
}

/*
  非极大值抑制:以消除边缘检测带来的杂散响应
  优化: 根据梯度的方向来决定权重,
    通过权重和八领域像素点的像素值求得插值,从而更加准确
  (sFiltered, angles) -> nonMaxSupped
*/
void canny::nonMaxSupp() {
  nonMaxSupped = CImg<double>(width, height);

  cimg_forXY(nonMaxSupped, x, y) {
    nonMaxSupped(x, y) = 0;
  }

  for (int i = 1; i < sFiltered.width() - 1; ++i) {
    for (int j = 1; j < sFiltered.height() - 1; ++j) {
      if (sFiltered(i, j) != 0) {
        double Gx = xGradient(i, j); // X方向梯度图
        double Gy = yGradient(i, j); // Y方向梯度图
        double weight; // 权重
        if (fabs(Gy) > fabs(Gx) && Gy * Gx > 0) {
          weight = fabs(Gx) / fabs(Gy);
          if (sFiltered(i, j) >= weight * sFiltered(i - 1, j - 1) + (1 - weight) * sFiltered(i, j - 1) && 
            sFiltered(i, j) >= weight * sFiltered(i + 1, j + 1) + (1 - weight) * sFiltered(i, j + 1)) {
            nonMaxSupped(i, j) = sFiltered(i, j);
          }
        }
        else if (fabs(Gy) > fabs(Gx) && Gy * Gx < 0) {
          weight = fabs(Gx) / fabs(Gy);
          if (sFiltered(i, j) >= weight * sFiltered(i + 1, j - 1) + (1 - weight) * sFiltered(i, j - 1) && 
            sFiltered(i, j) >= weight * sFiltered(i - 1, j + 1) + (1 - weight) * sFiltered(i, j + 1)) {
            nonMaxSupped(i, j) = sFiltered(i, j);
          }
        }
        else if (fabs(Gy) < fabs(Gx) && Gy * Gx > 0){
          weight = fabs(Gy) / fabs(Gx);
          if (sFiltered(i, j) >= weight * sFiltered(i - 1, j - 1) + (1 - weight) * sFiltered(i - 1, j) && 
            sFiltered(i, j) >= weight * sFiltered(i + 1, j + 1) + (1 - weight) * sFiltered(i + 1, j)) {
            nonMaxSupped(i, j) = sFiltered(i, j);
          }
        }
        else if (fabs(Gy) < fabs(Gx) && Gy * Gx < 0) {
          weight = fabs(Gy) / fabs(Gx);
          if (sFiltered(i, j) >= weight * sFiltered(i - 1, j + 1) + (1 - weight) * sFiltered(i - 1, j) && 
            sFiltered(i, j) >= weight * sFiltered(i + 1, j - 1) + (1 - weight) * sFiltered(i + 1, j)) {
            nonMaxSupped(i, j) = sFiltered(i, j);
          }
        }
      }
    }
  }
}

/*
  双阈值检测:确定真实的和潜在的边缘。抑制孤立的弱边缘
  nonMaxSupped -> thres
*/
void canny::threshold(double highThresh, double lowThresh) {
  thresholded = CImg<double>(width, height);
  // 指定一个高阈值和一个低阈值
  // 一般高阈值为整体灰度级分布的70%
  // 高阈值是低阈值的1.5倍~2倍
  if (lowThresh > 255) {
    lowThresh = 255;
  }
  if (highThresh > 255) {
    highThresh = 255;
  }

  cimg_forXY(thresholded, x, y) {
    thresholded(x, y) = nonMaxSupped(x , y);
    // 高于高阈值的像素作为边缘
    if (thresholded(x, y) > highThresh) {
      thresholded(x, y) = 255;
    }
    // 低于低阈值的像素置为非边缘
    else if (thresholded(x, y) < lowThresh) {
      thresholded(x, y) = 0;
    }
    // 对高于低阈值且低于高阈值的像素点进行处理
    else {
      bool anyHigh = false;
      for (int i = x - 1; i < x + 2; i++) {
        for (int j = y - 1; j < y + 2; j++) {
          if (i < 0 || j < 0 || i > width || j > height) //Out of bounds
            continue;
          else {
            if (thresholded(i, j) > highThresh) {
              // 若八邻域内有边缘像素点,则该像素也作为边缘
              thresholded(x, y) = 255;
              anyHigh = true;
              break;
            }
          }
        }
        if (anyHigh) {
          break;
        }
        // 八邻域内不存在边缘像素点, 则该像素不作为边缘
        if (!anyHigh) {
          thresholded(x, y) = 0;
        }
      }
    }
  }
}

/*
  连接边缘, 消除长度小于20的边缘
*/
void canny::linkEdge() {
  linked = CImg<double>(width, height);
  // initial
  cimg_forXY(linked, x, y) {
    linked(x, y) = thresholded(x, y);
  }

  /* 连接边缘 */
  // 用于标记每个像素点所属的边的标号
  CImg<double> SignMap(width, height);
  cimg_forXY(SignMap, x, y) {
    SignMap(x, y) = 0;
  }

  int sign = 1;
  // 用广度优先搜索, 给每个边缘点打上边的标记
  // 将所有的边缘点按照是否八邻域连通划分到不同标记的边上
  cimg_forXY(linked, x, y) {
    if (linked(x, y) == 255 && SignMap(x, y) == 0) {
      queue<pair<int, int>> temp;

      SignMap(x, y) = sign;
      temp.push(make_pair(x, y));
      while (!temp.empty()) {
        int nowX = temp.front().first;
        int nowY = temp.front().second;
        temp.pop();
        for (int i = nowX - 1; i < nowX + 2; i++) {
          for (int j = nowY - 1; j < nowY + 2; j++) {
            // 搜索八邻域
            if (i >= 0 && i < width && j >= 0 && j < height) {
              if (thresholded(i, j) == 255 && SignMap(i, j) == 0) {
                SignMap(i, j) = sign;
                temp.push(make_pair(i, j));
              }
            }
          }
        }
      }
      sign++;
    }
  }

  // 标记非边缘点
  // 若该点的八邻域内存在两个属于不同边的边缘点
  // 则将该点作为边缘点
  cimg_forXY(linked, x, y) {
    if (linked(x, y) == 0) {
      bool findTwoEdge = false;
      int count = 0;
      for (int i = x - 1; i < x + 2; i++) {
        for (int j = y - 1; j < y + 2; j++) {
          // 搜索八邻域
          if (i >= 0 && i < width && j >= 0 && j < height) {
            if (thresholded(i, j) == 255) {
              count++;
              // 未被标记过
              if (SignMap(x, y) == 0) {
                SignMap(x, y) = SignMap(i, j);
              }
              // 已被标记一次
              else {
                if (SignMap(x, y) != SignMap(i, j)) {
                  // 找到至少两个不同边上的边缘点
                  findTwoEdge = true;
                }
              }
            }
          }
        }
      }
      if (findTwoEdge == true && count == 2) {
        // 该点可以作为新的边缘
        linked(x, y) = 255;
      }
    }
  }

  /* 删除长度小于20的边缘 */
  // 初始化标记图
  // 该图用于标记边缘点是否访问过
  cimg_forXY(SignMap, x, y) {
    SignMap(x, y) = 0;
  }

  // 用广度优先搜索遍历每条边
  // 若该边的长度小于20,则将该边删除
  // 遍历过程中将边上的点放入容器,以便后续删除
  cimg_forXY(linked, x, y) {
    if (linked(x, y) == 255 && SignMap(x, y) == 0) {
      int count = 0;
      queue<pair<int, int>> PointSet;
      queue<pair<int, int>> temp;

      count++;
      SignMap(x, y) = 1;
      temp.push(make_pair(x, y));
      PointSet.push(make_pair(x, y));
      while (!temp.empty()) {
        int nowX = temp.front().first;
        int nowY = temp.front().second;
        temp.pop();
        for (int i = nowX - 1; i < nowX + 2; i++) {
          for (int j = nowY - 1; j < nowY + 2; j++) {
            // 搜索八邻域
            if (i >= 0 && i < width && j >= 0 && j < height) {
              if (linked(i, j) == 255 && SignMap(i, j) == 0) {
                count++;
                SignMap(i, j) = 1;
                temp.push(make_pair(i, j));
                PointSet.push(make_pair(i, j));
              }
            }
          }
        }
      }
      // 若长度小于20
      if (count < 20) {
        while (!PointSet.empty()) {
          // 将预先保存在容器中的边上的点删除
          linked(PointSet.front().first, PointSet.front().second) = 0;
          PointSet.pop();
        }
      }
    }
  }
}