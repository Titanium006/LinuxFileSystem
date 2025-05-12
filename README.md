# 模拟Linux文件系统

在Windows环境下模拟一个Linux文件系统，该程序分为三阶段实现，参考了Github上的相似开源项目。

## Design 1

1. 在Windows环境下，开辟一个硬盘空间，作为文件系统的设定硬盘空间。

2. 编写管理程序Simdisk对硬盘空间进行管理，其关键配置为：
   - 盘块大小1kb；
   - 使用位图法管理空闲盘块；
3. 实现的功能（指令）包括：info、cd、dir、md、rd、newfile、cat、copy、del、check等。

## Design 2

在**Design 1**的基础上，实现了一个简单的Shell命令行解释器，并将**Design 1**中的Simdisk程序作为后台进程运行，利用Shell对其进行操作。

## Design 3

在**Design 2**的基础上，实现多个进程（通过多个Shell）同时对模拟文件系统进行操作（初步实现了2个用户）。在Simdisk程序中添加了用户访问权限管理，根据不同Shell上登录的用户的访问权限决定其对模拟文件系统的操作。