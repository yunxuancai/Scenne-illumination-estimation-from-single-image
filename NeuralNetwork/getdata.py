# System
import argparse
# Math
import numpy as np
import math
# Visualize
import progressbar
# Pytorch
import torch
import torch.nn as nn
from torch.utils.data import DataLoader
# Custom
from loader import IlluminationModule, Eval_Dataset
from libs.projections import bin2Sphere, rotateSP
from utils import getAngle

def recoverSP(sp,x,y):
    originSP = rotateSP(sp, x, -y)
    return originSP

def make360Datalist(fname):
	'''
	construct 360 panorama data list with labels:
	panoID =>> sun position(theta,phi), turbidity, exposure
	'''
	data_list = []
	with open(fname, newline='') as f:
		rows = csv.DictReader(f)
		for row in rows:
			panoID, tur, omg = row['PanoID'], row['turbidity'], row['exposure']
			sunpos = np.array([float(row['theta']), float(row['phi'])])
			data_list.append({'panoID': panoID, 'sunpos': sunpos, 'turbidity': tur, 'exposure': omg})
	return data_list

def generate():
    pano_data = make360Datalist('data/GS_skymodel.csv')
    mode = 'test'
    fname = 'data/' + mode + '_origin.csv'
    with open(fname, 'w', newline = '') as f:
        fieldnames = ['filepath', 'theta', 'phi', 'turbidity', 'exposure', 'elevation', 'fov']
        writer = csv.DictWriter(f, fieldnames=fieldnames)
        writer.writeheader()
        for i in progressbar.progressbar(range(len(pano_data))):
            img_path = 'D://360 google/360_dataset/' + mode + '/' + pano_data[i]['panoID'] + '.png'
            if os.path.isfile(img_path):
                tur, omg = pano_data[i]['turbidity'], pano_data[i]['exposure']
                #### Two cropping/saving processes: front and back view ####
                crop, sp, ele, fov = cropPano(img_path, 0, pano_data[i]['sunpos'])
                save_name = 'C://Users/caiyx/Desktop/dashcam-illumination-estimation-master/data' + mode + '/' + pano_data[i]['panoID'] + '_F.png'
                cv2.imwrite(save_name, crop)
                writer.writerow({'filepath': save_name, 'theta': sp[0], 'phi': sp[1], 'turbidity': tur, 'exposure': omg, 'elevation': ele, 'fov': fov})
                crop, sp, ele, fov = cropPano(img_path, 180, pano_data[i]['sunpos'])
                save_name = 'C://Users/caiyx/Desktop/dashcam-illumination-estimation-master/data' + mode + '/' + pano_data[i]['panoID'] + '_B.png'
                cv2.imwrite(save_name, crop)
                writer.writerow({'filepath': save_name, 'theta': sp[0], 'phi': sp[1], 'turbidity': tur, 'exposure': omg, 'elevation': ele, 'fov': fov})


if __name__ == '__main__':
    generate()