# 员工考勤系统-c语言实现
写一个模拟的员工考勤系统，包括服务端(即员工考勤数据处理和存储服务器),两个客户端(即模拟员工打卡的客户端和管理员的客户端)需求如下:
## 服务端
1. 服务端支持多个客户端随时连接，最多同时连接数量为20; 
2. 根据客户端的请求返回相应的数据，做到每条数据传输都有应答;
3. 服务器退出时(包括终端使用CTRL+c按键退出)，需要通过广播或者组播的方式发送服务器已退出的消息。
## 员工打卡客户端
1. 实现网络连接服务器并发送上下班打卡信息到服务器，实现模拟打卡操作。终端显示打卡成功或失败。
2. 可以查看自己的每一天考勤数据，通过终端打印出来。
## 管理员客户端
1. 可以管理服务器中的员工信息，包括注册、修改、删除、查看所有员工的信息。
2. 设置上班打卡时间、迟到和旷工的规则。例如: 08:30分前为正常打卡、08:30-08:45为迟到、08:45 后为旷工处理。
## 提示:
1. 服务器多个连接可以使用io复用或者多线程的方式实现，多线程注意资源共享问题;
2. 网络通信协议可以使用udp. 或者tcp实现;
3. 客户端和服务端传输的数据格式可以尝试使用json数据格式或者其他;.
4. 程序安全退出，使用信号处理;