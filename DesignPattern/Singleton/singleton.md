### 单例模式
_______________________________

####什么是单例模式

单例模式，又称为单件模式，英文全称为Singleton Pattern。如果用一句话描述什么是单例模式，那么可以这样说：单例模式保证了某对象实例的全局唯一。在该类没有对象实例的时候，会构造一个新实例，以后每次调用构造函数的时候，将仅返回第一次创建的实例。

举个“栗子”：对于带有图形界面的操作系统，我曾经尝试过同时将两个鼠标连接上，原以为屏幕上会出现两个鼠标指针，但是没有，而是这两个鼠标都可以控制这个指针，同一时刻仅有一个生效。鼠标指针在图形界面上仅可以有一个，它将在操作系统第一次检测到鼠标接入的时候创建，后面不管接入多少个鼠标，也只能操作这一个指针对象。这样做的好处是显而易见的：避免了有些调皮捣蛋的“熊孩子”拿两个鼠标一个往左滑一个往右滑而把电脑折腾疯的尴尬。

上面的例子不够贴切，重新举例：例如在windows里，资源管理器(explorer.exe)和任务管理器只能开一个。又例如在iOS系统里，微信、QQ等应用APP都只允许开一个。不是这些软件设计成了单例模式，而是操作系统在启动的时候按照单例模式的方式获取了实例。

讲真，单例模式在实际的软件开发中有很多应用场景：线程池、缓存、对话框、处理偏好设置和注册表对象、日志对象、充当打印机、显卡等设备的驱动程序的对象等，如果构造出多个实例，可能会导致程序行为异常、资源过量使用、结果不一致等问题 。

当然，可以利用全局变量来解决这些问题，但全局变量是在程序开始时就创建好的，假如它很耗费资源，并且本次程序运行并没有用到它，那就造成了资源浪费。单例模式保证可以在需要时才创建对象。

以Java程序为例，公有类可以被多次实例化，或者不是公有类，同一个包内的类也可以实例化它多次。假设公有类里的构造方法声明为私有，如：


```
public Car {
    private Car() {}
}

```

它看起来有点奇怪：如果想要构造一个实例，必须使用这个私有的构造函数，但是使用这个私有方法又必须先有一个实例。这看起来很矛盾。那么如果是下面这样呢？


```
public Car {
    private Car() {}
    public static Car getInstance() {
        return new Car();
    }
}

```

这样要想构造一个实例，可以利用静态方法getInstance()，它会帮我们返回想要的新实例：```mybenz = Car.getInstance();```。下面是单例模式的经典实现：

```
public Car {
    // the only instance
    private static Car uniqInstance

    private Car() {}
    public static Car getInstance() {
        if (uniqInstance == NULL ) { 
            uniqInstance = new Car()
        }
        return uniqInstance

    }
}

```


单例模式确保一个类只有一个实例，并提供一个全局的访问点，它几乎可以说是所有设计模式的类图中最简单的，它的类图上仅有一个类。


####进一步思考
假设有多线程调用了getInstance()，很可能是这样的场景，某一个getInstance()已经创建了实例，但未完成，另一个getInstance也去创建了实例，仍会造成多个实例的问题。

可以采用同步方法来解决这个问题，即将getInstance()利用synchronized进行修饰：

```
    public static synchronized Car getInstances(){}

```

这样就迫使每个线程在进入这个方法之前，要先等别的线程离开该方法。但这又引入了新的问题：只有在第一次执行该方法的时候需要同步，其他时候是不需要的。

有这样一些选择：

1. 如果getInstance()的性能对应用不是很关键，可以略过这点。
2. 如果应用程序总是创建并使用单例，可以使用“急切”创建实例，而不用延迟实例化的做法。实例会在静态初始化的时候创建，下面的代码还保证了线程安全。

```
    public class Car() {
        private static Car uniqInstance = new Car();
        private Car() {}
        public static Car getInstance() {
            return uniqInstance;
        }
    }
```

3. 利用“双重检查加锁”，首先检查实例是否创建，如果没有才去同步。

```
    public class Car() {
        private volatile *static Car uniqInstance;
        private Car() {}
        
        public static Car getInstance() {
            if (uniqInstance == NULL) {
                synchronized (Car.class) {
                    if (uniqInstance == NULL) {
                        uniqInstance = new Car();
                    }
                }
            }
            return uniqInstance;
        }
    }

```


####golang与单例模式

在利用golang完成一个httpClient的设计的时候，需要用到单例模式，可以这样考虑：因为golang没有类的概念，但是可以给结构体指派方法，于是创建一个全局变量，标识将要创建的唯一实例，利用sync.Once的一次性，就可以实现单例模式：

```
    var carInstance *MyCar
    var once sync.Once

    func NewCar() *MyCar {
        once.Do(func() {
            carInstance = &MyCar{Brand: "LandRover", Type: "RangeRover", Price: "$4,0000"}
        })
        return carInstance 
    }


```



####References:

1. "Head Fisrt 设计模式" 
2. github.com/lth2015/

