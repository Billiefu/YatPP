## Lab7 仓库注意事项

本次实验Lab7包含两个部分，分别对应两个不同的并行计算任务。由于其中一部分任务与Lab6内容相关，因此在仓库中我们对实验内容进行分目录管理，以便清晰组织和查阅。

* **Lab7-1：MPI并行应用任务**

  本部分为Lab7的主要实验任务，要求对参考代码 `fft_serial.cpp` 实现的串行快速傅里叶变换（FFT）进行MPI并行化处理。实验需利用MPI的多进程通信机制，对代码结构进行合理调整，实现高效的并行计算。相关实验内容和源代码统一存放于 `lab7-1` 文件夹中。

* **Lab7-2：parallel_for并行应用分析任务**

  本部分为Lab6任务的扩展实验，重点分析`heated_plate_openmp`程序在不同并行规模（线程数）和问题规模（矩阵尺寸N）下的性能表现。实验还包括使用Valgrind Massif工具对程序内存使用情况的详细分析（包括栈内存），生成的内存日志可使用`ms_print`工具进行可视化展示。所有实验材料和分析脚本存放在 `lab7-2` 文件夹中。

**说明**：有关 Valgrind 工具的使用和配置方式，可参考其官方文档：[https://valgrind.org/docs/manual/ms-manual.html](https://valgrind.org/docs/manual/ms-manual.html)。
