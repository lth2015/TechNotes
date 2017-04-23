Kubernetes Events之捉妖记（中）
==================

上一回，历经千辛万苦，终于破解了Events的姓名之谜，寻得Events真身。那么Events的身世究竟如何？根据Pod怎样才能找到对应的Events？本回将为各位揭开谜底。

身世之谜
----------------

#### 顺藤摸瓜

前面说到在DockerManager里定义了EventRecorder的成员，它的方法Event()、Eventf()、PastEventf()都可以用来构造Events实例，略有区别的地方是Eventf()调用了Sprintf()来输出Events message，PastEventf()可创建指定时间发生的Events。

一方面可以推测所有拥有EventsRecorder成员的Kubernetes资源定义都可以产生Events。经过暴力搜索发现，EventsRecorder主要被K8s的重要组件ControllerManager和Kubelet使用。比如，负责管理注册、注销等的NodeController，会将Node的状态变化信息记录为Events。DeploymentController会记录回滚、扩容等的Events。他们都在ControllerManager启动时被初始化并运行。与此同时Kubelet除了会记录它本身运行时的Events，比如：无法为Pod挂载卷、无法带宽整型等，还包含了一系列像docker_manager这样的小单元，它们各司其职，并记录相关Events。

另一方面在调查的时候发现，Events分为两类，并定义在kubernetes/pkg/api/types.go里，分别是EventTypeNormal和EventTypeWarning，它们分别表示该Events“仅表示信息，不会造成影响”和“可能有些地方不太对”。

在types.go里，还找到了Event数据结构的定义：

```golang
type Event struct {
    unversioned.TypeMeta `json:",inline"`
    ObjectMeta           `json:"metadata,omitempty"`

    // Required. The object that this event is about.
    InvolvedObject ObjectReference `json:"involvedObject,omitempty"`

     Reason string `json:"reason,omitempty"`
     Message string `json:"message,omitempty"`
     Source EventSource `json:"source,omitempty"`
     FirstTimestamp unversioned.Time `json:"firstTimestamp,omitempty"`
     LastTimestamp unversioned.Time `json:"lastTimestamp,omitempty"`
     Count int32 `json:"count,omitempty"`
     Type string `json:"type,omitempty"`
}
```

除了标准的Kubernetes资源必备的`unversioned.TypeMeta`和`ObjectMeta`成员外，Event结构体还包含了Events相关的对象、原因、内容、消息源、首次记录时间、最近记录时间、记录统计和类型。

另外还定义了EventsList的结构类型，这就是我们使用`kubectl get events`和`GET /api/v1/namespaces/{namespace}/events`获取Events列表时K8s使用的数据结构。

在Events的定义里，比较重要的有两个成员，一个是InvolvedObject， 另一个是Source。

首先，InvolvedObject表示的是这个Events所属的资源。它的类型是ObjectReference，定义如下：

```golang
type ObjectReference struct {
    Kind            string    `json:"kind,omitempty"`
    Namespace       string    `json:"namespace,omitempty"`
    Name            string    `json:"name,omitempty"`
    UID             types.UID `json:"uid,omitempty"`
    APIVersion      string    `json:"apiVersion,omitempty"`
    ResourceVersion string    `json:"resourceVersion,omitempty"`
    FieldPath string `json:"fieldPath,omitempty"`
}
```

ObjectReference里包含的信息足够我们唯一确定该资源实例。

然后，Source表示的是该Events的来源，它的类型是EventSource，定义如下：

```golang
type EventSource struct {
    // Component from which the event is generated.
    Component string `json:"component,omitempty"`
    // Host name on which the event is generated.
    Host string `json:"host,omitempty"`
}
```

#### 来龙无去脉

前面的研究已经为我们大致画出了Events的内部轮廓。回到开始时的问题: 既然Events的名字跟发生它的Pod的名字不同，那么`kubectl describe pod`时如何找到对应的Events的？我们可以大胆推测，正是通过Events定义里的InvolvedObject成员来锁定了它们之间的关系。

在前面分析`kubeadm`原理的文章中已经介绍过Kubernetes的命令都是利用第三方包[Cobra](github.com/spf13/cobra)生成的，`kubectl describe`也不例外，它定义在kubernetes/pkg/kubectl/cmd/describe.go里。

Kubernetes中只有部分资源可以被`describe`，可以称为“DescribableResources”，通过一个资源类型(unversioned.GroupKind)和对应描述器(Describer)的Map映射相关联。这些资源有我们常见的Pod、ReplicationController、Service、Node、Deloyment、ReplicaSet等等。注意这些资源里并不包含Events。

