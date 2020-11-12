# Data processing 
import pandas as pd
from skimage import io, transform
# Math
import numpy as np
import math
# Pytorch
import torch
import torch.nn as nn
import torch.nn.functional as F
from torch.utils.data import Dataset
from torchvision import transforms, utils
# Custom
from utils import vMF

#__all__ = ['ResNeXt', 'resnext18', 'resnext34', 'resnext50', 'resnext101',
 #          'resnext152']

'''
	input: (240, 320, 3) image
	output: two heads--> 1. distrubution (1D 240 vector), 2. parameters (1D 4 vector)
'''
class IlluminationModule(nn.Module):
	def __init__(self):
		super().__init__()
		self.cv_block1 = conv_bn_elu(3, 64, kernel_size=7, stride=2) # 160 x 120
		self.cv_block2 = conv_bn_elu(64, 128, kernel_size=5, stride=2) # 80 x 60
		self.cv_block3 = conv_bn_elu(128, 256, stride=2) # 40 x 30
		self.cv_block4 = conv_bn_elu(256, 256)
		self.cv_block5 = conv_bn_elu(256, 256, stride=2)  # 20 x 15
		self.cv_block6= conv_bn_elu(256, 256)
		self.cv_block7 = conv_bn_elu(256, 256, stride=2) # 10 x 8

		self.fc = nn.Linear(256*10*8, 2048)
		self.fc_bn = nn.BatchNorm1d(2048)
		''' two heads regression'''
		self.ds_fc = nn.Linear(2048, 256) # sky distribution
		self.ds_bn = nn.BatchNorm1d(256)
		self.pr_fc = nn.Linear(2048, 4) # sky and camera parameters
		self.pr_bn = nn.BatchNorm1d(4)
	
	def forward(self, x):
		x = self.cv_block1(x)
		x = self.cv_block2(x)
		x = self.cv_block3(x)
		x = self.cv_block4(x)
		x = self.cv_block5(x)
		x = self.cv_block6(x)
		x = self.cv_block7(x)
		x = x.view(-1, 256*10*8)
		x = F.elu(self.fc_bn(self.fc(x)))
		return F.log_softmax(self.ds_bn(self.ds_fc(x)), dim=1), self.pr_bn(self.pr_fc(x))
		

def conv_bn_elu(in_, out_, kernel_size=3, stride=1, padding=True):
	## conv layer with BN and ELU function 
	pad = int(kernel_size/2)
	if padding is False:
		pad = 0
	return nn.Sequential(
		nn.Conv2d(in_, out_, kernel_size, stride=stride, padding=pad),
		nn.BatchNorm2d(out_),
		nn.ELU(),
	)

'''
	Dataset loader 
	dataset standardization ==> 
		mean: [0.48548178 0.48455666 0.46329196] std: [0.21904471 0.21578524 0.23359051]
'''
class Train_Dataset(Dataset):
	def __init__(self, csv_path):
		self.to_tensor = transforms.ToTensor()
		self.normalize = transforms.Normalize(mean=[0.48548178, 0.48455666, 0.46329196],
												std= [0.21904471, 0.21578524, 0.23359051])
		self.data = pd.read_csv(csv_path)
		self.path_arr = np.asarray(self.data['filepath'])

		''' sun position: theta, phi '''
		self.theta_arr = np.asarray(self.data['theta'])
		self.phi_arr = np.asarray(self.data['phi'])

		''' parameters: turbidity, omega(scaling factor, exposure), camera evelation (degrees) and FoV (radians) '''
		self.tub_arr = np.asarray(self.data['turbidity'])
		self.omg_arr = np.asarray(self.data['exposure'])
		self.elv_arr = np.asarray(self.data['elevation'])
		self.fov_arr = np.asarray(self.data['fov'])

		self.data_len = len(self.data.index)

	def __getitem__(self, index):
		source_img_name = self.path_arr[index]
		source_img = io.imread(source_img_name)[:, :, :3]/255.0
		tensor_img = self.to_tensor(source_img)
		sun_pos = np.asarray([self.theta_arr[index], self.phi_arr[index]])
		sp_pdf = vMF(sun_pos) # target probability distribution of sun position
		pr_vec = np.asarray([self.tub_arr[index], self.omg_arr[index], float(self.elv_arr[index]), math.radians(float(self.fov_arr[index]))]) # target parameters
		label = {'img': self.normalize(tensor_img), 'dis': sp_pdf, 'prrs': pr_vec}
		return label

	def __len__(self):
		return self.data_len

