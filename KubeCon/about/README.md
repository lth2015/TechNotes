KubeCon视频系列简介
================================

### How We Scaled Kubernetes to 2000 Nodes Clusters 
-----------------------

主讲人是来自Google的Marek Grabowski，分享的主要内容有：

1. 99%的ApiServer的请求都在1秒内结束 

2. 99%的pod调度、创建在5秒内完成 

3. 解决了反复go struct 到 json序列化和反序列化的问题，能支持100个节点，这是v1.0 

4. 优化过程：
	* v1.1: 使用go的反射机制、对象与版本抽离的方式，代码生成器（转换类型）等手段，能支持到250个节点
	* v1.2: 减少watcher的数量，引入cacher，如果不是非要取得实时的数据，尽量都走cacher，能支持1000个节点
	* v1.3: 使用protocol buffer二进制协议代替内部的json协议，能支持2000个节点
	* 计划在1.5版本将etcd 从2.0版本迁移到3.0版本，预计能够支持5000 
	* 未来Scheduler筛选的过程考虑使用map-reduce加速调度过程

### A Cloud Native State of the Union 
------------------------

主讲人是来自Linux Foundation的Dan Kohn，分享的主要内容有:

1. 云计算的发展历史以及CloudNative的发展概况

2. 容器技术在资源隔离、开源、扩展性、敏捷和可维护性以及效率和资源利用率方面的影响

3. CNCF在会议召开、奖励机制、文档编写以及市场合作方面提供的服务

4. CloudNative包含的公司、项目的技术栈和全景图

5. CNCF在认证培训方面的工作以及KubeCon 2017的安排

### Logging for Containers:
------------------------

主讲人是来自TreasureData的Eduardo Silva，分享的主要内容有:

1. 日志采集的三大意义：监控、排错和统计分析

2. 容器和微服务场景下日志处理遇到的挑战

3. 分布式日志4种聚合模式

4. 轻量级日志聚合器fluentbit的介绍及示例


LightningTalk:  Micro-Services Lifecycle Management at Twitter 
------------------------

主讲人是来自Twitter的Micheal Benedict，分享的主要内容有：

1. 单体应用的问题,为什么要向微服务转换

2. Twitter的前端路由设计

3. 微服务管理的要点：
	* 清晰的服务隔离，管理权转换
	* 稳定性，容错
	* 可扩展性
	* 开发人员的生产率
	* 团队管理
	* 元数据管理
	* 标识管理
	* 资源管理
	* 计费管理

4. 介绍Twitter的服务生命周期管理——Kite系统
