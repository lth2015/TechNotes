Kubernetes的“优雅的停止”特性
=====================================================


Docker容器可能被随时终止，引发终止的原因包括自动扩/缩容策略、Pod或者Deployment升级或回滚过程中的删除操作。在很多时候，我们需要
让正在容器中运行的应用可以相对“优雅的”被停止。

我们遇到的实际情况是，想要所有当前的请求（或者任务）全部处理完成后再停止容器，需要优雅的停止的原因还有很多，
其中包括释放资源，分布式锁和打开的连接等等。


### 工作原理
------------------------------------------------------

当一个Pod将要被停止时：

* 每个容器的主进程（PID 1)会收到一个SIGTERM信号，和一个“优雅的时间周期”的倒计时器开始计时（默认是30秒，下面会讲如何修改它）

* 当主进程收到SIGTERM信号，每个容器开始执行优雅停止定义的任务然后退出

* 如果在这个“优雅的时间周期”内容器没有停止，会向它发送SIGKILL信号，强制该容器停止

更加详细的信息请查看下面的文档：

* (Kubernetes: Termination of pods)[http://kubernetes.io/docs/user-guide/pods/#termination-of-pods]

* (Kubernetes: Pods lifecycle hooks and termination notice)[http://kubernetes.io/docs/user-guide/production-pods/#lifecycle-hooks-and-termination-notice]

* (Kubernetes: Container lifecycle hooks)[http://kubernetes.io/docs/user-guide/container-environment/]

### 一个常见的错误:使用SIGTERM信号。
------------------------------------------------------

我们在编写Dockerfile通常会使用CMD命令来结束shell表单：

```bash
CMD myapp
```

这个shell表单实际运行/bash/sh -c myapp命令，所以收到信号的进程是/bin/sh而不是它的子进程
myapp。信号能不能被传递给子进程，要由实际运行的脚本决定。例如，在默认的情况下，继承自Alpine
Linux的容器不会将信号传递给它的子进程，但是Bash会传递。如果您的shell没有将信号传递给子进程，
您可以通过如下方法来确保子进程收到信号。

#### 方法一：从exec表单执行CMD命令
------------------------------------------------------

可以显式的在exec表单中使用CMD命令，它会运行myapp而不是/bin/sh -c myapp，但是它不允许传递环境变量作为参数。

```bash
CMD ["myapp"]
```

#### 方法二：使用Bash运行命令
------------------------------------------------------

确认您的容器包含了Bash并且使用Bash运行您的命令，这么做的目的是支持环境变量做为参数。

```bash
CMD ["/bin/bash", "-c", "myapp --arg=$ENV_VAR"]
```

###　如何修改“优雅的时间周期”
------------------------------------------------------

在默认的情况下，“优雅的时间周期”是30秒，很多时候，默认的参数不一定适合特定的场景，可以通过下面两种方式来改变默认值：　

1. 在Deployment .yaml中修改

2. 使用命令行，挡在运行kubectl delete时

### Deployment
------------------------------------------------------

您可以定制“优雅的时间周期”，通过修改Pod定义中sepc下面的terminationGracePeriodSeconds值来实现。例如，
下面的yaml文件将举例说明，如何在一个简单的Deployment配置文件中将默认的“优雅的时间周期”设置为60秒。

```yaml
apiVersion: extensions/v1beta1
kind: Deployment
metadata:
    name: test
spec:
    replicas: 1
    template:
        spec:
            containers:
              - name: test
                image: ...
            terminationGracePeriodSeconds: 60
``` 

### 命令行
------------------------------------------------------

您还可以在手工执行kubectl delete命令时改变默认的“优雅时间周期”：加上--grace-period=SECONDS选项，例如：

```bash
kubectl delete deployment test --grace-period=60
```

### 备选方案
------------------------------------------------------

有些情况SIGTERM信号会粗暴的杀死正在运行的应用，让我们进行优雅停止的努力化为乌有。比如停止Nginx，
可以使用SIGTERM快速停止，但我们更应该使用/usr/bin/目录的nginx的-s选项，使Nginx正常、优雅地终止。

在某些情形下，您可以使用preStop钩子来实现优雅的停止，根据kubernetes官方文档，preStop工作原理如下：

```
这个钩子函数在容器停止前运行，它不能带有参数。这个事件处理函数是阻塞的，而且必须在调用delete
操作发送给docker daemon之前完成。SIGTERM信号仍然会被发送。更多关于停止行为的描述请查阅Pod终止相关文档。
```

preStop钩子可以配置在容器级别，并且让您在发送SIGTERM之前运行一个自定义的命令（请注意，优雅的停止倒计时从
调用preStop前的钩子真正开始，而不是SIGTERM信号被发送时）

下面的例子将展示如何自定义一个preStop命令：

```yaml
apiVersion: extensions/v1beta1
kind: Deployment
metadata:
  name: nginx
spec:
  template:
    metadata:
      labels:
        app: nginx
    spec:
      containers:
      - name: nginx
        image: nginx
        ports:
        - containerPort: 80
        lifecycle:
          preStop:
            exec:
              # SIGTERM triggers a quick exit; gracefully terminate instead
              command: ["/usr/sbin/nginx","-s","quit"]
```


### 反馈
------------------------------------------------------

这篇博客基于我个人的知识完成，如有疏漏，请您务必在留言中指出，那样我将收货更多的知识，以及改正文章中的错误。十分感谢！


### 相关文章
------------------------------------------------------

* (在bash提示符中显示kubectl的上下文)[https://pracucci.com/display-the-current-kubelet-context-in-the-bash-prompt.html]

### RSS Feed
------------------------------------------------------

如果您想在我们更新文章后立即收到通知，请关注如下地址：

*Slack* /feed subscribe https://pracucci.com/feed.xml




### 原文地址：
------------------------------------------------------
https://pracucci.com/graceful-shutdown-of-kubernetes-pods.html
