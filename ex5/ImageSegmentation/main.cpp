#include "ImgSegment.cpp"
#include "houghLine.cpp"

int main(void) {
    ImgSegment("./Dataset/paper1.bmp", "./Result/paper1.bmp");
    ImgSegment("./Dataset/paper2.bmp", "./Result/paper2.bmp");
    ImgSegment("./Dataset/paper3.bmp", "./Result/paper3.bmp");
    ImgSegment("./Dataset/paper4.bmp", "./Result/paper4.bmp");
    ImgSegment("./Dataset/paper5.bmp", "./Result/paper5.bmp");
    ImgSegment("./Dataset/paper6.bmp", "./Result/paper6.bmp");
}