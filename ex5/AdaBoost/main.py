import os
import cv2
import struct
import numpy as np
import matplotlib

def load_mnist(mode):
    TRAIN_IMG = './Dataset/train-images.idx3-ubyte'
    TRAIN_LABEL = './Dataset/train-labels.idx1-ubyte'
    TEST_IMG = './Dataset/t10k-images.idx3-ubyte'
    TEST_LABEL = './Dataset/t10k-labels.idx1-ubyte'

    if mode is 'train':
        lblpath = TRAIN_LABEL
        imgpath = TRAIN_IMG
    else:
        lblpath = TEST_LABEL
        imgpath = TEST_IMG

    with open(os.path.join(lblpath), 'rb') as flbl:
        flbl.read(8)
        lbl = np.fromfile(flbl, dtype=np.int8)

    with open(os.path.join(imgpath), 'rb') as fimg:
        rows, cols = struct.unpack(">IIII", fimg.read(16))[2:]
        img = np.fromfile(fimg, dtype=np.uint8).reshape(len(lbl), rows, cols)

    return img, lbl

def test_mnist_data():
    img, lbl = load_mnist('train')
    for current in img:
        cv2.imshow("test", current)
        cv2.waitKey(0)


if __name__ == '__main__':
    test_mnist_data()
