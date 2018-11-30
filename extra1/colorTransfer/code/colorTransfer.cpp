#include "colorTransfer.hpp"

/**
 * @msg: construct funciton, load and process the image
 * @param source: filePath of source image
 * @param target: filePath of target image
 */
colorTransfer::colorTransfer(string source, string target, string fileName) {
    sourceImg.load(source.c_str());     // load source image
    targetImg.load(target.c_str());     // load target image
    
    sourceImg.display("SourceImage");   // display source image
    targetImg.display("TargetImage");   // display target image

    sourceImg = rgbTolab(sourceImg);    // sourceImg : rgb -> lab
    targetImg = rgbTolab(targetImg);    // targetImg : rgb -> lab
    transferlab();                      // resultlab : sourcelab -> targetlab
    sourceImg = labToRGB(sourceImg);    // sourceImg : resultlab -> rgb
    sourceImg.save((string("../result/") + fileName).c_str());

    sourceImg.display("ResultImage");   // display result image
}

/**
 * @msg: convert the img from rgb to lab
 * @param img: a rgb-space image
 * @return: a lab-space image
 */
CImg<double> colorTransfer::rgbTolab(CImg<double> img) {
    cimg_forXY(img, x, y) {
        // RGB -> XYZ
        double R = img(x, y, 0);
        double G = img(x, y, 1);
        double B = img(x, y, 2);

        double X = (0.4124564 * R + 0.3575761 * G + 0.1804375 * B);
        double Y = (0.2126729 * R + 0.7151522 * G + 0.0721750 * B);
        double Z = (0.0193339 * R + 0.1191920 * G + 0.9503041 * B);

        // XYZ -> lab
        double xr = X / 95.047;
        double yr = Y / 100.0;
        double zr = Z / 108.883;

        double fx = (xr > 0.008856) ? pow(xr, 1.0 / 3.0) : ((903.3 * xr + 16) / 116);
        double fy = (yr > 0.008856) ? pow(yr, 1.0 / 3.0) : ((903.3 * yr + 16) / 116);
        double fz = (zr > 0.008856) ? pow(zr, 1.0 / 3.0) : ((903.3 * zr + 16) / 116);

        double l = 116 * fy - 16;
        double a = 500 * (fx - fy);
        double b = 200 * (fy - fz);

        // assign value
        img(x, y, 0) = l;
        img(x, y, 1) = a;
        img(x, y, 2) = b;
    }
    return img;
}

/**
 * @msg: convert the img from lab to rgb
 * @param img: a lab-space image
 * @return: a rgb-space image
 */
CImg<double> colorTransfer::labToRGB(CImg<double> img) {
    cimg_forXY(img, x, y) {
        // lab -> XYZ
        double l = img(x, y, 0);
        double a = img(x, y, 1);
        double b = img(x, y, 2);

        double fy = (l + 16.0) / 116.0;
        double fx = a / 500.0 + fy;
        double fz = fy - b / 200.0;

        double xr = (pow(fx, 3) > 0.008856) ? pow(fx, 3) : ((116 * fx - 16.0) / 903.3);
        double yr = (l > 0.008856 * 903.3) ? pow(fy, 3) : (l / 903.3);
        double zr = (pow(fz, 3) > 0.008856) ? pow(fz, 3) : ((116 * fz - 16.0) / 903.3);

        double X = xr * 95.047;
        double Y = yr * 100.0;
        double Z = zr * 108.883;

        // XYZ -> RGB
        double R = cimg::cut(3.24045420 * X - 1.5371385 * Y - 0.4985314 * Z, 0, 255);
        double G = cimg::cut(-0.9692660 * X + 1.8760108 * Y + 0.0415560 * Z, 0, 255);
        double B = cimg::cut(0.05564340 * X - 0.2040259 * Y + 1.0572252 * Z, 0, 255);

        // assign value
        img(x, y, 0) = R;
        img(x, y, 1) = G;
        img(x, y, 2) = B;
    }
    return img;
}

/**
 * @msg: transfer the source lab to target lab
 */
void colorTransfer::transferlab() {
    double num1 = sourceImg.width() * sourceImg.height();
    double num2 = targetImg.width() * targetImg.height();
    double sum1[3] = {0}, mean1[3] = {0}, variance1[3] = {0}, temp1[3] = {0};
    double sum2[3] = {0}, mean2[3] = {0}, variance2[3] = {0}, temp2[3] = {0};

    // get the source mean
    cimg_forXY(sourceImg, x, y) {
        sum1[0] += sourceImg(x, y, 0);
        sum1[1] += sourceImg(x, y, 1);
        sum1[2] += sourceImg(x, y, 2);
    }
    mean1[0] = sum1[0] / num1;
    mean1[1] = sum1[1] / num1;
    mean1[2] = sum1[2] / num1;
    // get the source variance
    cimg_forXY(sourceImg, x, y) {
        temp1[0] += (sourceImg(x, y, 0) - mean1[0]) * (sourceImg(x, y, 0) - mean1[0]);
        temp1[1] += (sourceImg(x, y, 1) - mean1[1]) * (sourceImg(x, y, 1) - mean1[1]);
        temp1[2] += (sourceImg(x, y, 2) - mean1[2]) * (sourceImg(x, y, 2) - mean1[2]);
    }
    variance1[0] = temp1[0] / num1;
    variance1[1] = temp1[1] / num1;
    variance1[2] = temp1[2] / num1;

    // get the target mean
    cimg_forXY(targetImg, x, y) {
        sum2[0] += targetImg(x, y, 0);
        sum2[1] += targetImg(x, y, 1);
        sum2[2] += targetImg(x, y, 2);
    }
    mean2[0] = sum2[0] / num2;
    mean2[1] = sum2[1] / num2;
    mean2[2] = sum2[2] / num2;
    // get the target variance
    cimg_forXY(targetImg, x, y) {
        temp2[0] += (targetImg(x, y, 0) - mean2[0]) * (targetImg(x, y, 0) - mean2[0]);
        temp2[1] += (targetImg(x, y, 1) - mean2[1]) * (targetImg(x, y, 1) - mean2[1]);
        temp2[2] += (targetImg(x, y, 2) - mean2[2]) * (targetImg(x, y, 2) - mean2[2]);
    }
    variance2[0] = temp2[0] / num2;
    variance2[1] = temp2[1] / num2;
    variance2[2] = temp2[2] / num2;

    // transfer
    cimg_forXY(sourceImg, x, y) {
        sourceImg(x, y, 0) = (sourceImg(x, y, 0) - mean1[0]) * variance2[0] / variance1[0] + mean2[0];
        sourceImg(x, y, 1) = (sourceImg(x, y, 1) - mean1[1]) * variance2[1] / variance1[1] + mean2[1];
        sourceImg(x, y, 2) = (sourceImg(x, y, 2) - mean1[2]) * variance2[2] / variance1[2] + mean2[2];
    }
}