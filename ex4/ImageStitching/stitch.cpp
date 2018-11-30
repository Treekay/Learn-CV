#include "stitch.hpp"
#include "utils.hpp"

#define NOCTAVES 3
#define NLEVELS 7
#define O_MIN  0
#define PEAK_THRESH 0.8
#define EDGE_THRESH 10.0
#define NN_DIST_RATIO_THR 0.8
#define CONFIDENCE 0.995
#define MAX_ITERS 2000
#define REJECT_THRESH 3

Stitch::Stitch(vector<string> srcList) {
	LoadImg(srcList[0]);
	Sift(0);
	for (int current = 1; current < srcList.size(); current++) {
		LoadImg(srcList[current]);
		Sift(current);
		Match(current);
	}
}

void Stitch::LoadImg(string src) {
	CImg<float> temp(src.c_str());
	displayImgs.push_back(temp);
	imgList.push_back(Image(temp));
}

void Stitch::Sift(int current) {
	cout << "SIFT Begin\n";

	// 转换成灰度图像
	CImg<float> currentImg = imgList[current].img;
	int width = currentImg.width();
	int height = currentImg.height();
	CImg<float> grayImg = CImg<float>(width, height);
	cimg_forXY(currentImg, x, y) {
		grayImg(x, y) = (currentImg(x, y, 0) * 0.2126 + currentImg(x, y, 1) * 0.7152 + currentImg(x, y, 2) * 0.0722);
	}
	// 读取灰度图数据
	vl_sift_pix *ImageData = grayImg.data();

	// 创建一个新的sift滤波器
	VlSiftFilt *SiftFilt = vl_sift_new(width, height, NOCTAVES, NLEVELS, O_MIN);
	vl_sift_set_peak_thresh(SiftFilt, PEAK_THRESH);
	vl_sift_set_edge_thresh(SiftFilt, EDGE_THRESH);

	//计算每层中的关键点
	if (vl_sift_process_first_octave(SiftFilt, ImageData) != VL_ERR_EOF) {
		do {
			// 检测关键点
			vl_sift_detect(SiftFilt);
			// 遍历并绘制检测到的关键点数目
			for (int i = 0; i < SiftFilt->nkeys; i++) {
				VlSiftKeypoint keyPoint = SiftFilt->keys[i];
				unsigned int RED[] = { 255, 0, 0 };
				displayImgs[current].draw_circle((int)keyPoint.x, (int)keyPoint.y, (int)keyPoint.sigma / 2, RED);
				
				double angles[4];
				// 计算特征点的主方向
				vl_sift_calc_keypoint_orientations(SiftFilt, angles, &keyPoint);
				float *descr = new float[128];
				vl_sift_calc_keypoint_descriptor(SiftFilt, descr, &keyPoint, angles[0]);
				imgList[current].keyPoints.push_back(keyPoint);
				imgList[current].descriptors.push_back(descr);
			}
		} while (vl_sift_process_next_octave(SiftFilt) != VL_ERR_EOF);
	}
	vl_sift_delete(SiftFilt);
	cout << "SIFT Done\n";

	displayImgs[current].display("sift");
}

void Stitch::Match(int current) {
	Image leftImage = imgList[current - 1];
	Image rightImage = imgList[current];

	match.clear();
	cout << "Match Begin\n";
	vector<float*> ldescriptors = leftImage.descriptors;
	vector<float*> rdescriptors = rightImage.descriptors;

	/* KDTree, L1 comparison metric, dimension 128, 1 tree, L1 metric */
	VlKDForest* forest = vl_kdforest_new(VL_TYPE_FLOAT, 128, 1, VlDistanceL1);

	/* Build the tree from the left descriptors */
	float *data = new float[128 * ldescriptors.size()];
	for (unsigned int i = 0; i < ldescriptors.size(); i++) {
		memcpy(data + 128 * i, ldescriptors[i], 128 * sizeof(float));
	}
	vl_kdforest_build(forest, ldescriptors.size(), data);

	/* Searcher object */
	VlKDForestSearcher* searcher = vl_kdforest_new_searcher(forest);

	VlKDForestNeighbor neighbours[2];
	for (int i = 0; i < rdescriptors.size(); i++) {
		vl_kdforestsearcher_query(searcher, neighbours, 2, rdescriptors[i]);
		if (neighbours[0].distance < neighbours[1].distance * NN_DIST_RATIO_THR) {
			match.push_back(make_pair(leftImage.keyPoints[neighbours[0].index], rightImage.keyPoints[i]));
		}
	}
	vl_kdforestsearcher_delete(searcher);
	vl_kdforest_delete(forest);
	cout << "Match Done\n";

	Ransac();

	// 匹配点之间连线
	CImg<unsigned char> display;
	display.append(displayImgs[current]);
	display.append(displayImgs[current - 1]);
	for (int i = 0; i < match.size(); i++) {
		int x0 = match[i].second.x;
		int y0 = match[i].second.y;
		int x1 = match[i].first.x + displayImgs[current].width();
		int y1 = match[i].first.y;
		unsigned int BLUE[] = { 0, 0, 255 };
		display.draw_line(x0, y0, x1, y1, BLUE);
	}
	display.display("match");
}

