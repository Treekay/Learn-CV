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

TRAIN_IMG = './train/train-images.idx3-ubyte'
TRAIN_LABEL = './train/train-labels.idx1-ubyte'

class AdaBoost(object):
    def __init__(self):
        # 训练样本
        self.trainImgs, self.trainLabels = self.load_mnist(TRAIN_IMG, TRAIN_LABEL)
        for img in self.trainImgs:
            for pixel in img:
                pixel = 0 if pixel == 0 else 255
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
        print('train done\n')
        joblib.dump(bdt_discrete, 'train_model.m')
        
if __name__ == '__main__':
    test = AdaBoost()
