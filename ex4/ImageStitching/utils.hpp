#ifndef _UTILS_H
#define _UTILS_H

#include <iostream>
#include <vector>
#include <string>

#include "CImg.h"

using namespace std;
using namespace cimg_library;

extern "C" {
#include <vl/generic.h>
#include <vl/stringop.h>
#include <vl/pgm.h>
#include <vl/sift.h>
#include <vl/getopt_long.h>
#include <vl/kdtree.h>
#include <vl/random.h>
}

/*用于检查三个点是否共线*/
bool check(VlSiftKeypoint p1, VlSiftKeypoint p2, VlSiftKeypoint p3) {
	return (p3.y - p1.y) * (p3.x - p2.x) != (p3.y - p2.y) * (p3.x - p1.x);
}

/*用于随机生成四对不同的不同线匹配点*/
vector<pair<VlSiftKeypoint, VlSiftKeypoint>> getRandomPoints(vector<pair<VlSiftKeypoint, VlSiftKeypoint>> match) {
	srand(time(0));
	vector<pair<VlSiftKeypoint, VlSiftKeypoint>> ret;
	int allThree[4][3] = { { 0, 1, 2 },{ 0, 1, 3 },{ 0, 2, 3 },{ 1, 2 ,3 } };
	bool flag = true;
	int visitd[4] = { 0 };
	while (flag) {
		ret.clear();
		flag = false;
		for (int i = 0; i < 4; ) {
			int index = rand() % match.size(), j;
			for (j = 0; j < i; ++j) {
				if (index == visitd[j])
					break;
			}
			if (j == i) {
				ret.push_back(match[index]);
				visitd[i] = index;
				++i;
			}
		}
		for (int i = 0; i < 4; ++i) {
			if (!check(ret[allThree[i][0]].first, ret[allThree[i][1]].first, ret[allThree[i][2]].first))
				flag = true;
		}
	}
	return ret;
}

/*利用高斯消元法求取目标矩阵*/
bool gauss_jordan(int x1[4], int y1[4], int x2[4], int y2[4], int b[8], double H[9]) {
	double src[8][8] = { { x1[0], y1[0], 1, 0, 0, 0, -x1[0] * x2[0], -x2[0] * y1[0] },
	{ 0, 0, 0, x1[0], y1[0], 1, -x1[0] * y2[0], -y1[0] * y2[0] },
	{ x1[1], y1[1], 1, 0, 0, 0, -x1[1] * x2[1], -x2[1] * y1[1] },
	{ 0, 0, 0, x1[1], y1[1], 1, -x1[1] * y2[1], -y1[1] * y2[1] },
	{ x1[2], y1[2], 1, 0, 0, 0, -x1[2] * x2[2], -x2[2] * y1[2] },
	{ 0, 0, 0, x1[2], y1[2], 1, -x1[2] * y2[2], -y1[2] * y2[2] },
	{ x1[3], y1[3], 1, 0, 0, 0, -x1[3] * x2[3], -x2[3] * y1[3] },
	{ 0, 0, 0, x1[3], y1[3], 1, -x1[3] * y2[3], -y1[3] * y2[3] } };
	int n = 8;
	double B[8][9];
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < n; ++j) {
			B[i][j] = src[i][j];
		}
		B[i][n] = b[i];
	}

	for (int i = 0; i < n; ++i) {
		int pivot = i;
		for (int j = i; j < n; ++j) {
			if (abs(B[j][i]) > abs(B[pivot][i]))
				pivot = j;
		}
		swap(B[i], B[pivot]);

		//无解或者有无穷解 
		if (abs(B[i][i]) < 1E-8) return false;

		double k = B[i][i];
		for (int j = i; j <= n; ++j) B[i][j] /= k;
		for (int j = 0; j < n; ++j) {
			if (i != j) {
				for (int k = i + 1; k <= n; ++k) {
					B[j][k] -= B[j][i] * B[i][k];
				}
			}
		}
	}
	for (int i = 0; i < n; ++i)
		H[i] = B[i][n];
	return true;
}

#endif