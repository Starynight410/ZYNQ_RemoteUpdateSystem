# ZYNQ_RemoteUpdateSystem
本项目实现FPGA的网口远程固化升级，基于ALINX的ZYNQ固化例程进行二次开发，并且在测试中完成了该功能。并添加了各部分固件升级耗时测试，在写入Flash前进行CRC校验。在本工程中还包含了PC端和ZYNQ PS端的CRC校验实现，固化升级的测试示例为两个LED交替闪烁。

# 开发平台

黑金AX7020开发板 + Vitis 2021.2

# 实现功能

（1）PC计算固化BIN文件的CRC校验码；

（2）采用ALINX提供的网口上位机完成网口连接和发送BIN文件；

（3）ZYNQ的PS端实现网口接收，接收完成进行CRC校验，校验无误待写入flash；

（4）写入flash，重新上电完成程序自动更新；

（5）计算每个步骤耗时。

# 测试结果

（1）生成CRC校验码

上位机端导入生成好的测试固化程序，执行将BIN文件末尾添加CRC校验码。
 
（2）网口在线升级结果测试

网口连接并发送BIN文件，结果显示PS端完成了网口接收，CRC32校验完成，下一步进行flash的写入操作。写入包括：擦除flash、以页为单位写入、读出逐一比对。

在线升级完成后重新上电，LED交替闪烁已被固化完成。

（3）每个步骤的耗时

固化程序大小为4MB，在此情况下对在线升级中各部分的耗时测算如下：LWIP接收数据包（8.35s）+ 擦除flash（13.40s）+ 写入flash（8.05s）+ 写flash检验（1.60s）= 31.40s
