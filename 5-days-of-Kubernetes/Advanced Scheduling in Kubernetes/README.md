Kubernetes 1.6新特性系列 | 高级调度
===================================


> 导读：*Kubernetes 1.6*高级调度的新特性主要集中在三个方面：
>   * Node的亲和性和反亲和性(*Affinity/Anti-Affinity*)
>   * Node的污点和容忍(*Taints and Tolerations*)
>   * Pod的亲和性和反亲和性(*Affinity/Anti-Affinity*)
>   * 自定义调度器

作者注：这是深入*Kubernetes 1.6*特性系列的第四篇。

*Kubernetes*的调度器在大多数情况下能过运行的很好，例如：它能够将*Pod*调度到有充足资源的*Node*上；它能够将一组*Pod*(*ReplicaSet*, *StatefulSet*,等)均匀的调度到不同的*Node*上；它尽力平衡各个节点的资源使用率等。

但有些时候您会想控制您的Pod如何调度，例如：可能您想让一些Pod被确定调度到某些使用特殊硬件的节点上，或者您想让交互频繁的服务一起调度，或者您想让一些*Node*只给特性的一些用户提供服务等等。最终，您对于应用程序调度和部署的需求永远要比*Kubernetes*提供的多。因此，*Kubernetes 1.6*提供了四个高级高级调度功能：**节点亲和性和反亲和性**，**污点和容忍**，**Pod亲和性/反亲和性**和**自定义调度**。这四个特性在*Kubernetes 1.6*版本中都是*beta*版。


### *Node*亲和性/反亲和性
---------------------------

