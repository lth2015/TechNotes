从Go语言的日志包说起
----------
在计算机世界里，[日志文件](https://en.wikipedia.org/wiki/Logfile)记录了发生在操作系统或其他软件运行时的事件或状态。技术人员通过日志记录进而判断系统的运行状态，寻找导致系统出错、崩溃的成因等。日志在软件系统里是非常重要的一环。

本系列文章将按照分析Go语言的日志包、Docker的日志实现、Kubernetes的日志实现以及如何打造自己的日志框架进行学习和思考。本文是系列第一篇。

###日志初探
----------
在“开扒”Golang的日志包之前，先来看看日志是什么。下面是Ubuntu系统里/var/log路径下的文件清单，大部分软件的日志都在这里存放。

![](https://github.com/maxwell92/TechTips/blob/master/MTalking/pics/varlog2.png)
常见的Apache服务器日志如下：

```bash
Apache
1. access.log
127.0.0.1 - - [19/Jul/2016:16:50:35 +0800] "GET / HTTP/1.0" 200 11783 "-" "check_http/v2053 (nagios-plugins 1.4.13)"
23.251.51.125 - - [19/Jul/2016:16:54:18 +0800] "GET http://domkrim.com/av.php HTTP/1.0" 404 459 "-" "Mozilla/5.0 (compatible; MSIE 6.0; Windows NT 5.1; Trident/5.0)"
23.251.51.125 - - [19/Jul/2016:16:54:19 +0800] "CONNECT www.baidu.com:443 HTTP/1.0" 405 532 "-" "-"

2. error.log
[Tue Jul 19 15:08:34.193813 2016] [:error] [pid 12738] [client 23.251.51.125:2618] script '/var/www/html/av.php' not found or unable to stat
[Tue Jul 19 16:04:55.225442 2016] [:error] [pid 12740] [client 185.25.148.240:45535] script '/var/www/html/testproxy.php' not found or unable to stat
[Tue Jul 19 17:00:37.692294 2016] [mpm_prefork:notice] [pid 27556] AH00169: caught SIGTERM, shutting down
```

日志可以简单地分为事件日志和消息日志两种。事件日志记录了发生在系统运行过程中的事件，可以用来审计操作、诊断问题等，对于理解复杂系统（例如很少人为操作的服务系统）的运行非常关键。消息日志则被应用到如即时通信等软件里，用来记录来往的消息。

常用的日志标准是定义在[Internet Engineering Task Force](https://en.wikipedia.org/wiki/Internet_Engineering_Task_Force)(IETF) [RFC](https://en.wikipedia.org/wiki/Request_for_Comments) [5424](https://tools.ietf.org/html/rfc5424)里的[syslog](https://en.wikipedia.org/wiki/Syslog)。syslog标准使用一个标准化的子系统来生成、过滤、记录和分析日志消息。

从上面的日志示例中可以了解到日志记录某个事件，需要记录它的时间、地点、参与者、起因、简要经过等信息。而在syslog相关标准[RFC3164](https://tools.ietf.org/html/rfc3164#section-4.1)中，还定义了严重性等级（Severity Level），用来标识该条日志记录的紧要程度：

| 编号 | 等级 |
|:---:|:---:|
| 0 | Emergency: system is unusable|
| 1 | Alert: action must be taken immediately |
| 2 | Critical: critical conditions |
| 3 | Error: error conditions |
| 4 | Warning: warning conditions |
| 5 | Notice: normal but significant condition |
| 6 | Informational: informational messages |
| 7 | Debug: debug-level messages |

那么综上，日志记录的一般格式可以概括为：

[时间] [日志级别] [地点] [参与者] [事件] [起因]

那么Golang打印的日志长什么样？

###Golang的日志格式
----------
利用Golang的log包提供的Println()方法可以很快地打印出来一条日志：

```bash
2016/09/04 18:09:41 Hello World from log pkg
```
它的构成相当简单，前面是事件发生的时间，后面是事件记录。

还可以继续测试：如果试图利用```>``` 将它重定向到文件aaa中是不行的，只能使用```2>``` 来完成重定向，这说明了它是输出到stderr的本质。

log包里提供了多种格式化日志输出方法：Printf()、Fatalf()、Panicf()。

Fatalf()的输出如下：
```bash
2016/09/04 18:39:45 It's Fatal
exit status 1
```
Panicf()的输出如下：
```bash
panic: It's Fatal
goroutine 1 [running]:
panic(0x954e0, 0x4201ae0a0)
    /usr/local/go/src/runtime/panic.go:500 +0x1a1
log.Panicf(0xb0d2f, 0x8, 0x420173f18, 0x1, 0x1)
    /usr/local/go/src/log/log.go:327 +0xe3
main.main()
    /Users/yp-tc-m-7019/mworks/gopl/log/log1.go:8 +0xb4
exit status 2
```

相比Printf(), Fatalf()会在输出日志消息之后退出程序，退出代码为1，而Panicf()还会打印相关的调用信息，再退出程序，代码为2。

最后log包还可以自定义日志前缀：
借助于SetPrefix()方法，
```bash
[Custom Prefix] 2016/09/04 18:45:18 Hello World from log pkg
```
和借助于SetFlags()方法修改默认的前缀为打印文件及行号：
```bash
log2.go:7: Hello World from log pkg
```


### Golang的日志包
----------
Golang源码的日志目录结构是这样的：

```
|-log
    |-syslog
        |-syslog.go
        |-syslog_test.go
        |-syslog_unix.go
 |-log.go
 |-log_test.go
```

 其中包含了两个包，一个是syslog系统日志包，另一个是log包。这里先看log包。

log包里主要定义了Logger类型及它的各种方法。同时还定义了全局变量std，利用它的辅助函数Print[f|ln]，Fatal[f|ln]和Panic[f|ln]可以快速地打印日志，而无需手动创建新的Logger实例。

在log包里首先定义了一些常量，它们是日志输出前缀的标识:

```javascript
 const (
    Ldate = 1 << iota  //当前时区的日期，格式是:2009/01/23
    Ltime          //当前时区的时间，格式是：01:23:23
    Lmicroseconds  //微秒解析
    Llongfile      //全文件名和行号
    Lshortfile     //当前文件名和行号, 会覆盖Llongfile
    LUTC           //使用UTC而非本地时区
    LstdFlags = Ldate | Ltime //标准Logger的默认值
 )
```

这些值除了Ldate和LstdFlags显式赋值，其它的并没有赋值，那么它们的值如何呢？经过测试，发现它们的值分别为1、2、4、8、16、32和3。这样的常量定义方法是通过iota来实现的。

 iota的定义在/src/builtin/builtin.go里，它跟true 和 false一样都是untyped int类型。 它是预定义的标识符，作用在const块里，从0开始索引。可以把iota看作是常量计数器，而iota也只能用于常量表达式，使用它可以简化递增数值的定义，

```javascript
 const iota = 0 // Untyped int.
```
可以对iota做一点小实验：
若有下面的常量定义：
```javascript
 const (
    ID0 = iota //0
    ID1
    ID2  
 )
```
分别输出ID0、ID1、ID2可以得到结果为0、1、2。
或者定义常量如下：
```javascript
 const (
    ID0 = 1 << iota //1
    ID1
    ID2  
 )
```
分别输出ID0、ID1、ID2可以得到结果为1、2、4。
而在下面的情况里，输出会略有不同：

```javascript
 const (
    ID0 = iota
    ID1
    ID2
    ID3 = iota + 3
    ID4
    ID5
 )
```
  分别输出ID0、ID1、ID2、ID3、ID4、ID5可以得到结果为0、1、2、6、7、8。这说明当ID3赋值的时候，iota的值已经变成了3，而非开始时的0。iota值的改变由编译器完成。

接下来log包里定义了类型Logger如下：
```javascript
type Logger struct {
    mu sync.Mutex
    prefix string
    flag int
    out io.Writer
    buf []byte
}
```
它表示一个活动的日志对象，给io.Writer生成多行输出。每次记录都简单地调用io.Writer的write方法。一个Logger可以被多个goroutines同步执行。

在Logger的定义里，其中：

* mu是sync包提供的Mutex锁，为来提供原子写，并保护下列域。
* prefix是在每条记录的前缀。
* flag是标识属性。
* out是输出目标。
* buf是为了用来输出的块缓冲。

log包里其它的内容是Logger的一些方法定义：

* 有构造函数:
```javascript
func New(out io.Writer, prefix string, flag int) *Logger{}
```
通过它得到Logger的新实例。

* 有专门设置输出的方法：
```javascript
func (l *Logger) SetOutput(w io.Writer){}
```
它用来将Logger的out域赋值为w。io.Writer是个接口类型，任何实现了方法Write(p []byte)(n int, err error)的类型都可以在这里使用。

* 有辅助函数，将整型转换为定长十进制ASCII码。赋予负数宽度来防止左侧补0(zero-padding)
```javascript
func iota(buf *[]byte, i int, wid int) {}
```
在这里它将输入的整型i转换为ASCII码，并逆序存储再buf里。

* 有格式化输出前缀方法：
```javascript
func (l *Logger) formatHeader(buf *[]byte, t time.Time, file string, line int) {}
```
函数里通过l.flag与前面定义的常量标识求与，以确定是否设定该标识。

* 有日志事件的输出方法，
```javascript
func (l *Logger) Output(calldepth int, s string) error {
    ...
    l.mu.Lock()
    defer l.mu.Unlock()
    if l.flag&(Lshortfile|Llongfile) != 0 {
        l.mu.Unlock()
        var ok bool
        _, file, line, ok = runtime.Caller(calldepth)
        if !ok {
            file = "???"
            line = 0
        }
        l.mu.Lock()
    }
    ...
}
```
输出字符串s包含了被flags指定的前缀。这里在获取函数调用信息的时候要先解锁，否则开销会比较大。调用深度被用来恢复PC（Program Counter程序计数器）。一般情况下，所有的默认深度都是2。如果字符串的最后一个字符不是换行符'\n'，会自动追加一个。

* 有用来打印输出的各种方法：
```javascript
func (l *Logger) Print(v ...interface{}) {
    l.Output(2, fmt.Sprint(v...)) 
}
//下面的实现与上面基本相同，分别采用了Sprintf()方法和Sprintln()方法。
func (l *Logger) Printf(format string, v ...interface{}) {}
func (l *Logger) Println(v ...interface{}) {}
```
相比Print，Fatal仅比它多了os.Exit(1)调用。

```javascript
func (l *Logger) Fatal(v ...interface{}) {
    ...
    os.Exit(1)
}
func (l *Logger) Fatalf(format string, v ...interface{}) {}
func (l *Logger) Fatalln(v ...interface){} {}
```
而panic则稍有不同：
```javascript
func (l *Logger)Panic(v ...interface{}) {
    s := fmt.Sprint(v...)
    l.Output(2, s)
    panic(s)
}
func (l *Logger)Panicf(format string, v ...interface{}) {}
func (l *Logger)Panicln(v ...interface{}) {}
```
Panic()里调用的panic()是内建函数，定义在/usr/local/go/src/builtin/builtin.go里。它会终止当前goroutine的正常执行，使其立即停止。

假如函数F调用了panic之后F会立马结束，返回到调用F的函数里。又假设调用F的函数为G，这时对F的调用就像是直接对panic的调用一样，又会将G停止。如此直到所有的goroutine停止，程序结束，错误被输出。终止时的操作可以由内建函数recover()来控制。

* 最后设置标识位的函数，以及设置前缀的函数：
```javascript
    func (l *Logger) Flags() int {}
    func (l *Logger) SetFlags(flag int) {}
    func (l *Logger)Prefix() string {}
    func (l *Logger) SetPrefix(prefix string) {} 
```

上面就是Golang的log包的全貌，了解到了Golang里Logger的定义及实现，发现在Logger的方法里，凡是涉及到更改Logger对象成员的处理，均被加锁。另外Logger并没有提供对于消息的严重级别（日志级别）、消息的分类的相应操作。整体够用但是还不完善，这为后面自定义日志框架留下了空间。

### 问题
----------
1. Logger定义里的Mutex锁如何保护后面的域？
2. 在获取函数调用信息时，要先解锁以减少开销。这部分开销是怎么造成的？

### 脑图
----------
![](https://github.com/maxwell92/TechTips/blob/master/MTalking/pics/mindmap.png)

### 参考资料
----------
[Logfile Wiki](https://en.wikipedia.org/wiki/Logfile)
[RFC 5424](https://tools.ietf.org/html/rfc5424)
[RFC 3164](https://tools.ietf.org/html/rfc3164#section-4.1)
[Go1.7 Source Code](http://docs.studygolang.com/doc/go1.7)

