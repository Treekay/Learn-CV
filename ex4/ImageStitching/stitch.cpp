#include "stitch.hpp"
#include "utils.hpp"

#define NOCTAVES 3
#define NLEVELS 7
#define O_MIN 0
#define PEAK_THRESH 0.8
#define EDGE_THRESH 10.0
#define NN_DIST_RATIO_THR 0.8
#define CONFIDENCE 0.995
#define MAX_ITERS 20000
#define REJECT_THRESH 1
#define offset 0.86

Stitch::Stitch(vector<string> srcList) {
	LoadImg(srcList[0]);
	Sift(0);
	resultImg = imgList[0].img;
	for (int current = 1; current < srcList.size(); current++) {
		LoadImg(srcList[current]);
		Sift(current);
		Match(current);
		Blending(current);
	}
	resultImg.save("stitchImg.bmp");
}

void Stitch::LoadImg(string src) {
	CImg<float> temp(src.c_str());
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
	CImg<unsigned char> display;
	display.append(imgList[current].img);
	if (vl_sift_process_first_octave(SiftFilt, ImageData) != VL_ERR_EOF) {
		do {
			// 检测关键点
			vl_sift_detect(SiftFilt);
			// 遍历并绘制检测到的关键点数目
			for (int i = 0; i < SiftFilt->nkeys; i++) {
				VlSiftKeypoint keyPoint = SiftFilt->keys[i];
				unsigned int RED[] = { 255, 0, 0 };
				display.draw_circle((int)keyPoint.x, (int)keyPoint.y, 1, RED);
				
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

	display.display("sift");
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
	display.append(imgList[current - 1].img);
	display.append(imgList[current].img);
	for (int i = 0; i < match.size(); i++) {
		unsigned int RED[] = { 255, 0, 0 };
		display.draw_circle((int)match[i].first.x, (int)match[i].first.y, 2, RED);
		display.draw_circle((int)match[i].second.x + imgList[current - 1].img.width(), (int)match[i].second.y, 2, RED);
		int x0 = match[i].first.x;
		int y0 = match[i].first.y;
		int x1 = match[i].second.x + imgList[current - 1].img.width();
		int y1 = match[i].second.y;
		unsigned int BLUE[] = { 0, 0, 255 };
		display.draw_line(x0, y0, x1, y1, BLUE);
	}
	display.display("match");
}

/* 计算变换矩阵*/
bool calHomography(const vector<pair<VlSiftKeypoint, VlSiftKeypoint>> &samples, double H[9]) {
	int x1[4], y1[4], x2[4], y2[4];
	/* 将本次选中的四组匹配点放入矩阵*/
	for (int i = 0; i < samples.size(); ++i) {
		x1[i] = samples[i].first.x;
		y1[i] = samples[i].first.y;
		x2[i] = samples[i].second.x;
		y2[i] = samples[i].second.y;
	}
	int v[8];
	for (int i = 0; i < 4; ++i) {
		v[2 * i] = x2[i];
		v[2 * i + 1] = y2[i];
	}
	double InverseMatrix[8][8] = { 0 };
	/* 利用高斯消元法求取逆矩阵和变换矩阵*/
	if (!gauss_jordan(x1, y1, x2, y2, v, H)) {
		return false;
	}

	H[8] = 1;
	return true;
}

/* 对匹配到的特征点对进行筛选*/
void Stitch::Ransac() {
	cout << "RANSAC Begin\n";

	int max_iters = MAX_ITERS;
	double confidence = CONFIDENCE;
	double reject_thresh = REJECT_THRESH;
	double xform[9];
	CImg<float> srcPoints(match.size(), 3);
	CImg<float> resPoints(match.size(), 3);
	vector<double> err(match.size());
	vector<bool> mask(match.size());
	cimg_forX(srcPoints, i) {
		srcPoints(i, 0) = match[i].first.x;
		srcPoints(i, 1) = match[i].first.y;
		srcPoints(i, 2) = 1;

		resPoints(i, 0) = match[i].second.x;
		resPoints(i, 1) = match[i].second.y;
		resPoints(i, 2) = 1;
	}

	int maxGoodCount = 0, goodCount = 0;
	double H[9];
	for (int iter = 0; iter < max_iters; ++iter) {
		/* 随机找出四对不相同且不共线的点*/
		vector<pair<VlSiftKeypoint, VlSiftKeypoint>> samples = getRandomPoints(match);
		/* 计算变换矩阵*/
		if (calHomography(samples, H)) {
			//找出内点
			goodCount = 0;
			for (int i = 0; i < match.size(); ++i) {
				double ww = 1. / (H[6] * srcPoints(i, 0) + H[7] * srcPoints(i, 1) + 1.);
				double dx = (H[0] * srcPoints(i, 0) + H[1] * srcPoints(i, 1) + H[2]) * ww - resPoints(i, 0);
				double dy = (H[3] * srcPoints(i, 0) + H[4] * srcPoints(i, 1) + H[5]) * ww - resPoints(i, 1);
				err[i] = dx * dx + dy * dy;
			}
			/* 误差在允许范围内的点则为内点*/
			reject_thresh *= reject_thresh;
			for (int i = 0; i < match.size(); ++i) {
				if (err[i] <= reject_thresh) {
					mask[i] = true;
				}
				else mask[i] = false;
				goodCount += mask[i];
			}
			/*更新迭代次数*/
			if (goodCount > VL_MAX(maxGoodCount, samples.size() - 1)) {
				memcpy(xform, H, 9 * sizeof(float));
				maxGoodCount = goodCount;
				max_iters = log(1. - confidence) / log(1. - pow((double)goodCount / match.size(), 4));
			}
			printf("maxInlines: %d\titer: %d\tmaxIters: %d\n", maxGoodCount, iter, max_iters);
		}
	}
	/*将筛选后的匹配点集保存*/
	vector<pair<VlSiftKeypoint, VlSiftKeypoint>> ret;
	for (int i = 0; i < match.size(); ++i) {
		if (mask[i])
			ret.push_back(match[i]);
	}
	match = ret;

	cout << "RANSAC Done\n";
}

/*图像配准融合*/
void Stitch::Blending(int current) {
	CImg<float> leftImg = resultImg;
	CImg<float> rightImg = imgList[current].img;

	/*根据平均变换位移确定图像的平移距离*/
	double dx = 0, dy = 0;
	for (int i = 0; i < match.size(); i++) {
		dx += (match[i].first.x - match[i].second.x);
		dy += (match[i].first.y - match[i].second.y);
	}
	dx = abs(dx / match.size()) * offset;
	dy = - abs(dy / match.size());

	/*确定拼接图像的宽高大小*/
	CImg<float> blendImg(leftImg.width() + rightImg.width() - dx,
		leftImg.height() + abs(dy), 1, 3);

	cimg_forXY(blendImg, x, y) {
		if (dy <= 0 && (rightImg.width() > x - (leftImg.width() - dx) && rightImg.height() > y - (0 - dy))) { //右侧图片需要往下左移动
			if (x < leftImg.width() && y < leftImg.height()) {
				if (x >= (leftImg.width() - dx) && y >= (0 - dy)) { // 重叠部分
					blendImg(x, y, 0) = (float)leftImg(x, y, 0) * (float)(leftImg.width() - x) / (float)abs(dx)
						+ (float)rightImg(x - (leftImg.width() - dx), y - (0 - dy), 0) * (float)(x - (leftImg.width() - dx)) / (float)abs(dx);
					blendImg(x, y, 1) = (float)leftImg(x, y, 1) * (float)(leftImg.width() - x) / (float)abs(dx)
						+ (float)rightImg(x - (leftImg.width() - dx), y - (0 - dy), 1) * (float)(x - (leftImg.width() - dx)) / (float)abs(dx);
					blendImg(x, y, 2) = (float)leftImg(x, y, 2) * (float)(leftImg.width() - x) / (float)abs(dx)
						+ (float)rightImg(x - (leftImg.width() - dx), y - (0 - dy), 2) * (float)(x - (leftImg.width() - dx)) / (float)abs(dx);
				}
				else {    //A独在部分
					blendImg(x, y, 0) = leftImg(x, y, 0);
					blendImg(x, y, 1) = leftImg(x, y, 1);
					blendImg(x, y, 2) = leftImg(x, y, 2);
				}
			}
			else if (x >= (leftImg.width() - dx) && y >= (0 - dy) && y < (0 - dy) + rightImg.height()) {    //B独在部分
				blendImg(x, y, 0) = rightImg(x - (leftImg.width() - dx), y - (0 - dy), 0);
				blendImg(x, y, 1) = rightImg(x - (leftImg.width() - dx), y - (0 - dy), 1);
				blendImg(x, y, 2) = rightImg(x - (leftImg.width() - dx), y - (0 - dy), 2);
			}
			else {    //黑色部分
				blendImg(x, y, 0) = 0;
				blendImg(x, y, 1) = 0;
				blendImg(x, y, 2) = 0;
			}
		}
		else {    //右侧图片需要往上左移动
			if (x < leftImg.width() && y < rightImg.height()) {
				if (x >= (leftImg.width() - dx) && y >= (0 - dy)) {    //混合
					blendImg(x, y, 0) = (float)leftImg(x, y - dy, 0) * (float)(leftImg.width() - x) / (float)abs(dx)
						+ (float)rightImg(x - (leftImg.width() - dx), y, 0) * (float)(x - (leftImg.width() - dx)) / (float)abs(dx);
					blendImg(x, y, 1) = (float)leftImg(x, y - dy, 1) * (float)(leftImg.width() - x) / (float)abs(dx)
						+ (float)rightImg(x - (leftImg.width() - dx), y, 1) * (float)(x - (leftImg.width() - dx)) / (float)abs(dx);
					blendImg(x, y, 2) = (float)leftImg(x, y - dy, 2) * (float)(leftImg.width() - x) / (float)abs(dx)
						+ (float)rightImg(x - (leftImg.width() - dx), y, 2) * (float)(x - (leftImg.width() - dx)) / (float)abs(dx);
				}
				else {    //A独在部分
					blendImg(x, y, 0) = leftImg(x, y - dy, 0);
					blendImg(x, y, 1) = leftImg(x, y - dy, 1);
					blendImg(x, y, 2) = leftImg(x, y - dy, 2);
				}
			}
			else if (x >= (leftImg.width() - dx) && y < rightImg.height()) {    //B独在部分
				blendImg(x, y, 0) = rightImg(x - (leftImg.width() - dx), y, 0);
				blendImg(x, y, 1) = rightImg(x - (leftImg.width() - dx), y, 1);
				blendImg(x, y, 2) = rightImg(x - (leftImg.width() - dx), y, 2);
			}
			else {    //黑色部分
				blendImg(x, y, 0) = 0;
				blendImg(x, y, 1) = 0;
				blendImg(x, y, 2) = 0;
			}
		}
	}
	blendImg.display("blendImg");
	resultImg = blendImg;
}