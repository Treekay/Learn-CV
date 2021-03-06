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

TRAIN_IMG = './Dataset/train-images.idx3-ubyte'
TRAIN_LABEL = './Dataset/train-labels.idx1-ubyte'
TEST_IMG = './Dataset/t10k-images.idx3-ubyte'
TEST_LABEL = './Dataset/t10k-labels.idx1-ubyte'

class AdaBoost(object):
    def __init__(self):
        # 训练样本
        self.trainImgs, self.trainLabels = self.load_mnist(TRAIN_IMG, TRAIN_LABEL)
        # 测试样本
        self.testImgs, self.testLabels = self.load_mnist(TEST_IMG, TEST_LABEL)
        self.AdaBoost(self.trainImgs, self.trainLabels, self.testImgs, self.testLabels)

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
    def AdaBoost(self, trainImgs, trainLabels, testImgs, testLabels):
        # 最大决策树深度： 50
        # 最大迭代次数：150
        # 学习速率： 0.3 
        # 采用算法： SAMME
        bdt_discrete = AdaBoostClassifier(
                        DecisionTreeClassifier(max_depth=50,min_samples_leaf=50,min_samples_split=30),
                        n_estimators=150,
                        learning_rate=0.3,
                        algorithm="SAMME")

        bdt_discrete.fit(trainImgs, trainLabels)

        discrete_test_errors = []

        for discrete_train_predict in bdt_discrete.staged_predict(testImgs):
            discrete_test_errors.append(1. - accuracy_score(discrete_train_predict, testLabels))

        n_trees_discrete = len(bdt_discrete)
        discrete_estimator_errors = bdt_discrete.estimator_errors_[:n_trees_discrete]
        discrete_estimator_weights = bdt_discrete.estimator_weights_[:n_trees_discrete]

        # 测试错误率随迭代次数的变换
        plt.figure(figsize=(15, 5))
        plt.subplot(131)
        plt.plot(range(1, n_trees_discrete + 1), discrete_test_errors, c='black', label='SAMME')
        plt.annotate('%.3f' % discrete_test_errors[-1], xy=(n_trees_discrete, discrete_test_errors[-1]))
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
        
if __name__ == '__main__':
    test = AdaBoost()
