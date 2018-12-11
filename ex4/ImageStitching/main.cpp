#include "stitch.hpp"

#define PATH1 "./test/imageData1"
#define PATH2 "./test/imageData2"

int main() {
	vector<string> imgList1;
	imgList1.push_back(PATH1 "/1.bmp");
	imgList1.push_back(PATH1 "/2.bmp");
	imgList1.push_back(PATH1 "/3.bmp");
	imgList1.push_back(PATH1 "/4.bmp");
	Stitch stitch1(imgList1);

	vector<string> imgList2;
	imgList2.push_back(PATH2 "/1.bmp");
	imgList2.push_back(PATH2 "/2.bmp");
	imgList2.push_back(PATH2 "/3.bmp");
	imgList2.push_back(PATH2 "/4.bmp");
	imgList2.push_back(PATH2 "/5.bmp");
	imgList2.push_back(PATH2 "/6.bmp");
	imgList2.push_back(PATH2 "/7.bmp");
	imgList2.push_back(PATH2 "/8.bmp");
	imgList2.push_back(PATH2 "/9.bmp");
	imgList2.push_back(PATH2 "/10.bmp");
	imgList2.push_back(PATH2 "/11.bmp");
	imgList2.push_back(PATH2 "/12.bmp");
	imgList2.push_back(PATH2 "/13.bmp");
	imgList2.push_back(PATH2 "/14.bmp");
	imgList2.push_back(PATH2 "/15.bmp");
	imgList2.push_back(PATH2 "/16.bmp");
	imgList2.push_back(PATH2 "/17.bmp");
	imgList2.push_back(PATH2 "/18.bmp");
	Stitch stitch2(imgList2);
    return 0;
}