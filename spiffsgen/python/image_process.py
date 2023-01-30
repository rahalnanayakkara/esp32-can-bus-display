import numpy as np
import cv2

file_name = "green"

img = cv2.imread("E:/Projects/CAN_Display/spiffsgen/images/"+file_name+".jpg",cv2.IMREAD_COLOR).astype("uint16")

IMAGE_H = img.shape[0]
IMAGE_W = img.shape[1]

print("Image width : ", IMAGE_W)
print("Image height : ", IMAGE_H)

b = img[:,:,0]
g = img[:,:,1]
r = img[:,:,2]

rgb = ((r & 0b11111000) << 8) | ((g & 0b11111100) << 3) | (b >> 3)

hb = (rgb & 0xFF00)>>8
lb = rgb & 0xFF

dat = np.zeros([IMAGE_H*IMAGE_W,2])
dat[:,0] = hb.reshape(-1)
dat[:,1] = lb.reshape(-1)
dat = dat.reshape(-1).astype("uint8")

f = open("E:/Projects/CAN_Display/spiffsgen/spiffs_image/"+file_name+".hex",'wb')
f.write(bytearray(dat))
f.close()

# print(rgb[150][150])
