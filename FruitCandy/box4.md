How to write smooth Makefile?
------------

chrome VPN plugins lead to cann't access to localhost ?

ssh login without password: server need public key(from client) to encrypt one random string sent to client, if it could be decrypted by client private key, it is authenticated.

In normal way, server public keys are saved in client .ssh/known_hosts, client public keys are saved in server .ssh/authorized_keys. 

In MySQL 5.7 version, function JSON_EXTRACT() is supported, [MySQL Doc](http://dev.mysql.com/doc/refman/5.7/en/json-search-functions.html#operator_json-column-path). Usage: SELECT dcIdList, JSON_EXTRACT(dcIdList, "$.dcIdList[0]") FROM organization;

How to make global unique ID: 
1: database based
* MySQL(5.6) AUTO_INCREMENT
* Postgres(REL 9.6 Stable) SEQUENCE
* Oracle SEQUENCE
* Flickr Ticket Servers, supporting sharding

2: Distributed Cluster .. 
* Strong Consistency, Zookeeper (Paxos Protocol)
* Evetually Consistency, Consul (Gossip Protocol)

Raft Protocol ,etc.

3: Flake: Twitter Snowflake, MongoDB ObjectId
    12-bytes MongoDB ObjectId 16777216 Id

[source](http://mp.weixin.qq.com/s?__biz=MzA5Nzc4OTA1Mw==&mid=2659598286&idx=1&sn=3172172ccea316b0ed83429ae718b54d&chksm=8be9eadcbc9e63caa10d708274b4fa34ceffa416ef4527e10e6b7a1a2d2f32cf8592d65bf728#rd)
