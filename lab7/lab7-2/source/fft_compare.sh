#!/bin/bash

# 编译 fft_compare.cpp
echo "正在编译 fft_compare.cpp ..."
mpic++ -o fft_compare fft_compare.cpp -lm

# 检查编译是否成功
if [ $? -ne 0 ]; then
  echo "编译失败，请检查代码。"
  exit 1
fi

# 运行 mpi 程序
echo "运行 mpi 程序..."

echo "进行 1 线程实验"
mpirun --allow-run-as-root -np 1 ./fft_compare
mpirun --allow-run-as-root -np 1 ./fft_compare
mpirun --allow-run-as-root -np 1 ./fft_compare
mpirun --allow-run-as-root -np 1 ./fft_compare
mpirun --allow-run-as-root -np 1 ./fft_compare
mpirun --allow-run-as-root -np 1 ./fft_compare
mpirun --allow-run-as-root -np 1 ./fft_compare
mpirun --allow-run-as-root -np 1 ./fft_compare
mpirun --allow-run-as-root -np 1 ./fft_compare
mpirun --allow-run-as-root -np 1 ./fft_compare

echo "进行 2 线程实验"
mpirun --allow-run-as-root -np 2 ./fft_compare
mpirun --allow-run-as-root -np 2 ./fft_compare
mpirun --allow-run-as-root -np 2 ./fft_compare
mpirun --allow-run-as-root -np 2 ./fft_compare
mpirun --allow-run-as-root -np 2 ./fft_compare
mpirun --allow-run-as-root -np 2 ./fft_compare
mpirun --allow-run-as-root -np 2 ./fft_compare
mpirun --allow-run-as-root -np 2 ./fft_compare
mpirun --allow-run-as-root -np 2 ./fft_compare
mpirun --allow-run-as-root -np 2 ./fft_compare

echo "进行 4 线程实验"
mpirun --allow-run-as-root -np 4 ./fft_compare
mpirun --allow-run-as-root -np 4 ./fft_compare
mpirun --allow-run-as-root -np 4 ./fft_compare
mpirun --allow-run-as-root -np 4 ./fft_compare
mpirun --allow-run-as-root -np 4 ./fft_compare
mpirun --allow-run-as-root -np 4 ./fft_compare
mpirun --allow-run-as-root -np 4 ./fft_compare
mpirun --allow-run-as-root -np 4 ./fft_compare
mpirun --allow-run-as-root -np 4 ./fft_compare
mpirun --allow-run-as-root -np 4 ./fft_compare

echo "进行 8 线程实验"
mpirun --allow-run-as-root -np 8 ./fft_compare
mpirun --allow-run-as-root -np 8 ./fft_compare
mpirun --allow-run-as-root -np 8 ./fft_compare
mpirun --allow-run-as-root -np 8 ./fft_compare
mpirun --allow-run-as-root -np 8 ./fft_compare
mpirun --allow-run-as-root -np 8 ./fft_compare
mpirun --allow-run-as-root -np 8 ./fft_compare
mpirun --allow-run-as-root -np 8 ./fft_compare
mpirun --allow-run-as-root -np 8 ./fft_compare
mpirun --allow-run-as-root -np 8 ./fft_compare

echo "进行 16 线程实验"
mpirun --allow-run-as-root --oversubscribe -np 16 ./fft_compare
mpirun --allow-run-as-root --oversubscribe -np 16 ./fft_compare
mpirun --allow-run-as-root --oversubscribe -np 16 ./fft_compare
mpirun --allow-run-as-root --oversubscribe -np 16 ./fft_compare
mpirun --allow-run-as-root --oversubscribe -np 16 ./fft_compare
mpirun --allow-run-as-root --oversubscribe -np 16 ./fft_compare
mpirun --allow-run-as-root --oversubscribe -np 16 ./fft_compare
mpirun --allow-run-as-root --oversubscribe -np 16 ./fft_compare
mpirun --allow-run-as-root --oversubscribe -np 16 ./fft_compare
mpirun --allow-run-as-root --oversubscribe -np 16 ./fft_compare

# 检查运行是否成功
if [ $? -ne 0 ]; then
  echo "运行失败，请检查 MPI 设置或程序错误。"
  exit 1
fi

echo "任务完成！"
