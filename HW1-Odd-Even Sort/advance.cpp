#include <mpi.h>
#include <stdlib.h>

#include <iostream>

#define TAIL_SEND 0
#define HEAD_SEND 1

// Odd-Even Sort数字类型
typedef float ValueType;



int main(int argc, char* argv[]) {
  int rank, num_processors;
  int rc;
  MPI_File fh_in, fh_out;
  MPI_Offset offset;
  MPI_Comm custom_world = MPI_COMM_WORLD;
  MPI_Group origin_group, new_group;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &num_processors);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // 查看物理CPU的core总数
  // std::cout << "number of processor: " << num_processors << std::endl;

  const int kN = atoi(argv[1]);
  const char* kInputFilename = argv[2];
  const char* kOutputFilename = argv[3];

  // Produces a group by excluding ranges of processes from an existing group
  if (kN < num_processors) {
    // obtain the group of proc. in the world communicator
    MPI_Comm_group(custom_world, &origin_group);
    // remove unwanted ranks，将MPI_COMM_WORLD中多余的rank排除
    int ranges[][3] = {{kN, num_processors - 1, 1}};
    MPI_Group_range_excl(origin_group, 1, ranges, &new_group);
    // create a new communicator
    MPI_Comm_create(custom_world, new_group, &custom_world);
    if (custom_world == MPI_COMM_NULL) {
      // terminate those unwanted processes
      MPI_Finalize();
      exit(0);
    }
    num_processors = kN;
  }

  // Declare parameter
  int num_per_rank = kN / num_processors;
  int head = rank * num_per_rank;      // index number
  int tail = head + num_per_rank - 1;  // index number
  offset = rank * num_per_rank *
           sizeof(ValueType);  // assign un-allocate remainder offset(It is very
                               // important for the last processor)

  if (rank == (num_processors - 1)) {
    // the last rank assigned the remainder (N % num_processors)
    num_per_rank += (kN % num_processors);
    tail = kN - 1;
  }

  ValueType* local_buf = new ValueType[num_per_rank];

  // Read file using MPI-IO
  rc = MPI_File_open(custom_world, kInputFilename, MPI_MODE_RDONLY,
                     MPI_INFO_NULL, &fh_in);
  // Detection file open state
  if (rc != MPI_SUCCESS) {
    std::cout << "File open failed!!\n";
    MPI_Abort(custom_world, rc);
  }
  // 从fh_in中读取数据至local_buf
  MPI_File_read_at(fh_in, offset, local_buf, num_per_rank, MPI_FLOAT,
                   MPI_STATUS_IGNORE);
  MPI_File_close(&fh_in);
  std::cout << "rank:" << rank << " head:" << head << " tail:" << tail
            << " offeset:" << offset << " head value:" << local_buf[0]
            << std::endl;

  // TODO: Advance sort
  // 思路：先把每一个rank中的数据sort，然后与周围的rank中的数据merge排序
  
  // Write file using MPI-IO
  MPI_File_open(custom_world, kOutputFilename,
                MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &fh_out);
  MPI_File_write_at(fh_out, offset, local_buf, num_per_rank, MPI_FLOAT,
                    MPI_STATUS_IGNORE);
  MPI_File_close(&fh_out);

  // free unused buffer
  delete[] local_buf;

  MPI_Barrier(custom_world);
  MPI_Finalize();

  return 0;
}
