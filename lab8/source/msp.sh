#!/bin/bash

# 编译 msp_openmp.cpp
echo "正在编译 msp_openmp.cpp ..."
g++ -fopenmp -O2 msp_openmp.cpp -o msp

# 检查编译是否成功
if [ $? -ne 0 ]; then
  echo "编译失败，请检查代码。"
  exit 1
fi

# 运行 omp 程序
echo "运行 omp 程序..."

echo "进行 1 线程实验"
./msp updated_mouse.csv updated_flower.csv 1
./msp updated_mouse.csv updated_flower.csv 1
./msp updated_mouse.csv updated_flower.csv 1
./msp updated_mouse.csv updated_flower.csv 1
./msp updated_mouse.csv updated_flower.csv 1
./msp updated_mouse.csv updated_flower.csv 1
./msp updated_mouse.csv updated_flower.csv 1
./msp updated_mouse.csv updated_flower.csv 1
./msp updated_mouse.csv updated_flower.csv 1
./msp updated_mouse.csv updated_flower.csv 1

echo "进行 2 线程实验"
./msp updated_mouse.csv updated_flower.csv 2
./msp updated_mouse.csv updated_flower.csv 2
./msp updated_mouse.csv updated_flower.csv 2
./msp updated_mouse.csv updated_flower.csv 2
./msp updated_mouse.csv updated_flower.csv 2
./msp updated_mouse.csv updated_flower.csv 2
./msp updated_mouse.csv updated_flower.csv 2
./msp updated_mouse.csv updated_flower.csv 2
./msp updated_mouse.csv updated_flower.csv 2
./msp updated_mouse.csv updated_flower.csv 2

echo "进行 3 线程实验"
./msp updated_mouse.csv updated_flower.csv 3
./msp updated_mouse.csv updated_flower.csv 3
./msp updated_mouse.csv updated_flower.csv 3
./msp updated_mouse.csv updated_flower.csv 3
./msp updated_mouse.csv updated_flower.csv 3
./msp updated_mouse.csv updated_flower.csv 3
./msp updated_mouse.csv updated_flower.csv 3
./msp updated_mouse.csv updated_flower.csv 3
./msp updated_mouse.csv updated_flower.csv 3
./msp updated_mouse.csv updated_flower.csv 3

echo "进行 4 线程实验"
./msp updated_mouse.csv updated_flower.csv 4
./msp updated_mouse.csv updated_flower.csv 4
./msp updated_mouse.csv updated_flower.csv 4
./msp updated_mouse.csv updated_flower.csv 4
./msp updated_mouse.csv updated_flower.csv 4
./msp updated_mouse.csv updated_flower.csv 4
./msp updated_mouse.csv updated_flower.csv 4
./msp updated_mouse.csv updated_flower.csv 4
./msp updated_mouse.csv updated_flower.csv 4
./msp updated_mouse.csv updated_flower.csv 4

echo "进行 5 线程实验"
./msp updated_mouse.csv updated_flower.csv 5
./msp updated_mouse.csv updated_flower.csv 5
./msp updated_mouse.csv updated_flower.csv 5
./msp updated_mouse.csv updated_flower.csv 5
./msp updated_mouse.csv updated_flower.csv 5
./msp updated_mouse.csv updated_flower.csv 5
./msp updated_mouse.csv updated_flower.csv 5
./msp updated_mouse.csv updated_flower.csv 5
./msp updated_mouse.csv updated_flower.csv 5
./msp updated_mouse.csv updated_flower.csv 5

echo "进行 6 线程实验"
./msp updated_mouse.csv updated_flower.csv 6
./msp updated_mouse.csv updated_flower.csv 6
./msp updated_mouse.csv updated_flower.csv 6
./msp updated_mouse.csv updated_flower.csv 6
./msp updated_mouse.csv updated_flower.csv 6
./msp updated_mouse.csv updated_flower.csv 6
./msp updated_mouse.csv updated_flower.csv 6
./msp updated_mouse.csv updated_flower.csv 6
./msp updated_mouse.csv updated_flower.csv 6
./msp updated_mouse.csv updated_flower.csv 6

echo "进行 7 线程实验"
./msp updated_mouse.csv updated_flower.csv 7
./msp updated_mouse.csv updated_flower.csv 7
./msp updated_mouse.csv updated_flower.csv 7
./msp updated_mouse.csv updated_flower.csv 7
./msp updated_mouse.csv updated_flower.csv 7
./msp updated_mouse.csv updated_flower.csv 7
./msp updated_mouse.csv updated_flower.csv 7
./msp updated_mouse.csv updated_flower.csv 7
./msp updated_mouse.csv updated_flower.csv 7
./msp updated_mouse.csv updated_flower.csv 7

echo "进行 8 线程实验"
./msp updated_mouse.csv updated_flower.csv 8
./msp updated_mouse.csv updated_flower.csv 8
./msp updated_mouse.csv updated_flower.csv 8
./msp updated_mouse.csv updated_flower.csv 8
./msp updated_mouse.csv updated_flower.csv 8
./msp updated_mouse.csv updated_flower.csv 8
./msp updated_mouse.csv updated_flower.csv 8
./msp updated_mouse.csv updated_flower.csv 8
./msp updated_mouse.csv updated_flower.csv 8
./msp updated_mouse.csv updated_flower.csv 8

