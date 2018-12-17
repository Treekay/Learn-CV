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
        self.trainImgs, self.trainLabels = self.load_mnist(TRAIN_IMG, TRAIN_LABEL)
        self.testImgs, self.testLabels = self.load_mnist(TEST_IMG, TEST_LABEL)
        self.AdaBoost(self.trainImgs, self.trainLabels, self.testImgs, self.testLabels)

    def load_mnist(self, imgPath, labelPath):
        with open(os.path.join(labelPath), 'rb') as labelFile:
            labelFile.read(8)
            labels = np.fromfile(labelFile, dtype=np.int8)
        with open(os.path.join(imgPath), 'rb') as imgFile:
            rows, cols = struct.unpack(">IIII", imgFile.read(16))[2:]
            imgs = np.fromfile(imgFile, dtype=np.uint8).reshape(len(labels), rows, cols)
        return imgs.reshape(len(labels), -1), labels

    def AdaBoost(self, trainImgs, trainLabels, testImgs, testLabels):
        bdt_discrete = AdaBoostClassifier(
                        DecisionTreeClassifier(max_depth=5),
                        n_estimators=1000,
                        learning_rate=1.5,
                        algorithm="SAMME")

        bdt_discrete.fit(trainImgs, trainLabels)

        discrete_test_errors = []

        for discrete_train_predict in bdt_discrete.staged_predict(testImgs):
            discrete_test_errors.append(1. - accuracy_score(discrete_train_predict, testLabels))

        n_trees_discrete = len(bdt_discrete)
        discrete_estimator_errors = bdt_discrete.estimator_errors_[:n_trees_discrete]
        discrete_estimator_weights = bdt_discrete.estimator_weights_[:n_trees_discrete]

        plt.figure(figsize=(5, 5))
        plt.plot(range(1, n_trees_discrete + 1), discrete_test_errors, c='black', label='SAMME')
        plt.legend()
        plt.ylim(0, 1.00)
        plt.ylabel('Test Error')
        plt.xlabel('Number of Trees')
        plt.show()
        
if __name__ == '__main__':
    test = AdaBoost()
