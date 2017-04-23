#1# 

There are 2 concepts in TCP/IP protocol stack: MSS and MTU.

To TCP, MTU(Max Transmission Unit) is the max protocol data unit that the layer can pass onwards. It is decided by the Standards(e.g. Ethernet) or Systems(e.g. P2P serial links).

MSS(Max Segment Size) is max data segment which can be transmitted by it. 

In most cases, both sides will negotiate the MSS for better transport performance. MSS is always the same with MTU when realizing TCP protocol.

It's value is 1460 after reducing the IP header for 20bytes and TCP header for 20bytes. The max MSS is the smaller one of both sides. 

MTU is different with minimum datagram size and also different with physically transmitted frame.

Different transportation media has different MTU, e.g. IP -> 65535, Ethernet -> 1500, PPPoE -> 1492

In some ideal way, MSS is the max data size which will not be divided in IP protocol. 

The size of IP package depends on MTU, the larger package will lead a hgiher transmit speed in theoratical condition. But in fact, larger MTU will cause greater delay and be more problematic. So PPPoE set MTU to 1492, Ethernet set MTU 1500 at the same time 576bytes in Internet. 

It is always not the same MTU on different on the link. IP allows fragmentation: divide datagram into pieces. This overcome the problem but lead to other issues at the meanwhile.

#2#

To UDP, the max length of the data package is 65535 which will be 65515 while 20 is reduced for the package header.

Does UDP has max data size ?

It's mac address(both source and destination) in datalink layer head.
It's IP address in IP layer head.
It's TCP header in Transportation layer.

[UDP、TCP、IP最大传输数据](http://blog.csdn.net/qiaoliang328/article/details/7580787)

[TCP协议中的MSS理解](http://blog.chinaunix.net/uid-20788636-id-2626119.html?/11207.html)

[图解TCP/IP](http://)
