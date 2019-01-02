#include "cstdio"

#include "ImgSegment.cpp"
#include "houghLine.cpp"
#include "myCanny.cpp"

int main(void) {
    for (int i = 1; i < 11; i++) {
        int count = 1;
        char srcPath[50];
        sprintf(srcPath, "./src/paper%d.bmp", i);
        char resPath[50];
        sprintf(resPath, "./res/peper%d.bmp", i);
        vector<CImg<double>> testImages = ImgSegment(srcPath, resPath).getImages();
        for (int t = 1; t <= testImages.size(); t++) {
            char testPath[50];
            sprintf(testPath, "./test/%d/%d.bmp", i, count++);
            testImages[t-1].save(testPath);
        }
    }
}