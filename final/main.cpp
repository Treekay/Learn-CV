#include "ImgSegment.cpp"
#include "houghLine.cpp"

int main(void) {
    ImgSegment("./ImageData/paper1.bmp", "./Result/paper1.bmp");
    ImgSegment("./ImageData/paper2.bmp", "./Result/paper2.bmp");
    ImgSegment("./ImageData/paper3.bmp", "./Result/paper3.bmp");
    ImgSegment("./ImageData/paper4.bmp", "./Result/paper4.bmp");
    ImgSegment("./ImageData/paper5.bmp", "./Result/paper5.bmp");
    ImgSegment("./ImageData/paper6.bmp", "./Result/paper6.bmp");
    ImgSegment("./ImageData/paper7.bmp", "./Result/paper3.bmp");
    ImgSegment("./ImageData/paper8.bmp", "./Result/paper4.bmp");
    ImgSegment("./ImageData/paper9.bmp", "./Result/paper5.bmp");
    ImgSegment("./ImageData/paper10.bmp", "./Result/paper6.bmp");
}