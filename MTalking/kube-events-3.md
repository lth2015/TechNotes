Kubernetes Events之捉妖记（下）
==================

经过前两回的“踏雪寻妖”，一个完整的Events原形逐渐浮出水面。我们已经摸清了它的由来和身世，本回将一起探索Events的去向，是一个终点，又是另一个起点。

#### 蜜汁去向

前面已经了解到，Event是由一个叫EventRecorder的东西幻化而生。通过研究源码经典发现，在Kubelet启动的时候获取一个EventBroadcaster的实例，以及根据KubeletConfig获取一个EventRecorder实例。EventRecorder自不必多说。EventBroadcaster用来接收Event并且把它们转交给EventSink、Watcher和Log。

EventBroadcaster定义了包括四个方法的一组接口，分别是：

```golang
    // 将收到的Events交于相应的处理函数
    StartEventWatcher(eventHandler func(*api.Event)) watch.Interface

    // 将收到的Events交于EventSink
    StartRecordingToSink(sink EventSink) watch.Interface

    // 将收到的Events交于相应的Log供日志输出
    StartLogging(logf func(format string, args ...interface{})) watch.Interface

    // 初始化一个EventRecorder，并向EventBroadcaster发送Events
    NewRecorder(source api.EventSource) EventRecorder
```

EventBroadcaster由定义在kubernetes/pkg/client/record/event.go里的NewBroadcaster()方法进行初始化，实际上靠调用kubernetes/pkg/watch/mux.go里的NewBroadcaster()方法实现。在定义里，每一个EventBroadcaseter都包含一列watcher，而对于每个watcher，都监视同一个长度为1000的Events Queue，由此保证分发时队列按Events发生的时间排序。但是同一个Events发送至Watcher的顺序得不到保证。为了防止短时间内涌入的Events导致来不及处理，每个EventBroadcaster都拥有一个长度为25的接收缓冲队列。定义的最后指定了队列满时的相应操作。

当完成初始化并加入waitGroup之后，EventBroadcaster便进入无限循环。在这个循环中，Broadcaster会不停地从缓冲队列里取走Event。如果获取失败就将退出循环，并清空所有的watcher。如果获取成功就将该Event分发至各个watcher。在分发的时候需要加锁，如果队列已满则不会阻塞，直接跳过到下一个watcher。如果队列未满，则会阻塞，直到写入后再分发下一个watcher。

在Kubelet运行过程初始化EventBroadcaster之后，如果KubeletConfig里的EventClient不为空，即指定对应的EventSink(EventSink是一组接口，包含存储Events的Create、Update、Patch方法，实际由对应的Client实现):

```golang
    eventBroadcaster.StartRecordingToSink(&unversionedcore.EventSinkImpl{Interface: kcfg.EventClient.Events("")})
```

StartRecordingToSink()方法先根据当前时间生成一个随机数发生器randGen，接着实例化一个EventCorrelator，最后将recordToSink()函数作为处理函数，实现了StartEventWatcher。StartLogging()类似地将用于输出日志的匿名函数作为处理函数，实现了StartEventWatcher。

#### 总钻风StartEventWatcher

StartEventWatcher()首先实例化watcher，每个watcher都被塞入该Broadcaster的watcher列表中，并且新实例化的watcher只能获得后续的Events，不能获取整个Events历史。入队列的时候加锁以保证安全。接着启动一个goroutine用来监视Broadcaster发来的Events。EventBroadcaster会在分发Event的时候将所有的Events都送入一个ResultChan。watcher不断从ResultChan取走每个Event，如果获取过程发送错误，将Crash并记录日志。否则在获得该Events后，交于对应的处理函数进行处理。

StartEventWatcher()方法使用recordToSink()函数作为处理。因为同一个Event可能被多个watcher监听，所以在对Events进行处理前，先要拷贝一份备用。接着同样使用EventCorrelator对Events进行整理，然后在有限的重试次数里通过recordEvent()方法对该Event进行记录。

