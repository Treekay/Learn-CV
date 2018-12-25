#include "ImgSegment.cpp"
#include "houghLine.cpp"
#include "myCanny.cpp"

int main(void) {
    int count = 0;
    for (int i = 1; i < 11; i++) {
        stringstream ss;
        ss << i;
        string srcPath = "./src/paper" + ss.str() + ".bmp";
        string resPath = "./res/peper" + ss.str() + ".bmp";
        vector<CImg<double>> testImages = ImgSegment(srcPath, resPath).getImages();
        for (int t = 1; t <= testImages.size(); t++) {
            stringstream tt;
            tt << t;
            string testPath = "./train/paper" + ss.str() + "_" + tt.str() + ".bmp";
            testImages[t-1].save(testPath.c_str());
            if (i == 1) {
                stringstream cc;
                cc << count++;
                testImages[t-1].save(("./test/" + cc.str() + ".bmp").c_str());
            }
        }
    }
}