echo "进行 9 线程实验"
./msp updated_mouse.csv updated_flower.csv 9
./msp updated_mouse.csv updated_flower.csv 9
./msp updated_mouse.csv updated_flower.csv 9
./msp updated_mouse.csv updated_flower.csv 9
./msp updated_mouse.csv updated_flower.csv 9
./msp updated_mouse.csv updated_flower.csv 9
./msp updated_mouse.csv updated_flower.csv 9
./msp updated_mouse.csv updated_flower.csv 9
./msp updated_mouse.csv updated_flower.csv 9
./msp updated_mouse.csv updated_flower.csv 9

echo "进行 10 线程实验"
./msp updated_mouse.csv updated_flower.csv 10
./msp updated_mouse.csv updated_flower.csv 10
./msp updated_mouse.csv updated_flower.csv 10
./msp updated_mouse.csv updated_flower.csv 10
./msp updated_mouse.csv updated_flower.csv 10
./msp updated_mouse.csv updated_flower.csv 10
./msp updated_mouse.csv updated_flower.csv 10
./msp updated_mouse.csv updated_flower.csv 10
./msp updated_mouse.csv updated_flower.csv 10
./msp updated_mouse.csv updated_flower.csv 10

echo "进行 11 线程实验"
./msp updated_mouse.csv updated_flower.csv 11
./msp updated_mouse.csv updated_flower.csv 11
./msp updated_mouse.csv updated_flower.csv 11
./msp updated_mouse.csv updated_flower.csv 11
./msp updated_mouse.csv updated_flower.csv 11
./msp updated_mouse.csv updated_flower.csv 11
./msp updated_mouse.csv updated_flower.csv 11
./msp updated_mouse.csv updated_flower.csv 11
./msp updated_mouse.csv updated_flower.csv 11
./msp updated_mouse.csv updated_flower.csv 11

echo "进行 12 线程实验"
./msp updated_mouse.csv updated_flower.csv 12
./msp updated_mouse.csv updated_flower.csv 12
./msp updated_mouse.csv updated_flower.csv 12
./msp updated_mouse.csv updated_flower.csv 12
./msp updated_mouse.csv updated_flower.csv 12
./msp updated_mouse.csv updated_flower.csv 12
./msp updated_mouse.csv updated_flower.csv 12
./msp updated_mouse.csv updated_flower.csv 12
./msp updated_mouse.csv updated_flower.csv 12
./msp updated_mouse.csv updated_flower.csv 12

echo "进行 13 线程实验"
./msp updated_mouse.csv updated_flower.csv 13
./msp updated_mouse.csv updated_flower.csv 13
./msp updated_mouse.csv updated_flower.csv 13
./msp updated_mouse.csv updated_flower.csv 13
./msp updated_mouse.csv updated_flower.csv 13
./msp updated_mouse.csv updated_flower.csv 13
./msp updated_mouse.csv updated_flower.csv 13
./msp updated_mouse.csv updated_flower.csv 13
./msp updated_mouse.csv updated_flower.csv 13
./msp updated_mouse.csv updated_flower.csv 13

echo "进行 14 线程实验"
./msp updated_mouse.csv updated_flower.csv 14
./msp updated_mouse.csv updated_flower.csv 14
./msp updated_mouse.csv updated_flower.csv 14
./msp updated_mouse.csv updated_flower.csv 14
./msp updated_mouse.csv updated_flower.csv 14
./msp updated_mouse.csv updated_flower.csv 14
./msp updated_mouse.csv updated_flower.csv 14
./msp updated_mouse.csv updated_flower.csv 14
./msp updated_mouse.csv updated_flower.csv 14
./msp updated_mouse.csv updated_flower.csv 14

echo "进行 15 线程实验"
./msp updated_mouse.csv updated_flower.csv 15
./msp updated_mouse.csv updated_flower.csv 15
./msp updated_mouse.csv updated_flower.csv 15
./msp updated_mouse.csv updated_flower.csv 15
./msp updated_mouse.csv updated_flower.csv 15
./msp updated_mouse.csv updated_flower.csv 15
./msp updated_mouse.csv updated_flower.csv 15
./msp updated_mouse.csv updated_flower.csv 15
./msp updated_mouse.csv updated_flower.csv 15
./msp updated_mouse.csv updated_flower.csv 15

echo "进行 16 线程实验"
./msp updated_mouse.csv updated_flower.csv 16
./msp updated_mouse.csv updated_flower.csv 16
./msp updated_mouse.csv updated_flower.csv 16
./msp updated_mouse.csv updated_flower.csv 16
./msp updated_mouse.csv updated_flower.csv 16
./msp updated_mouse.csv updated_flower.csv 16
./msp updated_mouse.csv updated_flower.csv 16
./msp updated_mouse.csv updated_flower.csv 16
./msp updated_mouse.csv updated_flower.csv 16
./msp updated_mouse.csv updated_flower.csv 16

# 检查运行是否成功
if [ $? -ne 0 ]; then
  echo "运行失败，请检查 OpenMP 设置或程序错误。"
  exit 1
fi

echo "任务完成！"