class Eval_Dataset(Dataset):
	def __init__(self, csv_path):
		self.to_tensor = transforms.ToTensor()
		self.normalize = transforms.Normalize(mean=[0.48548178, 0.48455666, 0.46329196],
												std= [0.21904471, 0.21578524, 0.23359051])
		self.data = pd.read_csv(csv_path)
		self.path_arr = np.asarray(self.data['filepath'])

		''' sun position: theta, phi '''
		self.theta_arr = np.asarray(self.data['theta'])
		self.phi_arr = np.asarray(self.data['phi'])

		''' parameters: turbidity, omega(scaling factor, exposure), camera evelation and FoV '''
		self.tub_arr = np.asarray(self.data['turbidity'])
		self.omg_arr = np.asarray(self.data['exposure'])
		self.elv_arr = np.asarray(self.data['elevation'])
		self.fov_arr = np.asarray(self.data['fov'])

		self.data_len = len(self.data.index)

	def __getitem__(self, index):
		source_img_name = self.path_arr[index]
		source_img = io.imread(source_img_name)[:, :, :3]/255.0
		tensor_img = self.to_tensor(source_img)
		sun_pos = np.asarray([self.theta_arr[index], self.phi_arr[index]])
		sp_pdf = vMF(sun_pos) # target probability distribution of sun position
		pr_vec = np.asarray([self.tub_arr[index], self.omg_arr[index], float(self.elv_arr[index]), math.radians(float(self.fov_arr[index]))]) # target parameters
		label = {'img': self.normalize(tensor_img), 'dis': sp_pdf, 'prrs': pr_vec, 'sp': sun_pos}
		return label

	def __len__(self):
		return self.data_len
from skimage.transform import resize
class Inference_Data(Dataset):
	def __init__(self, img_path):
		self.input_img = resize(io.imread(img_path), (320, 240),
                       anti_aliasing=True)
		self.to_tensor = transforms.ToTensor()
		self.normalize = transforms.Normalize(mean=[0.48548178, 0.48455666, 0.46329196],
												std= [0.21904471, 0.21578524, 0.23359051])
		self.data_len = 1

	def __getitem__(self, index):
		tensor_img = self.to_tensor(self.input_img)
		return self.normalize(tensor_img)

	def __len__(self):
		return self.data_len

def conv3x3(in_planes, out_planes, stride=1):
    """3x3 convolution with padding"""
    return nn.Conv2d(in_planes, out_planes, kernel_size=3, stride=stride,
                     padding=1, bias=False)


class BasicBlock(nn.Module):
    expansion = 1

    def __init__(self, inplanes, planes, stride=1, downsample=None):
        super(BasicBlock, self).__init__()
        self.conv1 = conv3x3(inplanes, planes*2, stride)
        self.bn1 = nn.BatchNorm2d(planes*2)
        self.relu = nn.ReLU(inplace=True)
        self.conv2 = conv3x3(planes*2, planes*2)
        self.bn2 = nn.BatchNorm2d(planes*2)
        self.downsample = downsample
        self.stride = stride

    def forward(self, x):
        residual = x

        out = self.conv1(x)
        out = self.bn1(out)
        out = self.relu(out)

        out = self.conv2(out)
        out = self.bn2(out)

        if self.downsample is not None:
            residual = self.downsample(x)

        out += residual
        out = self.relu(out)

        return out


class Bottleneck(nn.Module):
    expansion = 4

    def __init__(self, inplanes, planes, stride=1, downsample=None):
        super(Bottleneck, self).__init__()
        self.conv1 = nn.Conv2d(inplanes, planes*2, kernel_size=1, bias=False)
        self.bn1 = nn.BatchNorm2d(planes*2)
        self.conv2 = nn.Conv2d(planes*2, planes*2, kernel_size=3, stride=stride,
                               padding=1, bias=False)
        self.bn2 = nn.BatchNorm2d(planes*2)
        self.conv3 = nn.Conv2d(planes*2, planes * 4, kernel_size=1, bias=False)
        self.bn3 = nn.BatchNorm2d(planes * 4)
        self.relu = nn.ReLU(inplace=True)
        self.downsample = downsample
        self.stride = stride

    def forward(self, x):
        residual = x

        out = self.conv1(x)
        out = self.bn1(out)
        out = self.relu(out)

        out = self.conv2(out)
        out = self.bn2(out)
        out = self.relu(out)

        out = self.conv3(out)
        out = self.bn3(out)

        if self.downsample is not None:
            residual = self.downsample(x)

        out += residual
        out = self.relu(out)

        return out


