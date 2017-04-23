简析Docker日志之一
----------
最近这两年，火热的Docker容器技术已“烧遍”了大江南北，使用golang编写的它在github.com上已将近35,000颗星。本文作为深扒Go日志系列第二篇文章，将简要对Docker日志中的容器日志部分进行分析。

*注：本文参考的源码版本是Docker 1.12.1 Release。*

### 与Docker相关的日志
与Docker相关的日志主要有两部分：一是Docker Daemon的日志，即Docker自身运行产生的日志。二是Docker Container的日志，即Docker容器应用产生的日志。

Docker Daemon的日志输出到文件，存放在/var/log/upstream下，文件名一般是docker.log，内容示例如下：

```shell
time="2016-09-11T16:49:35.025923541+08:00" level=error msg="restartmanager canceled" 
time="2016-09-11T16:49:47.955594046+08:00" level=error msg="restartmanager canceled" 
time="2016-09-11T16:52:29.829518184+08:00" level=error msg="restartmanager canceled" 
time="2016-09-11T16:55:51.746554898+08:00" level=error msg="restartmanager canceled" 
```

*注：日志存放位置因宿主操作系统变化而变化，本文系统为ubuntu 14.04。*

结合上篇文章里对日志组成的了解，发现Docker Daemon的日志包含了时间、日志等级和事件三部分。

