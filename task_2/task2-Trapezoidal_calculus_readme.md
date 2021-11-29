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





### the result

```
$ mpirun -np 6 python task2-Trapezoidal_calculus.py -func x^2 -s 0 -end 10 -n 100
num is 0 ,0 processes running,send to process 1
num is 1 ,1 processes running,send to process 2
num is 2 ,2 processes running,send to process 3
num is 3 ,3 processes running,send to process 4
num is 4 ,4 processes running,send to process 5
get process 5's data ,the data is 162.596431724727,total porcess number is 5 
get process 4's data ,the data is 98.6120414659381,total porcess number is 4 
get process 2's data ,the data is 18.6432609483600,total porcess number is 3 
get process 3's data ,the data is 50.6276512071490,total porcess number is 2 
get process 1's data ,the data is 2.65887068957090,total porcess number is 1 
The area of the final trapezoid is 333.138256035745


```



