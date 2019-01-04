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

TEST_PAHT = '../other/'
TMP_PATH = '../temp/'
TRAIN_MODEL_PATH = './model.m'

# 用于身份证号和手机号写反的样本的识别
def diff():
    test_lbl = {}
    csv_reader = csv.reader(open('../labels.csv'))
    for row in csv_reader:
        test_lbl[row[0]] = row[0] + row[2] + row[1]

    # 读取模型
    clf = joblib.load(TRAIN_MODEL_PATH)

    # 写入文件
    csv_file = csv.writer(
        open('../res/data.csv', 'a', newline=''), dialect='excel')

    # 读取文件并进行识别
    current = 0  # 当前A4纸
    dirs = os.listdir(TEST_PAHT)
    #单独处理每张A4纸分割出来的数字子图
    for folder in dirs:
        # 初始化行数据
        initRow = []

        # 首项为文件名
        fileName = str(folder) + '.bmp'
        initRow.append(fileName)

        # 四个角点
        f = open(TMP_PATH + str(folder) + '/' + str(folder) + '.txt')
        line = f.readline()
        while line:
            initRow.append(line.replace('\n', ''))
            line = f.readline()

        # 检测到的数字数据
        imgFiles = os.listdir(TEST_PAHT + str(folder) + '/')
        row = copy.copy(initRow)
        count = 0
        num = len(imgFiles)

        # 数字识别
        errorCount = 0
        stuId = ''
        phone = ''
        identity = ''
        data = '#'
        print(str(folder))
        for i in range(num):
            # 读取数字图灰度图
            PATH = TEST_PAHT + str(folder) + '/' + str(i + 1) + '.bmp'
            img = cv2.imread(PATH, cv2.IMREAD_GRAYSCALE)
            img = np.array(img).reshape(1, -1)

            # 用模型识别
            test_predict = clf.predict(img)[0]
            # 是否判断错误
            if test_predict != int(test_lbl[str(folder)][i]):
                errorCount += 1
            data += str(test_predict)
            count += 1

            # 按位数填入csv
            if count % 37 == 8:
                # row.append(data)
                stuId = data
                data = '#'
            if count % 37 == 26:
                identity = data
                # row.append(data)
                data = '#'
            if count % 37 == 0:
                phone = data
                # row.append(data)
                data = '#'
                if i == num - 1:
                    print(folder, errorCount, (num - errorCount) / num)
                    row.append(stuId)
                    row.append(phone)
                    row.append(identity)
                    row.append(errorCount)
                    row.append((num - errorCount) / num)
                csv_file.writerow(row)
                row = copy.copy(initRow)
        # 处理位数不全的身份证号
        if len(data) != 1:
            row.append(stuId)
            row.append(data)
            row.append(identity)
            print(folder, errorCount, (num - errorCount) / num)
            row.append(errorCount)
            row.append((num - errorCount) / num)
            csv_file.writerow(row)
        # 该A4纸所有数字识别结束
        current += 1

# 用于普通弄样本的识别
if __name__ == "__main__":
    test_lbl = {}
    csv_reader = csv.reader(open('../labels.csv'))
    for row in csv_reader:
        test_lbl[row[0]] = row[0] + row[1] + row[2]
    
    # 读取模型
    clf = joblib.load(TRAIN_MODEL_PATH)

    # 写入文件
    csv_file = csv.writer(open('../res/data.csv', 'a', newline=''), dialect='excel')
    
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
        f = open(TMP_PATH + str(folder) + '/' + str(folder) + '.txt')
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
        print(str(folder))
        for i in range(num):
            # 读取数字图灰度图
            PATH = TEST_PAHT + str(folder) + '/' + str(i + 1) + '.bmp'
            img = cv2.imread(PATH, cv2.IMREAD_GRAYSCALE)
            img = np.array(img).reshape(1, -1)

            # 用模型识别
            test_predict = clf.predict(img)[0]
            # 是否判断错误
            if test_predict != int(test_lbl[str(folder)][i]):
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
