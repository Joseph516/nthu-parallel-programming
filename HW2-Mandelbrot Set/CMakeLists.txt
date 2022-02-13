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
