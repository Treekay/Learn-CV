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
TRAIN_MODEL_PATH = './myModel.m'

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

    # testImgs, testLabels = load_mnist(TEST_IMG, TEST_LABEL)
    # for img in testImgs:
    #         for pixel in img:
    #             pixel = 0 if pixel == 0 else 255

    # 测试
    clf = joblib.load(TRAIN_MODEL_PATH)

    count = 0
    for test_predict in clf.predict(imgs):
        os.rename(os.path.join('./test/', str(count)+'.bmp'), os.path.join('./test/', str(count) + "_" + str(test_predict) + ".bmp"))
        count += 1

    # discrete_test_errors = []
    # for test_predict in clf.staged_predict(testImgs):
    #      discrete_test_errors.append(1. - accuracy_score(test_predict, testLabels))

    # n_trees_discrete = len(clf)
    # # 测试错误率随迭代次数的变换
    # plt.figure()
    # plt.plot(range(1, n_trees_discrete + 1), discrete_test_errors, c='black', label='SAMME')
    # plt.annotate('%.3f' % discrete_test_errors[-1], xy=(n_trees_discrete, discrete_test_errors[-1]))
    # plt.legend()
    # plt.ylim(0, 1.00)
    # plt.ylabel('Test Error')
    # plt.xlabel('Number of Trees')
    # plt.show()

    # # 写入文件
    # csv_file = csv.writer(open('./res/data.csv', 'a', newline=''), dialect='excel')
