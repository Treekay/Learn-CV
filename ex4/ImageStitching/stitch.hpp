#ifndef _STITCH_H
#define _STITCH_H

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

struct Image{
	CImg<float> img;
	vector<VlSiftKeypoint> keyPoints;
	vector<float*> descriptors;
	Image(CImg<float> image) {
		img = image;
	};
};

class Stitch {
public:
	Stitch(vector<string> srcList);
	void LoadImg(string);
	void Sift(int);
	void Match(int);
	void Ransac();
	void Blending(int);
private:
	vector<Image> imgList;
	vector<pair<VlSiftKeypoint, VlSiftKeypoint>> match;
	CImg<float> resultImg;
};

#endif