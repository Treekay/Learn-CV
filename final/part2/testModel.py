import os
import csv
import cv2
import struct
import numpy as np
import matplotlib.pyplot as plt

from sklearn.externals.six.moves import zip
from sklearn.datasets import make_gaussian_quantiles
from sklearn.ensemble import AdaBoostClassifier
from sklearn.metrics import accuracy_score
from sklearn.tree import DecisionTreeClassifier
from sklearn.externals import joblib

TEST_PAHT = './test/'
TRAIN_MODEL_PATH = './mnist_bw_model.m'

if __name__ == "__main__":
    # 读取模型
    clf = joblib.load(TRAIN_MODEL_PATH)

    # 读取文件并进行识别
    for t in range(3, 11):
        PATH = TEST_PAHT + str(t) + '/'
        imgFiles = []
        print('[', end='')
        for root, dirs, files in os.walk(PATH):
            num = len(files)
            for i in range(1, num + 1):
                PATH = TEST_PAHT + str(t) + '/' + str(i) + '.bmp'
                img = cv2.imread(PATH, cv2.IMREAD_GRAYSCALE)
                img = np.array(img).reshape(1, -1)
                test_predict = clf.predict(img)[0]
                os.rename(os.path.join('./test/' + str(t) + '/', str(i) + ".bmp"),
                          os.path.join('./test/' + str(t) + '/', str(i) + '_' + str(test_predict) + ".bmp"))
                print(test_predict, end=' ')
        print(']')

    # # 写入文件
    csv_file = csv.writer(open('./res/data.csv', 'a', newline=''), dialect='excel')
