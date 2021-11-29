#!/usr/bin/env python
# -*- coding: utf-8 -*-
'''
@author: soliva
@Site: 
@file: task1-message_collect.py
@time: 2020/12/30
@desc:
'''
import os, sys, time
import numpy as np
import mpi4py.MPI as MPI

#create some data
all_data = [[ 0 , 1  ,2  ,3  ,4],
             [ 5 , 6  ,7 , 8 , 9],
             [10, 11 ,12, 13,14],
             [15, 16 ,17 ,18 ,19,1]]



comm = MPI.COMM_WORLD
comm_rank = comm.Get_rank()
comm_size = comm.Get_size()


#send data to Sub-processes
if comm_size<len(all_data):
    data = all_data[comm_rank]
else:
    if comm_rank>=len(all_data):
        data = None
    else:
        data = all_data[comm_rank]
#use gather get sub-processes data
data_main = comm.gather(data, root=0)
if __name__ == "__main__":


    if comm_rank==0:
        #create a matrix for receiving data range by rank id

        print("rank:{} Receiving {}".format(comm_rank, data))
        print("input:\n",all_data)
        new_data = list(filter(None,data_main))
        new = np.matrix(new_data)
        print("output:\n",new)

    else:
        print("rank:{} Receiving {}".format(comm_rank,data))