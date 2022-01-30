# NTHU-Parallel-Programming

This is the course project of NTHU parallel programming in 2017-2018.

## HW1: Odd-Even Sort

This assignment helps you get familiar with MPI by implementing odd-even sort.

testcases为测试数据文件夹:

   - *.config: 配置文件，其中n为待排序个数，其它参数含义未知。
   - *.in: 待排序数据。
   - *.out: 排序完成数据，个人程序输出要与.out数据一致。

### Install open-mpi

1. download open-mpi: https://www.open-mpi.org/software/ompi/v4.1/

2. build and install: https://www.open-mpi.org/faq/?category=building#easy-build

   ```shell
   gunzip -c openmpi-4.1.2.tar.gz | tar xf -
   cd openmpi-4.1.2
   ./configure --prefix=/usr/local
   # <...lots of output...>
   make all install
   ```

3. Doc: https://www.open-mpi.org/doc/current/
   
   ```shell 
   mpic++ main.cpp
   mpirun a.out arg1 arg2 ...
   ```

### Quick Start

```shell
# compile
mpic++ -o hw1 basic.cpp
# run, 4为待排序的个数，见testcases/01.config文件
mpirun hw1 4 testcases/01.in out
# view
./b2float out
```