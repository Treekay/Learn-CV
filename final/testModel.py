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

TEST_IMG = './test/t10k-images.idx3-ubyte'
TEST_LABEL = './test/t10k-labels.idx1-ubyte'
TEST_PAHT = './test'
TRAIN_MODEL_PATH = './train_model.m'

# 读取数据
def load_mnist(imgPath, labelPath):
    with open(os.path.join(labelPath), 'rb') as labelFile:
        labelFile.read(8)
        labels = np.fromfile(labelFile, dtype=np.int8)
    with open(os.path.join(imgPath), 'rb') as imgFile:
        rows, cols = struct.unpack(">IIII", imgFile.read(16))[2:]
        imgs = np.fromfile(imgFile, dtype=np.uint8).reshape(len(labels), rows, cols)
    return imgs.reshape(len(labels), -1), labels

if __name__ == "__main__":
    # 读取bmp文件
    imgFiles = []
    for root, dirs, files in os.walk(TEST_PAHT):
        for f in files:
            if os.path.splitext(f)[1] == '.bmp':
                imgFiles.append(os.path.join(root, f))
    # 读取图片
    imgs = []
    for f in imgFiles:
        tmp = cv2.imread(f)
        imgs.append(cv2.imread(f)[..., 0])
    imgs = np.array(imgs).reshape(len(imgFiles), -1)

    testImgs, testLabels = load_mnist(TEST_IMG, TEST_LABEL)

    # 测试
    clf = joblib.load(TRAIN_MODEL_PATH)
    for test_predict in clf.predict(imgs):
        print(test_predict)

    # # 写入文件
    # csv_file = csv.writer(open('./res/data.csv', 'a', newline=''), dialect='excel')
