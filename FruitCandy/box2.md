1. 包、帧、段、数据报、消息：
  包：全能
  帧：数据链路层中包的单位
  段：TCP数据流中的消息
  数据报：IP和UDP等网络层以上的分层中包的单位
  消息：应用协议中的数据单位
  
2. Concurrency is not Parallelism. 并发不等同于并行。并发是结构上的概念，是某一很短的时间段上同时dealing with两件事情。并行是从一个时间点上进行衡量，该时刻同时Doing两件事情。
这两个概念的差别非常微妙，从计算机的角度来讲，单核系统不存在并行，并发。而多核系统可以并发，也可以并行。
并行可以以一下两种模式来实现：

   A.
   Blocks --> worker1 --> Factory

   Blocks --> worker2 --> Factory
   
   Blocks --> worker3 --> Factory

   B. 
   Blocks --> worker1 --> Transfer --> worker2 -- Transfer --> worker3 --> Factory
  
  
3. Stripslashes用于处理url或uri中的斜线。在kataras/iris里有个StaticWeb(request_url_path, system_directory, stripslashes_level)方法，它的第三个参数就是用来设置Stripslashes的等级，。示意如下：
 
   0: "/foo/bar" --> "/foo/bar"

   1: "/foo/bar" --> "/bar"

   2: "/foo/bar" --> ""

4. session, Web Storage, SessionStorage, LocalStorage, cookies


5. 10个性能测试工具
  * uptime
  * dmesg | tail
  * vmstat 1
  * mpstat -P ALL 1
  * pidstat 1
  * iostat -xz 1
  * free -m
  * sar -n DEV 1
  * sar -n TCP, ETCP 1
  * top
  
mpstat pidstat 需要安装sysstat [](http://mp.weixin.qq.com/s?__biz=MzAwNjY4NTQ4MA==&mid=400797122&idx=1&sn=b73ba049be8699eab97d58792a68676d&scene=23&srcid=0804fUgBWj9NmVEeiL3EKJUg#rd)