void Stitch::Ransac() {
	cout << "RANSAC Begin\n";

	int reject_thresh = REJECT_THRESH;
	int max_iters = MAX_ITERS;
	int maxGoodCount = 0;
	double H[9];
	vector<bool> mask(match.size());
	// 找最优变换矩阵
	for (int iter = 0; iter < max_iters; iter++) {
		int goodCount = 0;
		// 随机取四点
		VlRand *rand = vl_get_rand();
		vector<pair<VlSiftKeypoint, VlSiftKeypoint>> samples;
		while (samples.size() < 4) {
			int index = vl_rand_int31(rand) % match.size();
			samples.push_back(match[index]);
		}
		// 计算变换矩阵
		if (findHomography(samples, H)) {
			//计算每组点的投影误差
			vector<double> err(match.size());
			for (int i = 0; i < match.size(); ++i) {
				double ww = 1. / (H[6] * match[i].first.x + H[7] * match[i].first.y + 1.);
				double dx = (H[0] * match[i].first.x + H[1] * match[i].first.y + H[2]) * ww - match[i].second.x;
				double dy = (H[3] * match[i].first.x + H[4] * match[i].first.y + H[5]) * ww - match[i].second.y;
				err[i] = dx * dx + dy * dy;
			}
			reject_thresh *= reject_thresh;
			//找出内点
			for (int i = 0; i < match.size(); i++) {
				//误差在限定范围内，加入‘内点集’
				mask[i] = (err[i] <= reject_thresh);
				goodCount += mask[i];
			}
			// 迭代
			if (goodCount > VL_MAX(maxGoodCount, samples.size() - 1)) {
				memcpy(transform, H, 9 * sizeof(double));
				maxGoodCount = goodCount;
				max_iters = log(1. - CONFIDENCE / log(1. - pow((double)goodCount / match.size(), 4)));
			}
		}
	}
	vector<pair<VlSiftKeypoint, VlSiftKeypoint>> ret;
	for (int i = 0; i < match.size(); ++i) {
		if (mask[i]) {
			ret.push_back(match[i]);
		}
	}
	match = ret;

	cout << "RANSAC Done\n";
}

bool Stitch::findHomography(const vector<pair<VlSiftKeypoint, VlSiftKeypoint>> &samples, double H[]) {
	vector<pair<int, int>> srcPoints;
	vector<pair<int, int>> resPoints;

	for (int i = 0; i < 4; ++i) {
		srcPoints.push_back(make_pair(samples[i].first.x, samples[i].first.y));
		resPoints.push_back(make_pair(samples[i].second.x, samples[i].second.y));
	}

	double InverseMatrix[8][8] = { 0 };
	if (!GetInverseMatrix(srcPoints, resPoints, 8, InverseMatrix)) {
		return false;
	}
	int uv[8] = {
		resPoints[0].first, resPoints[0].second,
		resPoints[1].first, resPoints[1].second,
		resPoints[2].first, resPoints[2].second,
		resPoints[3].first, resPoints[3].second
	};

	for (int i = 0; i < 8; i++) {
		H[i] = 0;
		for (int j = 0; j < 8; j++) {
			H[i] += InverseMatrix[i][j] * uv[j];
		}
	}
	H[8] = 1;
	return true;
}

void Stitch::Combine() {

}

void Stitch::Blending() {

}