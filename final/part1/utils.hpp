#ifndef _UTILS_H_
#define _UTILS_H_

#include <iostream>
#include <direct.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <vector>
using namespace std;
#define N 8    //测试矩阵维数定义

//按第一行展开计算|A|
double getA(double arcs[N][N], int n) {
    if (n == 1){
        return arcs[0][0];
    }
    double ans = 0;
    double temp[N][N] = { 0.0 };
    int i, j, k;
    for (i = 0; i<n; i++) {
        for (j = 0; j<n - 1; j++) {
            for (k = 0; k<n - 1; k++) {
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
void  getAStart(double arcs[N][N], int n, double ans[N][N]) {
    if (n == 1) {
        ans[0][0] = 1;
        return;
    }
    int i, j, k, t;
    double temp[N][N];
    for (i = 0; i<n; i++) {
        for (j = 0; j<n; j++) {
            for (k = 0; k<n - 1; k++) {
                for (t = 0; t<n - 1; t++) {
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
bool GetMatrixInverse(double src[N][N], int n, double des[N][N]) {
    double flag = getA(src, n);
    double t[N][N];
    if (0 == flag) {
        cout << "error" << endl;
        return false;//如果算出矩阵的行列式为0，则不往下进行
    }
    else {
        getAStart(src, n, t);
        for (int i = 0; i<n; i++) {
            for (int j = 0; j<n; j++) {
                des[i][j] = t[i][j] / flag;
            }

        }
    }

    return true;
}

vector<string> getFiles(string cate_dir) {
	vector<string> files;//存放文件名
	_finddata_t file;
	long lf;
	//输入文件夹路径
	if ((lf = _findfirst(cate_dir.c_str(), &file)) == -1) {
		cout<< cate_dir << " not found!!!" << endl;
	} else {
		while(_findnext(lf, &file) == 0) {
			//输出文件名
			if (strcmp(file.name, ".") == 0 || strcmp(file.name, "..") == 0)
				continue;
			files.push_back(file.name);
		}
	}
	_findclose(lf);
 
	//排序，按从小到大排序
	sort(files.begin(), files.end());
	return files;
}


#endif // !_UTILS_H_