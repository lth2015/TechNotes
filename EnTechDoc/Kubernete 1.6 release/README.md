写在升级Kubernetes到1.6之前
==================================

> 简介：
> 最近我们的团队正在制定升级生产环境*Kubernetes*集群的计划，今天*Kubernetes*官方正式发布了ReleaseNote，重点强调了一些注意事项。希望对同样有升级*Kubernetes 1.6*计划的朋友有所帮助。


### ETCD集群备份
------------------------

在更新到1.6之前，强烈建议您备份ETCD集群中的数据。请查阅您正在使用的安装工具(*kargo*, *kops*, *kube-up*, *kube-aws*, *kubeadm*)以获得具体建议。

*Kubernetes 1.6*鼓励使用*ETCD 3.0*，从*ECTD 2.0*切换到*ETCD 3.0*涉及不同的存储引擎之间的数据的完全迁移。在从*ETCD 2.0*-->*ETCD 3.0*迁移期间，您必须停止*API Server*，防止向*ETCD*集群中写入数据。*HA*的方案目前无法使用官方的*Kubernetes*工具进行迁移。

如果使用*ETCD 3.0*，*Kubernetes 1.6*也默认采用*protobuf*编码。这种变化是不可逆的。如果要回滚，**您必须使用*protobuf*/*ETCD 3.0*切换之前的数据进行备份还原，并且自备份以来的数据更改都会丢失**。由于*Kubernetes 1.5*不支持*protobuf*编码，如果升级到*protobuf*后回滚到*1.5*版本那么，您仍需从切换之前的备份中恢复，同样，自转换*protobuf*之后的任何数据更改都将丢失。转换到*protobuf*后，您应该在验证*Kubernetes*集群能够正确工作之后再投入生产使用。


备份是一个很好的预防措施，特别是在升级的过程中。此次升级过程有多个已知的问题，唯一的应对方法就是从备份中还原。



#### *1.6*的主要更新和发布主题 
------------------------

* 通过使用*ETCD 3.0*， *Kubernetes*集群现在默认支持5000个节点。

* 基于角色的访问控制（*RBAC*)升级到*Beta*版，并为控制平面、节点、控制器等组件定义了默认的安全角色。

* *kubeadm*的集群引导工具升级到了*Beta*版，亮点：所有的通信都通过TLS进行。

* 授权(*Authorzation*)插件可以有*kubeadm*安装，包括了新的*RBAC*默认的设置。

* 引导令牌系统现在允许令牌的管理和过期

* *kubefed*联邦引导工具现在也升级到了*Beta*版。

* 现在*Kubernetes*与容器运行时的交互通过*CRI*接口进行，这使*kubelet*与*container runtime*集成更加容易。*Docker*仍然是通过*Docker-CRI*(*Beta*版)。

* 多种调度相关的特性已经升级到*Beta*版：

    * 您现在可以同时使用多种调度器。

    * *Node*和*Pod*现在支持亲和性和反亲和性。

    * 高级的调度可以使用污点(*taint*)和耐受(*toleration*)特性。

* 当一个节点有问题时，您可以指定(每个*Pod*)*Pod*在这个节点上的停留时间。

* 多种存储相关的特性升级到了*GA*版：

    * *StorageClass*预先安装并在*Azuer*, *AWS*, *GCE*, *OpenStack*和*vSphere*上设置为默认值。

    * 可配置*Dynamic Provisioning*和*StorageClass*。

* 现在可以通过滚动升级来更新*DaemonSet*。


#### 更新细节
------------------------

请参考《容器时代》关于*Kubernetes 1.6*新特性的详细介绍。
