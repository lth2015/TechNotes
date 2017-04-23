[How Baidu rewrite using Golang](http://mp.weixin.qq.com/s?__biz=MzA5Nzc4OTA1Mw==&mid=2659598270&idx=1&sn=3e05f03e1a3c45a40bc8eed46566af9a&chksm=8be9eaacbc9e63ba4f6531a939bbff96e4b4a5781b78ae23a5b58fe9d8c4df148c68fbb67f57#rd)

JSON_SET and JSON_EXTRACT are used when dealing with JSON columns in MySQL.
use case:
1. SELECT JSON_EXTRACT(navList, "$.list[6].className") FROM user WHERE id=2; 
2. UPDATE user SET navList = JSON_SET(navList, '$.list[5].className', "fa-child") WHERE id=10;

Overlay Network is some kind of virtual network built on the existing underlay network such as IP network for some particular applications.

I found some extra words when I do kubectl cluster-info | cut -d " " -f6 | head -1 | cut -d "/" -f3 | cut -d ":" -f2
like: 
    8080^[[0m
so What is ^[[0m ?
It is ANSI color. Like we always could change font color in PS/2 in shell config files. like /etc/profile, etc.

GO build compiler will check packages in GOPATH index1 location, then index2.
like:
    GOPATH:/usr/local/go:/mworks
compiler will first find packages in /usr/local/go, then mworks
