# task2 Trapezoidal calculus

## Principle

![{\displaystyle \int _{a}^{b}f(x)\,dx\approx {\frac {b-a}{2N}}\left(f(x_{0})+2f(x_{1})+2f(x_{2})+\cdots +2f(x_{N-1})+f(x_{N})\right)}](https://wikimedia.org/api/rest_v1/media/math/render/svg/cf8c9470069bd9163d532f6ac4a620cfde3604db)

![img](https://upload.wikimedia.org/wikipedia/commons/thumb/0/05/Trapezoidal_rule_illustration_small.svg/220px-Trapezoidal_rule_illustration_small.svg.png)

if n as number of regions
$$
h=(b-a)/n
$$

$$
x_0=a,x_1=a+h,x_2=a+2h...x_(n-1)=a+(n-1)h,x_n=b
$$
$$
h*[f(x_0)/2+f(x_1)+f(x_2)+...+f(x_{n-1})+f(x_n)/2]
$$

## compute every node need compute postation
```
h=(b-a)/n
sub_process_n=n/commsize
sub_process_a=a+rank*sub_process_n*h
sub_process_b=sub_process_a+sub_process_n*h
```

## main Code

```python
if comm_rank != 0 :
    comm.send(result,dest=0)
else:

    all_result = result
    for i in range(1,comm_size):

        data = comm.recv(source=i)
        print(data)
        all_result+=data
print(all_result)
```

Use 0 porcess as main processes then use ohter process compute data for the respective acquired positions



### the result

```
$ mpirun -np 6 python task2-Trapezoidal_calculus.py -func x^2 -s 0 -end 10 -n 1024
process 0 compute result 1.49872857663367
process 1 computer result 10.6501873168680
process 2 computer result 29.0247363348802
process 3 computer result 56.6223756306701
process 4 computer result 93.4431052042378
process 5 computer result 139.486925055583
330.726058118873


```



## TODO

Bcast with reduce better fits the master-slave model

```
# comm.Bcast([a, MPI.FLOAT], root=0)
# comm.Bcast([b, MPI.FLOAT], root=0)
# comm.Bcast([n, MPI.INT], root=0)
then compute result
use reduce sum result

```

