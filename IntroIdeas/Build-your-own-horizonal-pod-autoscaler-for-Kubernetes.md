##为Kubernetes构建自动伸缩控制器(AutoScaler)
-----------------

Kubernetes的1.3版本包含了许多可以让你的容器化应用顺利运行的特性。例如水平Pod自动伸缩器（[Horizontal Pod Autoscaler: HPA](http://kubernetes.io/docs/user-guide/horizontal-pod-autoscaling/)），它虽小但很实用，现在支持通过CPU和内存消耗来进行伸缩控制（在[alpha](https://github.com/kubernetes/kubernetes/blob/release-1.3/docs/proposals/custom-metrics.md)版本里支持自定义伸缩度量）。

我们有一个应用是一个WebSocket服务器，它与客户端间都是非常“长”的连接。当我们对它进行性能测试时，发现性能瓶颈为25,000个活动的WebSocket连接，一旦超过它，我们的应用就会变得不稳定进而崩溃。在这个性能测试的时候，每个Pod并没有增加CPU负载或内存压力。这让我们有了通过WebSocket连接数进行伸缩的需求。这篇博客将讲述了我们在构建HPA时的收获。

关键字：Kubernetes; RxJS; Prediction; Scaling;

###Kubernetes原生HPA是怎么工作的
-----------------

阅读[Kubernetes关于HPA的源码](https://github.com/kubernetes/kubernetes/blob/c5e82a01b15136141801ba93de810fdf3165086b/pkg/controller/podautoscaler/horizontal.go#L133)后发现，目前它的实现非常“简单粗暴”：

1. 计算所有Pods的CPU利用率。
2. 基于targetUtilization计算所有Pod的数量。
3. 按照计算出的副本总数进行扩展。

我们想做得更好，所以自定义了HPA的一些目标如下：

1. 当前负载下应用不崩溃（即使负载超过了可用量）。
2. 快速扩展，如果需要可以过度扩展（overscale）。
3. 在扩展时考虑新应用实例的启动时间。
4. 逐渐收缩，直到当前负载低于最大可用量时停止。

###确保我们的应用不会崩溃
-----------------

为了保证我们的应用不崩溃，我们实现了“[可读性检测](http://kubernetes.io/docs/user-guide/pod-states/#container-probes)”：如果达到了WebSocket连接数限制，就将我们的Pod标记为NotReady。这使得Kubernetes负载均衡器不会再向这个Pod转发流量。一旦连接总数小于限制，这个Pod会被再次标记为Ready，并开始接收来自Kubernetes负载均衡器转发的流量。这个过程需要“手把手”地扩展Pod，不然新的请求最终会停留在负载均衡器，因为它的池里已经没有可用的Pod了。

###快速扩展
-----------------

当扩展的时候，我们是想要保证我们可以搞定不断增长的连接数。这样的扩展应该是迅速的，甚至会过度扩展。因为应用需要一些时间来启动生效，我们需要预测当扩展操作过程结束时最新的负载，并且还需要了解websocketConnectionCount的历史值。

我们开始想使用基于最后5个websocketConnectionCount值的线性预测，但是当连接总数按指数增加或减少时它会得到次优预测。我们然后使用[npm回归库](https://www.npmjs.com/package/regression)来做[二元多项式回归](https://en.wikipedia.org/wiki/Polynomial_regression#Matrix_form_and_calculation_of_estimates)，并找到一个满足我们的连接数变化方程。求解之后来为下个值进行预测。
![](https://github.com/maxwell92/TechTips/blob/master/IntroIdeas/pics/k8s-custom-hpa-scaling-up.png)
*点线是预测负载*


###逐渐收缩
-----------------

我们不基于预测进行收缩，因为可能在收缩的时候，这些Pod仍为当前负载所需要。又因为断掉的websocket连接会尝试重连，所以我们需要较为宽松的收缩策略。当我们检测到来自多项式回归预测值小于上一个websocketConnectionCount时，我们将它减少5%，并将它作为预测值。这样收缩时间就会变得更长，让我们准备好返回连接。

![](https://github.com/maxwell92/TechTips/blob/master/IntroIdeas/pics/k8s-custom-hpa-scaling-down.png)
*点线是减少5%后的值，因为预测比当前负载小*

如果超时，这些连接将不会返回，但我们仍然会以一个较低的速率收缩。

###执行Kubernetes伸缩操作
-----------------

因为我们自定义的HPA运行在同一个Kubernetes集群里，它可以在*/var/run/secrets/kubernetes.io/serviceaccount/token*取得一个Service令牌，以此来访问运行在主节点上的API。通过这个令牌我们访问API来应用一个HTTP patch请求，它向与包含我们应用Pods的deployment相关联的replicas发指令，完成伸缩操作。


###用RxJS合并操作
-----------------

我们使用[RxJS](https://github.com/Reactive-Extensions/RxJS)对未来事件流的进行函数组合，它的代码可读性挺高，如下：

```javascript
   const Rx = require('rx');
    const credentials = getKubernetesCredentials();

    Rx.Observable.interval(10 * 1000)
      .map(i => getMetricsofPods(credentials.masterUrl, credentials.token))
      .map(metrics => predictNumberOfPods(metrics, MAX_CONNECTIONS_PER_POD))
      .distinctUntilChanged(prediction => prediction)
      .map(prediction => scaleDeploymentInfiniteRetries(credentials.masterUrl, credentials.token, prediction))
      .switch()
      .subscribe(
        onNext => { },
        onError => {
          console.log(`Uncaught error: ${onError.message} ${onError.stack}`);
          process.exit(1);
        });
      // NOTE: getKubernetesCredentials(), getMetricsofPods(), predictNumberOfPods(), scaleDeploymentInfiniteRetries() left out for brevity
```

它真的是非常优雅：我们可以使用*map() + [switch()](https://github.com/Reactive-Extensions/RxJS/blob/master/doc/api/core/operators/switch.md)*来一直尝试伸缩deployment(+log errors)直到它成功或者收到初始化了另一个更新的伸缩请求。

###部分想法
-----------------

构建我们自己的HPA过程很有趣。使用Kubernetes API对于了解如何设计一个API非常有益。开始我们觉得开发自己的HPA工作量会很大，但看到这些拼在一起感觉也不错。使用RxJS试着描述你的代码流，而不需要杂乱的状态管理。综合来看，我们可以说我们的预测对于真实的连接处理得挺好。


##译者说
-----------------
Kubernetes在1.2版本以前，对Pod的扩展和收缩由Replica Controller完成。在1.2版本及以后，将逐渐由Replica Set来完成。

名词Pod、Deployments、Replicas为Kubernetes专有名词。

除了伸缩的精度（粒度、数量）外，伸缩的时机以及伸缩的对象也是进行伸缩操作的重要指标。

##原文链接
-----------------
[Building your own horizonal pod autoscaler for Kubernetes](http://markswanderingthoughts.nl/post/148836326495/building-your-own-horizontal-pod-autoscaler-for)
