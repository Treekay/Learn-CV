#ifndef _IMAGEMORPHING_H
#define _IMAGEMORPHING_H

#include <iostream>
#include <vector>
#include <string>
#include <cmath>

#include "CImg.h"

#define totalFrames 11

using namespace std;
using namespace cimg_library;

struct Point {
    double x, y; // 点的横纵坐标
    Point() : x(0), y(0) {}
    Point(double _x, double _y) : x(_x), y(_y) {}
};

struct Line {
    Point P, Q; // 直线的两个端点
    int len;
    Line(Point _P, Point _Q) : P(_P), Q(_Q) {
        len = sqrt(pow(P.x - Q.x, 2) + pow(P.y - Q.y, 2));
    }
    double getU(Point X) {
        double u = ((X.x - P.x) * (Q.x - P.x) + (X.y - P.y) * (Q.y - P.y)) / (len * len);
        return u;
    }
    double getV(Point X) {
        double v = ((X.x - P.x) * (Q.y - P.y) + (X.y - P.y) * (P.x - Q.x)) / len;
        return v;
    }
    Point getPoint(double u, double v) {
        double Point_x = P.x + u * (Q.x - P.x) + v * (Q.y - P.y) / len;
        double Point_y = P.y + u * (Q.y - P.y) + v * (P.x - Q.x) / len;
        return Point(Point_x, Point_y);
    }
    double getWeight(Point point) {
        double a = 1;   // parameter_a;
        double b = 2;   // parameter_b;
        double p = 0;   // parameter_p;
        double d = 0.0;
        double u = getU(point);
        if (u > 1.0) {
            d = sqrt((point.x - Q.x) * (point.x - Q.x) + (point.y - Q.y) * (point.y - Q.y));
        }
        else if (u < 0) {
            d = sqrt((point.x - P.x) * (point.x - P.x) + (point.y - P.y) * (point.y - P.y));
        }
        else {
            d = abs(getV(point));
        }
        double weight = pow((pow(len, p) / (a + d)), b);
        return weight;
    }
};

class ImageMorphing {
public:
    ImageMorphing(string, string, vector<Line>, vector<Line>);
    void setLines();                    // 确定特征线
    void morphing();                    // 变形过程
    void generateCurrentLines();        // 生成所有中间帧的特征线
    void generateCurrentImages();       // 生成过程图像
    void bilinearInterpolation(double*, int, double, double);     // 双线性插值
private:
    CImg<double> sourceImg;             // 初始图像
    CImg<double> resultImg;             // 结果图像
    CImg<double> currentImg;            // 中间帧图像
    vector<Line> sourceLines;           // 初始特征线集
    vector<Line> resultLines;           // 结果特征线集
    vector<Line> currentLines;          // 中间帧的特征线集
    int width, height;                  // 图像的宽高
    int currentFrame;                   // 当前帧序号
    double weight;
};

#endif // !_IMAGEMORPHING_H