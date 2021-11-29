#!/usr/bin/env python
# -*- coding: utf-8 -*-
'''
@author: soliva
@Site: 
@file: task2-Trapezoidal_calculus.py
@time: 2021/1/1
@desc:
'''
import numpy as np
from mpi4py import MPI
import sys
import argparse
from sympy  import sympify

# input star  end   Number of regions and function
parser = argparse.ArgumentParser(description='IGV snapshot automator')
parser.add_argument("-s",type=float, dest='start',help="Starting position of the function")
parser.add_argument("-end",type=float, dest='end',help="Ending position of the function")
parser.add_argument("-n",type=int, dest='n',help="Number of regions")
parser.add_argument("-func",type=str,dest="func",help="function")
args = parser.parse_args()
def f(formula=args.func,**kwargs):
    ''' Convert the input formula string to a code'''
    # print(formula)
    expr = sympify(formula)
    return expr.evalf(subs=kwargs)
def integrate(start,end,n):
    '''
    h = （b-a)/n
    h*[f(x_0)/2+f(x_1)+f(x_2)+...+f(x_{n-1})+f(x_n)/2]

    :param start:Value of the starting x of the function
    :param end:Value of the ending x of the function
    :param n:Number of regions
    :return:
    '''
    h=(end-start)/n
    integral = (f(x=start) + f(x=end))/2.0
    for i in range(0,int(n-1)):
        x1=start+i*h
        integral += f(x=x1)
    integral = integral * h
    return integral




comm = MPI.COMM_WORLD
comm_rank = comm.Get_rank()
comm_size = comm.Get_size()
a = args.start
b = args.end
n = args.n

all_result=0
## Compute the start and end of each porcess's  and h

h=(b-a)/n
sub_process_n=n/comm_size
sub_process_a=a+comm_rank*sub_process_n*h
sub_process_b=sub_process_a+sub_process_n*h
result = integrate(sub_process_a,sub_process_b,sub_process_n)


## use point to point mothed compute
if comm_rank != 0 :
    comm.send(result,dest=0)
else:

    all_result = result
    print("process {} compute result {}".format(0,result))
    for i in range(1,comm_size):

        data = comm.recv(source=i)

        all_result+=data
        print("process {} computer result {}".format(i,data))
    print(all_result)