recordEvent()方法试着将Event写到对应的EventSink里，如果写成功或可无视的错误将返回true，其他错误返回false。如果要写入的Event已经存在，就将它更新，否则创建一个新的Event。在这个过程中如果出错，不管是构造新的Event失败，还是服务器拒绝了这个event，都属于可无视的错误，将返回true。而HTTP传输错误，或其他不可预料的对象错误，都会返回false，并在上一层函数里进行重试。在kubernetes/pkg/client/record/event.go里指定了单个Event的最大重试次数为12次。另外，为了避免在master挂掉之后所有的Event同时重试导致不能同步，所以每次重试的间隔时间将随机产生(第一次间隔由前面的随机数发生器randGen生成)。

#### 小钻风EventCorrelator

EventCorrelator定义包含了三个成员，分别是过滤Events的filterFunc，进行Event聚合的aggregator以及记录Events的logger。它负责处理收到的所有Events，并执行聚合等操作以防止大量的Events冲垮整个系统。它会过滤频繁发生的相似Events来防止系统向用户发送难以区分的信息和执行去重操作，以使相同的Events被压缩为被多次计数单个Event。

EventCorrelator通过NewEventCorrelator()函数进行实例化:

```golang
func NewEventCorrelator(clock clock.Clock) *EventCorrelator {
    cacheSize := maxLruCacheEntries
    return &EventCorrelator{
        // 默认对于所有的Events均返回false，表示不可忽略
        filterFunc: DefaultEventFilterFunc,
        aggregator: NewEventAggregator(
            // 大小为4096
            cacheSize,
            // 通过相同的Event域来进行分组
            EventAggregatorByReasonFunc,
            // 生成"根据同样的原因进行分组"消息
            EventAggregatorByReasonMessageFunc,
            // 每个时间间隔里最多统计10个Events
            defaultAggregateMaxEvents,
            // 最大时间间隔为10mins
            defaultAggregateIntervalInSeconds,
            clock),
        logger: newEventLogger(cacheSize, clock),
    }
}
```

Kubernetes的Events可以按照两种方式分类：相同和相似。相同指的是两个Events除了时间戳以外的其他信息均相同。相似指的是两个Events除了时间戳和消息(message)以外的其他信息均相同。按照这个分类方法，为了减少Event流对etcd的冲击，将相同的Events合并计数和将相似的Events聚合，提出“最大努力”的Event压缩算法。最大努力指的是在最坏的情况下，N个Event仍然会产生N条Event记录。

每个Event对象包含不只一个时间戳域：FirstTimestamp、LastTimestamp，同时还有统计在FirstTimestamp和LastTimestamp之间出现频次的域Count。同时对于每个可以产生Events的组件，都需要维持一个生成过的Event的历史记录：通过Least Recently Used Cache实现。

EventCorrelator的主要方法是EventCorrelate()，每次收到一个Event首先判断它是否可以被跳过(前面提过默认均不可忽略)。然后对该Event进行Aggregate处理。

EventCorrelator包含两个子组件：EventAggregator和EventLogger。EventCorrelator检查每个接收到的Event，并让每个子组件可以访问和修改这个Event。其中EventAggregator对每个Event进行聚合操作，它基于`aggregateKey`将Events进行分组，组内区分的唯一标识是`localKey`。默认的聚合函数将event.Message作为`localKey`，使用event.Source、event.InvolvedObject、event.Type和event.Reason一同构成`aggregateKey`。

aggregator是类型EventAggregator的一个实例，定义如下：

```golang
type EventAggregator struct {
    // 读写锁
    sync.RWMutex

    // 存放整合状态的Cache
    cache *lru.Cache

    // 用来对Events进行分组的函数、
    keyFunc EventAggregatorKeyFunc

    // 为整合的Events生成消息的函数
    messageFunc EventAggregatorMessageFunc

    // 每个时间间隔里可统计的最大Events数
    maxEvents int

    // 相同的Events间最大时间间隔以及一个时钟
    maxIntervalInSeconds int

    clock clock.Clock
}

```

* 通过EventAggregatroKeyFunc，EventAggregator会将10mins内出现过10次的相似Event进行整合：丢弃作为输入的Event，并且创建一个仅有Message区别的新Event。这条Message标识这是一组相似的Events，并且会被后续的Event操作序列处理。
* EventLogger观察相同的Event，并通过在Cache里与它关联的计数来统计它出现的次数。

在Cache里的Key是Event对象除去Timestamp/Counts等剩余部分构成的。下面的任意组合都可以唯一构造Cache里Event唯一的Key：

