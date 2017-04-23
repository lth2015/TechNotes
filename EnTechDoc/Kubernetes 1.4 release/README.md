Kubernetes 1.4: 任何地方，轻松部署
=======================================

今天，我们喜迎Kubernetes 1.4版本的发布。

15个月前,我们发布一般可用版本，Kubernetes持续发展，被工业界广泛地接受。无论是初创公司还是大规模企业，用户都在说Kubernetes为他们带来的了部署和管理分布式应用方面的巨大改变。然而，对我们来说，一个非常重要的用户反馈是让Kubernetes更加容易地安装和使用。我们用心考虑这些反馈，使这些反馈在1.4版本中得到重大的改善和提升。

我们和社区紧密协作、相互配合，经过20多位来自[SIG-Cluster-Lifecycle](https://groups.google.com/forum/#!forum/kubernetes-sig-cluster-lifecycle)伙伴的共同努力, 极大地简化了Kubernetesd的安装、启动、证书生成、发现、网络和应用部署等诸多方面的用户体验。

在1.4版本中，我们还提供了一些附加的功能，主要包括：简化集群部署，方便有状态应用的安装，增强了集群联邦的能力，能够实现跨多个云，多个集群的直接部署。

### 新特性：
------------------------

#### 两条命令创建集群
------------------------

为了启动Kubernetes，用户需要提供计算节点、安装Kubernetes和启动集群。用户都渴望一种简易、可移植的方法，在任何云（公有云、私有云或裸金属）上部署Kubernetes集群。

* Kubernetes 1.4引入了*kubeadm*命令，它将集群启动简化为两条命令，不需要再使用复杂的kube-up脚本。一旦Kubernetes被安装，`kubeadm init`启动master节点，而`kubeadm join`可以将节点并入集群。

* 安装过程的流水线化，通过打包Kubernetes和它的依赖，对于Linux主要发行版本，包括*Red Hat*和*Ubuntu Xenial*。这意味着用户可以使用熟悉的`apt-get`和`yum`来安装Kubernetes。

* 附加部署，例如对于一个overlay网络， 使用daemonset方式可以将操作简化为一条命令。

* 简化这个使用的是新的认证API，它使用kubelet TLS bootstrap，和新的API发现。

#扩展有状态应用的支持#
------------------------

当云原生应用被构建并运行在容器里时，更普遍的的批处理、数据库和KV存储等有状态的应用需要额外的特性来兼容容器。在Kubernetes 1.4里，我们引入了一系列特性来简化这些应用的部署，包括：

* ScheduledJob，目前是alpha版，用户可以按正常间隔运行批处理任务。

* Init-Container，目前是beta版，强调了运行一个或多个容器在启动主容器前，例如启动数据库或多层应用前的序列化依赖

* 动态PVC升级到beta版。这个特性允许集群管理员暴露多个存储提供者，并且允许用户通过一个新的存储API来选择使用哪个存储提供者。

* Curated和预测试[Helm Charts](https://github.com/kubernetes/charts)，对于公共的有状态应用，例如MariaDB、MySQL和Jenkins，使用Helm Package Manager版本2的一条命令就可使用Curated和Helm Charts。

#### 集群联邦API
------------------------

我们全球的用户共同希望的是在划分了的区域和云上能够构建应用。

* 联邦ReplicaSets Beta版，副本集现在可以在集群中的全部或者部分，跨区域、跨云创建副本。整个联邦副本统计和相对集群权重/副本统计会被持续地保持一致，通过联邦ReplicaSet控制器来保证你在每个区域/云上都拥有这些Pods。

* 联邦服务现在是beta版，并且secrets，events和namespaces也同样被添加到联邦API中。

* 联邦Ingress目前是Alpha版，由GCP启动，用户可以创建一个L7的全局负载均衡器，通过VIP负载部署在GCP中跨集群联邦的服务。通过GCP的联邦Ingress，外部客户端通过一个单独IP地址访问，请求会被发送至联邦中最近的可用集群中。

#### 容器安全支持
------------------------

多租户集群的管理员需要能够提供不同的租户、基础设施组件和系统的最终用户之间的权限集合。

* Pod安全策略，是一种新的对象，使集群管理员来控制Pod/Containers安全上下文的创建和验证，管理员可以联合service accounts，groups和拥有一些限制的用户来定义一个安全上下文。

* 添加了AppArmor支持，使管理员能够运行一个更安全的部署，提供了更好的审计和系统监控。用户可以通过在AppArmor的配置文件中设置一个简单的字段就可完成容器的配置。

#### 基础设施增强
------------------------

我们持续的为基于Kubernetes的用户和生态圈增加调度器、存储和客户端的能力。

* Scheduler，引入了[pod间亲缘性和反亲缘性](http://kubernetes.io/docs/user-guide/node-selection/) Alpha版本，用户能够定制Kubernetes如何组合或分配Pods，还有，集群附加组件的优先调度能力，例如DNS、Heapster和Kube Dashboard。

* Disruption SLOs，Pod中断预算被引入来限制在任意时刻被集群删除pod带来的影响，例如节点升级。

* 存储，新的[卷插件](http://kubernetes.io/docs/user-guide/volumes/)，适用于Quobyte和Azure Data Disk的被添加。

* 客户端：Swagger 2.0支持添加，支持非Go的客户端。

#### Kubernetes Dashboard UI
------------------------

最后，一个匹配了90%的Dashboard UI提供了方便的管理。

对于一个完全列表的更新，我们可以参见Github上的release note。除了Kubernetes开发的深刻印象外，是社区的贡献者。对于1.4版本来说只是十分重要的，以后会更加重要。

#可用性#
Kubernetes 1.4现在在get.k8s.io上可用，通过Github上的开源仓库，可以通过Hello World app来尝鲜。

可以加入到项目中来，通过weekly community meeting和进行贡献。

#### 用户和使用学习
------------------------

在过去的15个月以来，自从Kubernetes 1.0 GA版本，项目超出了每个人的想象，Kubernetes运行在上百个组织和上千个开发团队的产品中。这是一些运行Kubernetes的重点：

* Box，加速了它们从发布到上线服务的6个月周期至一周以内。这里阅读Box是如何在Kubernetes上运行任务重要的产品服务的。

* Pearson，最小化复杂度，增加它们工程师的生产力，这里阅读Pearson如何使用Kubernetes来重新发明世界最大的教育公司。

* OpenAI，一个非盈利人工智能研究公司，构建应用于深度学习的基础设施，通过Kubernets，最大化研究人员的生产力来让它们能专注于科研。

我们非常感谢社区超过900名贡献者，超过5000条提交来使得这个版本发布，想要更多地了解社区如何使用Kubernetes，加入我们在KubeCon上，直接聆听来自用户和贡献者的声音吧。

#### 连接
------------------------

* 在Stack Overflow上提出问题（或回答问题）

* 在Slack上连接社区

* 在Twitter @Kubernetesio上关注我们

#### 感谢您的支持
------------------------

-- Aparna Sinha, Product Manager, Google
