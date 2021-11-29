#include <iostream>
#include <cstdio>
#include <mpi.h>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <vector>
using namespace std;

const int n = 1e3;	//n是矩阵大小

int MatrixA[n][n], MatrixB[n][n], MatrixC[n][n];	//三个矩阵 已知A B 计算C=A*B
int block, blocknum;	//每个分块的大小(一行有多少元素) blocknum=block*block
int numprocs, sqrnumprocs;	//前者为处理器的个数 后者为其根号
int move_size;	//=blocknum*sizeof(int) 用于memcpy memset等函数

int* blockA, * blockB, * blockC, * tmpa, * tmpb;	//存储 分块矩阵 以及传输数据所需要的缓冲区
int myid, row, col;	//处理器ID 把整个矩阵划分成若干个分块矩阵分给其它处理器 则该处理器处理第row行 第clo列的分块矩阵

inline void init_AB()//初始化矩阵 A B
{
	srand(time(0));
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			MatrixA[i][j] = rand() % 10;
			MatrixB[i][j] = rand() % 10;
		}
	}
}

inline void send_block_AB()
{
	int rowmin, rowmax, colmin, colmax;	//记录分块矩阵的范围
	for (int i = 0; i < numprocs; i++)
	{
		rowmin = (i / sqrnumprocs) * block;
		rowmax = rowmin + block;
		colmin = (i % sqrnumprocs) * block;
		colmax = colmin + block;
		for (int j = rowmin; j < rowmax; j++)
		{
			for (int k = colmin; k < colmax; k++)
			{
				int idx = (j - rowmin) * block + k - colmin;	//由于tmp是一维数组 所以要计算当前元素对应的下标
				tmpa[idx] = MatrixA[j][k];
				tmpb[idx] = MatrixB[j][k];
			}
		}
		if (!i)	//0号处理器
		{
			memcpy(blockA, tmpa, move_size);
			memcpy(blockB, tmpb, move_size);
		}
		else
		{		//发送分块矩阵 A B
			MPI_Send(tmpa, blocknum, MPI_INT, i, 1, MPI_COMM_WORLD);
			MPI_Send(tmpb, blocknum, MPI_INT, i, 2, MPI_COMM_WORLD);
		}
	}
}

inline int getidx(int row, int col)	//通过 分块矩阵的 行row 列col 得到管理它的 处理器ID
{
	//row=id/sqrnumprocs col=id%sqrnumprocs
	return ((row + sqrnumprocs) % sqrnumprocs) * sqrnumprocs + (col + sqrnumprocs) % sqrnumprocs;
}

inline void cal()	//计算过程
{
	MPI_Status s;
	int send_col_idx = row;	//在分块矩阵的视图上看 初始时 需要发送分块矩阵(row,send_col_idx)
	int idxmin, idxmax;	//记录 需要接收分块的 处理器的id范围
	for (int times = 0; times < sqrnumprocs; times++)	//sqrnumprocs次 乘法和累加
	{
		//该处理器处理的分块的坐标为 (row,col)
		//所以需要从 处理器 getidx(row,send_idx[row]) 处得到分块A 然后进行乘法累加
		if (col == send_col_idx)
		{
			idxmin = getidx(row, 0);
			idxmax = getidx(row, sqrnumprocs - 1);
			for (int i = idxmin; i <= idxmax; i++)
			{
				if (i == myid)	//自己就没必要发送了
					continue;
				MPI_Send(blockA, blocknum, MPI_INT, i, 1, MPI_COMM_WORLD);//发送
			}
			memcpy(tmpa, blockA, move_size);	//直接拷贝到目标位置
		}
		else	//接收分块
		{
			MPI_Recv(tmpa, blocknum, MPI_INT, getidx(row, send_col_idx), 1, MPI_COMM_WORLD, &s);
		}
		send_col_idx = (send_col_idx + 1) % sqrnumprocs;	//递增列号

		for (int i = 0; i < block; i++)	//c[i][j]=a[i][k]*b[k][j]
		{								//c[i][j]=blockC[i * block + j]
			for (int j = 0; j < block; j++)
			{
				int sum = blockC[i * block + j];
				for (int k = 0; k < block; k++)
					sum += tmpa[i * block + k] * blockB[k * block + j];
				blockC[i * block + j] = sum;
			}
		}	//每个分块计算完毕后
		//A中分块保持不动 B中分块上移1步
		MPI_Sendrecv(blockB, blocknum, MPI_INT, getidx(row - 1, col), 2, tmpb, blocknum, MPI_INT, getidx(row + 1, col), 2, MPI_COMM_WORLD, &s);
		//拷贝
		memcpy(blockB, tmpb, move_size);
	}
}

