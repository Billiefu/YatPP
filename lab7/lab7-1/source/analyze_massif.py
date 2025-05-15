import os
import re
import pandas as pd
import matplotlib.pyplot as plt

log_dir = "valgrind_logs"
data = []

for filename in os.listdir(log_dir):
    if not filename.startswith("massif_") or not filename.endswith(".out"):
        continue

    path = os.path.join(log_dir, filename)

    with open(path, 'r') as f:
        lines = f.readlines()

    # 提取最大堆大小（单位: bytes）
    heap_bytes = 0
    for line in lines:
        if line.startswith("mem_heap_B="):
            heap = int(line.split('=')[1])
            heap_bytes = max(heap_bytes, heap)

    # 提取元数据
    match = re.match(r"massif_M(\d+)_N(\d+)_T(\d+)_S(\d).out", filename)
    if match:
        M, N, T, S = map(int, match.groups())
        data.append({
            "Matrix": f"{M}x{N}",
            "Threads": T,
            "Schedule": "Static" if S == 0 else "Dynamic",
            "MaxHeapMB": heap_bytes / (1024 * 1024)
        })

# 生成 DataFrame
df = pd.DataFrame(data)
df.sort_values(by=["Matrix", "Threads", "Schedule"], inplace=True)

# 打印结果表
print(df.to_string(index=False))

# 可视化
plt.figure(figsize=(10, 6))
for matrix in df["Matrix"].unique():
    subset = df[df["Matrix"] == matrix]
    for schedule in ["Static", "Dynamic"]:
        sched_data = subset[subset["Schedule"] == schedule]
        plt.plot(sched_data["Threads"], sched_data["MaxHeapMB"], marker='o', label=f"{matrix} - {schedule}")

plt.title("Valgrind Massif Peak Heap Usage")
plt.xlabel("Threads")
plt.ylabel("Max Heap (MB)")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig("valgrind_memory_plot.png")