class ResNeXt(nn.Module):

    def __init__(self, block, layers):
        self.inplanes = 64
        super(ResNeXt, self).__init__()
        self.conv1 = nn.Conv2d(3, 64, kernel_size=7, stride=2, padding=3,
                               bias=False)
        self.bn1 = nn.BatchNorm2d(64)
        self.relu = nn.ReLU(inplace=True)
        self.maxpool = nn.MaxPool2d(kernel_size=3, stride=2, padding=1)
        self.layer1 = self._make_layer(block, 64, layers[0])
        self.layer2 = self._make_layer(block, 128, layers[1], stride=2)
        self.layer3 = self._make_layer(block, 256, layers[2], stride=2)
        self.layer4 = self._make_layer(block, 256, layers[3], stride=4)
        self.avgpool = nn.AvgPool2d(7, stride=1)
        #self.fc = nn.Linear(512 * block.expansion, num_classes)
        self.fc = nn.Linear(256*10*8, 2048)
        self.fc_bn = nn.BatchNorm1d(2048)
        ''' two heads regression'''
        self.ds_fc = nn.Linear(2048, 160) # sky distribution
        self.ds_bn = nn.BatchNorm1d(160)
        self.pr_fc = nn.Linear(2048, 4) # sky camera parameters
        self.pr_bn = nn.BatchNorm1d(4)

        for m in self.modules():
            if isinstance(m, nn.Conv2d):
                n = m.kernel_size[0] * m.kernel_size[1] * m.out_channels
                m.weight.data.normal_(0, math.sqrt(2. / n))
            elif isinstance(m, nn.BatchNorm2d):
                m.weight.data.fill_(1)
                m.bias.data.zero_()

    def _make_layer(self, block, planes, blocks, stride=1):
        downsample = None
        if stride != 1 or self.inplanes != planes * block.expansion:
            downsample = nn.Sequential(
                nn.Conv2d(self.inplanes, planes * block.expansion,
                          kernel_size=1, stride=stride, bias=False),
                nn.BatchNorm2d(planes * block.expansion),
            )

        layers = []
        layers.append(block(self.inplanes, planes, stride, downsample))
        self.inplanes = planes * block.expansion
        for i in range(1, blocks):
            layers.append(block(self.inplanes, planes))

        return nn.Sequential(*layers)

    def forward(self, x):
        x = self.conv1(x)
        x = self.bn1(x)
        x = self.relu(x)
        x = self.maxpool(x)

        x = self.layer1(x)
        x = self.layer2(x)
        x = self.layer3(x)
        x = self.layer4(x)

        #x = self.avgpool(x)
        x = x.view(-1, 256*10*8)
        x = F.elu(self.fc_bn(self.fc(x)))
        return F.log_softmax(self.ds_bn(self.ds_fc(x)), dim=1), self.pr_bn(self.pr_fc(x))


def resnext18( **kwargs):
    """Constructs a ResNeXt-18 model.
    """
    model = ResNeXt(BasicBlock, [2, 2, 2, 2], **kwargs)
    return model


def resnext34(**kwargs):
    """Constructs a ResNeXt-34 model.
    """
    model = ResNeXt(BasicBlock, [3, 4, 6, 3], **kwargs)
    return model


def resnext50(**kwargs):
    """Constructs a ResNeXt-50 model.
    """
    model = ResNeXt(Bottleneck, [3, 4, 6, 3], **kwargs)
    return model


def resnext101(**kwargs):
    """Constructs a ResNeXt-101 model.
    """
    model = ResNeXt(Bottleneck, [3, 4, 23, 3], **kwargs)
    return model


def resnext152(**kwargs):
    """Constructs a ResNeXt-152 model.
    """
    model = ResNeXt(Bottleneck, [3, 8, 36, 3], **kwargs)
    return model

####################################################################



def conv_bn(inp, oup, stride):
    return nn.Sequential(
        nn.Conv2d(inp, oup, 3, stride, 1, bias=False),
        nn.BatchNorm2d(oup),
        nn.ReLU6(inplace=True)
    )


def conv_1x1_bn(inp, oup):
    return nn.Sequential(
        nn.Conv2d(inp, oup, 1, 1, 0, bias=False),
        nn.BatchNorm2d(oup),
        nn.ReLU6(inplace=True)
    )


