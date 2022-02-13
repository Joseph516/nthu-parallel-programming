# NTHU-Parallel-Programming

This is the course project of NTHU parallel programming in 2017-2018.

## HW1: Odd-Even Sort

This assignment helps you get familiar with MPI by implementing odd-even sort.

testcases 为测试数据文件夹:

- \*.config: 配置文件，其中 n 为待排序个数，其它参数含义未知。
- \*.in: 待排序数据。
- \*.out: 排序完成数据，个人程序输出要与.out 数据一致。

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

## HW2-Mandelbrot Set

static: 静态分配，根据总的点数（num_points_x * num_points_y），平均分配给每个processor的core。

dynamic：初次分配+worker根据自身完成进度向master申请，增加通信成本。

### Open MPI vs Open MP

1. 纯Open MP实现，为并发而非并行，多线程实现。

2. 纯Open MPI则是利用多核并行完成。

### Cmake for Open MPI and Open MP

```cmake
cmake_minimum_required(VERSION 3.0)
project(mandelbort-set)

find_package(MPI REQUIRED)
find_package(OpenMP REQUIRED)

set(MPI_COMPILE_FLAGS "-O3 -march=native -Wall -std=c++17")

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/build/bin") # 可执行文件输出目录

include_directories(${MPI_INCLUDE_PATH} "${PROJECT_SOURCE_DIR}/include")
AUX_SOURCE_DIRECTORY(src DIR_SRCS) # 添加源代码文件夹, 自动扫描所有文件
add_executable(hw2 ${DIR_SRCS})
target_link_libraries(hw2 PUBLIC OpenMP::OpenMP_CXX ${MPI_LIBRARIES})

if(MPI_COMPILE_FLAGS)
  set_target_properties(hw2 PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

if(MPI_LINK_FLAGS)
  set_target_properties(hw2 PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()

```

### Quick Start

```shell
# compile
cd build && cmake .. && make
# run
mpirun -n 2 ./bin/hw2 4 0 2 -2 2 800 400 output_file
# show
python ../show.py
```
