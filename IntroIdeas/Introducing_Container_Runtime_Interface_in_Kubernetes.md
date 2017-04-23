Introducing Container Runtime Interface(CRI) in Kubernetes
============================================================

Kubernetes容器运行时接口(CRI)简介
===============================
编者注：本文属于[Kubernetes 1.5 新特性深度解析](http://blog.kubernetes.io/2016/12/five-days-of-kubernetes-1.5.html)系列文章。

Kubernetes节点的底层由一个叫做“容器运行时”的软件进行支撑，它负责比如启停容器这样的事情。最广为人知的容器运行时当属Docker，但它不是唯一。事实上，容器运行时这个领域发展迅速。为了使Kubernetes的扩展变得更容易，我们一直在打磨支持容器运行时的K8s插件API：容器运行时接口(Container Runtime Interface, CRI)。

### 什么是CRI？Kubernetes为什么需要它？

每种容器运行时各有所长，许多用户都希望Kubernetes支持更多的运行时。在Kubernetes 1.5发布版里，我们引入了CRI--一个能让kubelet无需编译就可以支持多种容器运行时的插件接口。CRI包含了一组[protocol buffers](https://developers.google.com/protocol-buffers/)，[gRPC API](http://www.grpc.io/)，相关的[库](https://github.com/kubernetes/kubernetes/tree/release-1.5/pkg/kubelet/server/streaming)，以及在活跃开发下的额外规范和工具。CRI目前是Alpha版本。

支持可替换的容器运行时在Kubernetes中概念中并不新鲜。在1.3发布版里，我们介绍了[rktnetes](http://blog.kubernetes.io/2016/07/rktnetes-brings-rkt-container-engine-to-Kubernetes.html)项目，它可以让[rkt容器引擎](https://github.com/coreos/rkt)作为Docker容器运行时的一个备选。然而，不管是Docker还是Rkt都需要通过内部、不太稳定的接口直接集成到kubelet的源码中。这样的集成过程要求十分熟悉kubelet内部原理，并且还会在Kubernetes社区引发巨大的维护反响。这些因素都在为容器运行时的初期造成了巨大的困难。我们通过提供一个清晰定义的抽象层消除了这些障碍，开发者可以专注于构建他们的容器运行时。这是很小的一步，但对于真正提供可插拔的容器运行时和构建一个更健康的生态系统却意义非凡。

### CRI总览

Kubelet与容器运行时通信（或者是CRI插件填充了容器运行时）时，Kubelet就像是客户端，而CRI插件就像对应的服务器。它们之间可以通过Unix 套接字或者gRPC框架进行通信。

![](https://github.com/maxwell92/TechTips/blob/master/IntroIdeas/pics/overview-cri.png)

protocol buffers [API](https://github.com/kubernetes/kubernetes/blob/release-1.5/pkg/kubelet/api/v1alpha1/runtime/api.proto)包含了两个gRPC服务：ImageService和RuntimeService。ImageService提供了从镜像仓库拉取、查看、和移除镜像的RPC。RuntimeSerivce包含了Pods和容器生命周期管理的RPC，以及跟容器交互的调用(exec/attach/port-forward)。一个单块的容器运行时能够管理镜像和容器（例如：Docker和Rkt），并且通过同一个套接字同时提供这两种服务。这个套接字可以在Kubelet里通过标识--container-runtime-endpoint和--image-service-endpoint进行设置。

### Pod和容器生命周期管理

```go
    // Sandbox operations.
    rpc RunPodSandbox(RunPodSandboxRequest) returns (RunPodSandboxResponse) {}
    rpc StopPodSandbox(StopPodSandboxRequest) returns (StopPodSandboxResponse) {}
    rpc RemovePodSandbox(RemovePodSandboxRequest) returns (RemovePodSandboxResponse) {}
    rpc PodSandboxStatus(PodSandboxStatusRequest) returns (PodSandboxStatusResponse) {}
    rpc ListPodSandbox(ListPodSandboxRequest) returns (ListPodSandboxResponse) {}
    // Container operations.
    rpc CreateContainer(CreateContainerRequest) returns (CreateContainerResponse) {}
    rpc StartContainer(StartContainerRequest) returns (StartContainerResponse) {}
    rpc StopContainer(StopContainerRequest) returns (StopContainerResponse) {}
    rpc RemoveContainer(RemoveContainerRequest) returns (RemoveContainerResponse) {}
    rpc ListContainers(ListContainersRequest) returns (ListContainersResponse) {}
    rpc ContainerStatus(ContainerStatusRequest) returns (ContainerStatusResponse) {}
    ...
}
```

在资源受限的隔离环境里的一组应用容器组成一个Pod。在CRI，这个环境被称为PodSandbox。我们故意留下一些空间，让容器运行时根据它们内部不同的原理来产生不同的PodSandbox。对于基于hypervisor的运行时，PodSandbox可能代表的是虚拟机。对于其他的，比如Docker，它可能是Linux命名空间。这个PodSandbox一定遵循着Pod的资源定义。在v1alpha1版API里，kubelet将创建pod级的cgroup限制下的一组进程，并传递给容器运行时，由此实现。

在Pod启动前，kubelet调用RuntimeService.RunPodSandbox来创建环境，包括为Pod设置网络（例如：分配IP）等。当PodSandbox启动后，就可以分别创建/启动/停止/移除独立的容器。为了删除Pod，kubelet会在停止和移除所有容器前先停止和移除PodSandbox。

Kubelet负责通过RPC来进行容器生命周期的管理，测试容器生命周期钩子和健康/可读性检查，同时为Pod提供重启策略。

### 为什么需要以容器为中心的接口？

Kubernetes拥有对Pod资源的声明式API。我们认为一个可能的设计是为了使CRI能够在它的抽象里重用这个声明式的Pod对象，给容器运行时实现和测试达到期望状态的逻辑的自由。这会极大地简化API，并让CRI可以兼容更广泛的运行时。在早期的设计阶段我们讨论过这个方法，但由于几个原因否决了它。首先，Kubelet有许多Pod级的特性和特定的技术（比如crash-loop backoff逻辑），这会成为所有运行时重新实现时的巨大负担。其次，越来越重要的是，Pod的定义更新快速。只要kubelet直接管理容器，那么许多新特性（比如init container）不需要底层容器运行时做任何改变。CRI包含了一个必要的容器级接口，这样运行时就可以共享这些特性，拥有更快的开发速度。当然这并不意味着我们偏离了"level triggered"哲学。kubelet负责保证实际状态到期望状态的变化。

### Exec/attach/port-forward requests

```go
service RuntimeService {
    ...
    // ExecSync runs a command in a container synchronously.
    rpc ExecSync(ExecSyncRequest) returns (ExecSyncResponse) {}
    // Exec prepares a streaming endpoint to execute a command in the container.
    rpc Exec(ExecRequest) returns (ExecResponse) {}
    // Attach prepares a streaming endpoint to attach to a running container.
    rpc Attach(AttachRequest) returns (AttachResponse) {}
    // PortForward prepares a streaming endpoint to forward ports from a PodSandbox.
    rpc PortForward(PortForwardRequest) returns (PortForwardResponse) {}
    ...
}
```

Kubernetes提供了一些用户可以与Pod及其内部容器进行交互的特性（例如kubectl exec/attach/port-forward）。Kubelet现在通过调用容器原生的方法或使用节点上可用的工具（例如nsenter和socat）来支持这些特性。在节点上使用这些工具不是一个可移植的好办法，因为这些工具的大部分假定Pod是通过Linux命名空间进行隔离的。在CRI，我们显式定义了这些调用，允许特定的运行时实现。

另外一个潜在的问题是，kubelet如今的实现是kubelet处理所有的流式连接请求。所以这会给节点的网络流量带来瓶颈。在设计CRI的时候，我们采纳了这个反馈，支持运行时防范中间人。容器运行时可以启动一个对应请求的单独的流服务器（甚至可能为Pod审计资源使用），并且将地址返回给Kubelet。Kubelet然后将这个信息再返回给Kubernetes API Server，它会打开直接与运行时提供的服务器相连的流连接，并将它跟客户端连通。

这篇博文并没有包含CRI的全部。更多细节请参考[设计文档和建议](https://github.com/kubernetes/community/blob/master/contributors/devel/container-runtime-interface.md#design-docs-and-proposals)。

### 当前状态

尽管CRI还处于早期阶段，已经有不少使用CRI来集成容器运行时的项目在开发中。下面是一些列子：

* [cri-o](https://github.com/kubernetes-incubator/cri-o): OCI运行时
* [rktlt](https://github.com/kubernetes-incubator/rktlet): rkt容器运行时
* [frakti](https://github.com/kubernetes/frakti): 基于hypervisor的容器运行时
* [docker CRI shim](https://github.com/kubernetes/kubernetes/tree/release-1.5/pkg/kubelet/dockershim)

如果你对上面列出的这些运行时感兴趣，你可以关注这些独立的github仓库，获取最新的进展和说明。

对集成新的容器运行时感兴趣的开发者，请参考[开发指南](https://github.com/kubernetes/community/blob/master/contributors/devel/container-runtime-interface.md)了解这些API的限制和问题。我们会从早期的开发者那里积极采纳反馈来提升API。开发者可能会遇到API的一些意外改动（毕竟是Alpha版）。

### 试着集成CRI-Docker

Kubelet至今还没有默认使用CRI，但我们仍在极地推动。第一步就是使用CRI将Docker重新集成到kubelet里。在1.5发布版里，我们扩展了Kubelet来支持CRI，并且为Docker添加了内置的CRI插件。kubelet启动一个gRPC服务器，代表Docker。如果尝试新的kubelet-CRI-Docker集成，你可能仅仅会使用--feature-gates=StreamingProxyRedirects=true来打开Kubernetes API Server，以启动新的流重定向特性，并且通过设置kubelet的标识--experimental-cri=true来启动。

除了一些[欠缺的特性](https://github.com/kubernetes/community/blob/master/contributors/devel/container-runtime-interface.md#docker-cri-integration-known-issues)，新的集成可以一直通过主要的端到端测试。我们计划尽快扩展测试的覆盖率，并且鼓励社区反应关于这个转化的任何问题。

### CRI和Minikube

如果你想要尝试新的集成，但是没有时间在云上启动一个新的测试集群。[minikube](https://github.com/kubernetes/minikube)是一个很棒的工具，你可以迅速在本地搭建集群。在你开始前，请阅读说明并下载安装minikube。

1. 检查可用的Kubernetes版本，选择可用的最新1.5.x版本。我们使用v1.5.0-beta.1作为示例。

```shell
$ minikube get-k8s-versions
```

2. 通过内建的Docker CRI集成启动一个Minikube集群。
```shell $  minikube start --kubernetes-version=v1.5.0-beta.1 --extra-config=kubelet.EnableCRI=true --network-plugin=kubenet --extra-config=kubelet.PodCIDR=10.180.1.0/24 --iso-url=http://storage.googleapis.com/minikube/iso/buildroot/minikube-v0.0.6.iso ``` 
--extra-config=kubelet.EnableCRI=true开启了kubelet的CRI实现。--network-plugin=kubenet和--extra-config=kubelet.PodCIDR=10.180.1.0/24设置Kubenet网络插件，保证分配给节点的PodCIDR。--iso-url设置本地节点启动的minikube iso镜像。

3. 检查minikube日志，查看启动CRI

```shell
$ minikube logs | grep EnableCRI
I1209 01:48:51.150789    3226 localkube.go:116] Setting EnableCRI to true on kubelet.
```

4. 创建pod，检查它的状态。你应该可以看见一个"SandboxReceived"事件，证明Kubelet在使用CRI

```shell
$ kubectl run foo --image=gcr.io/google_containers/pause-amd64:3.0
deployment "foo" created
$ kubectl describe pod foo
...
... From                Type   Reason          Message
... -----------------   -----  --------------- -----------------------------
...{default-scheduler } Normal Scheduled       Successfully assigned foo-141968229-v1op9 to minikube
...{kubelet minikube}   Normal SandboxReceived Pod sandbox received, it will be created.
...
```

注意：kubectl attach/exec/port-forward还不能在minikube的CRI中运行。但这会[在新版本的minikube中得到改善](https://github.com/kubernetes/minikube/issues/896)。 

### 社区

CRI的开发很活跃，由Kubernetes SIG-Node兴趣小组维护。我们热切地期盼你的回复。加入社区吧：

* 通过[Github](https://github.com/kubernetes/kubernetes)反馈问题和特性请求
* 加入[Slack](https://kubernetes.slack.com/)上的#sig-node频道
* 参与[SIG-Node](kubernetes-sig-node@googlegroups.com)邮件列表
* 关注我们[Twitter @Kubernetesio](https://twitter.com/kubernetesio)的后续更新

[原文作者](Yu-Ju Hong, Software Engineer, Google)
[原文链接](http://blog.kubernetes.io/2016/12/container-runtime-interface-cri-in-kubernetes.html)