inline void getans()	//处理器0 从其余处理器处得到分块矩阵的结果并合并
{
	MPI_Status s;
	int rowmin, rowmax, colmin, colmax;
	//处理器0 可直接得到
	for (int i = 0; i < block; i++)
		for (int j = 0; j < block; j++)
			MatrixC[i][j] = blockC[i * block + j];
	//其余的需要 接收
	for (int i = 1; i < numprocs; i++)
	{
		MPI_Recv(blockC, blocknum, MPI_INT, i, 1, MPI_COMM_WORLD, &s);
		rowmin = (i / sqrnumprocs) * block;	//首行坐标
		rowmax = rowmin + block;	//最后一行的坐标
		colmin = (i % sqrnumprocs) * block;	//首列坐标
		colmax = colmin + block;	//最后一列的坐标
		for (int j = rowmin; j < rowmax; j++)
			for (int k = colmin; k < colmax; k++)
				MatrixC[j][k] = blockC[(j - rowmin) * block + k - colmin];
	}
}

inline void print_matrix(int ans[][n])	//输出矩阵
{
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
			printf("%-5d", ans[i][j]);
		printf("\n");
	}
	printf("\n");
}

int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);	//个数
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);		//ID

	clock_t start = clock();	//开始时间

	sqrnumprocs = sqrt(numprocs);
	if (sqrnumprocs * sqrnumprocs != numprocs || n % sqrnumprocs)
	{
		if (myid == 0)
		{
			if (n % sqrnumprocs == 0)
				cout << "处理器个数应该为完全平方数！\n";
			else
				cout << "sqrnumprocs必须整除矩阵大小n!\n";
		}
		MPI_Finalize();
		return 0;
	}
	block = n/sqrnumprocs;	//分块大小
	blocknum = block * block;	//每个分块的元素总数
	move_size = blocknum * sizeof(int);
	row = myid / sqrnumprocs;		//计算自己处理的分块矩阵的 坐标
	col = myid % sqrnumprocs;
	blockA = new int[blocknum];	//分配空间
	blockB = new int[blocknum];
	blockC = new int[blocknum];
	tmpa = new int[blocknum];
	tmpb = new int[blocknum];
	memset(blockC, 0, move_size);	//初始化c
	if (!myid)	//0号处理器
	{
		init_AB();	//初始化矩阵A B
		send_block_AB();	//计算分块矩阵 并将其发送给其余处理器
	}
	else
	{	//接受0号发过来的 分块矩阵
		MPI_Status s;
		MPI_Recv(blockA, blocknum, MPI_INT, 0, 1, MPI_COMM_WORLD, &s);
		MPI_Recv(blockB, blocknum, MPI_INT, 0, 2, MPI_COMM_WORLD, &s);
	}
	cal();	//计算过程
	if (myid == 0)
	{
		getans();
		cout << "矩阵A为:\n";
		print_matrix(MatrixA);
		cout << "矩阵B为:\n";
		print_matrix(MatrixB);
		cout << "矩阵C=A*B为:\n";
		print_matrix(MatrixC);
		clock_t end = clock();	//结束时间
		cout << "Fox乘法耗时: " << end - start << "\n";
	}
	else
	{
		MPI_Send(blockC, blocknum, MPI_INT, 0, 1, MPI_COMM_WORLD);
	}

	delete[] blockA;
	delete[] blockB;
	delete[] blockC;
	delete[] tmpa;
	delete[] tmpb;
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
	return 0;}