* event.Source.Component
* event.Source.Host
* event.InvolvedObject.Kind
* event.InvolvedObject.Namespace
* event.InvolvedObject.Name
* event.InvolvedObject.UID
* event.InvolvedObject.APIVersion
* event.Reason
* event.Message

不管对于EventAggregator或EventLogger，LRU Cache大小仅为4096。这也意味着当一个组件（比如Kubelet）运行很长时间，并且产生了大量的不重复Event，先前产生的未被检查的Events并不会让Cache大小继续增长，而将最老的Event从Cache中排除。当一个Event被产生，先前产生的Event Cache会被检查:

* 如果新产生的Event的Key跟先前产生的Event的Key相匹配（意味着前面所有的域都相匹配），那么它被认为是重复的，并且在etcd里已存在的这条记录将被更新。
    * 使用PUT方法来更新etcd里存放的这条记录，仅更新它的LastTimestamp和Count域。
    * 同时还会更新先前生成的Event Cache里对应记录的Count、LastTimestamp、Name以及新的ResourceVersion。
* 如果新产生的Event的Key并不能跟先前产生的Event相匹配（意味着前面所有的域都不匹配），这个Event将被认为是新的且是唯一的记录，并写入etcd里。
    * 使用POST方法来在etcd里创建该记录
    * 对该Event的记录同样被加入到先前生成的Event Cache里

当然这样还存在一些问题。对于每个组件来说，Event历史都存放在内存里，如果该程序重启，那么历史将被清空。另外，如果产生了大量的唯一Event，旧的Event将从Cache里去除。只有从Cache里去除的Event才会被压缩，同时任何一个此Event的新实例都会在etcd里创建新记录。

举个例子，下面的`kubectl`结果表示有20条相互独立的Event记录(请看表示调度错误的记录:Scheduling Failure)被压缩至5条。

```shell
FIRSTSEEN                         LASTSEEN                          COUNT               NAME                                          KIND                SUBOBJECT                                REASON              SOURCE                                                  MESSAGE
Thu, 12 Feb 2015 01:13:02 +0000   Thu, 12 Feb 2015 01:13:02 +0000   1                   kubernetes-node-4.c.saad-dev-vms.internal     Node                                                         starting            {kubelet kubernetes-node-4.c.saad-dev-vms.internal}     Starting kubelet.
Thu, 12 Feb 2015 01:13:09 +0000   Thu, 12 Feb 2015 01:13:09 +0000   1                   kubernetes-node-1.c.saad-dev-vms.internal     Node                                                         starting            {kubelet kubernetes-node-1.c.saad-dev-vms.internal}     Starting kubelet.
Thu, 12 Feb 2015 01:13:09 +0000   Thu, 12 Feb 2015 01:13:09 +0000   1                   kubernetes-node-3.c.saad-dev-vms.internal     Node                                                         starting            {kubelet kubernetes-node-3.c.saad-dev-vms.internal}     Starting kubelet.
Thu, 12 Feb 2015 01:13:09 +0000   Thu, 12 Feb 2015 01:13:09 +0000   1                   kubernetes-node-2.c.saad-dev-vms.internal     Node                                                         starting            {kubelet kubernetes-node-2.c.saad-dev-vms.internal}     Starting kubelet.
Thu, 12 Feb 2015 01:13:05 +0000   Thu, 12 Feb 2015 01:13:12 +0000   4                   monitoring-influx-grafana-controller-0133o    Pod                                                          failedScheduling    {scheduler }                                            Error scheduling: no nodes available to schedule pods
Thu, 12 Feb 2015 01:13:05 +0000   Thu, 12 Feb 2015 01:13:12 +0000   4                   elasticsearch-logging-controller-fplln        Pod                                                          failedScheduling    {scheduler }                                            Error scheduling: no nodes available to schedule pods
Thu, 12 Feb 2015 01:13:05 +0000   Thu, 12 Feb 2015 01:13:12 +0000   4                   kibana-logging-controller-gziey               Pod                                                          failedScheduling    {scheduler }                                            Error scheduling: no nodes available to schedule pods
Thu, 12 Feb 2015 01:13:05 +0000   Thu, 12 Feb 2015 01:13:12 +0000   4                   skydns-ls6k1                                  Pod                                                          failedScheduling    {scheduler }                                            Error scheduling: no nodes available to schedule pods
Thu, 12 Feb 2015 01:13:05 +0000   Thu, 12 Feb 2015 01:13:12 +0000   4                   monitoring-heapster-controller-oh43e          Pod                                                          failedScheduling    {scheduler }                                            Error scheduling: no nodes available to schedule pods
Thu, 12 Feb 2015 01:13:20 +0000   Thu, 12 Feb 2015 01:13:20 +0000   1                   kibana-logging-controller-gziey               BoundPod            implicitly required container POD        pulled              {kubelet kubernetes-node-4.c.saad-dev-vms.internal}     Successfully pulled image "kubernetes/pause:latest"
Thu, 12 Feb 2015 01:13:20 +0000   Thu, 12 Feb 2015 01:13:20 +0000   1                   kibana-logging-controller-gziey               Pod                                                          scheduled           {scheduler }                                            Successfully assigned kibana-logging-controller-gziey to kubernetes-node-4.c.saad-dev-vms.internal

```

