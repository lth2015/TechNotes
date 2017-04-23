A brief introduction to golang -- a beginners perspective
----------------------------------------

Outline:
* Overview
* Golang Web Programming
* Golang System Programming
* Pros & Cons

### Overview
2007年的时候，三巨头Robert Griesemer，Rob Pike，Ken Thompson有了创造一门新语言的idea，[为了什么目的？]。这三位都是神一般的存在，名声如雷贯耳的Ken Thompson，他是google员工，是Unix、C的作者之一，就连我们现在用的utf-8编码也是他打造的。

在这三位的带领下，Golang在2009年正式开源，在2012年发布了1.0版本。去年的8月份更新了1.7版本，我们的容器云就是用1.7的版本写的。最近有消息表示golang的1.8版本提供了对动态链接库的支持，这个版本也即将发布。

Golang一开源就吸引了很多开发者的青睐。到目前在Github上，已有超过700名贡献者提交了30,000次代码，收到23,000颗星的同时，还有3,000次fork。这无不标识着golang的火热。另一方面今年来由Golang编写Docker、Etcd以及Kubernetes等容器技术、分布式系统的热潮也为golang吸引了众多的粉丝。

### Golang Web Programming
Golang标准库里提供的net包，支持基于IP层、TCP/UDP层以及更高层（HTTP、FTP、SMTP）的网络操作。比如定义了客户端和服务端之间请求和相应的传输标准的HTTP是互联网上应用最为广泛的一种网络协议，通过net/http包我们可以很轻松地使用GET请求和POST请求交互数据。

下面是一个最简单的Web Server的例子


下面实现了一个HTTP客户端和服务器端


网络包方面的第三方包邮Gorilla/Mux，beego、iris。iris是比较轻量的web server引擎，它自己号称是地球上最快的web server。容器云就是在它的基础上搭建的。

同时，golang标准库也提供了对rpc的支持，下面是一个rpc客户端和服务器端的例子

### Golang system programming
Golang标准库里自带了os、syscall等包，里面实现了很多和操作系统相关的类型及方法。比如Docker容器，就利用了golang亲近底层的特性，通过多个系统调用，创建了相应的namespace和cgroups等，最终提供容器环境。我们可以用少于100行的golang代码表示一个简单的容器：

从某些角度来说Golang并不是一门系统语言，因为它具有垃圾回收机制。但它也可以被看作是一门系统语言，因为它可以用来开发一些非核心工具，比如应用容器等，比如Docker和Etcd都使用Golang进行了开发。

有人认为golang满足下面五个特点，因而可以认为是系统级语言：
* open source, google 
* fast、simple、safe
* concurrent，modern
* good cross-platform support
* rapid development and growing community

Golang可以用来写一些底层的内容，比如编译器甚至内核？Golang可以和C语言混合使用。上到用户友好的命令行接口，下到底层巨大的分布式系统。Golang都能一展身手。

对Docker有点了解的话都知道Docker是基于Linux 的Namespace和CGroups机制来实现的。Docker本身也是由Golang编写的。那么我们可以试着自己写一个简化的容器。应用golang，在100行代码以内，我们就可以实现这个目标。

在以前Linux有一个叫chroot的命令/系统调用，它可以修改根目录，将用户“囚禁”到某一个目录下。用户在这个目录下看到的根/实际上在外面并不是根目录，而且不能访问外面的内容。

Demo

### Pros & Cons
#### Golang Features
跨平台(Linux、Windows、MacOS等)
具有动态特征的静态语言
适合高并发和大量I/O操作的服务器开发

#### Golang tools / IDE
我们使用的主要是

#### Golang vs Java / Python
TIOBE排名
这些都是别人总结的一些:
vsJava:
Common:
* C family(imperative, braces)
* Stataically typed
* Garbage collected
* Memory safe(nil references, runtime bounds)
* Variables are always intialized(zero/nil/false)
* Methods
* Interfaces
* Type assertions(instanceof)
* Reflection

Differences:
* Compile, no VM
* Statically linked binaries
* Control over memory layout
* Function values and lexical closures
* Built-in strings(UTF-8)
* Built-in generic maps and arrays/slices
* Built-in concurrency

No
* Classes
* Constructors
* Inheritance
* final
* Exceptions
* Annotations
* User-defined generics

#### Cloud Native
12 Factors app



#### Reference
[](http://letzgro.net/blog/9-reasons-to-choose-golang-for-your-next-web-application/)
[](https://www.quora.com/Is-Go-a-systems-programming-language)
[](http://coolshell.cn/articles/17010.html)
[](https://talks.golang.org/2015/go-for-java-programmers.slide#14)
[](https://talks.golang.org/2016/applicative.slide#13)
[](https://talks.golang.org/2015/simplicity-is-complicated.slide#11)
[](http://www.dockerinfo.net/4123.html)
