Clustered RabbitMQ on Kubernetes
===============================

有很多方法可以让我们搭建Kubernetes上的RabbitMQ集群。今天将为大家分享我们在Fuel CCP项目上搭建RabbitMQ集群时踩过的那些坑。这些坑在大多数搭建方法里都会遇到，如果你想提出你的解决方案，你应该可以从本文中获得灵感。

### 节点命名
在Kubernetes上运行RabbitMQ集群会带来一系列有趣的问题。首要的问题就是我们该如何给节点命名，以便它们可以相互发现？下面是一些例子：

* rabbit@hostname
* rabbit@hostname.domainname
* rabbit@172.17.0.4

在你启动任何节点之前，你应该先确保容器之间可以通过这个名称相互连通。例如可以ping通@后面的内容。

Erlang发布版（常被RabbitMQ使用）有两种命名模式：短名称或长名称。最佳实践是当它包含"."时采用长名称，否则采用短名称。对于上面的命名示例，第一个是短名称，而第二个和第三个都是长名称。

看看Kubernetes命名nodes时的选项：

* 我们可以使用[PetSet](http://kubernetes.io/docs/user-guide/petset/)（也叫做StatlefulSets）得到稳定的DNS名称。通常，副本一旦不健康就会被丢弃。与此相反，PetSet是一组有状态、拥有强标记的pods，
* 使用IP地址和一些自动化的节点发现（例如[autocluster plugin](https://github.com/aweber/rabbitmq-autocluster/)，它可以以一种可发现的方法自动集群化RabbitMQ）。

这些选项都需要运行在长名称模式下。注意在Kubernetes Pod里配置的DNS/hostname与RabbitMQ 3.6.6之前的版本是不兼容的，有必要的话请在开始前先进行[修复](https://github.com/rabbitmq/rabbitmq-server/issues/890/)。

### Erlang cookie
成功集群化的第二要素是RabbitMQ节点需要拥有共享的secret cookie。默认情况下，RabbitMQ从一个文件里读取这个cookie（如果没有这个文件则会生成）。为了保证所有节点上的这个Cookie一致，我们采用的办法是：

* #在Docker打镜像的时候就创建cookie文件。#但不推荐这么做，因为拿到cookie就意味着获得了进入RabbitMQ内部的所有权限。
* #在entrypoint脚本里创建cookie文件，#将secret作为环境变量，如果我们还需要entrypoint脚本，这是一个次好的办法。

---------- 校对分割线 ----------



* #通过环境变量向Rabbit MQ传入更多的选项。#比如：RABBITMQ_CTL_ERL_ARGS="-setcookie <our-cookie>"，RABBITMQ_SERVER_ADDITIONAL_ERL_ARGS="-setcookie <our-cookie>"  

### 集群须知
关于RabbitMQ集群我们还需要知道的一点是：当一个节点加入集群时，不管怎样它的数据都会丢失。通常这没什么关系，如果加入集群的空节点，那更是没什么好丢失的了。但是如果我们有两个独立运行了一段时间的节点，并且积累了一些数据，这种是没有办法在不丢失数据的情况下加入集群的（注意在网络划分或节点中断后集群恢复是一种特例，也会导致数据丢失）。对于特殊的负载，我们可以发明一些周边，例如即将要重置的节点手动或自动排空。但是没有一种健壮的解决办法，自动的或者全部的。

但是我们的自动化集群解决方案受到我们能承受的数据损失，根据具体的工作负载

### 集群信息
假定你解决了所有与命名相关的问题，并且可以通过rabbitmqctl集群化你所有的兔子。是时候让我们的集群变得自动化起来了。但是在前面的部分我们可以看到，没有一而全的解决办法。

一个特别的办法，仅仅适合于我们不在意服务器宕机或失联时数据丢失的工作负载。例如你在使用RPC时的工作负载，客户端只是重试（更愿意幂等）请求，在遇到任何错误/超时后，因为当服务器从错误中恢复后，这个RPC请求会变陈旧，变的没那么多关系。幸运的是，这个调用常发生在[OpenStack](https://www.mirantis.com/software/openstack/?utm_campaign=MOS%20weight%20increasing)各种各样的组件之间。

知道上面这些之后，我们可以开始设计我们的解决方案了。状态越少越好。所以使用IP地址比使用PetSets要好，而且[autocluster](http://aweber.github.io/rabbitmq-autocluster/)插件是我们的第一备选，来将一些动态不可拆卸的节点组成集群的任务。

阅读整个[autocluster](https://github.com/aweber/rabbitmq-autocluster/wiki/General%20Settings)的文档，我们可以得到下面的配置选项；

* {backend, etct}: 这是任意的选择，consul 或 k8s都不能工作好。选择它的唯一原因是它更容易测试。你可以下载etcd的二进制，不需要任何参数就运行，并且开始在本地localhost构建集群。
* {autocluster_failure, stop}: 一个加入集群失败的Pod对我们来说是没用的。所以它应该被排出，并希望下一个restart发生在一个更友好的环境里。
* {cluster_cleanup, true}, {cleanup_interval, 30}, {cleanup_warn_only, false}, {etcd_ttl, 15}: 节点被注册在etcd里，仅在它成功加入集群，并完全启动后。注册TTL通常在节点活着的时候被更新。如果节点死亡（或用别的方式更新TTL失败），它会被强行提出集群。所以如果一个节点使用同样的节点重启，它仍可以重新加入到集群里。

### 预料不到的竞争
如果你多次试着用上面的配置装配集群，你可能会注意到有些时候它会组成几个不同的、未连接的集群。这是因为在autocluster在启动竞争时的保护只是一些节点启动时的随机延迟，在一些坏的情况下， 每个节点都会认为它们自己是第一个节点（例如，etcd里还没有记录的时候），并且运行在非集群状态。

这个问题让我们去开发一个更好的大[补丁](https://github.com/aweber/rabbitmq-autocluster/pull/98)给autocluster。它添加了合适的启动锁-节点在启动前获取该启动锁，并且在正确注册后端后释放它。只有在etcd支持的时候，但其他可以轻松的加入进来（通过后端模块实现2个新回调实现）。

另外一个解决方案是问题，Kubernetes PetSets，因为它可以执行启动编排-通过只有一个节点在任何给定的时间执行启动-但现在考虑到它是alpha版，而且上面的补丁已经提供了对每个用户的解决方案，不只是K8s用户

### 监控

在我们的集群无人值守运行的最后一件事是添加一些监控。我们需要监控兔子的健康状态和它是否被正确地用剩余的节点集群化

你可能会记住rabbitmqctl list_queues / rabbitmqctl list_channels 被用做一种监控方法的时候了，但不够理想，因为它不能区分本地和远程错误，并且它还产生了很大的网络负载。最后，介绍新的闪耀的rabbitmqctl node_health_check，自从3.6.4开始，它是最好的检查每个RabbitMQ节点的方法了。

检查一个节点是否正确的集群化需要下面的一些检查：

* 它应该被集群化，使用最好的节点，注册在autocluster后端。这是新节点会加入的节点，它是按字母序第一个存活的节点。
* 甚至当节点被正确的集群化后，使用发现模式，它的数据仍然可以被分离，同样，这个检查不是过渡的，所以我们需要检查分区列表，在当前节点和发现节点上。

当这些集群检查都以独立的[commits](https://github.com/Mirantis/rabbitmq-autocluster/commit/5fee57752a0788bd2358d3f09eae76d4da67f039)实现，并且可以使用下面的方法调用：

rabbitmqctl eval 'autocluster: cluster_health_check_report()'

使用这个rabbitmq命令我们可以同时检测到兔子节点的任何问题，并且立刻停止它，所以Kubernetes将有一个机会来进行重启魔法

### 自己搭建RabbitMQ集群
如果你想要赋值这个安装，你需要一个新版本的[RabbitMQ](https://github.com/rabbitmq/rabbitmq-server/releases/tag/rabbitmq_v3_6_6)或一个[定制版本的autocluster插件](https://github.com/Mirantis/rabbitmq-autocluster/releases/tag/0.6.1.950)（因为启动锁补丁尚未被上流upstream接受）。

你可以在[Fuel CCP](https://github.com/openstack/fuel-ccp-rabbitmq/tree/master/service/files)中深入了解这个设置是如何实现的，或使用[单机版](https://github.com/binarin/rabbit-on-k8s-standalone)按照同样的设置作为基础来实现你的。

为了给你这是如何工作的提示，嘉定你克隆了第二个仓库，并且你有命名为`demo`的Kubernetes命名空间和一个`etcd`服务器运行在里面，并且可以使用同样的`etcd`名字进行访问。你可以创建这个设置通过运行下面的命令：

```shell
    kubectl create namespace demo
    kubectl run etcd --image=microbox/etcd --port=4001 \
    --namespace=demo -- --name etcd
    kubectl --namespace=demo expose deployment etcd
```

一旦这个成功，为了设置RabbitMQ，按照下面的步骤来：

    1. 构建一个合适版本的RabbitMQ的Docker镜像，以及autocluster，并且拥有必须的配置部分
    ```shell
        docker build . -t rabbitmq-autocluster
    ```
    
    2. 存储erlang cookie到k8s的secret存储里
    ```shell
        kubectl create secret generic --namespace=demo erlang
    --from-file=./erlang.cookie
    ```
    
    3. 创建一个3节点的RabbitMQ应用，为了简单，你可以使用https://github.com/binarin/rabbit-on-k8s-standalone/blob/master/rabbitmq.yaml上的yaml文件。
    ```shell
        kubectl create -f rabbitmq.yaml
    ```

    4. 检查集群是否工作
    ```shell
        FIRST_POD=$(kubectl get pods --namespace demo -l 'app -o jsonpath='{.items[0].metadata.name}')
        $ kubectl exec --namespace=demo $FIRST_POD rabbitmqctl cluster_status
    ```

你应该可以看到输出类似下面的：
```shell
    Cluster status of node 'rabbit@172.17.0.3' ...
[{nodes,[{disc,['rabbit@172.17.0.3','rabbit@172.17.0.4',
                'rabbit@172.17.0.7']}]},
 {running_nodes,['rabbit@172.17.0.4','rabbit@172.17.0.7','rabbit@172.17.0.3']},
 {cluster_name,<<"rabbit@rabbitmq-deployment-861116474-cmshz">>},
 {partitions,[]},
 {alarms,[{'rabbit@172.17.0.4',[]},
          {'rabbit@172.17.0.7',[]},
          {'rabbit@172.17.0.3',[]}]}]
```
重要的部分是`nodes`和`running_nodes`都包含3个节点

### 原文链接
[Clustered RabbitMQ on Kubernetes](https://www.mirantis.com/blog/clustered-rabbitmq-kubernetes/?utm_content=buffer1fa9c&utm_medium=social&utm_source=twitter.com&utm_campaign=buffer)
[Alexey Lebedev]()
