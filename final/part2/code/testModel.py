import os
import csv
import cv2
import copy
import struct
import numpy as np
import matplotlib.pyplot as plt

from sklearn.externals.six.moves import zip
from sklearn.datasets import make_gaussian_quantiles
from sklearn.ensemble import AdaBoostClassifier
from sklearn.metrics import accuracy_score
from sklearn.tree import DecisionTreeClassifier
from sklearn.externals import joblib

TEST_PAHT = '../test/'
TMP_PATH = '../tmp/'
TRAIN_MODEL_PATH = './model.m'

test_lbl = np.array([[1, 5, 3, 3, 1, 0, 2, 9, 1, 3, 8, 2, 7, 4, 1, 8, 3, 9, 2,
                      4, 4, 1, 7, 2, 1, 1, 9, 9, 6, 1, 2, 2, 7, 3, 0, 1, 2,
                      1, 5, 3, 3, 1, 0, 2, 9, 1, 3, 8, 2, 7, 4, 1, 8, 3, 9, 2,
                      4, 4, 1, 7, 2, 1, 1, 9, 9, 6, 1, 2, 2, 7, 3, 0, 1, 2,
                      1, 5, 3, 3, 1, 0, 2, 9, 1, 3, 8, 2, 7, 4, 1, 8, 3, 9, 2,
                      4, 4, 1, 7, 2, 1, 1, 9, 9, 6, 1, 2, 2, 7, 3, 0, 1, 2],
                     [1, 5, 3, 3, 1, 0, 4, 6, 1, 3, 6, 1, 9, 1, 5, 4, 7, 2, 1,
                      4, 4, 5, 3, 0, 1, 1, 9, 9, 8, 1, 0, 2, 5, 2, 7, 3, 4,
                      1, 5, 3, 3, 1, 0, 4, 6, 1, 3, 6, 1, 9, 1, 5, 4, 7, 2, 1,
                      4, 4, 5, 3, 0, 1, 1, 9, 9, 8, 1, 0, 2, 5, 2, 7, 3, 4,
                      1, 5, 3, 3, 1, 0, 4, 6, 1, 3, 6, 1, 9, 1, 5, 4, 7, 2, 1,
                      4, 4, 5, 3, 0, 1, 1, 9, 9, 8, 1, 0, 2, 5, 2, 7, 3, 4],
                     [1, 5, 3, 3, 1, 0, 5, 2, 1, 5, 5, 6, 0, 5, 6, 3, 9, 8, 3,
                      3, 5, 0, 4, 2, 6, 1, 9, 9, 7, 1, 2, 2, 2, 1, 0, 1, 2,
                      1, 5, 3, 3, 1, 0, 5, 2, 1, 5, 5, 6, 0, 5, 6, 3, 9, 8, 3,
                      3, 5, 0, 4, 2, 6, 1, 9, 9, 7, 1, 2, 2, 2, 1, 0, 1, 2,
                      1, 5, 3, 3, 1, 0, 5, 2, 1, 5, 5, 6, 0, 5, 6, 3, 9, 8, 3,
                      3, 5, 0, 4, 2, 6, 1, 9, 9, 7, 1, 2, 2, 2, 1, 0, 1, 2],
                     [1, 5, 3, 3, 1, 1, 8, 0, 1, 3, 2, 6, 0, 8, 3, 1, 0, 4, 8, 
                      4, 4, 2, 3, 1, 3, 1, 9, 9, 8, 0, 3, 2, 7, 3, 0, 5, 9,
                      1, 5, 3, 3, 1, 1, 8, 0, 1, 3, 2, 6, 0, 8, 3, 1, 0, 4, 8, 
                      4, 4, 2, 3, 1, 3, 1, 9, 9, 8, 0, 3, 2, 7, 3, 0, 5, 9,
                      1, 5, 3, 3, 1, 1, 8, 0, 1, 3, 2, 6, 0, 8, 3, 1, 0, 4, 8,
                      4, 4, 2, 3, 1, 3, 1, 9, 9, 8, 0, 3, 2, 7, 3, 0, 5, 9],
                     [1, 5, 3, 3, 1, 3, 4, 4, 1, 5, 5, 2, 1, 1, 4, 5, 6, 7, 8,
                      3, 5, 0, 1, 0, 2, 1, 9, 9, 6, 0, 2, 2, 9, 4, 3, 2, 1,
                      1, 5, 3, 3, 1, 3, 4, 4, 1, 5, 5, 2, 1, 1, 4, 5, 6, 7, 8,
                      3, 5, 0, 1, 0, 2, 1, 9, 9, 6, 0, 2, 2, 9, 4, 3, 2, 1,
                      1, 5, 3, 3, 1, 3, 4, 4, 1, 5, 5, 2, 1, 1, 4, 5, 6, 7, 8,
                      3, 5, 0, 1, 0, 2, 1, 9, 9, 6, 0, 2, 2, 9, 4, 3, 2, 1],
                     [1, 5, 3, 3, 1, 3, 4, 7, 1, 8, 9, 5, 0, 1, 8, 2, 3, 2, 3, 
                      3, 5, 0, 1, 0, 5, 1, 9, 9, 7, 0, 5, 0, 4, 2, 3, 1, 4,
                      1, 5, 3, 3, 1, 3, 4, 7, 1, 8, 9, 5, 0, 1, 8, 2, 3, 2, 3, 
                      3, 5, 0, 1, 0, 5, 1, 9, 9, 7, 0, 5, 0, 4, 2, 3, 1, 4,
                      1, 5, 3, 3, 1, 3, 4, 7, 1, 8, 9, 5, 0, 1, 8, 2, 3, 2, 3, 
                      3, 5, 0, 1, 0, 5, 1, 9, 9, 7, 0, 5, 0, 4, 2, 3, 1, 4],
                     [1, 5, 3, 3, 1, 3, 4, 8, 1, 5, 6, 2, 6, 4, 1, 1, 4, 1, 8,
                      4, 4, 0, 3, 0, 2, 1, 9, 9, 6, 1, 0, 0, 2, 2, 2, 1, 8,
                      1, 5, 3, 3, 1, 3, 4, 8, 1, 5, 6, 2, 6, 4, 1, 1, 4, 1, 8,
                      4, 4, 0, 3, 0, 2, 1, 9, 9, 6, 1, 0, 0, 2, 2, 2, 1, 8,
                      1, 5, 3, 3, 1, 3, 4, 8, 1, 5, 6, 2, 6, 4, 1, 1, 4, 1, 8,
                      4, 4, 0, 3, 0, 2, 1, 9, 9, 6, 1, 0, 0, 2, 2, 2, 1, 8],
                     [1, 5, 3, 3, 1, 3, 5, 1, 1, 3, 7, 1, 9, 2, 7, 4, 5, 0, 6,
                      4, 4, 5, 2, 0, 2, 2, 0, 0, 0, 0, 1, 0, 1, 0, 0, 5, 8,
                      1, 5, 3, 3, 1, 3, 5, 1, 1, 3, 7, 1, 9, 2, 7, 4, 5, 0, 6,
                      4, 4, 5, 2, 0, 2, 2, 0, 0, 0, 0, 1, 0, 1, 0, 0, 5, 8,
                      1, 5, 3, 3, 1, 3, 5, 1, 1, 3, 7, 1, 9, 2, 7, 4, 5, 0, 6,
                      4, 4, 5, 2, 0, 2, 2, 0, 0, 0, 0, 1, 0, 0, 5, 8],
                    [1, 5, 3, 3, 1, 3, 5, 3, 1, 3, 7, 7, 7, 1, 3, 2, 4, 6, 1,
                      4, 5, 6, 7, 5, 7, 1, 3, 2, 4, 6, 6, 3, 3, 1, 5, 7, 8],
                     [1, 5, 3, 3, 1, 3, 6, 4, 1, 3, 6, 3, 2, 5, 5, 2, 8, 3, 1,
                      4, 4, 1, 2, 8, 1, 2, 3, 4, 5, 0, 3, 0, 9, 3, 4, 3, 5,
                      1, 5, 3, 3, 1, 3, 6, 4, 1, 3, 6, 3, 2, 5, 5, 2, 8, 3, 1,
                      4, 4, 1, 2, 8, 1, 2, 3, 4, 5, 0, 3, 0, 9, 3, 4, 3, 5,
                      1, 5, 3, 3, 1, 3, 6, 4, 1, 3, 6, 3, 2, 5, 5, 2, 8, 3, 1,
                      4, 4, 1, 2, 8, 1, 2, 3, 4, 5, 0, 3, 0, 9, 3, 4, 3, 5]])