#### 小结

到此基本上捋出了Events的来龙去脉：Event由Kubernetes的核心组件Kubelet和ControllerManager等产生，用来记录系统一些重要的状态变更。ControllerManager里包含了一些小controller，比如deployment_controller，它们拥有EventBroadCaster的对象，负责将采集到的Event进行广播。Kubelet包含一些小的manager，比如docker_manager，它们会通过EventRecorder输出各种Event。当然，Kubelet本身也拥有EventBroadCaster对象和EventRecorder对象。

EventRecorder通过generateEvent()实际生成各种Event，并将其添加到监视队列。我们通过`kubectl get events`看到的NAME并不是Events的真名，而是与该Event相关的资源的名称，真正的Event名称还包含了一个时间戳。Event对象通过InvolvedObject成员与发生该Event的资源建立关联。Kubernetes的资源分为“可被描述资源”和“不可被描述资源”。当我们`kubectl describe`可描述资源，比如Pod时，除了获取Pod的相应信息，还会通过FieldSelector获取相应的Event列表。Kubelet在初始化的时候已经指明了该Event的Source为Kubelet。

EventBroadcaster会将收到的Event交于各个处理函数进行处理。接收Event的缓冲队列长为25，不停地取走Event并广播给各个watcher。watcher由StartEventWatcher()实例产生，并被塞入EventBroadcaster的watcher列表里，后实例化的watcher只能获取后面的Event历史，不能获取全部历史。watcher通过recordEvent()方法将Event写入对应的EventSink里，最大重试次数为12次，重试间隔随机生成。

在写入EventSink前，会对所有的Events进行聚合等操作。将Events分为相同和相似两类，分别使用EventLogger和EventAggregator进行操作。EventLogger将相同的Event去重为1个，并通过计数表示它出现的次数。EventAggregator将对10分钟内出现10次的Event进行分组，依据是Event的Source、InvolvedObject、Type和Reason域。这样可以避免系统长时间运行时产生的大量Event冲击etcd，或占用大量内存。EventAggregator和EventLogger采用大小为4096的LRU Cache，存放先前已产生的不重复Events。超出Cache范围的Events会被压缩。

#### 后记

这篇文章共150行文字，虽然我写了整整一天，但是并没有成为“捉妖”系列的完美收官之作，系列三篇文章也仍没有完完整整地梳理出Event的全貌。一个小小的Event研究起来却这么复杂，让我想起探花兄曾经说过“我们要时刻保持敬畏之心，不管对人还是对技术”。不管是学术上，还是工程上，每种技术的实现和发展无不凝聚了很多人的智慧和汗水，Kubernetes这样庞大的系统更是。我还只是刚“识字”的初学者，更有必要时刻保持敬畏之心。跟Event的故事仍未完结，后面的文章会继续围绕Event展开，敬请关注！


参考经卷
------------
* [Docker容器与容器云 浙江大学SEL实验室著](https://item.jd.com/12052716.html)
* [Kubernetes 1.2源码](https://github.com/kubernetes/kubernetes)
* [Kubernetes Event Compression](https://github.com/kubernetes/kubernetes/blob/master/docs/design/event_compression.md)


