#include "ImgSegment.hpp"
#include "KMeans.hpp"
#include "utils.hpp"
#include "houghLine.h"

ImgSegment::ImgSegment(string imgPath, string _resPath){
    resPath = _resPath;
    // 读取图片
    sourceImg.load(imgPath.c_str());
    width = sourceImg.width();
    height = sourceImg.height();
    // KMeans聚类
    clustering();
    edgeDetect();
    edgeDelete(3000);
    getPoints();
    calHomography();
    inverseProject();
}

/*
  将图像转为灰度图并模糊以进行边缘检测
  img -> grayscaled
*/
CImg<double> ImgSegment::gaussianFilter() {
    CImg<double> grayscaled = CImg<double>(width, height, 1, 1); // To one channel
    cimg_forXY(sourceImg, x, y) {
        grayscaled(x, y) = sourceImg(x, y, 0) * 0.2126 + sourceImg(x, y, 1) * 0.7152 + sourceImg(x, y, 2) * 0.0722;
    }
    // 高斯模糊
    return grayscaled.blur(1);
}

/*
    KMeans 对图像进行聚类，聚成两类因此得到的是二值图像
*/
void ImgSegment::clustering() {
    clusterImg = KMeans(gaussianFilter()).getCluster();
    clusterImg.display("Cluster Img");
}

/*
    根据像素点的八领域内是否存在两种点初步判断该点是否为边缘点
*/
void ImgSegment::edgeDetect() {
    edgeImg = CImg<double>(width, height, 1, 3);
    cimg_forXY(clusterImg, x, y) {
        edgeImg(x, y, 0) = 0;
        edgeImg(x, y, 1) = 0;
        edgeImg(x, y, 2) = 0;
        bool a = false, b = false;
        for (int i = x - 1; i < x + 2; i++) {
            for (int j = y - 1; j < y + 2; j++) {
                // 搜索八邻域
                if (i >= 0 && i < width && j >= 0 && j < height) {
                    if (clusterImg(i, j) == 255) {
                        a = true;
                    }
                    if (clusterImg(i, j) == 0) {
                        b = true;
                    }
                }
            }
        }
        // 若存在两种点
        if (a && b) {
            edgeImg(x, y, 0) = 255;
            edgeImg(x, y, 1) = 255;
            edgeImg(x, y, 2) = 255;
        }
    }
}

/*
  删除短边缘 
 */
void ImgSegment::edgeDelete(int len) {
    CImg<double> SignMap(width, height);
    // 初始化标记图
    // 该图用于标记边缘点是否访问过
    cimg_forXY(SignMap, x, y) {
        SignMap(x, y) = 0;
    }

    // 用广度优先搜索遍历每条边
    // 遍历过程中将边上的点放入容器,以便后续删除
    cimg_forXY(edgeImg, x, y) {
        if (edgeImg(x, y) == 255 && SignMap(x, y) == 0) {
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
                            if (edgeImg(i, j) == 255 && SignMap(i, j) == 0) {
                                count++;
                                SignMap(i, j) = 1;
                                temp.push(make_pair(i, j));
                                PointSet.push(make_pair(i, j));
                            }
                        }
                    }
                }
            }
        // 若长度小于len
            if (count < len) {
                while (!PointSet.empty()) {
                    // 将预先保存在容器中的边上的点删除
                    edgeImg(PointSet.front().first, PointSet.front().second, 0) = 0;
                    edgeImg(PointSet.front().first, PointSet.front().second, 1) = 0;
                    edgeImg(PointSet.front().first, PointSet.front().second, 2) = 0;
                    PointSet.pop();
                }
            }
        }
    }
    edgeImg.display("edgeImage");
}

// 用霍夫变换进行直线检测并找出角点
void ImgSegment::getPoints() {
    points = houghLine(edgeImg, 29, 502, 200).getPoints();
    double dist[3] = {0};
    // 将曼哈顿距离最小的点作为起点
    int minDist = points[0].first + points[0].second;
    for (int i = 1; i < 4; i++) {
        if (points[i].first + points[i].second < minDist) {
            minDist = points[i].first + points[i].second;
            swap(points[0], points[i]);
        }
    }
    // 将点按距离起始点距离的大小进行排列
    for (int i = 1; i < points.size(); i++) {
        dist[i - 1] = sqrt(pow(points[0].first - points[i].first, 2) + pow(points[0].second - points[i].second, 2));
    }
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3 - i - 1; j++) {
            if (dist[j] > dist[j + 1]) {
                swap(dist[j], dist[j + 1]);
                swap(points[j + 1], points[j + 2]);
            }
        }
    }
    int paperWidth = dist[0];
    int paperHeight = dist[1];
    resultImg = CImg<double>(paperWidth, paperHeight, 1, 3);
    
    // sort Points
    for (int i = 0; i < 4; i++) {
        sourcePoints.push_back(points[i]);
    }
    targetPoints.push_back(make_pair(0, 0));
    targetPoints.push_back(make_pair(paperWidth, 0));
    targetPoints.push_back(make_pair(0, paperHeight));
    targetPoints.push_back(make_pair(paperWidth, paperHeight));
}

