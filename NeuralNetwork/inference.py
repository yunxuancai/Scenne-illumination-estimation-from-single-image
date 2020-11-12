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
from loader import *
from libs.projections import bin2Sphere, rotateSP
import ctypes
import os

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
			print('image: ', img_path)
			print('sun position: phi=', pred_sunpos[0], 'theta=', pred_sunpos[1])
			print('tubidity:', pred_tur)
			print('exposure:', pred_omg)
			print('elevation (degrees):', pred_elv)
			print('FoV (degrees):', pred_fov)
	return recoverSP((pred_sunpos[0], pred_sunpos[1]),pred_elv), pred_tur, pred_omg, pred_elv, pred_fov

def recoverSP(sp,elevation):
    return rotateSP(sp, 0, -elevation)
    

def main(args):
	print('loading weights ...')
	# device configuration
	torch.cuda.set_device(0)
	# get network module 
	module = IlluminationModule().cuda()
	#load pre-trained weight
	module.load_state_dict(torch.load(args.pre_trained))

	sp, tur, omg, elv, fov = inference(module, args.img_path)
	
	CUR_PATH=os.path.dirname(__file__)
	dllPath=os.path.join(CUR_PATH,"ahPhysicalSky.dll")
	print (dllPath)
	#mydll=ctypes.cdll.LoadLibrary(dllPath)
	#print mydll
	pDll=ctypes.WinDLL(dllPath)
	print (pDll)

	pDll.createSky(ctypes.c_float(sp[1]),ctypes.c_float(sp[0]),ctypes.c_float(tur),ctypes.c_float(omg))
	f = open('para.txt','w')
 
	f.write(str(sp[0])+"\n")
	f.write(str(sp[1])+"\n")
	f.write(str(tur)+"\n")
	f.write(str(elv)+"\n")
	f.write(str(fov)+"\n")
	
	f.close()

if __name__ == '__main__':

	parser = argparse.ArgumentParser()
	parser.add_argument('--img_path', required=True, help='test image path')
	parser.add_argument('--pre_trained', default='weights.pth', help='pre-trained weight path')

	args = parser.parse_args()
	main(args)