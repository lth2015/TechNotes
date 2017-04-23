8日早上，mac发现网络连接不通，具体表现为：微信QQ可用，Chrome打不开网页，ping不通远程服务器，提示：Ping: sendto: No buffer space available.

最终重启解决。网上给出的说法主要是网卡缓冲区满、Arp表满等。

通过netstat -m可以看到网卡缓冲区的使用情况：

结果1、2见下图

![](https://github.com/maxwell92/TechTips/blob/master/Articles/ping1.png)
![](https://github.com/maxwell92/TechTips/blob/master/Articles/ping2.png)

前一天晚上睡觉前，连了VPN测了程序。然后合上笔记本睡觉，早上起来就发现出现了这个问题。但是不知道原因。
是测试程序的RESTful 接口，通过GET方法带了很多参数造成的？还是VPN没关占用了资源？

