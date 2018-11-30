#ifndef _UTILS_H
#define _UTILS_H

#include <iostream>
#include <vector>

using namespace std;

#define N 8

//按第一行展开计算|A|
double getA(double arcs[N][N], int n) {
	if (n == 1) {
		return arcs[0][0];
	}
	double ans = 0;
	double temp[N][N] = { 0.0 };
	for ( int i = 0; i < n; i++) {
		for (int j = 0; j < n - 1; j++) {
			for (int k = 0; k < n - 1; k++) {
				temp[j][k] = arcs[j + 1][(k >= i) ? k + 1 : k];
			}
		}
		double t = getA(temp, n - 1);
		if (i % 2 == 0) {
			ans += arcs[0][i] * t;
		}
		else {
			ans -= arcs[0][i] * t;
		}
	}
	return ans;
}

//计算每一行每一列的每个元素所对应的余子式，组成A*
void getAStar(double arcs[N][N], int n, double ans[N][N]) {
	if (n == 1) {
		ans[0][0] = 1;
		return;
	}
	int i, j, k, t;
	double temp[N][N];
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			for (k = 0; k < n - 1; k++) {
				for (t = 0; t < n - 1; t++) {
					temp[k][t] = arcs[k >= i ? k + 1 : k][t >= j ? t + 1 : t];
				}
			}
			ans[j][i] = getA(temp, n - 1);  //此处顺便进行了转置
			if ((i + j) % 2 == 1) {
				ans[j][i] = -ans[j][i];
			}
		}
	}
}

//得到给定矩阵src的逆矩阵保存到des中。
bool GetInverseMatrix(vector<pair<int, int>> srcPoints, vector<pair<int, int>> resPoints, int n, double des[N][N]) {
	double src[8][8] =
	{ { srcPoints[0].first, srcPoints[0].second, 1, 0, 0, 0, -resPoints[0].first*srcPoints[0].first, -resPoints[0].first*srcPoints[0].second },
	{ 0, 0, 0, srcPoints[0].first, srcPoints[0].second, 1, -resPoints[0].second*srcPoints[0].first, -resPoints[0].second*srcPoints[0].second },
	{ srcPoints[1].first, srcPoints[1].second, 1, 0, 0, 0, -resPoints[1].first*srcPoints[1].first, -resPoints[1].first*srcPoints[1].second },
	{ 0, 0, 0, srcPoints[1].first, srcPoints[1].second, 1, -resPoints[1].second*srcPoints[1].first, -resPoints[1].second*srcPoints[1].second },
	{ srcPoints[2].first, srcPoints[2].second, 1, 0, 0, 0, -resPoints[2].first*srcPoints[2].first, -resPoints[2].first*srcPoints[2].second },
	{ 0, 0, 0, srcPoints[2].first, srcPoints[2].second, 1, -resPoints[2].second*srcPoints[2].first, -resPoints[2].second*srcPoints[2].second },
	{ srcPoints[3].first, srcPoints[3].second, 1, 0, 0, 0, -resPoints[3].first*srcPoints[3].first, -resPoints[3].first*srcPoints[3].second },
	{ 0, 0, 0, srcPoints[3].first, srcPoints[3].second, 1, -resPoints[3].second*srcPoints[3].first, -resPoints[3].second*srcPoints[3].second } };

	double flag = getA(src, n);
	double t[N][N];
	if (0 == flag) {
		return false;//如果算出矩阵的行列式为0，则不往下进行
	}
	else {
		getAStar(src, n, t);
		for (int i = 0; i<n; i++) {
			for (int j = 0; j<n; j++) {
				des[i][j] = t[i][j] / flag;
			}
		}
	}
	return true;
}

#endif