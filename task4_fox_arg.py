#!/usr/bin/env python
# -*- coding: utf-8 -*-
'''
@author: soliva
@Site: 
@file: task4_fox_arg.py
@time: 2021/2/5
@desc:
'''
from mpi4py import MPI
import numpy as np
n=1e3
a = np.array([[1, 2], [3, 4], [5, 6]])
b = np.array([[1, 2, 3], [4, 5, 6]])
c = b.transpose()
print(a, b)

comm = MPI.COMM_WORLD
rank = comm.Get_rank()
size = comm.Get_size()

sqrtsize = np.sqrt(size)
if __name__ == '__main__':

    if (sqrtsize*sqrtsize != size) or (n % sqrtsize):
        if rank ==0:
            if n%sqrtsize==0:
                print("porssess should be Complete square number")
            else:
                print("sqrtsize should Integral division matrix size n")
    block = n/sqrtsize
    blocknum = block * block
    row = rank/sqrtsize
    col = rank%sqrtsize
    if rank == 0:
        pass #TODO 初始化矩阵 and 计算分块矩阵 并将其发给其余的处理器
    else:
        blockA = comm.recv(source = 0)
        blockB = comm.recv(source = 0)
    #TODO 计算过程
    if rank == 0:
        
        print("matrixA:",matrixA)
        print("matrixB:",matrixB)
        print("matrixC:",matrixC)
    else:
        comm.send(blockC,dest=0)