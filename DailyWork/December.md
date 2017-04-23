December
========
20161201

    [x] 修改确定新的navList，包含个人中心的修改、nodeport管理、顺序调整，准备更新SQL脚本，更新创建用户const
    [x] 创建数据中心时自动占用32080端口,更新里也是。
    [x] 完成个人中心修改密码的调试
    [x] 完成nodeport管理功能的文档编写及调试
    [ ] 校验及错误详情的文档编写及调试

20161202

    [x] 校验及错误详情的文档编写及调试 

20161203

    [x] 公众号RESTful API Design文章整理
    [x] 校验及错误详情程序编写

20161205

    [x] 解决睿哥的问题1和4：1是不能挂载日志卷，2是datayp配额问题
    [x] 解决勇哥的问题：更新模板时生成的volumes json格式为map形式，正确的应该是数组形式。问题的原因是在导入模板的时候，缺了一行：$scope.param.deployment.spec.template.spec.volumes = [{}]; 这样在一个if语句的else里。这个if语句的含义就是如果导入的模板里有存储卷，则执行if里面的内容，否则如果没有导入模板，就执行else里的内容。缺少了的这行代码为初始化，如果没有它，就导致了保存的volumes的json变成了map。导致了最后的错误。这句话应该出现在所有涉及导入模板的地方，包括三处：appManage/deployment/controller.js，template/addTemplate/controller.js，walkthrougth/controller.js里
    [x] 应用发布的格式校验，但是请求响应时间变慢了很多（太多if语句）
    
20161206

    [x] 新的应用发布的格式校验，完成了objmeta的校验
    [x] 在b28为jinchao.ma创建只能看应用管理和集群拓扑的navList
    [x] 更新navList 172.21.1.27
    [ ] 其他校验的设计

20161207

    [x] 应用发布校验完毕
    [ ] 服务更新后台

20161208

    [x] 为k8s 1.5准备文章，研究新特性

20161209

    [x] 为更新组织配额时，如果该组织没有配额（比如原来手动创建的组织），会去创建相应的配额，否则是更新配额。
    [x] 提供更新服务的后台，目前仅支持修改端口（增加、删除、修改）
    [x] 搭建Gitlab ci，在m6 test上测试yce代码推送到打镜像成功.不知道什么test上gitlab runner访问的仓库地址是127.0.0.1，而服务在223上，所以启动了docker nginx 做了代理转发

20161212

    [x] 调研测试工具，没有找到合适快速上手的
    [x] 调研iris的插件使用方法，还在研究中
    [x] YCE需求总结、整理
    [x] 重新部署yce到master

20161214

    [x] 人肉测试检查bug，尤其是环境变量处
    [x] 为FindNewRs()和镜像管理处添加nil返回结果的处理，目前仅支持错误信息提示
    [x] 检查其他bug，维护特性和bug表，并提交处理

20161215

    [x] 检查、整理bug
    [x] 上线mingzong.liu的netpay应用
    [x] 上线m6的新yce

20161219

    [x] 编写测试用例（用户的主要操作流：从模板创建，5个用户故事，文档，uml和测试数据）
    [ ] 编写测试用例（用户的异常操作流） 
    [x] 帮助dawei完成前端的gitlab-ci流
    [x] 帮助mingzong.liu更新3个应用，又将frontend-hessian停机（删除实例，服务保留）

20161220
   
    [x] 解决缓存的问题
    [x] 编写新的deployment列表文档和代码，添加了svcName显示
    [x] 测试172上的改bug版，并发布到m6
    [x] 翻译K8s CRI blog

20161221

    [x] 报销单
    [x] 审校K8s CRI blog译文
    [x] 帮mingzong.liu更新netpay-hessian和installment-hessian
    [x] OKR自评
    [x] 编写重启实例后台
    [x] 帮mingzong.liu再次更新installment-hessian
    [x] 帮mingzong.liu三次更新installment-hessian

20161222

    [x] 协助开发Feature-0022
    [ ] 调研testify, FSM

20161223
    
    [x] 开发Feature-0024 
    [ ] 测试testify, 编写测试demo

20161226

    [x] 为mingzong.liu更新installment 3次
    [x] 整理测试自查列表并推广
    [x] 给后台每个ReadJSON添加对应的log.Error
    [x] 测试新版本的超卖   
    [x] 修改apis.sh

20161227

    [x] 整理apis.sh结果
    [x] 为mingzong.liu更新installment
    [x] 整理APIv2
    [x] 测试

20161228
    
    [x] 为mingzong.liu更新installment 和 netpay: 3次
    [x] 为mingzong.liu上生产内测installment 和 netpay
    [x] 更新yce 0.6.2
    

20161229
    
    [x] 填表
    [x] 跟lu.hou交接CI/CD
    [x] 测试yce v0.6.2，编写Release Note
    [x] netpay电信和b28上production
    [x] 编写image sync agent
