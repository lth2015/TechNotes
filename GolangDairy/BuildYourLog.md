日志对于任何一个系统来说都是非常重要的一环，除了记录运行过程和状态以外，更重要的是记录一些系统出错、崩溃的成因，以便开发、运维人员寻找问题。

本系列将按照Go语言的日志包、Docker的日志实现、Kubernetes的日志实现、如何打造自己的日志框架进行学习和思考。

Go语言的日志包分析之一


Golang源码的日志目录结构是这样的：

-log
 |-syslog
 |    |-syslog.go
 |    |-syslog_test.go
 |    |-syslog_unix.go
 |-log.go
 |-log_test.go

 其中包含了两个包，一个是syslog系统日志包，另一个是log包。

 首先来看log包。

 Go语言提供了日志包log，里面定义了一个类型Logger以及它的格式化输出方法。同时还有一个预定义的“标准”Logger，可以通过辅助函数Print[f|ln]，Fatal[f|ln]和Panic[f|ln]来使用要比手动创建Logger简便。

 在导入了相关的包后，首先定义了一些常量，它们是确定日志输出前缀的标记:

 ```golang
 const (
    Ldate             = 1 << iota  //当前时区的日期，格式是:2009/01/23
    Ltime //当前时区的时间，格式是：01:23:23
    Lmicriseconds  //微秒解析
    Llongfile      //全文件名和行号
    Lshortfile     //当前文件名和行号，会覆盖Llongfile
    LUTC           //使用UTC而非当前时区
    LstdFlags         = Ldate | Ltime //标准Logger的初始值
 )

 ```

 这里这些值除了Ldate和LstdFlags显式赋值，其它的并没有赋值，那么它们的值如何呢？经过测试，发现它们的值分别为1、2、4、8、16、32和3。

 通过二进制位来进行标记的方法已经很常见。在这里是通过itoa来实现的














