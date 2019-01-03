#include "ImgSegment.cpp"
#include "houghLine.cpp"
#include "myCanny.cpp"

int main(void) {
    vector<string> files = getFiles("../src/*");
    for (int i = 0; i < files.size(); i++) {
        string fileName = files[i].substr(0, files[i].length() - 4);
        string srcPath = "../src/" + fileName + ".bmp"; // 原图片路径
        string resPath = "../tmp/" + fileName + "/"; // A4纸矫正后的存放路径
        // 创建文件夹保存分割后的数字和过程图片
        string command = "md ..\\test\\" + fileName;
        system(command.c_str());
        command = "md ..\\tmp\\" + fileName;
        system(command.c_str());
        // 分割数字
        vector<CImg<double>> testImages = ImgSegment(srcPath, resPath, fileName).getImages();
        int count = 1;
        for (int t = 1; t <= testImages.size(); t++) {
            char no[20];
            sprintf(no, "%d", count++);
            string testPath = "../test/" + fileName + "/" + no + ".bmp";
            testImages[t-1].save(testPath.c_str());
        }
    }
}