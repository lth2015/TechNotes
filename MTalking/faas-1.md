FaaS: 又一个未来？ 
----------------

### 什么是FaaS

云计算时代出现了大量XaaS形式的概念，从IaaS(Infrastructure as a Service)、PaaS(Platform as a Service)、SaaS(Software as a Service)到容器云引领的CaaS(Containers as a Service)，再到火热的微服务架构，它们都在试着将基础设施或平台等作为一种服务提供开发者使用，让他们不再担心基础设施、资源以及各种中间件等等，在减轻心智负担的同时更好地专注于业务。FaaS是Functions as a Service的简称，它往往和无服务架构(Serverless Architecture)被一同提起。

Serverless的概念刚刚出现在HackerNews时并不为大众所接受。后来随着微服务和事件驱动架构的发展才慢慢引起关注。Serverless并不是说没有服务器参与，它通过将复杂的服务器架构透明化，使开发者专注于“要做什么”，从而强调了减少开发者对服务器等计算资源的关注、工作粒度从服务器切换到任务的思想。在2006年第一个支持“随用随付”的代码执行平台Zimki问世。2014年亚马逊AWS推出了Lambda成为最主要的无服务架构的代表。接着Google、IBM和Microsoft也纷纷推出了各自支持Serverless的平台。

微服务架构近年来是一个非常火爆的话题，大大小小的公司都开始逐步分拆原来的单体应用，试着转换到由各个模块服务组合成大型的复杂应用。Serverless可以看作是比微服务架构更细粒度的架构模式，即FaaS。Lambda也是FaaS的典型代表，它允许用户仅仅上传代码而无需提供和管理服务器，由它负责代码的执行、高可用扩展，支持从别的AWS服务或其他Web应用直接调用等。以电子商务应用为例，微服务中可以将浏览商品、添加购物车、下单、支付、查看物流等拆分为解耦的微服务。在FaaS里，它可以拆分到用户的所有CRUD操作代码。当发生“下单”事件时，将触发相应的Functions，交由Lambda执行。人们在越来越多的场景里将Serverless和FaaS等同起来。

