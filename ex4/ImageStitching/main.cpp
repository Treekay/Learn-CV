#include "stitch.hpp"

#define PATH "./test/imageData"

int main() {
	vector<string> imgList;
	imgList.push_back(PATH "1/1.bmp");
	imgList.push_back(PATH "1/2.bmp");
	imgList.push_back(PATH "1/3.bmp");
	imgList.push_back(PATH "1/4.bmp");

	Stitch stitch1(imgList);
    return 0;
}