#ifndef _KMEANS_H_
#define _KMEANS_H_

#include <iostream>
#include <cmath>
#include <vector>

#include "CImg.h"

using namespace std;
using namespace cimg_library;

#define maxIter 2000

struct Point {
	int x, y;
	double val;
	int clusterID;
	Point(int _x, int _y, double _val): x(_x), y(_y), val(_val) {
		clusterID = 0;
	}
};

class KMeans {
public:
	KMeans(CImg<double> img) {
		width = img.width();
		height = img.height();
		cimg_forXY(img, x, y) {
			points.push_back(Point(x, y, img(x, y)));
		}
		initCentroids(); // 随机初始化聚类中心点
		for (int i = 0; i < maxIter; i++) {
			updateCluster();
			updateCentroids();
			if (accept) break;
		}
	}

	// 随机初始化聚类中心点
	void initCentroids() {
		// 随机第一个中心点
		srand(time(0));
		double value1 = points[rand() % points.size()].val;
		means.push_back(value1);
		// 找到距离第一个中心店最远的点
		double value2, max = 0;
		int index = 0;
		for (int i = 0; i < points.size(); i++) {
			double dist = calDist(points[i], value1);
			if (dist > max) {
				max = dist;
				index = i;
			}
		}
		means.push_back(points[index].val);
	}
	
	// 聚类
	void updateCluster() {
		for (int i = 0; i < points.size(); i++) {
			double minDist = calDist(points[i], means[0]);
			for (int j = 0; j < 2; j++) {
				double dist = calDist(points[i], means[j]);
				if (dist < minDist) {
					minDist = dist;
					points[i].clusterID = j;
				}
			}
		}
	}

	void updateCentroids() {
		accept = true;
		double newCost = 0;
		// caculate cost
		for (int i = 0; i < points.size(); i++) {
			newCost += calDist(points[i], means[points[i].clusterID]);
		}
		// check if acceptable
		if (newCost < cost || cost == 0) {
			// update centroids
			cost = newCost;
			for (int i = 0; i < 2; i++) {
				int sum = 0, count = 0;
				for (int j = 0; j < points.size(); j++) {
					if (points[j].clusterID == i) {
						sum += points[j].val;
						count++;
					}
				}
				means[i] = sum / count;
			}
		} else {
			accept = true;
		}
	} 

	// 计算两点的距离
	double calDist(Point a, double b) {
		return pow(a.val - b, 2);
	}

	CImg<unsigned char> getCluster(){
		CImg<unsigned char> cluster = CImg<unsigned char>(width, height);
		for (int i = 0; i < points.size(); i++) {
			cluster(points[i].x, points[i].y) = points[i].clusterID * 255;
		}
		return cluster;
	}

private:
	vector<Point> points;
	vector<double> means;
	double cost = 0;
	int width, height;
	bool accept = false;
};

#endif // !_KMEANS_H_