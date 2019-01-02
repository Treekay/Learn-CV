import os
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

# TRAIN_IMG = './train/train-images.idx3-ubyte'
# TRAIN_LABEL = './train/train-labels.idx1-ubyte'
TRAIN_PATH = './train'
TRAIN_LABEL = './train/labels.txt'

class AdaBoost(object):
    def __init__(self):
        # 训练样本
        # self.trainImgs, self.trainLabels = self.load_mnist(TRAIN_IMG, TRAIN_LABEL)
        # for img in self.trainImgs:
        #     for pixel in img:
        #         pixel = 0 if pixel == 0 else 255
        # 读取bmp文件
        imgFiles = []
        for root, dirs, files in os.walk(TRAIN_PATH):
            for f in files:
                if os.path.splitext(f)[1] == '.bmp':
                    imgFiles.append(os.path.join(root, f))
        # 读取图片
        self.trainImgs = []
        for f in imgFiles:
            tmp = cv2.imread(f)
            self.trainImgs.append(cv2.imread(f)[..., 0])
        self.trainImgs = np.array(self.trainImgs).reshape(len(imgFiles), -1)

        labels = []
        # 读取labels
        with open(TRAIN_LABEL, 'r') as f:
            for line in f:
                line = line.rstrip()
                for label in line:
                    labels.append(label)
        self.trainLabels = np.array(labels)

        self.AdaBoost(self.trainImgs, self.trainLabels)

    # 读取数据
    def load_mnist(self, imgPath, labelPath):
        with open(os.path.join(labelPath), 'rb') as labelFile:
            labelFile.read(8)
            labels = np.fromfile(labelFile, dtype=np.int8)
        with open(os.path.join(imgPath), 'rb') as imgFile:
            rows, cols = struct.unpack(">IIII", imgFile.read(16))[2:]
            imgs = np.fromfile(imgFile, dtype=np.uint8).reshape(len(labels), rows, cols)
        return imgs.reshape(len(labels), -1), labels

    # AdaBoost
    def AdaBoost(self, trainImgs, trainLabels):
        bdt_discrete = AdaBoostClassifier(
                        DecisionTreeClassifier(max_depth=50,min_samples_leaf=50,min_samples_split=30),
                        n_estimators=10000,
                        learning_rate=0.05,
                        algorithm="SAMME")

        bdt_discrete.fit(trainImgs, trainLabels)

        discrete_train_errors = []

        for discrete_train_predict in bdt_discrete.staged_predict(trainImgs):
            discrete_train_errors.append(1. - accuracy_score(discrete_train_predict, trainLabels))

        n_trees_discrete = len(bdt_discrete)
        discrete_estimator_errors = bdt_discrete.estimator_errors_[:n_trees_discrete]
        discrete_estimator_weights = bdt_discrete.estimator_weights_[:n_trees_discrete]

        # 测试错误率随迭代次数的变换
        plt.figure(figsize=(15, 5))
        plt.subplot(131)
        plt.plot(range(1, n_trees_discrete + 1), discrete_train_errors, c='black', label='SAMME')
        plt.annotate('%.3f' % discrete_train_errors[-1], xy=(n_trees_discrete, discrete_train_errors[-1]))
        plt.legend()
        plt.ylim(0, 1.00)
        plt.ylabel('Test Error')
        plt.xlabel('Number of Trees')

        # 训练错误率随迭代次数的变换
        plt.subplot(132)
        plt.plot(range(1, n_trees_discrete + 1), discrete_estimator_errors, "b", label='SAMME', alpha=.5)
        plt.legend()
        plt.ylabel('Error')
        plt.xlabel('Number of Trees')
        plt.ylim((0, discrete_estimator_errors.max() * 1.2))
        plt.xlim((-20, len(bdt_discrete) + 20))

        # 训练权重随迭代次数的变化
        plt.subplot(133)
        plt.plot(range(1, n_trees_discrete + 1), discrete_estimator_weights, "b", label='SAMME')
        plt.legend()
        plt.ylabel('Weight')
        plt.xlabel('Number of Trees')
        plt.ylim((0, discrete_estimator_weights.max() * 1.2))
        plt.xlim((-20, n_trees_discrete + 20))

        # prevent overlapping y-axis labels
        plt.subplots_adjust(wspace=0.25)
        plt.show()

        print('train done')
        joblib.dump(bdt_discrete, 'myModel.m')
        
if __name__ == '__main__':
    test = AdaBoost()
