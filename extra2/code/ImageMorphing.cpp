#include "ImageMorphing.hpp"

/**
 * @msg: 构造函数
 * @param _sourceImg: 初始图像文件路径
 * @param _resultImg: 结果图像文件路径
 * @param _sourceLines: 初始图像的特征线
 * @param _resultLines: 结果图像的特征线
 */
ImageMorphing::ImageMorphing(string _sourceImg, string _resultImg, vector<Line> _sourceLines, vector<Line> _resultLines) {
    sourceImg.load(_sourceImg.c_str());
    resultImg.load(_resultImg.c_str());
    width = sourceImg.width();
    height = sourceImg.height();
    sourceLines = _sourceLines;
    resultLines = _resultLines;
    setLines(); // 画出初始图和结果图的特征线
    morphing(); // 变形过程
}

/**
 * @msg: 确定每帧的特征线
 */
void ImageMorphing::setLines() {
    CImg<double> tempSource = sourceImg;
    CImg<double> tempResult = resultImg;
    double RED[] = {255, 0, 0};
    for (int i = 0; i < sourceLines.size(); i++) {
        tempSource.draw_line(sourceLines[i].P.x, sourceLines[i].P.y, sourceLines[i].Q.x, sourceLines[i].Q.y, RED);
        tempResult.draw_line(resultLines[i].P.x, resultLines[i].P.y, resultLines[i].Q.x, resultLines[i].Q.y, RED);
    }
    tempSource.display("SourceImage");
    tempResult.display("ResultImage");
}

/**
 * @msg: 变形过程
 */
void ImageMorphing::morphing() {
    cout << "Begin Morphing" << endl;
    currentFrame = 0;
    sourceImg.save("../res/0.bmp");
    while (currentFrame < totalFrames) {
        weight = double(currentFrame + 1) / double(totalFrames + 1);
        generateCurrentLines();
        generateCurrentImages();
        currentFrame++;
        cout << "Frame " << currentFrame << " Done" << endl;
    }
    resultImg.save("../res/12.bmp");
    //system("python generateGIF.py");
}

/**
 * @msg: 生成中间帧的特征线集
 */
void ImageMorphing::generateCurrentLines() {
    vector<Line> lines;
    for (int i = 0; i < sourceLines.size(); i++) {
        double x1 = (1 - weight) * sourceLines[i].P.x + weight * resultLines[i].P.x;
        double x2 = (1 - weight) * sourceLines[i].Q.x + weight * resultLines[i].Q.x;
        double y1 = (1 - weight) * sourceLines[i].P.y + weight * resultLines[i].P.y;
        double y2 = (1 - weight) * sourceLines[i].Q.y + weight * resultLines[i].Q.y;
        lines.push_back(Line(Point(x1, y1), Point(x2, y2)));
    }
    currentLines = lines;
}

/**
 * @msg: 生成过程图像
 */
void ImageMorphing::generateCurrentImages() {
    currentImg = sourceImg;
    cimg_forXY(currentImg, x, y) {
        double weightSum = 0.0;
        double sourceSumX = 0.0, sourceSumY = 0.0;
        double resultSumX = 0.0, resultSumY = 0.0;
        for (int i = 0; i < currentLines.size(); i++) {
            Line currentLine = currentLines[i];
            double u = currentLine.getU(Point(x, y));
            double v = currentLine.getV(Point(x, y));
            // 根据距离每条特征线的距离分配不同的权重
            double currentWeight = currentLine.getWeight(Point(x, y));
            // 以初始图为来源
            Point sourcePoint = sourceLines[i].getPoint(u, v);
            sourceSumX += (double)sourcePoint.x * currentWeight;
            sourceSumY += (double)sourcePoint.y * currentWeight;
            // 以结果图为来源
            Point resultPoint = resultLines[i].getPoint(u, v);
            resultSumX += (double)resultPoint.x * currentWeight;
            resultSumY += (double)resultPoint.y * currentWeight;
            // 所有特征线的权重, 用于求平均
            weightSum += currentWeight;
        }
        // 求平均得到在 初始图/结果图 上对应的亚像素点
        double sourceX = cimg::cut(sourceSumX / weightSum, 0, width - 1);
        double sourceY = cimg::cut(sourceSumY / weightSum, 0, height - 1);
        double resultX = cimg::cut(resultSumX / weightSum, 0, width - 1);
        double resultY = cimg::cut(resultSumY / weightSum, 0, height - 1);
        // 反向投影到 初始图/结果图, 用双线性插值求出该亚像素点的像素值
        double sourceValue[3] = {0}, resultValue[3] = {0};
        bilinearInterpolation(sourceValue, 0, sourceX, sourceY);
        bilinearInterpolation(resultValue, 1, resultX, resultY);
        // 按权求值
        currentImg(x, y, 0) = (1 - weight) * sourceValue[0] + weight * resultValue[0];
        currentImg(x, y, 1) = (1 - weight) * sourceValue[1] + weight * resultValue[1];
        currentImg(x, y, 2) = (1 - weight) * sourceValue[2] + weight * resultValue[2];
    }
    currentImg.save(("../res/" + to_string(currentFrame + 1) + ".bmp").c_str());
}

/**
 * @msg: 双线性插值
 */
void ImageMorphing::bilinearInterpolation(double *value, int target, double x, double y) {
    int xCeil = (int)x;
    int yCeil = (int)y;
    int xFloor = xCeil + 1 > width - 1 ? width - 1 : xCeil + 1;
    int yFloor = yCeil + 1 > height - 1 ? height - 1 : yCeil + 1;

    double a = x - xCeil;
    double b = y - yCeil;
    if (target == 0) {
        value[0] = (1 - a) * (1 - b) * sourceImg(xCeil, yCeil, 0) + a * (1 - b) * sourceImg(xFloor, yCeil, 0) + a * b * sourceImg(xFloor, yFloor, 0) + (1 - a) * b * sourceImg(xCeil, yFloor, 0);
        value[1] = (1 - a) * (1 - b) * sourceImg(xCeil, yCeil, 1) + a * (1 - b) * sourceImg(xFloor, yCeil, 1) + a * b * sourceImg(xFloor, yFloor, 1) + (1 - a) * b * sourceImg(xCeil, yFloor, 1);
        value[2] = (1 - a) * (1 - b) * sourceImg(xCeil, yCeil, 2) + a * (1 - b) * sourceImg(xFloor, yCeil, 2) + a * b * sourceImg(xFloor, yFloor, 2) + (1 - a) * b * sourceImg(xCeil, yFloor, 2);
    }
    else {
        value[0] = (1 - a) * (1 - b) * resultImg(xCeil, yCeil, 0) + a * (1 - b) * resultImg(xFloor, yCeil, 0) + a * b * resultImg(xFloor, yFloor, 0) + (1 - a) * b * resultImg(xCeil, yFloor, 0);
        value[1] = (1 - a) * (1 - b) * resultImg(xCeil, yCeil, 1) + a * (1 - b) * resultImg(xFloor, yCeil, 1) + a * b * resultImg(xFloor, yFloor, 1) + (1 - a) * b * resultImg(xCeil, yFloor, 1);
        value[2] = (1 - a) * (1 - b) * resultImg(xCeil, yCeil, 2) + a * (1 - b) * resultImg(xFloor, yCeil, 2) + a * b * resultImg(xFloor, yFloor, 2) + (1 - a) * b * resultImg(xCeil, yFloor, 2);
    }
}