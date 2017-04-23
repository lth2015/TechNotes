另外五个热爱Kubernetes的原因

![]()
[原文作者](https://twitter.com/sebgoa)
[原文链接](https://www.linux.com/blog/5-more-reasons-love-kubernetes)

在上一篇文章里，我讲述了让我热爱Kubernetes前5个理由。2015年7月Kubernetes被捐赠给Cloud Native Computing Foundation，并开始接受数十个包括Canonical，CoreOS，Red Hat等公司的开发。

我的前五个理由主要来自Kubernetes项目的历史、易于使用的特点和强大的功能。后面的五个理由将更"干货“。在上篇文章里提到：选择一个要在数据中心中执行任务的分布式系统不像是在一个电子表格里寻找性能或特征那样简单。你应该按照团队的需求灵活决定。然而要注意，我所说的这个十个理由是我的观点，并且我正以此使用、测试和开发系统。

###6 滚动升级和回滚
滚动升级和回滚对于应用管理来说毫无疑问是必需品，尤其是当我们拥抱快速迭代的开发周期和持续改进的时候。一个拥有这些特性的系统，它的设计和运行往往很复杂。

部署资源在Kubernetes里被重新定义--起初Kubernetes拥有Replication Controller(RC)，它定义了Pods的声明式状态--例如哪个容器、我在我的系统里想要多少个容器等。但是通过RC实现的滚动升级发生在客户端，客户端关闭经常会导致错误。

所以，Kubernetes引入了Deployment这个资源，使用Replica Sets（多种资源重命名中的一部分）替换掉了Replication Controllers。每次一个deployment被用户定义之后，系统就会创建一个新的replica set。对replica set的扩容和缩容造就了滚动升级和回滚的能力。确实，旧的replica set并没有被删除，而是保留在系统中，作为Deployment历史的一部分。

Deployment的扩容和部署策略可以在显式的定义中进行修改。

这些所有的功能，都是被一个基于REST API的HTTP请求触发。

让我们看看一个简单的deployment历史：
```shell
$ kubectl rollout history deploymetn ghost
deployments  "ghost":
REVISION    CHANGE-CAUSE
1           kubectl run ghost --image=ghost --record
2           kubectl edit deployment/ghost
```

这里演示一下升级。我们可以通过`rollout undo`来进行回滚。它将增加历史中的修订（Revise）。
```shell
$ kubectl rollout undo deployment ghost
deployment "ghost" rolled back
$ kubectl rollout history deployment ghost
deployments "ghost":
REVISION    CHANGE-CAUSE
2           kubectl edit deployment/ghost
3           kubectl run ghost --image=ghost --record
```
底下的一行经过编辑一个Deployment，你执行了滚动升级。回滚是一个`rollout undo`，嗯，是这样，你可以回滚到一个特定的版本。

###7 配额
开源世界里过去的15年涌现了大量的业务模型（及变体），它们其中有些一直是某个商业组件（插件）的形式出现。

在Kubernetes里内置了配额。它们可以被用来限制API资源的数量，或限制例如CPU和内存的物理资源的使用。与Pods和Deployments相似，配额也是K8s里的API资源。你可以通过yaml或json文件进行定义，并且在你的集群里利用kubectl进行创建。

例如，为了限制在一个特定命名空间里Pods的数量为1，你可以定义如下的资源配额：
```yaml
apiVersion: v1
kind: ResourceQuota
metadata:
    name: object-counts
    namespace: default
spec:
    hard:
        pods: "1"
```

就像对其他资源一样，你可以通过`kubectl get`和 `kubectl edit`通过下面的两条命令查看和修改配额:
```shell
$ kubectl get resourcequota
NAME            AGE
object-counts   15s
```
现在是单Pod运行，如果这时你继续创建新的Pod，K8s就会返回一个错误，告诉你超过了配额：
```shell
$ kubectl create -f redis.yaml
Error from server: error when creating "redis.yaml": pods "redis"
```
配额是内置的一级K8s API。这真令人惊奇!

###8 第三方资源
这在大多数系统中是一个难以理解的新概念。

Kubernetes的设计哲学是它包含了一组用来管理容器化应用的API。可以预料到，在一段时间内这个核心将相对稳定。用户可能使用到的任何另外的API资源可能不会被加入到核心中，而是会被动态地创建。Kubernetes将管理它们，并客户端将动态地使用它们。这个技术曾被[Pearson](http://www.devoperandi.com/kubernetes-automation-with-stackstorm-and-thirdpartyresources/)用来管理数据库。

这个例子我在LinuxCon上讲过，用来创建一个叫做pinguin的新的API对象。你可以通过第三方资源对象（ThirdParty Resource Object）进行创建。就像其他的K8s资源一样，它同样拥有metadata、apiversion、kind和一组versions的属性。metadata包含了一个用来定义新的资源组的名字：
```yaml
metadata:
    name: pin-guin.k8s.linuxcon.com
apiVersion: extensions/v1beta1
kind: ThirdPartyResource
description: "A crazy pinguin at Linuxcon"
versions:
- name: v1
```

让我们创建这个新资源： 
```shell
$ kubectl create -f pinguins.yml
$ kubectl get thirdpartyresources
NAME                        DESCRIPTION                 VERSION(S)
pin-guin.k8s.linuxcon.com   A crazy pinguin at Linuxcon v1
```
通过这种方式，你可以自由地创建一个pinguin（保持LinuxCon主题）：
```shell
$ cat pinguins.yml
apiVersion: k8s.linuxcon.com/v1
kind: PinGuin
metadata:
    name: crazy
    labels:
        linuxcon: rocks
$ kubectl create -f pinguin.yml
```

并且动态地，kubectl现在意识到了你创建的pinguin。注意这个特性仅在k8s:v1.4.0中可用。

```shell
$ kubeclt get pinguin
NAME    LABELS          DATA
crazy   linuxcon=rocks  {"apiVersion":"k8s.linuxcon.com/v1", "kind":"PinGui...
```

现在你可以想象能用它做点什么了，结果是你需要编写一个控制器，用一段代码来监控pinguins，当它被创建、删除或修改时做出某些动作。

这个特性意味着Kubernetes的API Server现在可以被每个用户任意地进行扩展，棒呆了！

###9 基于角色的访问控制（RBAC, Role-Based Access Control)
除了配额之外，基于角色的访问控制也是一个企业级系统的必须。类似于配额，在数据中心解决方案里，它通常是一个经过考虑之后的想法，而不是一个商业组件。

对于Kubernetes，我们拥有细粒度的访问控制，它基于角色，并且最好的部分当然是，100%的API驱动。通过这个，我意思是角色和绑定都是API资源了，管理员可以在集群上编写和创建这些资源，就像用户创建Pods和Deployments一样。

它最初在[v1.3.0](http://kubernetes.io/docs/admin/authorization/)版本里引入，它是一个alpha版本的API，仍被认为是实验的。但是多个发布版之后，我完全认为它是稳定的API了。

大致来说，你创建角色——API资源类型role，定义一些规则：
```yaml
kind: Role
apiVersion: rbac.authorization.k8s.io/v1alpha1
metadata:
    namespace: default
    name: pod-reader
rules:
 - apiGroups: ["api/v1"]
   resource: ["pods"]
   verbs: ["get", "watch", "list"]
   nonResourceURLs: []
```
然后你将用户和角色进行关联，通过创建绑定资源RoleBinding完成。一个绑定，就是一组用户，将他们和角色结合。一旦你创建了绑定，系统任何用户都会继承来自这个角色的访问规则。
```yaml
kind: RoleBinding
apiVersion: rbac.authorization.k8s.io/v1alpha1
metadata:
    name: admin
    namespace: default
subjects:
 - kind: ServiceAccount
   name: default
roleRef:
 kind: Role
 namespace: default
 name: admin
 apiVersion: rbac.authoziation.k8s.io/v1alpha1
```
对于这点，来自CoreOS的Eric Chiang有段非常精彩的[视频](http://www.youtube.com/watch?v=97VMYjfjWyg)。
内置RBAC，完全API驱动，简直了！

###10 集群联邦
最后但不是唯一的是[集群联邦](http://kubernetes.io/docs/admin/federation/)。
回到Borg论文里，我们热爱Kubernetes的第一个原因，你可能已经注意到了一个单独的K8s集群实际上等价于单个Borg的“Cell"，或者可用域。现在在Kubernetes 1.4.0里，已经拥有了将多个集群联邦起来并通过一个控制台进行管理的功能。这意味着我们拥有了Borg lite。

这是热爱Kubernetes的一个关键原因，因为它带来了混合容器云的解决方案。想象一下，拥有K8s集群前置和一个公有云（例如：AWS，GKE，Azure)。通过这个联邦控制台，你可以运行跨多个集群的微服务。扩容会自动平衡集群间的副本，并且提供一个单独的DNS端点，同时负载均衡也是联邦的。

这个让我非常激动，因为我们应该可以更快地在已有的集群和公有云上进行应用的迁移。是，你听对了，这全部都是内置的，而不是商业组件。

开始联邦吧，Kelsey Hightower写了一篇非常详细的[教程](https://github.com/kelseyhightower/kubernetes-cluster-federation)，值得一试。

以上就是我热爱Kubernetes的前十个原因。我非常确定别人也有其他的原因，这个系统的可以热爱的地方太多了。我一直使用、测试和开发数据中心解决方案，我感觉到Kubernetes是一个经过深度考虑的系统，它极其稳定，可扩展，和拥有一些我们本以为会是商业组件的关键特性。Kubernetes真的改变了游戏。

那么如果你听说过Kubernetes但却不知道它是什么，或者它如何工作？Linux Foundation的[Kubernetes Fundamentals course](http://bit.ly/2ewaVAs)将会带领你从零开始学会如何部署一个容器化的应用和通过API来操作资源。[立即注册吧！](http://bit.ly/2ewaVAs)
