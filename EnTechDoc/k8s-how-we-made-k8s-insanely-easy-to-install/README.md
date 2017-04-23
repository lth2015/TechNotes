我们是如何让K8s“巨易”安装的
----------
编者注：本文来自 Weaveworks的Head of Developer Experience——[Luke Marsden](https://twitter.com/lmarsden)。文中讲述了[SIG-Cluster-lifecycle](https://github.com/kubernetes/community/blob/master/sig-cluster-lifecycle/README.md)在一款可以简化Kubernetes安装的工具`kubeadm`上最近的工作。

我们之前收到用户的反馈说Kubernetes的安装非常困难，同时我们希望人们能够专注于编写优质的分布式应用而不用被基础设施所困扰。所以SIG-Cluster-lifecycle](https://github.com/kubernetes/community/blob/master/sig-cluster-lifecycle/README.md)近来一直在`kubeadm`上努力，`kubeadm`可以让Kubernetes的安装变得惊人的简单。

下列是Kubernetes集群安装的几个步骤，我们决定从第二步开始：

1. 准备（Provisioning）：准备一些机器
2. 启动（Bootstrapping）：在它们上面安装Kubernetes并且配置证书
3. 附件（Add-ons）：安装必需的集群附件，如DNS和监控服务，Pod网络等。

我们早就预料到用户有各种各样准备机器的方式。

他们准备的机器可能来自不同的云服务商、私有云、裸金属甚至树莓派，并且通常有他们自己偏爱的自动化部署工具：Terraform、CloudFormation、Chef、Puppet、Ansible甚至用来启动裸金属的PXE等等。所以我们做了一个重要的决定：*kubeadmin 不准备机器（kubeadm would not provision machines）*。唯一的要求是它假设用户拥有一些[运行了Linux的机器](http://kubernetes.io/docs/getting-started-guides/kubeadm/#prerequisites)。

另外一个重要的限制是我们不想构建又一个“按部就班部署Kubernetes”的外部工具。因为现在已经有许多这样的外部工具了，而我们的目标定得更高。我们选择让Kubernetes核心自身变得更容易安装。幸运的是它的基础早就开始打了。

我们意识到如果让Kubernetes变得“巨易”手动安装，用户显然会知道怎样借助于其他工具来自动化安装。

所以执行[kubeadm](http://kubernetes.io/docs/getting-started-guides/kubeadm/)吧。它不需要任何基础设施依赖，也满足了上面的需求，易于使用，并且是易于自动化的。目前它是alpha版，工作方式如下：

* 为你的Linux发行版安装Docker和官方的Kubernetes包
* 选择一个机器作为master节点，运行`kubeadm init`
* 它设置了控制面板，并且输出一条命令：`kubeadm join [...]`，包含了一个安全令牌。
* 在每个选为从节点的机器上，运行上一步得到的`kubeadm join [...]`命令。
* 最后安装一个pod网络，[Weave Net](https://github.com/weaveworks/weave-kube)是一个很好的起点。只需`kubectl apply -f https://git.io/weave-kube`即可安装。

快！你现在拥有了Kubernetes集群！[今天就试试`kubeadm`吧](http://kubernetes.io/docs/getting-started-guides/kubeadm/)


可以看下面的视频，[QQ视频](), [Youtube](https://youtu.be/b_fOIELGMDY)

跟着[kubeadm getting started guid](http://kubernetes.io/docs/getting-started-guides/kubeadm/)来亲自尝试吧，并且在Github上给我们[反馈](https://github.com/kubernetes/kubernetes/issues/new)，并@kubernetes/sig-cluster-lifecycle!

最后，我想要衷心感谢SIG-cluster-lifecycle的成员们，离开他们这些都无法实现。这里我只点到一小部分：
* [Joe Beda](https://twitter.com/jbeda)让我们关注于给予用户方便
* [Mike Danese](https://twitter.com/errordeveloper)在谷歌是一位传奇技术领导，并且总是知道发生了什么。Mike也不知疲倦地进行Code Review。
* [Ilya Dmitrichenko](https://twitter.com/errordeveloper)是我在Weaveworks的同事，写了大部分kubeadm的代码，并且乐意帮助别人进行贡献。
* [Lucas Kaldstrom](https://twitter.com/kubernetesonarm)来自芬兰，是组里最年轻的贡献者，并且在他学校数学考试的前夜仍然在合并PR。
* [Brandon Philips](https://twitter.com/brandonphilips)和他在CoreOS的团队领导了TLS启动的开发，这是我们少不了的关键组件。
* [Devan Goodwin](https://twitter.com/dgood)来自红帽，构建了Joe所想的JWS服务发现，整理了我们的RPM包。
* [Paulo Pires](https://twitter.com/el_ppires)来自葡萄牙，支持我们外部的etcd，并且负责了其他很多零碎工作。
* 以及许多其他的贡献者。

这真的是和这群可爱的人们共同完成的一项跨公司、跨时区的伟大成就。在SIG-cluster-lifecycle里还有很多要做的事情，如果你对这些挑战感兴趣，那么加入SIG吧，我们在期待跟你一同奋斗。

原文作者：[Luke Marsden](https://twitter.com/lmarsden)，Head of Developer Experience at [Weaveworks](https://twitter.com/weaveworks).
原文链接：[How we made kubernetes easy to install](http://blog.kubernetes.io/2016/09/how-we-made-kubernetes-easy-to-install.html)
