五个热爱Kubernetes的原因
![](https://github.com/maxwell92/TechTips/blob/master/IntroIdeas/pics/top5cube.png)
[原文作者](https://twitter.com/sebgoa)
[原文链接](https://www.linux.com/blog/top-5-reasons-love-kubernetes)

在柏林的LinuxCon上我发表过题为“我为什么喜爱Kubernetes的十个原因”的演讲，效果反响热烈，好多人都让我把这篇演讲写成博客。那么这篇博客就围绕这十个原因中关于前五个的展开，后五个将在下篇博客进行。

简明扼要地说，Kubernetes是一个“为了自动化部署、扩容和管理容器化应用的开源系统”，通常被看作是容器编排工具。

[Kubernetes](http://kubernetes.io/)项目在2014年6月由Google创立，现在它在Github上拥有超过1000位贡献者，超过3,7000次提交和超过1,7000颗星，并且目前由Linux Foundation下设的[Cloud Native Computing Foundation](http://cncf.io/)管理。据Garnter近期的一次[调查报告](https://twitter.com/giano/status/776403761694699523)显示，Kubernetes在所有管理大量容器的系统中处于领先地位（译者注：该报告显示Kubernetes占到大约37%的份额，Docker Swarm大约16%，Mesos大约13%）。

为了在数据中心中执行任务而选择合适的分布式系统并非易事，这是因为比较不同的解决方案要比阅读关于特性和性能的文档更为复杂。观测例如Kubernetes的性能存在很多变数，因此这是一项艰巨的挑战。我相信要做出这样的选择不仅需要考虑前面的这些因素，还需要考虑到过往的经历，团队中每个人的理解和技能。是的，这似乎不够理性，但我就是这么想的: )

所以在这里，下面列出热爱Kubernetes的原因，排名不分前后：

###1 Borg血统

Kubernetes(K8s)继承自谷歌的秘密应用管理工具：Borg。我常说K8s是Borg的开源重写版。

Borg长久以来是一个秘密，直到被[Borg论文](http://research.google.com/pubs/pub43438.html)所公布。这个系统由谷歌著名的SRE团队用来管理例如Gmail甚至GCE这样的应用。

![](https://github.com/maxwell92/TechTips/blob/master/IntroIdeas/pics/top5borg.png)

Borg管理容器化应用的历史，其实于在Borg诞生的时候，硬件虚拟化还不可用，并且容器提供了一套打包Google数据中心的细粒度计算单元，并且提高了很多效率。

作为云计算的“老鲜肉”，我为运行Borg的GCE所倾倒。这意味着我们从GCE得到的虚拟机实际上运行在容器里，并自GCE是一个被Borg管理的分布式应用。

所以，对我而言拥抱Kubernetes最大的原因是Google重写并开放了管理他们的云的解决方案。我把这理解为“想象AWS是EC2的开源版本”，这解决了我们很多头疼的问题。

所以，阅读Borg的论文吧，即使你只是走马观花地看了看，你也会学到很多Kubernetes内在的思想，这些都是非常宝贵的。

###2 轻松部署

这一点看起来有些争议。但当我在2015年早些时候就发现安装部署非常直接。

首先, 你可以在单节点上运行K8s，我们会回到这点。但对于一个非高可用安装，你只需要安装一个中心管理节点和一组工作节点。管理节点运行三个进行（API Server，Scheduler 和一个资源控制器）加上一个键值对存储etcd，工作节点运行两个进程（监控容器变化的Kubelet和暴露服务的Proxy）。

这个架构，从上层来看很像Mesos，CloudStack或者OpenStack等系统。如果用ZooKeeper替换掉etcd，用Mesos master替换掉挂历进行，并且用Mesos Worker替换掉kubelet/proxy，你就得到了Mesos。

我在开始的时候写了一本[Ansible Playbook](https://github.com/skippbox/ansible-cloudstack)，它使用CoreOS虚拟机并安装所有的K8s组件。CoreOS兼具覆盖网络（例如Flannel）和Docker的优点。结果是在5分钟内，我可以启动一个K8s集群。我一直在更新我的playbook，对于我来说，启动k8s只需要一条命令：
```shell
$ ansible-playbook k8s.yml
```

注意如果你使用Google云，你需要一个Kubernetes服务，GCE也可以让你一条命令启动集群：
```shell
$ gcloud container clusters create foobar
```

从我来看这非常简单，但我也知道这不是适用于每个人的。事情都是相对的，重用某人的playbook可能会很痛苦。

与此同时，Docker在做一件非常糟糕的事情，它重写了Swarm，并且将它嵌入到Docker engine里。它使得运行Swarm集群只需要两行bash命令。

如果你想要这种类型的启动，Kubernetes目前也支持一个叫做[kubeadm](http://kubernetes.io/docs/getting-started-guides/kubeadm)的命令，它使得你可以从命令行创建集群。启动一个主节点，并加入工作节点。就是这样
```shell
$ kubeadm ini
$ kubeadm join
```

我也同样为它建立了playbook，[戳这里查看](https://github.com/skippbox/kubeadm-centos)。

###3 使用minikube的开发方案
当你想尽快体验一个系统，你又不全量部署在你的数据中心或云上。你可能只是想在你本地的机器进行测试。

没事，你可以使用[minikube](https://github.com/kubernetes/minikube)。

下载，安装，你只需一条bash命令就可以搭建一个单节点、独立的Kubernetes实例。
```shell
$ minikube start
Staring local Kubernetes cluster...
Kubectl is now configured to use the cluster.
``` 

在很短的时间内，minikube将为你启动Kubernetes需要的每个程序，然后你就可以访问你的单节点k8s实例了：
```shell
$ kubectl get ndoes
NAME    STATUS  AGE
minikube Ready  25s
```

默认情况下，它支持Virtualbox，并启动一个只会运行单个程序（例如localkube）的VM，为你快速搭建Kubernetes。这个VM同时还会启动Docker，你也可以使用它作为Docker宿主机。

Minikube同时允许你测试不同的Kubernetes版本，和配置不同的测试特征。它也附带了Kubernetes dashboard，你可以快速打开：
```shell
$ minikube dashboard
```

###4 易于学习的API
在REST之前的世界是痛苦的，学习痛苦、编程痛苦、使用痛苦、调试痛苦。还包含了很多不断完善的、相互竞争的标准。但这一去不复返。这就是为什么我如此喜爱简洁的REST API，我可以查看和使用curl进行测试。对我来说，Kubernetes API很不错。仅仅包含了一组资源（对象）和HTTP动作，通过基于JSON或YAML的请求和响应我可以操作这些对象。

Kubernetes发展迅猛，我喜欢这些被划分到不同API组里的多种不同的资源，它们还被很好地控制了版本。我可以知道我使用的是alpha、beta还是stable版本，同时我也知道去哪里查看这些定义。

如果你阅读了第3个原因，那么你已经拥有了minikube，是么？那么最快地查看这些API的方法就是深入它们：
```shell
$ minikube ssh
$ curl localhost:8080
{
    "paths": [
        "/api",
        "/api/v1",
        "/apis",
        "/apis/apps",
        "/apis/apps/v1alpha1",
    ...
```

如果你看到了这些API分组，并且可以体验它们包含的这些资源，请尝试：
```shell
$ curl localhost:8080/api/v1
$ curl localhost:8080/api/v1/nodes
```

所有的资源都包含*kind*, *apiVersion*, *metadata*。

为了学习每个资源的定义，[Swagger API browser](http://kubernetes.io/kubernetes/third_party/swagger-ui/#/)非常实用。我也通常会去[文档](http://kubernetes.io/docs/api-reference/v1/definitions/)里查询一个特定的域等。学习API的下一步实际上是使用命令行接口kubectl，即原因5。

###5 极好的命令行接口
不管是学习API还是编写自己的客户端，Kubernetes都不会把你扔下。K8s的命令行客户端叫做kubectl，它基于语句，功能强悍。

你可以使用kubectl管理整个Kubernetes集群和所有的资源。

可能对于kubectl最难的部分是安装它或者找到它。这里有提升的空间。

让我们再次使用minikube，并且体验kubectl的动作，例如get，describe和run。

```
$ kubectl get nodes
$ kubectl get nodes minikube -o json
$ kubectl describe nodes minkube
$ kubectl run ghost --image=ghost
```

最后一条命令将会启动一个Ghost博客平台。你很快就会见到一个*pod*出现，一个pod是Kubernetes最小的计算单元和最基本的资源。当使用*run*命令的时候，Kubernetes创建了另外一个叫做*deployment*的资源。Deployment提供了一个容器化服务（看作单个微服务）的声明式的定义。对它的扩容通过下面的命令完成：
```
$ kubectl scale deployments/ghost --replicas=4
```

对于每个kubectl命令，你都可以使用两个小技巧：`--watch`和`--v=99`。watch标记会等待事件发生，这就跟标准Linux命令watch类似。值为99的verbose标记会给你跟kubectl功能一样的curl命令。这非常有助于学习API，找到它使用的资源和请求。

最后，你可以通过编辑deployment来更新它，这会出发一个滚动升级。

```
$ kubectl edit deployment/ghost
```

其余的5个热爱Kubernetes的原因，敬请期待。

那么你听说了Kubernetes但却对它是什么和它怎么工作的毫不知情？Linux Foundation的[Kubernetes Fundamentals Course](http://bit.ly/2ewaVAs)将带你从零开始学习如何部署一个容器化应用和通过API进行操作。[立马注册](http://bit.ly/2ewaVAs)。 

