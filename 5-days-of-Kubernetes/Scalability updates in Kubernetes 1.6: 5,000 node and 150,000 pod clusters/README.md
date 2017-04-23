# Kubernetes1.6的扩展性：单集群支持5000个Node，150000个Pod
=============================================

编者按：这篇文章是关于Kubernetes 1.6新功能深入解析系列的第二篇文章。

去年夏天我们分享了关于*Kubernetes*扩展能力的一些更新，从那之后我们一直在努力，今天我们非常自豪地宣布：*Kubernetes 1.6*单个集群可处理**5000**个*Node*，**150000**个*Pod*。而且这些集群的端到端*Pod*启动时间比之前* Kubernetes 1.3*版本**2000**个节点的集群更快；API调用时间延时可保持在1秒([*SLO*](https://en.wikipedia.org/wiki/Service_level_objective))之内。

译者注：集群支持2000个节点和**API**延时**SLO**请参考往期文章：《》。

本篇文章中我们将回顾在测试中所得到的一些指标，描述**Kubernetes 1.6**的性能测试结果。同时将讨论为达到系统能力更新所做的变动，和之后版本中对系统可伸缩性方面的一些计划。

### X-节点集群 - 意味着什么？
------------------------------

既然*Kubernetes 1.6*已发布，那现在是时候去回顾当我们说**“支持”**X-节点集群时到底意味着什么。在之前发布的文章中我们已有详细介绍目前有两个性能相关的服务层面指标(SLO)：

- *API*响应：99%的*API*调用在小于1秒内返回。

- *Pod*启动时间：99%的*Pod*以及他们的容器(和预拉取的镜像)在5秒内启动。

和以前一样，部署的集群超过**5000**节点是有可能的，有些用户也试验过，但是性能可能会下降，也许不能满足以上两个指标。

我们知道这些**SLO**的范围有限，在系统中的许多方面他们并不适用。比如说，我们没有衡量一个服务中的新**Pod**在启动之后能多快可以通过服务*IP*地址可达。如果你正考虑使用大型*Kubernetes*集群，并且有性能指标没包括在我们的*SLO*中，请联系[*Kubernetes Scalability SIG*](https://en.wikipedia.org/wiki/Service_level_objective)，可以帮助你了解目前*Kubernetes*是否可支持你的负载。

我们知道这些*SLO*的范围有限，在系统中的许多方面他们并不适用。比如说，我们没有衡量一个服务中的新pod在启动之后在多长时间内通过服务IP地址访问。如果你正考虑使用大型*Kubernetes*集群，并且有性能指标没包括在我们的*SLO*中，请联系*Kubernetes Scalability SIG*，可以帮助你了解目前*Kubernetes*是否可支持你的负载。

*Kubernetes*未来版本中与系统可伸缩性相关的最优先的事情是，通过以下方法来加强关于支持X-节点的定义：

- 改进现有的*SLO*

- 增加更多*SLO*(覆盖*Kubernetes*的不同领域，包括*Networking*)

### Kubernetes 1.6既定规模的性能指标
------------------------------

*Kubernetes 1.6*中大规模集群的性能如何？以下图片是2000节点集群和5000节点集群中端到端*Pod*启动时间延时。为了对比，我们也展示了相同指标在*Kubernetes 1.3*中结果，此结果发布在之前的系统可伸缩性文章中(支持2000节点的集群)。你可以看到，相对于2000节点的*Kubernetes 1.3*，2000节点和5000节点的*Kubernetes 1.6*的*Pod*启动更快。

![](https://lh6.googleusercontent.com/LdjAOmsLGdxLNTo222uif1V0Eupoyaq6dY-leg1FBGkyQxUNt5ROjrFh_XzW27P7nP865FYUVwTOaUpDEnirdHSBKvh9xl8PsBNEFlVWpJUbnj0FEdLX4MywqbjwK9oc8avLRNAX)

下一个图片是5000节点的*Kubernetes 1.6*集群*API*响应延时。所有延时都小于500毫秒，甚至90%的响应都小于100毫秒。

![](https://lh6.googleusercontent.com/RFGwgw9hvRshHH11vrUxGwl-X8vXdCvyd8ETdWS9Ud5_OFpG4WctzZbCy2ad4Ao_neYaMMDz46Z2JCQUzRI1jdk6OABTFIOyvZysZpDCAfr7Ztj-EM7v25sfHxf6dOe59fncDnra)

### 我们是如何做到的？
------------------------------

过去的9个月中(自从上一篇系统可伸缩性文章发表后)，*Kubernetes*关于性能与扩展性的有了巨大的变化。在本篇文章中我们将着重介绍两个最大的，并简单介绍一些其它变化。

#### etcd v3
------------------------------

在*Kubernetes 1.6*中我们将默认存储后端(*key-value*存储，也是整个集群状态存储的地方)从*etcd v2*转向[*etcd v3*](https://coreos.com/etcd/docs/3.0.17/index.html)。最初这个转变是从1.3版本开始的。你也许会奇怪为什么这个转变花了我们这么长时间，鉴于：

- *etcd*的第一个支持*v3*版本*API*的稳定版本于2016年6月30日发布

- 与*Kubernetes*团队一起设计了新API，以支持我们的需求(从功能和可伸缩性角度)

- *etcd v3*与*Kubernetes*的整合在*etcd v3*宣布时就已接近完成(毕竟*CoreOS*使用*Kubernetes*作为新*etcd v3 API*的概念验证)

事实上，这有很多原因。我们将说明以下几个重要原因。

- 以不向后兼容的方式改变存储是一个重大的变化，从*etcd v2*到*etcd v3*的迁移正是如此。因为如此我们需要有力的论证。9月份时我们找到了这个证据——我们确定，如果继续使用*etcd v2*我们不能够扩容至5000节点的集群（[*kubernetes/32361*](https://github.com/kubernetes/kubernetes/issues/32361)中包含一些关于它的讨论）。特别是*etcd v2*中*watch*的执行问题。在一个5000节点的集群中，我们需要能够做到每秒至少发送500个*watch*事件给单个*watcher*，而这在*etcd v2*中是不可能的。

- 一旦我们有了强烈的愿望想升级至*etcd v3*，我们就开始了全面测试。也许你想像的到，我们遇到了一些问题。*Kubernetes*中有一些小的*bug*，同时我们也要求了*etcd v3*中*watch*部署的性能提升（*watch*是*etcd v2*中的主要瓶颈）。这就促成了*3.0.10 etcd patch*的发布。

- 一旦这些变化完成之后，我们确信新的*Kubernetes*集群将与*etcd v3*一起协作。但是迁移现有集群仍是一个巨大挑战。为此我们需要将迁移过程自动化，全面测试底层*CoreOS etcd*升级工具，并做出从*v3*到*v2*的应急回滚计划。

最终我们还是自信的说，这个变化可行。

#### 将存储数据格式转为protobuf
------------------------------

在*Kubernetes 1.3*版本中，我们启用了[*protobufs*](https://developers.google.com/protocol-buffers/)作为*Kubernetes*组件见通信的数据格式，用以与*API Server*通信(同时保持对*JSON*的支持)。这对性能起了很大的提升。

然而，我们仍然使用*JSON*作为*etcd*中数据存储的格式，尽管从技术上讲我们完成可以改变它。推迟这个改变的原因与我们的*etcd v3*迁移计划有关。也许你正在奇怪，这个变化是如何与*etcd v3*迁移产生依赖的。原因是*etcd v2*中我们确实不能以二进制格式存储数据(为解决它 我们另外以*base64*对数据进行了编码 )，而在*etcd v3*中我们却可以。所以为了简化*etcd v3*迁移，同时避免迁移中对存储于*etcd*中的数据的重要转换，我们决定推迟将存储数据格式转为*protobuf*，直至*etcd v3*存储后端的迁移完成。

#### 其它的一些优化
------------------------------

在过去的三个版本中，我们对*Kubernetes*代码库做了几十项优化，包括：

- 优化*Scheduler*（产生**5至10倍**的*Scheduling*吞吐量提升）

- 使用共享*informers*将所有*controllers*转至一个新型的设计模式，这降低了*controller-manager*对资源的消耗 — 请参考[文章](https://github.com/kubernetes/community/blob/master/contributors/devel/controllers.md)

- 优化了*API Server*中的单个操作（*conversions*, *deep-copies*, *patch*）

- 减少了*API Server*中的内存分配（这对*API*调用延时的影响非常大）

我们想强调的是，对过去几个版本的优化工作(贯穿整个项目的历史)，是由很多公司和来自Kubernetes社区的个人共同努力的结果。

接下来会是什么？

人们经常问在提升*Kubernetes*可扩展性的道路上我们会走多远。目前我们没有计划在后续的几个版本中发展节点超过5000个的集群（在SLO内）。如果你需要集群超过5000节点，我们建议使用[*federation*](https://kubernetes.io/docs/concepts/cluster-administration/federation/)来聚合多个*Kubernetes*集群。

然而，这并不意味着我们将停止对可扩展性和性能的工作。在本文开始时我们提到过，我们最优先的事情是改进现有的两个*SLO*以及引进新的能够覆盖更多领域的指标，比如*Networking*。这项工作已经在*Scalability SIG*开始进行了。关于该如何定义性能*SLO*，我们已取得了很大进展。下个月这项工作应该可以结束。


#### 加入我们
------------------------------

如果你对可扩展性和性能感兴趣，请加入我们的社区，帮助我们共同打造*Kubernetes*，你可以：

- 在[*Kubernetes Slack scalability channel*](https://kubernetes.slack.com/messages/sig-scale/)中与我们讨论

- 加入我们的特别兴趣小组，*SIG-Scalability*, 每周四*9:00 AM PST*聚会

感谢支持与贡献！阅读更多关于*Kubernetes 1.6*新特性的深度文章请点击[这里](http://blog.kubernetes.io/2017/03/five-days-of-kubernetes-1.6.html).


作者： *Wojciech Tyczynski, Software Engineer, Google*

原文链接：http://blog.kubernetes.io/2017/03/scalability-updates-in-kubernetes-1.6.html