if __name__ == "__main__":
    # 读取模型
    clf = joblib.load(TRAIN_MODEL_PATH)

    # 写入文件
    csv_file = csv.writer(open('./res/data.csv', 'a', newline=''), dialect='excel')
    
    # 读取文件并进行识别
    current = 0 # 当前A4纸
    dirs = os.listdir(TEST_PAHT)
    #单独处理每张A4纸分割出来的数字子图
    for folder in dirs:
        # 初始化行数据
        initRow = []

        # 首项为文件名
        fileName = str(folder) + '.bmp'
        initRow.append(fileName)

        # 四个角点
        f = open(TMP_PATH + str(folder) + '.txt')
        line = f.readline()
        while line:
            initRow.append(line.replace('\n', ''))
            line = f.readline()

        # 检测到的数字数据
        imgFiles = os.listdir(TEST_PAHT + str(folder) + '/')
        row = copy.copy(initRow)
        count = 0
        data = '#'
        num = len(imgFiles)

        # 数字识别
        errorCount = 0
        for i in range(num):
            # 读取数字图灰度图
            PATH = TEST_PAHT + str(folder) + '/' + str(i + 1) + '.bmp'
            img = cv2.imread(PATH, cv2.IMREAD_GRAYSCALE)
            img = np.array(img).reshape(1, -1)

            # 用模型识别
            test_predict = clf.predict(img)[0]
            # 是否判断错误
            if test_predict != test_lbl[current][i]:
                errorCount += 1
            data += str(test_predict)
            count += 1

            # 按位数填入csv
            if count % 37 == 8:
                row.append(data)
                data = '#'
            if count % 37 == 19:
                row.append(data)
                data = '#'
            if count % 37 == 0:
                row.append(data)
                data = '#'
                if i == num - 1:
                    print(folder, errorCount, (num - errorCount) / num)
                    row.append(errorCount)
                    row.append((num - errorCount) / num)
                csv_file.writerow(row)
                row = copy.copy(initRow)
        # 处理位数不全的身份证号
        if len(data) != 1:
            row.append(data)
            print(folder, errorCount, (num - errorCount) / num)
            row.append(errorCount)
            row.append((num - errorCount) / num)
            csv_file.writerow(row)
        # 该A4纸所有数字识别结束
        current += 1