[*Node*亲和性/反亲和性](https://kubernetes.io/docs/user-guide/node-selection/#node-affinity-beta-feature)是在*Node*上设置如何被*Scheduler*选择的规则一种方式。此功能是自*Kubernetes 1.0*版本以来在*Kubernetes*中的*nodeSelector*的功能的通用化。规则是使用在*Pod*中指定和选择器上自定义的标签等用户熟悉的概念定义的，并且他们是**必需的**或者**首选**的，这取决于您希望调度程序强制执行他们的严格程度。

#### 必需的规则(Required)
---------------------------
只有满足必需的规则的*Pod*才会被调度到特定的*Node*上。如果没有*Node*匹配条件(加上所有其他所有正常的条件，例如为*Pod*请求提供足够的可用资源），否则*Pod*不会被调度。必需满足的规则在*nodeAffinity*的`requiredDuringSchedulingIgnoredDuringExecution`字段中指定。

例如，如果我们要求在多可用区域([*Multiple Zones*](https://kubernetes.io/docs/admin/multiple-zones/))的`us-central1-a`(*GCE*)区域中的节点上进行调度，则可以将以下的关联规则指定为*Pod*规范(*Spec*)的一部分：

```yaml
affinity:
  nodeAffinity:
    requiredDuringSchedulingIgnoredDuringExecution:
      nodeSelectorTerms:
        - matchExpressions:
          - key: "failure-domain.beta.kubernetes.io/zone"
            operator: In
            values: ["us-central1-a"]
```

`"IgnoredDuringExecution"`意味着如果*Node*上的标签发生更改，并且亲和性的规则不再满足。这个在未来会计划实现。

`"requiredDuringSchedulingRequiredDuringExecution"`意味着一旦他们不满足节点亲和性规则，将从*Node*上驱逐不再匹配规则的*Pod*。

#### 首选的规则
----------------------

首选规则意味着如果节点与规则匹配，则将优先选择它们，并且仅当没有优选节点可用时才选择非优选节点。 您可以选择使用**首选**规则，而不是通过**必需**规则强制将*Pod*部署到*GCE*的`us-central1-a`区域中的节点上。使用首选规则，则需指定`preferredDuringSchedulingIgnoredDuringExecution`：

```yaml
affinity:
  nodeAffinity:
    preferredDuringSchedulingIgnoredDuringExecution:
      nodeSelectorTerms:
        - matchExpressions:
          - key: "failure-domain.beta.kubernetes.io/zone"
            operator: In
            values: ["us-central1-a"]
```

*Node*的反亲和性能够使用负操作符(*NotIn*, *DoesNotExist*等)来表示。下面的例子说明了如何禁止您的*Pod*被调度到`us-central1-a`的区域中：

```yaml
affinity:
  nodeAffinity:
    requiredDuringSchedulingIgnoredDuringExecution:
      nodeSelectorTerms:
        - matchExpressions:
          - key: "failure-domain.beta.kubernetes.io/zone"
            operator: NotIn
            values: ["us-central1-a"]
```

可以使用的操作符有：*In*, *NotIn*, *Exists*, *DoesNotExist*, *Gt*, 和*Lt*。

这个特性还有一些另外的使用场景，比如需要在调度上严格区别*Node*上的**硬件架构**，**操作系统版本**，或者**专用的硬件**等。 

*Node*的亲和性和反亲和性在*Kubernetes 1.6*版本中是*Beta*版。


### 污点和容忍(*Taints and Tolerations*)
--------------------

此功能允许您标记一个*Node*(“受污染”，“有污点”），以便没有*Pod*可以被调度到此节点上，除非*Pod*明确地“容忍”污点。标记的是*Node*而不是*Pod*（如节点的亲和性和反亲和性），对于集群中大多数*Pod*应该避免调度到特定的节点上的功能特别有用，例如，您可能希望主节点（*Master*）标记为仅可调度*Kubernetes*系统组件，或将一组节点专用于特定的用户组，或者让常规的*Pod*远离具有特殊硬件的*Node*，以便为有特殊硬件需求的*Pod*留出空间。

使用`kubectl`命令可以设置节点的“污点”，例如：

```bash
kubectl taint nodes node1 key=value:NoSchedule
```

创建一个污点并标记到*Node*，那些没有设置容忍的*Pod*（通过*key-value*方式设置*NoSchedule*，这是其中一个选项）不能调度到该*Node*上。其他污点的选项是*PerferredNoSchedule*，这是*NoSchedule*首选版本；还有*NoExecute*，这个选项意味着在当*Node*被标记有污点时，该*Node*上运行的任何没有设置容忍的*Pod*都将被驱逐。容忍将被添加到*PodSpec*中，看起来像这样：

```yaml
tolerations: 
  - key: "key"
    operator: "Equal"
    value: "value"
    effect: "NoSchedule"
```

除了将污点和容忍(*Taints and Tolerations*)特性在*Kubernetes 1.6*中移至*Beta*版外，我们还引入了一个使用污点和容忍的*Alpha*的特性：允许用户自定义一个*Pod*被绑定到*Node*上后遇到了诸如网络分区的问题时的行为（可能*Pod*希望长时间允许在这个*Node*上，或者网络分区会很快恢复），而不是现在默认的等待五分钟超时。更详细的信息，请参阅[文档](https://kubernetes.io/docs/user-guide/node-selection/#per-pod-configurable-eviction-behavior-when-there-are-node-problems-alpha-feature)。


### *Pod*的亲和性和反亲和性
------------------------

*Node*的亲和性和反亲和性允许您基于节点的标签来限制*Pod*的调度行为。但是，如果您想要指定*Pod*的亲和关系时这种方法就无法奏效了。例如，如何实现在一个服务或者相关的服务中聚合*Pod*或将Pod均匀分布？为此，您可以使用*Pod*的亲和性和反亲和性特性，它在*Kubernetes 1.6*中也是*Beta*版。

我们来看一个例子。假设您在服务*S1*中有个前端应用，它经常与服务*S2*的后端应用进行通信。因此，您希望这两个服务共同位于同一个云提供商的区域中，但您不需要手动选择区域（如果有些区域暂时不可用），希望将*Pod*重新调度到另一个（单个的）区域。您可以像这样指定*Pod*的亲和性和反亲和性规则（假设您将该服务的*Pod*命名为*”service=S2"*，另外一个服务的*Pod*为*"service=S1"）：

```yaml
affinity:
    podAffinity:
      requiredDuringSchedulingIgnoredDuringExecution:
      - labelSelector:
          matchExpressions:
          - key: service
            operator: In
            values: [“S1”]
        topologyKey: failure-domain.beta.kubernetes.io/zone
```

如同*Node*的亲和性和反亲和性，也有一个`preferredDuringSchedulingIgnoredDuringExecution`变量。

*Pod*的亲和性和反亲和性非常灵活。想象一下，您已经分析了您的服务的性能，发现来自服务*S1*的容器会干扰运行在同一*Node*上的服务*S2*的容器，这可能是由于缓存干扰效应或者网络连接饱和引起，或者也许是由于安全性问题，您不会想要*S1*和*S2*的容器共享一个*Node*。要实现这些规则，只需对上述代码段进行两次更改即可将*podAffinity*更改为*podAntiAffinity"，并将*topologyKey*更改为*kubernetes.io/hostname*即可。

### 自定义调度器
-------------------

如果*Kubernetes Scheduler*的这些特性没能足够满足您控制负载的需求，您可以在将任意子集的*Pod*的调度任务交给您自己的自定义调度器，自定义的调度器可以跟默认的调度器（*Kubernetes Scheduler*）一起运行，或者替代默认的调度器。[多调度器(*Multiple schedulers*)](https://kubernetes.io/docs/admin/multiple-schedulers/)在*Kubernetes 1.6*中是*Beta*版。

在默认的情况下，每个新的*Pod*都使用默认的调度器进行调度。但是如果您提供了自定义调度器的名称，默认的调度器会忽略*Pod*的调度请求，允许您的自定义调度器对*Pod*进行调度。看看下面的例子：

在Pod的规范(*Spec*)中制定调度器的名字：

```yaml
apiVersion: v1
kind: Pod
metadata:
  name: nginx
  labels:
    app: nginx
spec:
  schedulerName: my-scheduler
  containers:
  - name: nginx
    image: nginx:1.10
```

如果我们在没有部署自定义调度器的情况下创建了这个*Pod*，结果会是默认的调度器将忽略这个*Pod*，它将处于*Pending*状态。因此，我们需要一个自定义调度程序来查找和调度其*schedulerName*字段为*my-scheduler*的*Pod8*。

自定义调度语言可以用任何语言编写，调度策略根据需要可以简单也可以复杂。这是一个非常简单的例子，它使用*Bash*编写，它可以为*Pod*随机分配一个*Node*。请注意，您需要让它与*kubectl proxy*一起运行：

```bash
#!/bin/bash
SERVER='localhost:8001'
while true;
do
    for PODNAME in $(kubectl --server $SERVER get pods -o json | jq '.items[] | select(.spec.schedulerName == "my-scheduler") | select(.spec.nodeName == null) | .metadata.name' | tr -d '"')
;
    do
        NODES=($(kubectl --server $SERVER get nodes -o json | jq '.items[].metadata.name' | tr -d '"'))
        NUMNODES=${#NODES[@]}
        CHOSEN=${NODES[$[ $RANDOM % $NUMNODES ]]}
        curl --header "Content-Type:application/json" --request POST --data '{"apiVersion":"v1", "kind": "Binding", "metadata": {"name": "'$PODNAME'"}, "target": {"apiVersion": "v1", "kind"
: "Node", "name": "'$CHOSEN'"}}' http://$SERVER/api/v1/namespaces/default/pods/$PODNAME/binding/
        echo "Assigned $PODNAME to $CHOSEN"
    done
    sleep 1
done
```

*Kubernetes 1.6*的[*release notes*](https://github.com/kubernetes/kubernetes/blob/master/CHANGELOG.md#v160)关于这个特性有更多的介绍，包括您已经使用了这些（其中一个或者多个）特性的*Alpha*版本改如何进行配置的细节（这些事必需的，因为对于这些特性，从*Alpha*到*Beta*的转变是突破性的）。

### 致谢
----------------------

这里描述的功能，包括*Alpha*和*Beta*版本，都是社区的工程师的努力，他们来自*Google*, *Huawei*, *IBM*, *Rad Hat*等公司。

### 加入我们
------------------

通过每周的[社区会议](https://github.com/kubernetes/community/blob/master/communication.md#weekly-meeting)发出您的声音：

* 在[*Stack Overflow*](http://stackoverflow.com/questions/tagged/kubernetes)上提问或者回答问题

* 在*Twitter*关注我们[*@Kubernetesio*](https://twitter.com/kubernetesio)

* 在[*Slack*](http://slack.k8s.io/)(room: #sig-scheduling )上参与社区的讨论


非常感谢您所做的贡献！

作者：

--Ian Lewis, Developer Advocate, and David Oppenheimer, Software Engineer, Google


原文链接：

http://blog.kubernetes.io/2017/03/advanced-scheduling-in-kubernetes.html
