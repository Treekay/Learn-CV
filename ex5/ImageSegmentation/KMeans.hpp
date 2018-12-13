#ifndef _KMEANS_H_
#define _KMEANS_H_

#include <iostream>
#include <cmath>
#include <vector>

#include "CImg.h"

using namespace std;
using namespace cimg_library;

#define k  			// 聚类的数目
#define maxIter 	// 最大迭代次数
#define accValue 	// 接受值, 如果上一次的所有k个聚类中心与本次的所有k个聚类中心的差都小于accept值, 则说明算法已经收敛

struct Cluster {
	Point centroid;
	vector<Point> samples;
	Cluster(Point c): centroid(c) {}
}

struct Point {
	int x, y;
	int val;
	int clusterID;
	Point(int _x, int _y, int _val): x(_x), y(_y), val(_val) {
		clusterID = 0;
	}
};

class KMeans {
public:
	KMeans(CImg<double> img) {
		cimg_forXY(img, x, y) {
			points.push_back(Point(x, y, img(x, y)));
		}
	}

	void run() {
		initCentroids(); // 随机初始化聚类中心点
		for (int i = 0; i < maxIter; i++) {
			cluster();
			calCost();
			updateCentroids();
		}
	}

	// 随机初始化聚类中心点
	void initCentroids() {
		srand(time(0));
		for (int i = 0; i < k; i++) {
			clusters.push_back(Cluster(points[rand() % points.size()]));
		}
	}
	
	// 聚类
	void cluster() {
		for (int i = 0; i < points.size(); i++) {
			for (int j = 0; j < centroids.size(); j++) {
				double dist = calDist(points[i], cenroids[j]);

			}
		}
	}
	
	// 更新聚类中心
	void updateCentroids() {

	}

	// 计算两点的距离
	double calDist(Point a, Point b) {
		return 0.4 * pow(a.val - b.val, 2) + 0.6 * (pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
	}

	double calCost(int current) {

	}

private:
	vector<Point> points;
	vector<Cluster> clusters;
};

#endif // !_KMEANS_H_