/*
    计算从矫正图像变换到原图像的变换矩阵
    根据矫正图像每一点的像素求得在原图像的位置并求插值
*/
void ImgSegment::calHomography() {
    // debug
    for (int i = 0; i < sourcePoints.size(); i++) {
        cout << "Point: " << sourcePoints[i].first << " " << sourcePoints[i].second << endl;
        cout << "Point: " << targetPoints[i].first << " " << targetPoints[i].second << endl;
    }
    // cal Homography
    double uv[8] = { sourcePoints[0].first, sourcePoints[0].second,
        sourcePoints[1].first, sourcePoints[1].second,
        sourcePoints[2].first, sourcePoints[2].second, 
        sourcePoints[3].first, sourcePoints[3].second };
    // 填充矩阵
    double src[8][8] =
    { { targetPoints[0].first, targetPoints[0].second, 1, 0, 0, 0, -sourcePoints[0].first*targetPoints[0].first, -sourcePoints[0].first*targetPoints[0].second },
    { 0, 0, 0, targetPoints[0].first, targetPoints[0].second, 1, -sourcePoints[0].second*targetPoints[0].first, -sourcePoints[0].second*targetPoints[0].second },

    { targetPoints[1].first, targetPoints[1].second, 1, 0, 0, 0, -sourcePoints[1].first*targetPoints[1].first, -sourcePoints[1].first*targetPoints[1].second },
    { 0, 0, 0, targetPoints[1].first, targetPoints[1].second, 1, -sourcePoints[1].second*targetPoints[1].first, -sourcePoints[1].second*targetPoints[1].second },

    { targetPoints[2].first, targetPoints[2].second, 1, 0, 0, 0, -sourcePoints[2].first*targetPoints[2].first, -sourcePoints[2].first*targetPoints[2].second },
    { 0, 0, 0, targetPoints[2].first, targetPoints[2].second, 1, -sourcePoints[2].second*targetPoints[2].first, -sourcePoints[2].second*targetPoints[2].second },

    { targetPoints[3].first, targetPoints[3].second, 1, 0, 0, 0, -sourcePoints[3].first*targetPoints[3].first, -sourcePoints[3].first*targetPoints[3].second },
    { 0, 0, 0, targetPoints[3].first, targetPoints[3].second, 1, -sourcePoints[3].second*targetPoints[3].first, -sourcePoints[3].second*targetPoints[3].second } };

    double matrix_after[N][N]{};
    bool flag = GetMatrixInverse(src, N, matrix_after);
    if (flag == false) {
        cout << "can't get the matrix" << endl;
        return;
    }

    for (int i = 0; i < 8; i++) {
        double sum = 0;
        for (int t = 0; t < 8; t++) {
            sum += matrix_after[i][t] * uv[t];
        }
        H[i] = sum;
    }

    for (int i = 0; i < 8; i++) {
        cout << H[i] << " ";
    }
    cout << endl;
}

// 双线性插值获得矫正图像
void ImgSegment::inverseProject() {
    cimg_forXY(resultImg, x, y) {
        // 根据逆变换矩阵求出矫正图像上的像素点在原图对应的位置
        double px = H[0] * x + H[1] * y + H[2] * 1;
        double py = H[3] * x + H[4] * y + H[5] * 1;
        double p = H[6] * x + H[7] * y + 1;

        // 在原图对应位置求插值
        double u = px / p;
        double v = py / p;
        if (u >= 0 && u < width && v >= 0 && v < height) {
            // 双线性插值
            double weightLeft = ceil(u) - u;
            double weightRight = u - floor(u);
            double weightUp = ceil(v) - v;
            double weightDown = v - floor(v);
            double value[3] = {
                sourceImg( floor(u), floor(v), 0) * weightLeft * weightUp +
                sourceImg( ceil(u), floor(v), 0) * weightRight * weightUp +
                sourceImg( floor(u), ceil(v), 0) * weightLeft * weightDown +
                sourceImg( ceil(u), ceil(v), 0) * weightRight * weightDown,

                sourceImg( floor(u), floor(v), 1) * weightLeft * weightUp +
                sourceImg( ceil(u), floor(v), 1) * weightRight * weightUp +
                sourceImg( floor(u), ceil(v), 1) * weightLeft * weightDown +
                sourceImg( ceil(u), ceil(v), 1) * weightRight * weightDown,

                sourceImg( floor(u), floor(v), 2) * weightLeft * weightUp +
                sourceImg( ceil(u), floor(v), 2) * weightRight * weightUp +
                sourceImg( floor(u), ceil(v), 2) * weightLeft * weightDown +
                sourceImg( ceil(u), ceil(v), 2) * weightRight * weightDown
            };
            resultImg(x, y, 0) = value[0];
            resultImg(x, y, 1) = value[1];
            resultImg(x, y, 2) = value[2];
        }
    }
    resultImg.display("resultImg");
    resultImg.save(resPath.c_str());
}