在Docker源码daemon/daemon.go里发现，Docker Daemon的日志均由一个第三方日志包[logrus](https://github.com/Sirupsen/logrus)来处理，对于它的学习将在下一篇文章进行。

另一方面，大部分应用对于它们产生的日志要么直接输出到标准输出、标准错误要么输出到xxx.log文件里。那么运行在容器里的应用它们的日志要怎么观察呢？

可以通过Docker 的诸多命令里的```docker logs``` 来获取运行容器的日志。该命令的参数说明如下：
```shell
  -f, --follow        Follow log output
  --help              Print usage
  --since             Show logs since timestamp
  -t, --timestamps    Show timestamps
  --tail=all          Number of lines to show from the end of the logs
```
通过这些参数可以设置所需的容器日志显示效果，利用```docker logs``` 命令可以将容器的日志打印到终端标准输出上。除此之外，在/var/lib/docker/containers目录下发现了许多以容器长ID命名的路径如下图：

![](https://github.com/maxwell92/TechTips/blob/master/MTalking/pics/var-lib-docker-containers.png)

它们的结构为：

```shell
    |- d8e4807c191b8e2594d31b2cb1796448f679f67ad2bcd04c0b19df607d0b5b09
        |-d8e4807c191b8e2594d31b2cb1796448f679f67ad2bcd04c0b19df607d0b5b09-json.log
        |-config.v2.json
        |-hostconfig.json
        |-hosts
        |-resolve.conf.hash
        |-hostname
        |-resolve.conf
```
如果对```d8e4807c191b8e2594d31b2cb1796448f679f67ad2bcd04c0b19df607d0b5b09-json.log``` 进行head 或 tail 等命令，可以发现它就是名d8e4807c191b8e2594d31b2cb1796448f679f67ad2bcd04c0b19df607d0b5b09的容器产生的日志。

这一切是怎么发生的呢？下面的部分将简单地描述这个过程。开始之前，先来回顾一下Docker Cli（命令行）是如何与后端的Docker Daemon进行交互的吧。

*注1. 这里所说的容器日志均指容器里应用产生的日志。*
*注2. 该目录下的文件内容会因容器里应用的不同略有差异。*

### Docker Cli与Docker Daemon的交互简析

孙宏亮同学在他的InfoQ专栏[《Docker源码分析（一）：Docker架构》](http://www.infoq.com/cn/articles/docker-source-code-analysis-part1/) 里对Docker的架构进行了深入的分析，这里将引用他的研究结果，并结合笔者的实践简单介绍Docker Cli与Docker Daemon的交互简析。

Docker Cli是Docker 命令行工具的简称，它由第三方命令行工具包[cobra](github.com/spf13/cobra)实现，将用户输入的命令转化为Docker Client的请求。

![](https://github.com/maxwell92/TechTips/blob/master/MTalking/pics/docker-cli-docker-daemon.jpg)

>“从图上不难看出，用户是使用Docker Client与Docker Daemon建立通信，并发送请求给后者。Docker Daemon作为Docker架构中的主体部分，首先提供Server的功能使其可以接受Docker Client的请求；而后Engine执行Docker内部的一系列工作，每一项工作都是以一个Job的形式的存在。”

![](https://github.com/maxwell92/TechTips/blob/master/MTalking/pics/docker-server.jpg)

>"Docker Daemon是Docker架构中一个常驻在后台的系统进程，它在后台启动了一个Server，Server负责接受Docker Client发送的请求；接受请求后，Server通过路由与分发调度，找到相应的Handler来执行请求。"

对于用户输入的```docker logs``` 自然也是由Docker Cli接收并转化为Docker Client的请求。在/api/client/container/logs.go里有一个名为```NewLogsCommand()```的函数，它提供了Docker Cli对解析```docker logs``` 的实现：依据解析的参数创建该命令的实例。创建之后便调用logs.go里的```runLogs()```函数，执行该命令。

```runLogs()``` 函数将通过Docker Client向后端Docker Server发出一个请求：```dockerCli.Client().ContainerLogs()```。这个请求的实现在vendor/github.com/docker/engine-api/client/container_logs.go里，按照前面设置的参数，通过```cli.get(ctx, "/containers/"+container+"/logs", query, nil)``` 发出请求。

位于后端的Docker Server将接收该请求，并转交给router寻找相应的handler来进行处理。在/api/server/router/container/container.go里，可以找到路由及处理函数的定义：

```router.Cancellable(router.NewGetRoute("/containers/{name:.*}/logs", r.getContainersLogs))```

忽略掉前面的router.Cancellable，可以发现Docker Client发来的请求由r.getContainersLogs()方法进行处理，这个方法的实现在该目录下的container_routes.go里，它将给Docker Client返回200和一组空数据，告知准备返回流(Stream)。调用```s.backend.ContainerLogs(ctx, containerName, logsConfig, chStarted)``` 来获取容器日志。

在/daemon/logs.go里有对ContainerLogs()的实现。它完成了：

首先根据容器名获取该容器实例：
```container, err := daemon.GetContainer(containerName)```

然后判断将日志显示到标准输出还是标准错误：
```if !(config.ShowStdout || config.ShowStderr) {```

接着得一个面向该容器的logger实例：
```cLog, err := daemon.getLogger(container)```

在完成一些对输出日志格式的设定后，开始读取日志，并声明了输出流的实例以及将读取的内容拷贝到标准输出或标准错误：
```javascript
    logs := logReader.ReadLogs(readConfig)
    wf := ioutils.NewWriteFlusher(config.OutStream)
    ...
    var outStream io.Writer = wf
    errStream := outStream
    if !container.Config.Tty {
        errStream = stdcopy.NewStdWriter(outStream, stdcopy.Stderr)
        outStream = stdcopy.NewStdWriter(outStream, stdcopy.Stdout)
    }
```

最后就是一个循环，它不停地读出日志的内容，添加相应的标记（时间戳等）并写入相应的输出流中：

```javascript
for {
        select {
        ...
        case msg, ok := <-logs.Msg:
        ...
            logLine := msg.Line
            if config.Details {
                logLine = append([]byte(msg.Attrs.String()+" "), logLine...)
            }
            if config.Timestamps {
                logLine = append([]byte(msg.Timestamp.Format(logger.TimeFormat)+" "), logLine...)
            }
            if msg.Source == "stdout" && config.ShowStdout {
                outStream.Write(logLine)
            }
            if msg.Source == "stderr" && config.ShowStderr {
                errStream.Write(logLine)
            }
        }
    }
```

在这样的过程中，Docker Daemon把日志写到用户终端上。

到此从Docker Cli收集到用户输入的命令，至转发到Docker Server相应的处理函数，再到Docker Daemon返回结果的过程就告一段落了。下面将简介Docker Daemon在对容器日志的处理中完成的工作。

### Docker Daemon对容器日志的处理

查看容器日志的命令```docker logs```仅对“活着”的容器有效，这是毋庸置疑的。如果使用```docker rm -f ```将这个容器彻底删除，它在宿主机/var/lib/docker/containers目录下的相应日志目录也会随之删除。从此可以了解到两点：

* Docker Daemon对容器日志的是随着容器的创建开始、随着容器的删除而终止，即伴随着容器的生命周期。
* Docker Daemon除了会在收到```docker logs```命令后将容器日志打印到终端外，还会将容器日志按json格式存放到/var/lib/docker/containers下。

前面已经简介了将容器日志打印到终端的过程，下面简介容器日志存放到json文件的过程。

首先，容器的创建过程也是来自Docker Client和Docker Server的交互。过程与上面所述大致相同，Docker Daemon将创建容器的过程交由libcontainer完成。关于libcontainer是什么可以参考下面的引用：

>“libcontainer是Docker架构中一个使用Go语言设计实现的库，设计初衷是希望该库可以不依靠任何依赖，直接访问内核中与容器相关的API。”

在libcontainerd/client_linux.go里，有```Create()```方法，它通过调用```newContainer()```创建一个新容器，在返回的时候使该容器开始运行```return container.start()```。而这个```start()```方法则定义在/libcontainerd/container_linux.go里。它主要执行了三个方法：

```javascript
  1. ctr.client.remote.apiClient.CreateContainer()
  2. ctr.client.backend.AttachStreams()
  3. ctr.client.backend.StateChanged()
```
  
其中1用来创建容器，3用来标记容器状态改变，2则进行了一些特殊操作。其中```AttachStreams()```的实现在/daemon/monitor.go里，它先根据容器ID获得该容器实例：
```c := daemon.containers.Get(id)```

然后判断该容器存在，执行```daemon.StartLogging(c)```

接着将容器的标准输出和标准错误跟Docker Daemon相连：
```javascript
    if iop.Stdout != nil {
        copyFunc(s.Stdout(), iop.Stdout)
    }
    if iop.Stderr != nil {
        copyFunc(s.Stderr(), iop.Stderr)
    }
```

与此同时，在daemon/logs.go里，方法```StartLogging()```开始执行：

首先它调用```container.StartLogger()```，该函数定义在/docker/container/container.go里，它将确定LogDriver，并在/var/lib/docker/container下创建相应json文件：
```
 container.GetRootResourcePath(fmt.Sprintf("%s-json.log", container.ID))
```
另外初始化一个logger.Copier对象，并将其运行：```copier.Run()```，实现在/daemon/logger/copier.go里，主要运行了一个执行copySrc()方法的go routine，它将日志读出来，并按行划分，轮流打印。打印调用的是
jsonfilelog.Log()方法，实现在/daemon/logger/jsonfilelog/jsonfilelog.go中，由它将日志内容输出到上面创建的以容器ID开头的json文件中 。

容器日志存放到json文件的过程结束。


### 上期问题
1. Logger定义里的Mutex锁如何保护后面的域？
  通过加锁实现同一时刻多个go routine对同一Logger实例域成员访问的互斥。
2. 在获取函数调用信息时，要先解锁以减少开销。这部分开销是怎么造成的？
 这部分开销主要来自调用runtime.Caller()时进行了trace stack造成的开销，以及有锁状态下多个go routine“忙则等待”的开销。个人认为调用runtime.Caller()前解锁，调用后再加锁虽然保证了多个go routine的并发访问，防止了该go routine意外挂起造成的死锁可能 ，但加解锁的操作本身也会带来一定的开销。

### 本期问题
1. 创建容器的过程中，对于libcontainer的调用如何由Docker Daemon发起 ? 在libcontainer/container_linux.go中，ctr.client.remote.apiClient().CreateContainer()的定义及实现在/vendor/github.com/docker/containerd/api/grpc/types里：通过err := grpc.Invoke(ctx, "/types.API/CreateContainer", in, out, c.cc, opts...)完成，它的下一步如何呢？


### 脑图

![](https://github.com/maxwell92/TechTips/blob/master/MTalking/pics/mindmap2.png)

### 参考资料
[Docker源码分析（一）：Docker架构](http://www.infoq.com/cn/articles/docker-source-code-analysis-part1/)

[Docker Github](https://github.com/docker/docker/)


