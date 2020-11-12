# System
import argparse
# Math
import numpy as np
import math
# Pytorch
import torch
import torch.nn as nn
from torch.utils.data import DataLoader
# Custom
from loader import IlluminationModule, Inference_Data
from libs.projections import bin2Sphere
import csv
import progressbar

def make360Datalist(fname):
	'''
	construct 360 panorama data list with labels:
	panoID =>> sun position(theta,phi), turbidity, exposure
	'''
	data_list = []
	with open(fname, newline='') as f:
		rows = csv.DictReader(f)
		for row in rows:
			panoID, tur, omg, elevation, fov = row['filepath'], row['turbidity'], row['exposure'], row['elevation'], row['fov']
			sunpos = np.array([float(row['theta']), float(row['phi'])])
			data_list.append({'filepath': panoID, 'sunpos': sunpos, 'turbidity': tur, 'exposure': omg, 'elevation': elevation, 'fov': fov})
	return data_list

def inference(module, img_path):
    # data loader
    inference_data = Inference_Data(img_path) 
    dataloader = DataLoader(inference_data, batch_size=1, shuffle=False, num_workers=1)

    module.eval()

    for i, data in enumerate(dataloader):
        input_img = data.cuda().float()
        with torch.no_grad():
            pred_dis, pred_prrs = module(input_img)
            pred_sunpos = bin2Sphere(np.argmax(pred_dis.cpu().numpy()[0])) # predicted sun position is the bin with highest probability
            pred_tur = pred_prrs.cpu().numpy()[0][0]
            pred_omg = pred_prrs.cpu().numpy()[0][1]
            pred_elv = pred_prrs.cpu().numpy()[0][2]
            pred_fov = math.degrees(pred_prrs.cpu().numpy()[0][3])
            # print('image: ', img_path)
            # print('sun position: theta=', pred_sunpos[0], 'phi=', pred_sunpos[1])
            # print('tubidity:', pred_tur)
            # print('exposure:', pred_omg)
            # print('elevation (degrees):', pred_elv)
            # print('FoV (degrees):', pred_fov)
            return img_path, pred_sunpos, pred_tur,pred_omg,pred_elv,pred_fov
    
from libs.projections import bin2Sphere, rotateSP

def main(args):
    print('loading weights ...')
    # device configuration
    torch.cuda.set_device(0)
    # get network module 
    module = IlluminationModule().cuda()
    #load pre-trained weight
    module.load_state_dict(torch.load(args.pre_trained))
    print('Preparing data ..')
    pano_data = make360Datalist('data/test_list.csv')

    #print(i['filepath'])
    mode = 'test'
    fname1 = 'data/' + mode + '_F.csv'
    fname2 = 'data/' + mode + '_B.csv'
    f2 = open(fname2, 'w', newline = '')
    with open(fname1, 'w', newline = '') as f1:
        fieldnames = ['filepath', 'theta', 'phi', 'turbidity', 'exposure', 'elevation', 'fov']
        writer1 = csv.DictWriter(f1, fieldnames=fieldnames)
        writer1.writeheader()
        writer2 = csv.DictWriter(f2, fieldnames=fieldnames)
        writer2.writeheader()
        for i in progressbar.progressbar(range(len(pano_data))):
            #filepath, sp, turbidity, exposure, elevation, fov = inference(module, pano_data[i]['filepath'])
            filepath = pano_data[i]['filepath']
            sp = pano_data[i]['sunpos']
            turbidity = pano_data[i]['turbidity']
            exposure = pano_data[i]['exposure']
            elevation = float(pano_data[i]['elevation'])
            fov = pano_data[i]['fov']
            h = 0
            if(filepath.find('_F.png')==-1):
                h = 180
                originSP = rotateSP(sp, h, elevation)
                writer2.writerow({'filepath': filepath, 'theta': originSP[0], 'phi': originSP[1], 'turbidity': turbidity, 'exposure': exposure, 'elevation': elevation, 'fov': fov})
            else:
                originSP = rotateSP(sp, h, elevation)
                writer1.writerow({'filepath': filepath, 'theta': originSP[0], 'phi': originSP[1], 'turbidity': turbidity, 'exposure': exposure, 'elevation': elevation, 'fov': fov})
            
                
    print('Done!')

if __name__ == '__main__':

    parser = argparse.ArgumentParser()
    parser.add_argument('--img_path', required=True, help='test image path')
    parser.add_argument('--pre_trained', default='pre-trained/weights.pth', help='pre-trained weight path')

    args = parser.parse_args()
    main(args)