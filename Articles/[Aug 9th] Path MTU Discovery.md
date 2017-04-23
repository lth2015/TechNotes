Path MTU Discovery is used for deciding MTU and avoiding IP fragmentation.

The source host will send a packet with a flag bit DF(Don't Fragmentation) in its headers. The packet will be sent to the destination host only if the middle host whose MTU is smaller will drop it and send back an ICMP packet Fragmentation Needed with its MTU to the source host.

Problems:
Many network security devices block all ICMP messages for perceived security benefits.(Prevent hacker probing by blocking ICMP traffic.)

[Path MTU Discovery](https://en.wikipedia.org/wiki/Path_MTU_Discovery)
