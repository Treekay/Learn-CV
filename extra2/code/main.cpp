#include "ImageMorphing.cpp"

int main(void) {
    vector<Line> sourceLines, resultLines;

    sourceLines.push_back(Line(Point(162, 29), Point(84, 66))); // 头部
    sourceLines.push_back(Line(Point(84, 66), Point(53, 113)));
    sourceLines.push_back(Line(Point(53, 113), Point(51, 168)));
    sourceLines.push_back(Line(Point(68, 178), Point(87, 252)));
    sourceLines.push_back(Line(Point(87, 252), Point(142, 297)));
    sourceLines.push_back(Line(Point(142, 297), Point(183, 297)));
    sourceLines.push_back(Line(Point(183, 297), Point(238, 252)));
    sourceLines.push_back(Line(Point(238, 252), Point(254, 180)));
    sourceLines.push_back(Line(Point(271, 167), Point(271, 119)));
    sourceLines.push_back(Line(Point(271, 119), Point(246, 69)));
    sourceLines.push_back(Line(Point(246, 69), Point(162, 29)));
    sourceLines.push_back(Line(Point(106, 166), Point(139, 167))); // 眉毛
    sourceLines.push_back(Line(Point(193, 167), Point(227, 167)));
    sourceLines.push_back(Line(Point(111, 186), Point(137, 186))); // 左眼
    sourceLines.push_back(Line(Point(114, 180), Point(133, 180)));
    sourceLines.push_back(Line(Point(190, 187), Point(218, 187))); // 右眼
    sourceLines.push_back(Line(Point(196, 180), Point(211, 180)));
    sourceLines.push_back(Line(Point(164, 178), Point(164, 222))); // 鼻子
    sourceLines.push_back(Line(Point(126, 244), Point(200, 244))); // 嘴巴
    sourceLines.push_back(Line(Point(153, 264), Point(174, 264)));
    sourceLines.push_back(Line(Point(47, 175), Point(69, 225))); // 耳朵
    sourceLines.push_back(Line(Point(273, 175), Point(255, 224)));
    sourceLines.push_back(Line(Point(113, 273), Point(113, 303))); // 脖子
    sourceLines.push_back(Line(Point(223, 264), Point(223, 294)));
    sourceLines.push_back(Line(Point(113, 303), Point(0, 353))); // 双肩
    sourceLines.push_back(Line(Point(223, 294), Point(320, 337)));

    
    resultLines.push_back(Line(Point(160, 12), Point(61, 58))); // 头部
    resultLines.push_back(Line(Point(61, 58), Point(43, 102)));
    resultLines.push_back(Line(Point(43, 102), Point(53, 180)));
    resultLines.push_back(Line(Point(65, 190), Point(86, 285)));
    resultLines.push_back(Line(Point(86, 285), Point(140, 340)));
    resultLines.push_back(Line(Point(140, 340), Point(189, 340)));
    resultLines.push_back(Line(Point(189, 340), Point(240, 280)));
    resultLines.push_back(Line(Point(240, 280), Point(255, 183)));
    resultLines.push_back(Line(Point(265, 170), Point(275, 100)));
    resultLines.push_back(Line(Point(275, 100), Point(250, 40)));
    resultLines.push_back(Line(Point(250, 40), Point(160, 12)));
    resultLines.push_back(Line(Point(94, 165), Point(130, 165))); // 眉毛
    resultLines.push_back(Line(Point(191, 163), Point(230, 162)));
    resultLines.push_back(Line(Point(101, 183), Point(134, 183))); // 左眼
    resultLines.push_back(Line(Point(108, 173), Point(125, 173)));
    resultLines.push_back(Line(Point(186, 183), Point(218, 183))); // 右睛
    resultLines.push_back(Line(Point(196, 173), Point(211, 173))); 
    resultLines.push_back(Line(Point(163, 174), Point(163, 232))); // 鼻子
    resultLines.push_back(Line(Point(141, 271), Point(183, 271))); // 嘴巴
    resultLines.push_back(Line(Point(150, 285), Point(176, 285)));
    resultLines.push_back(Line(Point(52, 189), Point(69, 238))); // 耳朵
    resultLines.push_back(Line(Point(269, 182), Point(255, 230)));
    resultLines.push_back(Line(Point(92, 290), Point(92, 338))); // 脖子
    resultLines.push_back(Line(Point(235, 287), Point(235, 322)));
    resultLines.push_back(Line(Point(93, 339), Point(0, 360))); // 双肩
    resultLines.push_back(Line(Point(235, 322), Point(322, 348)));
    
    ImageMorphing("../src/1.bmp", "../src/2.bmp", sourceLines, resultLines);
}