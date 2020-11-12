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
#-*-coding:utf-8-*-
import torch
import torchvision
from torch.autograd import Variable
from tensorboardX import SummaryWriter

import torch
from torch.autograd import Variable

from graphviz import Digraph

def make_dot(var, params=None):
    """
    画出 PyTorch 自动梯度图 autograd graph 的 Graphviz 表示.
    蓝色节点表示有梯度计算的变量Variables;
    橙色节点表示用于 torch.autograd.Function 中的 backward 的张量 Tensors.

    Args:
        var: output Variable
        params: dict of (name, Variable) to add names to node that
            require grad (TODO: make optional)
    """
    if params is not None:
        assert all(isinstance(p, Variable) for p in params.values())
        param_map = {id(v): k for k, v in params.items()}

    node_attr = dict(style='filled', shape='box', align='left',
                              fontsize='12', ranksep='0.1', height='0.2')
    dot = Digraph(node_attr=node_attr, graph_attr=dict(size="12,12"))
    seen = set()

    def size_to_str(size):
        return '(' + (', ').join(['%d' % v for v in size]) + ')'

    output_nodes = (var.grad_fn,) if not isinstance(var, tuple) else tuple(v.grad_fn for v in var)

    def add_nodes(var):
        if var not in seen:
            if torch.is_tensor(var):
                # note: this used to show .saved_tensors in pytorch0.2, but stopped
                # working as it was moved to ATen and Variable-Tensor merged
                dot.node(str(id(var)), size_to_str(var.size()), fillcolor='orange')
            elif hasattr(var, 'variable'):
                u = var.variable
                name = param_map[id(u)] if params is not None else ''
                node_name = '%s\n %s' % (name, size_to_str(u.size()))
                dot.node(str(id(var)), node_name, fillcolor='lightblue')
            elif var in output_nodes:
                dot.node(str(id(var)), str(type(var).__name__), fillcolor='darkolivegreen1')
            else:
                dot.node(str(id(var)), str(type(var).__name__))
            seen.add(var)
            if hasattr(var, 'next_functions'):
                for u in var.next_functions:
                    if u[0] is not None:
                        dot.edge(str(id(u[0])), str(id(var)))
                        add_nodes(u[0])
            if hasattr(var, 'saved_tensors'):
                for t in var.saved_tensors:
                    dot.edge(str(id(t)), str(id(var)))
                    add_nodes(t)

    # 多输出场景 multiple outputs
    if isinstance(var, tuple):
        for v in var:
            add_nodes(v.grad_fn)
    else:
        add_nodes(var.grad_fn)
    return dot

# 模拟输入数据
input_data = Variable(torch.rand(16, 3, 320, 240))

# 从torchvision中导入已有模型
net = MobileNetV2()

# # 声明writer对象，保存的文件夹，异己名称
# writer = SummaryWriter(log_dir='./log', comment='resnet18')
# with writer:
#     writer.add_graph(net, (input_data,))

x = torch.randn(16, 3, 320, 240).requires_grad_(True)
y = net(x)
vis_graph = make_dot(y, params=dict(list(net.named_parameters()) + [('x', x)]))
vis_graph.view()