class InvertedResidual(nn.Module):
    def __init__(self, inp, oup, stride, expand_ratio):
        super(InvertedResidual, self).__init__()
        self.stride = stride
        assert stride in [1, 2]

        hidden_dim = round(inp * expand_ratio)
        self.use_res_connect = self.stride == 1 and inp == oup

        if expand_ratio == 1:
            self.conv = nn.Sequential(
                # dw
                nn.Conv2d(hidden_dim, hidden_dim, 3, stride, 1, groups=hidden_dim, bias=False),
                nn.BatchNorm2d(hidden_dim),
                nn.ReLU6(inplace=True),
                # pw-linear
                nn.Conv2d(hidden_dim, oup, 1, 1, 0, bias=False),
                nn.BatchNorm2d(oup),
            )
        else:
            self.conv = nn.Sequential(
                # pw
                nn.Conv2d(inp, hidden_dim, 1, 1, 0, bias=False),
                nn.BatchNorm2d(hidden_dim),
                nn.ReLU6(inplace=True),
                # dw
                nn.Conv2d(hidden_dim, hidden_dim, 3, stride, 1, groups=hidden_dim, bias=False),
                nn.BatchNorm2d(hidden_dim),
                nn.ReLU6(inplace=True),
                # pw-linear
                nn.Conv2d(hidden_dim, oup, 1, 1, 0, bias=False),
                nn.BatchNorm2d(oup),
            )

    def forward(self, x):
        if self.use_res_connect:
            return x + self.conv(x)
        else:
            return self.conv(x)


class MobileNetV2(nn.Module):
    def __init__(self, n_class=1000, input_size=320, width_mult=1.):
        super(MobileNetV2, self).__init__()
        block = InvertedResidual
        input_channel = 32
        last_channel = 1280
        interverted_residual_setting = [
            # t, c, n, s
            [1, 16, 1, 1],
            [6, 24, 2, 2],
            [6, 32, 3, 2],
            [6, 64, 4, 2],
            [6, 96, 3, 1],
            [6, 160, 3, 2],
            [6, 320, 1, 2],
        ]

        # building first layer
        assert input_size % 32 == 0
        input_channel = int(input_channel * width_mult)
        self.last_channel = int(last_channel * width_mult) if width_mult > 1.0 else last_channel
        self.features = [conv_bn(3, input_channel, 2)]
        # building inverted residual blocks
        for t, c, n, s in interverted_residual_setting:
            output_channel = int(c * width_mult)
            for i in range(n):
                if i == 0:
                    self.features.append(block(input_channel, output_channel, s, expand_ratio=t))
                else:
                    self.features.append(block(input_channel, output_channel, 1, expand_ratio=t))
                input_channel = output_channel
        # building last several layers
        self.features.append(conv_1x1_bn(input_channel, self.last_channel))
        # make it nn.Sequential
        self.features = nn.Sequential(*self.features)

        # building classifier
        self.classifier = nn.Sequential(
            nn.Dropout(0.2),
            nn.Linear(self.last_channel, n_class),
        )

        self._initialize_weights()
        self.fc = nn.Linear(256*10*10, 2048)
        self.fc_bn = nn.BatchNorm1d(2048)
        ''' two heads regression'''
        self.ds_fc = nn.Linear(2048, 256) # sky distribution
        self.ds_bn = nn.BatchNorm1d(256)
        self.pr_fc = nn.Linear(2048, 3) # sky camera parameters
        self.pr_bn = nn.BatchNorm1d(3)

    def forward(self, x):
        x = self.features(x)
        # x = x.mean(3).mean(2)
        # x = self.classifier(x)
        # return x
        x = x.view(-1, 256*10*10)
        x = F.elu(self.fc_bn(self.fc(x)))
        return F.log_softmax(self.ds_bn(self.ds_fc(x)), dim=1), self.pr_bn(self.pr_fc(x))

    def _initialize_weights(self):
        for m in self.modules():
            if isinstance(m, nn.Conv2d):
                n = m.kernel_size[0] * m.kernel_size[1] * m.out_channels
                m.weight.data.normal_(0, math.sqrt(2. / n))
                if m.bias is not None:
                    m.bias.data.zero_()
            elif isinstance(m, nn.BatchNorm2d):
                m.weight.data.fill_(1)
                m.bias.data.zero_()
            elif isinstance(m, nn.Linear):
                n = m.weight.size(1)
                m.weight.data.normal_(0, 0.01)
                m.bias.data.zero_()