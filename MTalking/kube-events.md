Kubernetes Events之捉妖记（上）
==================

师出有名
--------
前些天群里有位同学提问说怎么通过API得到`kubectl describe pod`的结果，我立刻找到了Kubernetes相关的API并回复他，但他说这不是他要的东西。经过一番描述，我才了解到他想要的是原来如下图中红框里的信息：

![](https://github.com/maxwell92/TechTips/blob/master/MTalking/pics/kube-events.png)

Message属于Kubernetes的一种特殊的资源：Events。老实讲，我以前是没有怎么注意过这个Events是怎么来的，甚至一直觉得它应该是Pod的一部分。那么这个Events到底是什么样的资源？它从何而来？下面随我一起踏上捉妖之途。


捉妖不易
---------

#### 真身难觅 

熟悉Kubernetes的小伙伴应该对于它的资源比较有体会，作为调度基本单元的Pod是一种资源，控制Pod更新、扩容、数量的ReplicaSet是一种资源，作为发布单位的Deployment是一种资源，哦，还有Service，Endpoints等等。它们有这么一些共同点：

* 都可以通过`kubectl get $ResourceName`的方式获取
* 都有对应的RESTful API定义

Events亦如此，`kubectl get events`的结果为下图：

![](https://github.com/maxwell92/TechTips/blob/master/MTalking/pics/kubectl-get-events.png)

就像`kubectl get pods`一样，通过`kubectl get events`获得是当前命名空间下所有Events的列表。如果想查看某条Events的详细信息，是否也可以使用`kubectl describe events $EventsName`进行获取呢？我尝试了下，得到了下面的失败信息：

![](https://github.com/maxwell92/TechTips/blob/master/MTalking/pics/kubectl-describe-events-err03.png)

甚至连`kubectl get events $EventsName`也变得没那么好使了：

![](https://github.com/maxwell92/TechTips/blob/master/MTalking/pics/kubectl-describe-eventd-err01.png)

另外，Events的RESTful API为下图：

![](https://github.com/maxwell92/TechTips/blob/master/MTalking/pics/events-restapi.png)

而通过最朴素的*curl 之刃*也没能找出Events的真身：

![](https://github.com/maxwell92/TechTips/blob/master/MTalking/pics/curl-events-err.png)

这着实让人“大吃一斤”。 

#### 踏破铁鞋

那么这个Events到底是何方神圣？常规手段居然拿它没一丁点办法了？不，还有一招*json宝典*待我祭出使用。我们知道`kubectl get`一些资源的时候可以通过它的`-o json`参数得到该资源的json格式的信息描述，那么对上面的Events进行`kubectl get events -o json > /tmp/events.json`，得到了一个json数组，里面的每一条都对应着一个Events对象：

![](https://github.com/maxwell92/TechTips/blob/master/MTalking/pics/events-json.png)

Bingo!这就是我们要找的Events对应的实体json。仔细观察，图中红框里的名字正是`kubectl get events`里得到的Events名字，然而实际上它并不是真正的Events的名字。这是为何？在Kubernetes资源的数据结构定义中，一般都会包含一个`metav1.TypeMeta`和一个`ObjectMeta`成员，比如：

![](https://github.com/maxwell92/TechTips/blob/master/MTalking/pics/deployment-define.png)
![](https://github.com/maxwell92/TechTips/blob/master/MTalking/pics/pod-define.png)

`TypeMeta`里定义了该资源的类别和版本，对应我们平时写json文件或者yaml文件时的`kind: Pod`和`apiVersion: v1`。
`OjbectMeta`里定义了该资源的元信息，包括名称、命名空间、UID、创建时间、标签组等。

Events的数据结构定义里同样包含了`metav1.TypeMeta`和`ObjectMeta`，那么从前面的json图中可以确定红框里的名字并不是该Events对象的真实名字，它对应的是`InvolvedObject`的名字，而蓝框里对应的才是Events的真实名字。然后使用`kubectl get`和`curl`进行验证：

![](https://github.com/maxwell92/TechTips/blob/master/MTalking/pics/kubectl-events-succ.png)
![](https://github.com/maxwell92/TechTips/blob/master/MTalking/pics/curl-events-succ.png)

到此，我们已经探明了Events的真名。仔细观察它的名字，发现它由三部分构成："发生该Events的Pod名称" + "." + "神秘数字串"。发生该Events的Pod名称倒是好理解，而后面的神秘数字串却不知从何而来。既然这个Events是来自这个Pod的状态变化，那么想必这个数字串也可以在Pod的详情里找到吧。出乎意料的是在Pod的详情json未曾找到这个神秘数字串，甚至把这个Pod祖上的ReplicaSet和Deployment也翻了个底朝天也是没有一点线索。捉妖行动陷入僵局。

#### 廿八星宿之Kubelet

道高一尺，魔高一丈。纵使齐天大圣也得经常搬救兵，而不少时候这个妖怪就是某个大仙的坐骑或童子。拿起这个Events的json仔细观察，发现里面有个不起眼的`component: kubelet`：

![](https://github.com/maxwell92/TechTips/blob/master/MTalking/pics/Kubelet-show.png)

回想一下kubelet的作用是：负责管理和维护在这台主机上运行着的所有容器，维持pod的运行状态(status)和它的期望值(spec)一致。kubelet启动时，会加载它本身的配置信息、接入容器运行时(Docker或Rkt)、以及定义kubelet与整个集群进行交互所需的信息。在启动后，会进行一系列的初始化工作，创建ContainerManager、设置OOMScoreAdj值、创建DiskSpaceManager、PodManager等等。那么会不会是kubelet为这个Pod创建了该Events呢？

为了解答这个问题，不得不祭出究极“源码大法”。

果然在Kubernetes源码的kubernetes/pkg/kubelet/events/event.go里有了一些收获。event.go里定义了是Events资源常用的一些常量，挑选列出如下：

```go
const (
    FailedToStartContainer  = "Failed"
    BackOffStartContainer   = "BackOff"    
    ErrImageNeverPullPolicy = "ErrImageNeverPull"
    BackOffPullImage        = "BackOff"
    FailedSync = "FailedSync"
    ...
)
```

这里面的不少我们平时都会经常遇到。它们是各种事件的原因列表，包含容器事件（创建、启动、失败等）、镜像事件（镜像拉取失败等）、kubelet事件（节点失效、节点不可调度等）、Pod Worker事件（同步失败）等。随便拿出一条进行暴力搜寻，比如"FailedToStartContainer"，发现除了在此处的定义进行了引用外，还在kubernetes/pkg/kubelet/dockertools/docker_manager.go里711行进行了引用:

```go
    dm.recorder.Eventf(ref, api.EventTypeWarning, events.FailedToStartContainer, "Failed to start container with docker id %v with error: %v", utilstrings.ShortenString(createResp.ID, 12), err)
```

由此更加确认的是Events跟Kubelet密不可分。循着这条线索一直往下，还会有什么惊奇的发现？

#### 火眼金睛

上面引用了docker_manager.go里的一行，除开事件本身外，里面还有一些关键词值得琢磨：dm, recorder, Eventf。

先来看dm。dm是结构体DockerManager的实例，结构体定义也在这个文件中。定义比较长，这里就不贴出来了，它包含了DockerInterface的成员、EventRecorder的成员、OSInterface的成员以及cadvisorapi.MachineInfo的成员等等。这个EventRecorder的定义在k8s.io/kubernetes/pkg/client/record/event.go里，是一个接口，可以按照EventSource的表现记录相应的事件。

```go
type EventRecorder interface {
    Event(object runtime.Object, eventtype, reason, message string)
    Eventf(object runtime.Object, eventtype, reason, messageFmt string, args ...interface{})
    PastEventf(object runtime.Object, timestamp unversioned.Time, eventtype, reason, messageFmt string, args ...interface{})
}
```

在这个文件的285~295行有对这个三个接口的实现：

```go
func (recorder *recorderImpl) Event(object runtime.Object, eventtype, reason, message string) {
    recorder.generateEvent(object, unversioned.Now(), eventtype, reason, message)
}

func (recorder *recorderImpl) Eventf(object runtime.Object, eventtype, reason, messageFmt string, args ...interface{}) {
    recorder.Event(object, eventtype, reason, fmt.Sprintf(messageFmt, args...))
}

func (recorder *recorderImpl) PastEventf(object runtime.Object, timestamp unversioned.Time, eventtype, reason, messageFmt string, args ...interface{}) {
    recorder.generateEvent(object, timestamp, eventtype, reason, fmt.Sprintf(messageFmt, args...))
}
```

不管是PastEventf()、Eventf()还是Event()最终都指向了辅助函数generateEvent()，它的实现在255行：

```go
func (recorder *recorderImpl) generateEvent(object runtime.Object, timestamp unversioned.Time, eventtype, reason, message string) {
    // 获取发生事件的对象，比如前面说到的Pod
    ref, err := api.GetReference(object)
    if err != nil {
        glog.Errorf("Could not construct reference to: '%#v' due to: '%v'. Will not report event: '%v' '%v' '%v'", object, err, eventtype, reason, message)
        return
    }

    // 验证事件类型
    if !validateEventType(eventtype) {
        glog.Errorf("Unsupported event type: '%v'", eventtype)
        return
    }
    
    // 生成Event
    event := recorder.makeEvent(ref, eventtype, reason, message)
    event.Source = recorder.source

    go func() {
        // NOTE: events should be a non-blocking operation
        defer utilruntime.HandleCrash()
        // 将Event进行广播
        recorder.Action(watch.Added, event)
    }()
}
```

recorder.makeEvent()就是这个Events的真正来源：

```go
func (recorder *recorderImpl) makeEvent(ref *api.ObjectReference, eventtype, reason, message string) *api.Event {
    t := unversioned.Time{Time: recorder.clock.Now()}
    namespace := ref.Namespace
    if namespace == "" {
        namespace = api.NamespaceDefault
    }
    return &api.Event{
        ObjectMeta: api.ObjectMeta{
            // 事件的命名规则
            Name:      fmt.Sprintf("%v.%x", ref.Name, t.UnixNano()), 
            Namespace: namespace,
        },
        InvolvedObject: *ref,
        Reason:         reason,
        Message:        message,
        FirstTimestamp: t,
        LastTimestamp:  t,
        Count:          1,
        Type:           eventtype,
    }
}

```
到此恍然大悟，原来前面所说的Events的命名规则："发生该Events的Pod名称" + "." + "神秘数字串"实际上是"ref.Name" + "." + "t.UnixNano())"。

终于找到了Events的真身，原来它是Kubelet负责用来记录多个容器运行过程中的事件，命名由被记录的对象和时间戳构成。前面看起来难以捉摸的表现最终还是逃不过火眼金睛。到此Kubernetes Events之捉妖记（上）解决了Events从哪儿来的问题，暂告一段落。在下篇中将对Events关于到哪儿去、怎么根据Pod找到对应的等进行深度拷问。欢迎继续关注！


参考经卷及说明
------------
* [Docker容器与容器云 浙江大学SEL实验室著](https://item.jd.com/12052716.html)
* [Kubernetes 1.2源码](https://github.com/kubernetes/kubernetes)

本文中出现的*curl 之刃*、*json宝典*、*源码大法*均属于为了轻松阅读体验而凭空捏造的词汇，分别对应*用命令行工具curl进行访问和实验*、*查看该对象的json信息*、*阅读源码寻找答案释疑*。

