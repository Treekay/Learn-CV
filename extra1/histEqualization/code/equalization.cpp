#include "equalization.hpp"

/**
 * @msg: construct function. load and process the image
 * @param: filePath of image
 */
Equalization::Equalization(string filePath, string fileName) {
    colorImg.load(filePath.c_str());
    width = colorImg.width();
    height = colorImg.height();
    pixelNum = width * height;
    toGrayScale();                      // get the gray image

    grayImg.display("OriginImage");     // origin image
    grayImg.save((string("../src/gray_") + fileName).c_str());
    grayEqualization();                 // histogram equalize the gray image
    grayImg.save((string("../result/gray_") + fileName).c_str());
    grayImg.display("AfterProcess");    // show the result

    colorImg.display("OriginImage");    // origin image
    colorEqualization();                // histogram equalize the color image
    colorImg.save((string("../result/color_") + fileName).c_str());
    colorImg.display("AfterProcess");   // show the result
}

/**
 * @msg: convert the image to gray scale
 */
void Equalization::toGrayScale() {
    grayImg = CImg<unsigned char>(width, height); // To one channel
    cimg_forXY(colorImg, x, y) {
        int r = colorImg(x, y, 0);
        int g = colorImg(x, y, 1);
        int b = colorImg(x, y, 2);
        double value = (r * 0.2126 + g * 0.7152 + b * 0.0722);
        grayImg(x, y) = value;
    }
}

/**
 * @msg: histogram equalize the gray image
 */
void Equalization::grayEqualization() {
    double T[256] = {0};         // convert function
    double histogram[256] = {0}; // histogram
    double cdf = 0;              // cumulative distribution function

    // statistics the histogram
    cimg_forXY(grayImg, x, y) {
        histogram[grayImg(x,y)]++;
    }
    // get the convert function
    for (int i = 0; i < 256; i++) {
        cdf = cdf + histogram[i];
        T[i] = 255 * cdf / pixelNum;
    }
    // convert the pixel of originImg
    cimg_forXY(grayImg, x, y) {
        grayImg(x, y) = T[grayImg(x, y)];
    }
}

/**
 * @msg: histogram equalize the color image
 */
void Equalization::colorEqualization() {
    // convert image to YUV
    cimg_forXY(colorImg, x, y) {
        double R = colorImg(x, y, 0);
        double G = colorImg(x, y, 1);
        double B = colorImg(x, y, 2);
        // convert RGB to YUV
        double Y = cimg::cut(0.256789 * R + 0.504129 * G + 0.097906 * B + 16, 0, 255);
        double U = cimg::cut(-0.148223 * R - 0.290992 * G + 0.439215 * B + 128, 0, 255);
        double V = cimg::cut(0.439215 * R - 0.367789 * G - 0.071426 * B + 128, 0, 255);
        colorImg(x, y, 0) = Y;
        colorImg(x, y, 1) = U;
        colorImg(x, y, 2) = V;
    }

    //  do the histogram equalization on Y channel
    double transform[256] = {0};    // transform function on Y channel
    double histogram[256] = {0};    // histogram on Y channel, range 0 - 255
    double cdf = 0;                 // cumulative distribution function

    // statistics the histogram
    cimg_forXY(colorImg, x, y) {
        histogram[colorImg(x, y, 0)]++;
    }
    // get the convert function
    for (int i = 0; i < 255; i++) {
        cdf = cdf + histogram[i];
        transform[i] = 255 * cdf / pixelNum;
    }
    // convert the pixel of originImg
    cimg_forXY(colorImg, x, y) {
        colorImg(x, y, 0) = transform[colorImg(x, y, 0)];
    }

    // convert image to RGB
    cimg_forXY(colorImg, x, y) {
        double Y = colorImg(x, y, 0);
        double U = colorImg(x, y, 1);
        double V = colorImg(x, y, 2);
        colorImg(x, y, 0) = cimg::cut(1.164383 * (Y - 16) + 1.596027 * (V - 128), 0, 255);
        colorImg(x, y, 1) = cimg::cut(1.164383 * (Y - 16) - 0.391762 * (U - 128) - 0.812969 * (V - 128), 0, 255);
        colorImg(x, y, 2) = cimg::cut(1.164383 * (Y - 16) + 2.017230 * (U - 128), 0, 255);
    }
}