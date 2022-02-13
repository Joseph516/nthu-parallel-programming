#include <mpi.h>
#include <omp.h>

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "utils.h"

// Use (void) to silent unused warnings.
#define assertm(exp, msg) assert(((void)msg, exp))

// MPI tag
#define MPI_TAG_READY 0
#define MPI_TAG_TASK 1
#define MPI_TAG_TERMINATE 2
#define MPI_TAG_COMPLETE 3

int main(int argc, char* argv[]) {
  // get input parameters
  assertm(argc == 9, "Incorrect input number of parameters.");
  int num_threads = std::atoi(argv[1]);           // 1~12
  double left_range_real = std::stod(argv[2]);    // -10~10
  double right_range_real = std::stod(argv[3]);   // -10~10
  double lower_range_image = std::stod(argv[4]);  // -10~10
  double upper_range_image = std::stod(argv[5]);  // -10~10
  int num_points_x = std::atoi(argv[6]);          // 200~4000
  int num_points_y = std::atoi(argv[7]);          // 200~4000
  std::string output_filename = argv[8];

  // Initialize mpi
  int rank, num_processors;
  MPI_Comm custom_world = MPI_COMM_WORLD;
  // MPI_Group origin_group, new_group;
  MPI_Status status;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &num_processors);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (num_processors > num_points_x) {
    // todo
  }

  double x_scale = (right_range_real - left_range_real) / num_points_x;
  double y_scale = (upper_range_image - lower_range_image) / num_points_y;

  if (num_processors == 1) {
    // 单核情况
    std::vector<std::vector<int>> mand_set(num_points_x,
                                           std::vector<int>(num_points_y, 0));
    for (int j = 0; j < num_points_y; j++) {
      for (int i = 0; i < num_points_x; i++) {
        // from left to right and top to bottom
        double r = left_range_real + i * x_scale;
        double im = lower_range_image + j * y_scale;
        mand_set[i][j] = calPixel(Complex(r, im));
      }
    }
    twoDVecToFile(mand_set, output_filename);
  } else {
    // 多核情况
    int* local_buf = new int[num_points_y + 1];  // 0为task_pos

    if (rank == 0) {
      // rank0作为master分配任务, 将计算任务沿x轴划分
      int num_tasks = num_points_x;
      int active_precessor = 0;
      std::vector<std::vector<int>> mand_set(num_points_x,
                                             std::vector<int>(num_points_y, 0));

      while (active_precessor != 0 || num_tasks != 0) {
        // 接收worker任务申请，注意这里local_buf前面不要加&！！！
        MPI_Recv(local_buf, num_points_y + 1, MPI_INT, MPI_ANY_SOURCE,
                 MPI_ANY_TAG, custom_world, &status);
        int worker_rank = status.MPI_SOURCE;

        if (status.MPI_TAG == MPI_TAG_COMPLETE) {
          active_precessor--;
          // 将worker的结果汇总
          for (int j = 0; j < num_points_y; j++) {
            mand_set[local_buf[0]][j] = local_buf[j + 1];
          }
        }

        int task_pos_send;
        if (num_tasks != 0) {
          // 分配任务
          task_pos_send = num_tasks - 1;  // 位置序号从0开始
          MPI_Send(&task_pos_send, 1, MPI_INT, worker_rank, MPI_TAG_TASK,
                   custom_world);
          num_tasks--;
          active_precessor++;
        } else {
          MPI_Send(&task_pos_send, 1, MPI_INT, worker_rank, MPI_TAG_TERMINATE,
                   custom_world);
        }
      }
      std::cout << "Done master\n";

      twoDVecToFile(mand_set, output_filename);
    } else {
      // worker
      // 申请任务
      MPI_Send(local_buf, num_points_y + 1, MPI_INT, 0, MPI_TAG_READY,
               custom_world);
      int task_pos_get;
      // 循环等待接收任务
      while (MPI_Recv(&task_pos_get, 1, MPI_INT, 0, MPI_ANY_TAG, custom_world,
                      &status) == MPI_SUCCESS) {
        if (status.MPI_TAG == MPI_TAG_TERMINATE) {
          break;
        }
        // 执行任务
        double r = left_range_real + task_pos_get * x_scale;
        local_buf[0] = task_pos_get;
        for (int j = 0; j < num_points_y; j++) {
          double im = lower_range_image + j * y_scale;
          local_buf[j + 1] = calPixel(Complex(r, im));
        }
        // 发送任务结果
        MPI_Send(local_buf, num_points_y + 1, MPI_INT, 0, MPI_TAG_COMPLETE,
                 custom_world);
      }
      std::cout << "Done woker\n";
    }
    delete[] local_buf;
  }

  MPI_Barrier(custom_world);
  MPI_Finalize();
  return 0;
}