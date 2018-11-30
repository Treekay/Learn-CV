import os
import numpy as np
from PIL import Image

def generateGIF():
    print('Generate GIF')
    # 生成并保存GIF图
    frames = []
    i = 0
    while i <= 12:
        s = '../res/' + str(i) + '.bmp'
        img = np.array(Image.open(s))
        frames.append(Image.fromarray(img)) # 添加中间帧图片缓存序列
        i += 1
    try:
        # 用图片序列生成并保存GIF图
        frames[0].save('../res/morphing.gif', save_all=True, append_images=frames[1:], duration=0.01, loop=0) 
    except IOError:
        print("Error: file path error")
    else:
        # 显示GIF图
        print('Show GIF')
        os.system(r"start ../res/morphing.gif")

if __name__ == '__main__':
    generateGIF()
