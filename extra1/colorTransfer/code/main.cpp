#include "colorTransfer.cpp"

int main(void) {
    colorTransfer test1("../src/1.bmp", "../src/2.bmp", "1.bmp");
    colorTransfer test2("../src/2.bmp", "../src/3.bmp", "2.bmp");
    colorTransfer test3("../src/3.bmp", "../src/4.bmp", "3.bmp");
    colorTransfer test4("../src/4.bmp", "../src/5.bmp", "4.bmp");
    colorTransfer test5("../src/5.bmp", "../src/1.bmp", "5.bmp");
    return 0;
}