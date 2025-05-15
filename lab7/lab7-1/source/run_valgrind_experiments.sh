#!/bin/bash

EXE=./heated_plate_valgrind
OUT_DIR=valgrind_logs
mkdir -p $OUT_DIR

MATRIX_SIZES=("2 2" "4 4" "6 6" "8 8" "16 16" "32 32" "64 64" "128 128" "256 256" "512 512")
THREADS=(1 2 4 8 16)
SCHEDULES=(0 1)

PERF_LOG="performance_log.txt"
CSV_OUT="perf_data.csv"
echo "=== 性能测试记录（秒） ===" > $PERF_LOG
echo "M,N,T,S,Time_ms" > $CSV_OUT  # 初始化 CSV 文件

for size in "${MATRIX_SIZES[@]}"; do
    read -r M N <<< "$size"
    for t in "${THREADS[@]}"; do
        for sched in "${SCHEDULES[@]}"; do
            LOGFILE="${OUT_DIR}/massif_M${M}_N${N}_T${t}_S${sched}.out"
            echo "Running Valgrind: M=${M}, N=${N}, Threads=${t}, Schedule=${sched}"
            MS_ARGS="--massif-out-file=$LOGFILE --stacks=yes"

            START_TIME=$(date +%s.%N)
            OMP_NUM_THREADS=$t valgrind --tool=massif $MS_ARGS $EXE $M $N $t $sched > /dev/null 2>&1
            END_TIME=$(date +%s.%N)
            DURATION=$(echo "$END_TIME - $START_TIME" | bc)

            # 输出到日志文件，单位是秒
            echo "M=${M}, N=${N}, T=${t}, S=${sched}, Time=${DURATION}s" | tee -a $PERF_LOG
            
            # 转换为毫秒，写入 CSV
            printf "%d,%d,%d,%d,%.3f\n" $M $N $t $sched $DURATION >> $CSV_OUT
        done
    done
done

echo "启动 massif-visualizer 查看所有输出..."
for file in ${OUT_DIR}/massif_*.out; do
    echo "打开 $file"
    nohup massif-visualizer "$file" > /dev/null 2>&1 &
done

echo "所有 massif_*.out 文件已启动 visualizer。"
echo "性能数据已保存为 CSV 文件：$CSV_OUT"
