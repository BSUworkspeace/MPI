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
parser = argparse.ArgumentParser()
parser.add_argument("-s",type=float, dest='start',default=0,help="Starting position of the function")
parser.add_argument("-end",type=float, dest='end',default=10,help="Ending position of the function")
parser.add_argument("-n",type=int, dest='n',default=1024,help="Number of regions")
parser.add_argument("-func",type=str,dest="func",default="x^2",help="function")
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
def sub_intergrete(num):
    '''
    compute each number area
    :param num: input compute number
    :return: computer result
    '''
    h=(b-a)/n
    sub_process_a=a+num*h
    sub_process_b=sub_process_a+h
    result = integrate(sub_process_a,sub_process_b,1024)
    return result


if __name__ == '__main__':

    data_tag=0
    terminate_tag = 2
    if comm_rank ==0:
        count =0
        num = 0
        for i in range(1,comm_size):#Send h number to each process
            comm.send(num,dest= i,tag=data_tag )
            print("num is {} ,{} processes running,send to process {}".format(num,count,i))
            count +=1
            num +=1
        while count >0:
            s = MPI.Status()
            comm.Probe(status=s)
            data = comm.recv(source = s.source)#Receive messages returned by each process
            print("get process {}'s data ,the data is {},total porcess number is {} ".format(s.source,data,count,num))
            all_result+=data
            count = count - 1
            if num < n :#if the process task has not finished running, num<n send new task to the completed process
                comm.send(num,dest = s.source,tag=data_tag)
                count += 1
                num += 1
            else:#if fales send Terminate tag to child process
                comm.send(num,dest = s.source,tag=terminate_tag)
        print("The area of the final trapezoid is",all_result)
    else:
        sub_s = MPI.Status()
        comm.Probe(status=sub_s)#get num form 0 process
        num = comm.recv(source=0)

        while sub_s.tag == data_tag:
            result = sub_intergrete(num)# compute this area result
            comm.send(result,dest=0) #send result to 0 proc
            comm.Probe(status=sub_s) #probe message then update tag number if tag number == 2 break while loop
            num = comm.recv(source=0,tag=sub_s.tag)