显而易见，我们需要去仔细看看Pod的Describer做了什么。PodDescriber只有一个方法，那就是Describe()，实现在kubernetes/pkg/kubectl/describe.go里。

Describe()方法首先通过namespace和name唯一确定所请求的Pod。如果出错并且ShowEvents标识为true的情况下，会根据FieldSelector找到Events，并说明“获取Pod出错，但发现了Events”。如果请求Pod未出错且ShowEvents标识为true，则通过GetReference()方法找到相关的Events。

不管哪种方式，只要找到的Events不为空，总是会通过DescribeEvents()方法将Events列表按特定格式输出。即下图：

![](https://github.com/maxwell92/TechTips/blob/master/MTalking/pics/kube-events.png)

这么一说就明白了，原来我们在`kubectl describe pod`时得到的返回的结果不仅包含了Pod的信息，还有Events的信息，它们来自的是不同的处理过程。

到此我们已经摸清了Events的来龙。具体来说对于`describe pod`时看到的Events，它是由Kubelet的DockerManager生成，在执行`kubectl`命令时通过PodDescriber进行采集。显然如果我们不执行`kubectl`命令的时候这些Events仍然是存在的，那么这个时候这些Events会流向何处？也就说，我们还没捋顺Events的去脉。

Kubelet在启动的时候会初始化一个EventRecorder，这个EventRecorder又被交于Kubelet上每个小manager使用，比如DockerManager。它将产生的Events的Source成员进行初始化：Componets为“kubelet”，Host为该节点的名字。


#### 狡兔三窟

在追寻Events的去脉前，我们先来看看PodDescriber是如何采集这些Events的。

前面简单描述了PodDescriber的Describe()方法的作用，如果不够明朗，下面贴出它的源码：

```golang
func (d *PodDescriber) Describe(namespace, name string, describerSettings DescriberSettings) (string, error) {
    pod, err := d.Pods(namespace).Get(name)
    if err != nil {
        // 获取Pod失败时
        if describerSettings.ShowEvents {
            eventsInterface := d.Events(namespace)
            selector := eventsInterface.GetFieldSelector(&name, &namespace, nil, nil)
            options := api.ListOptions{FieldSelector: selector}
            events, err2 := eventsInterface.List(options)
            if describerSettings.ShowEvents && err2 == nil && len(events.Items) > 0 {
                return tabbedString(func(out io.Writer) error {
                    fmt.Fprintf(out, "Pod '%v': error '%v', but found events.\n", name, err)
                    DescribeEvents(events, out)
                    return nil
                })
            }
        }
        return "", err
    }

    var events *api.EventList
    // 获取Pod成功
    if describerSettings.ShowEvents {
        if ref, err := api.GetReference(pod); err != nil {
            glog.Errorf("Unable to construct reference to '%#v': %v", pod, err)
        } else {
            ref.Kind = ""
        // 通过Events().Search()获取
            events, _ = d.Events(namespace).Search(ref)
        }
    }

    return describePod(pod, events)
}
```

1. 获取Pod失败时
   Events的GetFieldSelector()方法同时根据InvolvedObject的名称、命名空间、资源类型和UID生成一个FieldSelector。使用它作为ListOptions，可以选中满足这个Selector对应的资源。如果选中的Events不为空，说明“获取Pod出错，但发现了Events”。的情况，并将其按照特定的格式打印。

2. 获取Pod成功，GetReference()失败
  GetReference()根据传入的K8s资源实例，构造它的引用说明。如果执行失败，记录失败日志，并直接执行describePod()，将目前获取的结果输出到屏幕上。

3. 获取Pod成功，GetReference()成功
  GetReference()成功后，调用Events的Search()方法，寻找关于该Pod的所有Events。最终执行describePod()，并将目前获取的结果输出到屏幕上。

当然，即使是Events的Search()方法，内部执行的仍是先GetFieldSelector()再Events List()的过程。这是因为DockerManager在生成Event的时候会调用它的makeEvent()方法（代码在上篇引用过，这里不再赘述），将Pod关联到该Events的InvolvedObject上。GetFieldSelector()返回的是一个field.Selector接口实例，它定义在kubernetes/pkg/fields/selector.go里，通过它的Matches()方法可以选中含有该Field且对应值相同的Events。

在Kubernetes里，FieldSelector和LabelSelector的设计异曲同工，不同的是Field匹配的是该资源的域，比如Name、Namespace，而Label匹配的是Labels域里的键值对。


参考经卷及说明
------------
* [Docker容器与容器云 浙江大学SEL实验室著](https://item.jd.com/12052716.html)
* [Kubernetes 1.2源码](https://github.com/kubernetes/kubernetes)