![](https://github.com/maxwell92/TechTips/blob/master/MTalking/pics/speed-up-the-platform.jpg)

假设有下面的JavaScript代码：

```js
module.exports = function(context, callback) { callback(200, "Hello, world!
"); }
```

它是一个函数，通过FaaS的方式，我们可以通过访问一个URL的方式调用这个函数。

```shell
$ curl -XGET localhost:8080 
Hello, World
```

FaaS拥有下面的特点：

1. FaaS里，应用逻辑的每个单元都可以看作是一个函数，开发人员只关注如何实现这些逻辑，而不用提前考虑性能优化，工作聚焦在这个函数里，而非应用整体。

2. FaaS是无状态的。在云原生(Cloud Native App)应用该满足的12因子(12 Factors)中一条正是无状态，而FaaS而言天生满足。无状态意味着本地内存、磁盘里的数据无法被后续的操作所使用。大部分的状态需要依赖于外部存储，比如数据库、网络存储等。

3. FaaS的功能生命周期应当是很短的，它们会在有限的时间里处理任务，并在返回执行结果后终止。如果它执行时间超过了某个阈值，也应该被终止。

4. 函数启动延时受很多因素的干扰。以AWS Lambda为例，如果采用了JS或Python实现了函数，它的启动时间一般不会超过10~100毫秒。但如果是实现在JVM上的函数，当遇到突发的大流量或者调用间隔过长的情况，启动时间会显著变长。

5. 借助于API Gateway可以将请求的路由和对应的处理函数进行映射，并将响应结果返回给调用方。API Gateway还会扮演认证、输入校验等的角色。


比如对于一个Web应用，在这里后端系统实现了大部分业务逻辑：认证、搜索、事务等，它的架构如下：
![](https://github.com/maxwell92/TechTips/blob/master/MTalking/pics/3-tiers-web-application.png)

如果采用Serverless架构，将认证、数据库等采用第三方的服务，从原来的单体后端里分拆出来，需要的话在原来的客户端里加入了一些业务逻辑。对于计算敏感型的搜索功能，也不再使用一直在线的服务器进行支持，如此一来它看起来就清晰多了：

![](https://github.com/maxwell92/TechTips/blob/master/MTalking/pics/web-serverless.png)

这么拆分之后很好地实现了Clean Code里单一职责原则(Single Responsibility Principle, SRP)。不仅如此，FaaS带来的好处还有：

* 开支减少。通过购买共享的基础设施，同时减少了花费在运维上的人力成本，最终减少了开支。
* 负担减轻。不再需要重复造轮子，需要什么功能直接集成调用即可，也无需考虑整体的性能，只专注于业务代码的实现。
* 易于扩展。云上提供了自动的弹性扩展，用了多少计算资源，就购买多少，完全按需付费。
* 简化管理。自动化的弹性扩展、减少了打包和部署的复杂度、快速推向市场化都让管理变得简单高效。
* 环保计算。即使在云的环境上，仍习惯于购买多余的服务器，最终倒置空闲。Serverless杜绝了这种情况。


### Kubernetes 与 FaaS

Fission是一款基于Kubernetes的FaaS框架。通过Fission可以轻而易举地将函数发布成HTTP服务。它在源码级工作，并抽象出容器镜像。同时它还帮助减轻了Kubernetes的学习前，无需了解太多K8s也可以搭建出实用的服务。Fission目前支持NodeJS和Python，并且可以与HTTP路由、Kubernetes Events和其他的事件触发器结合。这些函数只有在运行的时候才会消耗CPU和内存。

Kubernetes提供了强大的弹性编排系统，拥有易于理解的后端API和不断发展壮大的社区。Fission将容器编排功能交给了K8s，专注于FaaS特性。

对于FaaS来说，它最重要的两个特性是将函数转换为服务，同时管理服务的生命周期。有很多办法可以实现这两个特性，但需要考虑一些问题，比如“框架运行在源码级？还是Docker镜像？”，“第一次运行的负载多少能接受”，不同的选择会影响到平台的扩展性、易用性、资源使用以及性能等问题。

为了使Fission足够易用，它运行在源码级。用户不再参与镜像构建、推仓库、镜像认证、镜像版本等过程。但源码级的接口不允许用户打包二进制依赖。Fission采用的方式是在镜像内部放置动态的函数加载工具。这让用户可以在源码层操作，同时在需要的时候可以定制镜像。这些镜像在Fission里叫做“环境镜像”。包含了特定语言的运行时，一组常用的依赖和函数的动态加载工具。如果这些依赖已经足够，就直接使用这个镜像，不够的话需要重新导入依赖并构建镜像。

环境镜像是Fission中唯一与语言相关的部分。可以把它看做是框架里其余部门的统一接口。所以Fission可以更加容器扩展，就像VFS一样。

FaaS的一个目标是在运行的时候消费资源。这只是优化了函数运行时的资源使用，而冷启动的时候可能会有些资源使用过载。比如对于用户登录的过程，多等几秒都是不可接受的。

为了改变这个问题，Fission在任何环境里都维持了一个容器池。当有函数进来时，Fission无需启动新容器，直接从池里取一个，将函数拷贝到容器里，动态加载，并将请求路由到对应的实例。

![](https://github.com/maxwell92/TechTips/blob/master/MTalking/pics/fission-on-k8s.png)

Fission设计为一组微服务: 

* Controller: 记录了函数、HTTP路由、事件触发器和环境镜像
* Pool Manager: 管理环境容器，加载函数到容器，函数实例空闲时杀掉
* Router: 接受HTTP请求，并路由到对应的函数实例，必要的话从Pool Manager中请求容器实例

Controller支持Fission的API，其他的组件监视controller的更新。Router暴露为K8s里的LoadBalancer或NodePort类型的服务，这依赖于K8s集群放在哪里。

当Router收到请求，先去缓存Cache里查看该是否请求一个已经存在的服务。如果没有，然后访问请求映射的服务函数，并向Pool Manager申请一个容器实例。Pool Manager拥有一个空闲Pod池。它选择一个Pod，并把函数加载到里面（通过向容器里的Sidecar发送请求实现），并且把Pod的地址返回给router。router会把请求转发到该Pod。Pod会被缓存起来以应对后续的请求。如果空闲了几分钟，它就会被杀死。

目前，Fission将一个函数映射为一个容器，对于自动扩展为多个实例的特性在后续版本里。以及重用函数Pods来支持多个函数也在计划中，在这种情况下隔离不是必须的。

对于较小的REST API来说，Fission是个很好的选择，通过实现webhooks，可以为Slack或其他服务编写chatbots

在Github上提供了一个通讯录例子，可以使用函数来想通讯录读取和添加记录。通过Redis来保持状态。该应用有两个端点，一个GET是从Redis里读出通讯录，并渲染为HTML。POST是向通讯录里添加新的记录。这就是全部了，没有Dockerfile，更新appp

Fission同时还支持根据Kubernetes的Watch机制来出发函数的执行。例如你可以设置一个函数来watch某个命名空间下所有满足某个标签的pod，这个函数将得到序列化的对象和这个上下文的Watch Event类型(added/removed/updated)。这些事件处理函数可以用于简单的监控，例如你可以设置当任意一个服务添加到集群时向Slack发送一条消息。也有更复杂的应用，例如编写一个watching Kubernetes第三方资源(Third Party Resource)的自定义controller。

在Fission的官网上有个入门的例子：

```shell
$ cat hello.js
module.exports = function(context, callback) {
    callback(200, "Hello, world!\n");
}

# Upload your function code to fission
$ fission function create --name hello --env nodejs --code hello.js

# Map GET /hello to your new function
$ fission route create --method GET --url /hello --function hello

# Run the function.  This takes about 100msec the first time.
$ curl http://$FISSION_ROUTER/hello
Hello, world! 
```

如果是第一次运行，还需要准备NodeJS的运行环境：

```shell
# Add the stock NodeJS env to your Fission deployment
$ fission env create --name nodejs --image fission/node-env
```

阅读Fission的源码， 可以很清晰地看到它的执行过程：

1. fission env create --name nodejs --image fission/node-env

由fission主程序执行命令env和子命令create，通过--name指定语言为NodeJS，通过--image指定镜像为fission/node-env，通过HTTP的POST /v1/environments请求向controller发送环境信息JSON。controller拿到这个JSON先获取一个UUID进行标记，然后将放到ETCD里。由此完成了环境资源的存储。

![](fission-env) 

2. fission function create --name hello --env nodejs --code hello.js

同样，由fission主程序执行命令functino和子命令create，通过--name参数指定函数名为hello，--env参数确定环境，--code参数确定要执行的函数代码。通过POST向/v1/functions发出请求，携带函数信息的JSON。controller拿到JSON后进行函数资源的存储。首先将代码写到文件里，写之前会拿到UUID。然后写到文件名为该UUID的文件里。急着向ETCD的API发送HTTP请求，在file/name路径下有序存放UUID。最后同env，将UUID和序列化后的JSON数据写到etcd里。

3. fission route create --method GET --url /hello --function hello

通过参数--method指定请求所需方法为GET，--url指定API路由为hello，--function指定对应执行的函数为hello。通过POST请求向/v1/triggers/http发出请求，将路由和函数的映射关系信息发送到controller。controller会在已有的trigger列表里进行重名检查，如果不重复，才会获取UUID并将序列化后的JSON数据写到etcd里。

前面的都是由本地的fission程序完成的。我们已经预先创建了fission的deployment和service。它启动了fission-bundle。它创建了名为fission的命名空间，并在里面启动4个Deployment，分别是controller, router, poolMgr, etcd，并创建NodePort类型的服务controller和router，分别监听端口31313和31314。同时创建另一个名为fission-function的命名空间用来运行执行函数的Pod.

router使用Cache维护着一份function到service的映射，同时还有trigger集合(有个goroutine通过controller保持对这个trigger集合的更新），在启动时初始化路由按照路由添加trigger里的url和针对对应函数的handler。



4. curl http://$FISSION_ROUTER/hello

当执行该语句时，请求发送至router容器。收到请求后会转发到两个对应的handler。一个是用户定义的面向外部的，一个是内部的。实际上它们执行的是同一个handler。不管是什么handler都会先根据函数名去Cache里查找对应的service名。如果没有命中，将通过poolmgr为函数创建新的Service，并把记录添加到Cache。最后生成一个反向代理，接收外部请求，然后转发至Kubernetes Service。

Poolmgr创建新的service时，需要根据env创建Pod pool(初始大小为3个副本的deployment),然后从中随机选择一个Ready的Pod。再为此建立对应的Service。



在2017年1月份，Fission还处于alpha版，我们正在努力让Kubernetes上的FaaS更加方便，易于使用和轻松集成。在未来几个月我们将添加单元测试、与Git集成、函数监控和日志聚合。我们也会跟其他的Events进行集成。对了，还有为更多的语言创建环境，现在支持NodeJS和Python， 初步支持C# .NET

你可以在Github上找到roadmap

参与进来
Fission是一个开源项目，由Platform 9进行开发。在Github上关注，加入Slack，或者粉Twitter

### Fission 使用










通过`kubectl create`命令可以安装Fission。




### xx

FaaS可能是未来的一种走势，但一定不会是最终的未来。总有一天FaaS又会被其他技术所代替。生活在这个信息爆炸、技术飞速更迭的时代很烦恼也很幸福。但这就是我们所在的时代，我们正在用自己的双手打造着未来。

### 参考


[Function as a Service](https://en.wikipedia.org/wiki/Function_as_a_Service)
[Thinking Serverless](http://highscalability.com/blog/2017/1/30/part-1-of-thinking-serverlesshow-new-approaches-address-mode.html)
[Serverless Architecture](https://martinfowler.com/articles/serverless.html#unpacking-faas)
[Fission: Serverless Functions as a Service for Kubernetes](http://blog.kubernetes.io/2017/01/fission-serverless-functions-as-service-for-kubernetes.html)
[Fission Official Site](http://fission.io/)
[Fission Github](https://github.com/fission/fission)








![](serverless-web-application)

*此处引用了MartinFlower的图*

再如一个消息驱动的应用，比如在线广告应用，当用户点击广告时广告服务器需要同步响应用户(跳转到广告页)，同时向消息队列里发送一条消息(异步处理)，以便日后和广告商进行结算。如下图：

![](adserver-original)

在Serverless架构里它会是这样：

![](adserver-serverless)



FaaS的优点：



Serverless改变了下游软件部署和维护的理念，提高了软件开发人员的效率，但它没有减少软件生命周期中的环节。
### 在k8s上运行Fission, a voting demo

在Martin Flower的专栏里有这样的描述：

* Serverless architectures refer to applications that significantly depend on third-party services(AKA Backend as a Service or "BaaS") or on custom code that is run ephmemeral containers (Function as a Service or "FaaS") *

前面也提到了FaaS的每个功能都拥有很短的生命周期，容器作为任务运行的基本单位，非常适合FaaS的场景。

