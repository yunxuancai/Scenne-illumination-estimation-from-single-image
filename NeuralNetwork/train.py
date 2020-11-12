# Math
import numpy as np
# Pytorch
import torch
import torch.nn as nn
from torch.utils.data import DataLoader
# Custom
from loader import *
from PIL import Image,ImageFile
ImageFile.LOAD_TRUNCATED_IMAGES=True
# For updating learning rate
def update_lr(optimizer, lr):    
    for param_group in optimizer.param_groups:
        param_group['lr'] = lr

def trainloop(module, lr=0.01, train_epochs=30):
	           
	strlist = []
	
	# set training dataset
	train_dataset = Train_Dataset(csv_path='data/train_list.csv')
	train_dataloader = DataLoader(train_dataset, batch_size=20, shuffle=True, num_workers=1)

	# using KL divergence loss for sun distribution and MSE loss  
	sun_crit = nn.KLDivLoss()
	prr_crit = nn.MSELoss()
	# set optimizer
	optimizer = torch.optim.Adam(module.parameters(), lr=lr)

	# train the model
	cur_lr = lr
	for i in range(train_epochs):
		for i_batch, sample in enumerate(train_dataloader):
			# training input and targets
			img = sample['img'].cuda().float()
			label_dis, label_prrs= sample['dis'].cuda().float(), sample['prrs'].cuda().float()

			# forward pass 
			pred_dis, pred_prrs = module(img)
			beta = 0.1 # to compensate for the number od bins in output distribution
			sun_loss, prr_loss = sun_crit(pred_dis, label_dis), prr_crit(pred_prrs, label_prrs)
			loss = sun_loss + beta *  (prr_loss)
			
			# optimization
			if(i_batch%1==0):
				print('epoch:', i+1, 'steps:', i_batch+1, 'loss:', loss.item())
				strlist.append(str(loss.item())+"\n")
			optimizer.zero_grad()
			loss.backward()
			optimizer.step()

		# Decay learning rate (0.5/15 epochs)
		if i % 15 == 0:
			cur_lr *= 0.5
			update_lr(optimizer, cur_lr)
		if i % 20 == 0:
			torch.save(module.state_dict(), 'weights'+str(i)+'.pth')
		

	# Save the model checkpoint
	torch.save(module.state_dict(), 'weights.pth')

	f = open("out.txt", "w") 
	for i in strlist:
		f.write(i)

	f.close()

def main():

	# device configuration
	torch.cuda.set_device(0)
	# get network module 
	illuminationModule = IlluminationModule().cuda()

	trainloop(illuminationModule)

if __name__ == '__main__':
	main()