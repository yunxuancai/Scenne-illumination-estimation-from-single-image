# Scenne illumination estimation from single image
  These programs, completed as a graduation project in 2019, restore illumination from singe image using neural network(ResNext) and render the scene using image-based lighting in OpenGL.


|     |   |
|  ----  | ----  |
| ![image](https://github.com/CyxFTS/Scenne-illumination-estimation-from-single-image/blob/master/pic/2.png)  | ![image](https://github.com/CyxFTS/Scenne-illumination-estimation-from-single-image/blob/master/pic/3.png) |
| ![image](https://github.com/CyxFTS/Scenne-illumination-estimation-from-single-image/blob/master/pic/4.png)  | ![image](https://github.com/CyxFTS/Scenne-illumination-estimation-from-single-image/blob/master/pic/5.png) |

  The neural network is based on [Hold-Geoffroy et al. CVPR 2017] ([https://arxiv.org/abs/1611.06403](https://arxiv.org/abs/1611.06403)), many different CNN framework is being evaluated to improve the performance.

  The pinhole image dataset is sampled on a panorama dataset [360SP](https://cgv.cs.nthu.edu.tw/projects/360SP), which also provide part of the parameter needed for the network. The rest of the parameter is estimated using the methods provided in [Hold-Geoffroy](https://arxiv.org/abs/1611.06403)

  HDR image for IBL is generated using Hošek-Wilkie sky model.
  ![image](https://github.com/CyxFTS/Scenne-illumination-estimation-from-single-image/blob/master/pic/1.png)


