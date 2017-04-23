20170204
    
    [x] 整理yce的新需求
    [x] 检查各个数据中心的yce版本差异: master(dev): 0.6.3, m6(qa): 0.6.3, dianxin(prod): 0.6.3, shijihulian-b28(prod): 0.6.3
    [x] 多数据中心逻辑检查
    [x] 为生产两个yce准备受限用户(分离了同步镜像)的SQL，在siAgent-normal/sql/only-restricted-user/下

20170206

    [x] 多数据中心逻辑检查：更新组织时为组织添加新的数据中心支持
    [x] 多数据中心逻辑检查：更改应用返回结果结构，JSON设计
    [x] 分配Microservice on K8s and Docker翻译
    [x] 讨论本期任务

20170207

    [x] 多数据中心逻辑检查：更改应用返回结果结构，代码修改
    [ ] 帮助上线frontend-netpay-hessian 到生产环境
    [x] golang sort包学习

20170208

    [x] 多数据中心逻辑检查：更改服务返回结果结构，代码修改
    [x] 多数据中心文档更新：应用列表及详情（包含服务）、服务列表及详情

20170209
    
    [x] 多数据中心逻辑检查：健康检查livenessprobe controller编写
    [x] 多数据中心逻辑检查：模板排序
    [x] 多数据中心逻辑检查：更新节点详情controller
    [x] 多数据中心逻辑检查：为deployment/service controller添加defaultSort()方法

20170210

    [x] 用户权限更新文档
    [x] 用户权限更新navList及代码

20170213

    [x] 用户权限user初始化user/init, user列表/user的controller 及 doc  
    [x] 更新权限的设计

20170214

    [x] 新的权限设计文档和代码
    [x] 跟马睿安排golang

20170215
    
    [x] 服务列表bug排查，修复了pointer问题，更新了对多数据中心的支持 
    [x] 应用列表同样的pointer问题修复

20170216

    [x] 多数据中心小bug修复，联调
    [x] ReleaseNote编写
    [x] master更新

20170217

    [x] 前后联调
    [x] ReleaseNote编写
    [x] master更新，QA，生产

20170220

    [x] 确定0.6.5的需求 
    [x] 研究Watch()机制 

20170221

    [x] 整理数据库ER图
    [x] 编写service健康检查，修改pod健康检查
    [x] 前后联调数据中心dcIdList导入模板时的bug

20170222

    [x] 调试昨天dcIdList的bug
    [x] Dell SDN Share about Big Switch

20170223
    
    [x] 联调及测试
    [x] QA, Prod

20170224

    [x] fix liveness bug 
    [x] done new check logic
    [x] sync queue dedup
    [x] sort by name, deployment, template, service
    [x] fix oldRs bug with empty PodList
    [ ] errno check

20170227
    
    [x] errno check : 30%
    [x] watch + cache: 50%

20170228

    [x] datasource doc
    [x] datasource create, list controller

