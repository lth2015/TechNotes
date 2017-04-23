Exo me? | kubeadm你到底干了些什么？
----------
前面已经见识了`kubeadm`的威力，使用它的确可以让Kubernetes的部署变得轻松而快乐。那么下面就看看`kubeadm`到底是如何做到这一切的。

在开始部署之前需要在各个节点上安装`docker`、`kubelet`、`kubeadm`、`kubectl`、`kubernetes-cni`，这些是后面展开部署的前提。`docker`自不必多言。`kubelet` 是Kubernetes的基础组件，`kubectl` 用来执行Kubernetes的各项命令，`kuberentes-cni` 则是Kubernetes容器网络接口（Container Network Interface）。

安装完毕后kubelet会每隔几秒重新启动，直到kubeadm告诉它要做什么。

### Kubeadm源码走读
在Kubernetes 最新的源码包里增加了关于`kubeadm`的相关源代码文件，位于kubernetes/cmd下，其目录结构为：

    |--kubeadm
        |--app
            |--api
                |--types.go
            |--cmd
                |--cmd.go
                |--init.go
                |--join.go
                |--version.go
            |--images
                |--images.go
            |--master
                |--addons.go
                |--apiclient.go
                |--discovery.go
                |--kubeconfig.go
                |--manifests.go
                |--pki.go
                |--tokens.go
            |--node
                |--csr.go
                |--discovery.go
            |--util
                |--kubeconfig.go
                |tokens.go
            |--kubeadm.go
        |--kubeadm.go

kubeadm目录里发现有两个kubeadm.go，它们位于不同的路径。位于/kubeadm下的kubeadm.go只有一个`main()`函数，它执行`app.Run()`，app包里的`Run()`则定义在/kubeadm/app/kubeadm.go里，它调用cmd包下的`NewKubeadmCommand()` 来创建新的Kubeadm命令，并调用`cmd.Execute()` 使之生效（注：kubeadm采用的也是spf13/cobra命令行工具，[Github请戳](https://github.com/spf13/cobra) ）。

`NewKubeadmCommand()` 调用`AddCommand(NewCmdInit())`、`AddCommand(NewCmdJoin())`以及`AddCommand(NewCmdVersion())`分别创建了`kubeadm init`、`kubeadm join`、`kubeadm version`这三条命令。下面对`kubeadm init`和`kubeadm join`的源码简单走读。

#### kubeadm init
`NewCmdInit()`的实现在app/cmd/init.go里，它创建了一个`cobra.Command`类型的命令对象，并指定了该命令的相关参数。

当执行`kubeadm init`的时候实际运行的是同样位于该目录下的`RunInit()`函数，它就是`kubeadm init`的“黑魔法”。

`RunInit()`首先会调用[Kubernetes自带网络工具包]( "k8s.io/kubernetes/pkg/util/net")里的`ChooseHostInterface()`方法去`/proc/net/route`里获取本机IP（未指定其他网卡的情况下）。

接着判断是否在云主机上进行部署，目前支持的云主机列表定义在api/types.go里，包含aws、azure等。但是并没有实现判断后续的操作。

然后调用`app/master/tokens.go`里的`CreateTokenAuthFile()`方法创建令牌文件。创建过程为：

    1. 调用generateTokenIfNeeded() 检查是否需要创建令牌。目前已经拥有有效令牌的相应操作还未实现，所以会生成新的令牌, 并在屏幕输出“<master/tokens> generated token”信息。
    2. 调用`os.MkdirAll()`创建用来存放令牌文件的目录。`MkdirAll()`可以看作是Go语言对`mkdir -p`的实现。
    3. 序列化生成的令牌
    4. 调用`DumpReaderToFile()`将其写入文件。

令牌文件创建之后，调用`app/master/manifests.go`里的`WriteStaticPodManifests()`方法创建静态Pod资源描述文件。这些Pod包括Kubernetes的基础组件：kubeAPIServer、kubeControllerManager、kubeScheduler以及etcd。这些资源描述文件都是json格式，指定了相关的镜像及运行参数。

接着调用`app/master/pki.go`里的`CreatePKIAssets()`函数创建公钥基础设施PKI,  创建服务端的自签名CA证书，以及签名服务账户令牌。最终将它们写入文件。在屏幕上输出`“<master/pki> created keys and certificates in /etc/kubernetes/pki”`的信息。

创建了证书及密钥之后调用`app/master/kubeconfig.go里的CreateCertsAndConfigForClients()`函数，它负责创建客户端配置，指定访问的地址和端口，以及确定证书。并调用`app/util/kubeconfig.go里的WriteKubeconfigIfNotExists()`方法将两个配置文件分别写入/etc/kubernetes/kubelet.conf和/etc/kubernetes/admin.conf，并输出`<util/config> created ...`相关信息。

随后调用`app/master/apiclient.go`里的`CreateClientAndWaitForAPI（）`函数，它负责：

    1. 根据前面创建的admin.conf配置创建客户端配置，输出“<master/apiclient> created API client configuration”
    2. 根据配置创建API客户端，并等待“<master/apiclient> created API client, waiting for the control plane to become ready”
    3. 调用wait.PollInfinite()，传入条件函数对各个组件的健康状态进行无限探测，每隔apiCallRetryInterval（默认500毫秒）一次，直到所有的组件都健康，并计算花费的时间。输出“<master/apiclient> all control plane components are healthy after %f seconds”信息。
    4. 再次以同样频率调用wait.PollInfinite()，传入条件函数不断检测是否有节点加入，直到至少一个节点注册并就绪。如果有多个节点，会选择第一个进行注册。同时输出“<master/apiclient> first node is ready after %f seconds\n”信息。

调用同在apiclient.go里的`UpdateMasterRoleLabelsAndTaints()`函数，确定该节点为master节点，并设置为“不可调度”，即不是工作节点。

接着创建调用`app/master/discovery.go`的`CreateDiscoveryDeploymentAndSecret()` 发布KubeDiscovery实例和Secret实例，输出`“<master/discovery> created essential addon: kube-discovery, waiting for it to become ready”`信息。依然调用`wait.PollInfinite()`，传入条件函数不停获取KubeDiscovery实例，直到它拥有一个可用实例为止。计算等待的时间，并输出`“<master/discovery> kube-discovery is ready after %f seconds”`

最后调用`app/master/addons.go`的`CreateEssentialAddons()`方法创建KubeProxy的DaemonSet，以及KubeDNS实例。最后发布DNS的Service。输出`“<master/addons> created essential addon: kube-dns”信息。`

上面顺序完成后输出`Kubernetes master initialised successfully! You can now join any number of machines by running the following on each node: kubeadm join --token %s %s`信息，表示master节点配置结束。

概括起来，`kubeadm init`主要完成了通信令牌（用于双向认证）的创建、证书及密钥的生成、master节点的注册以及各个组件的启动。

#### kubeadm join
同`kubeadm init`，`kubeadm join`也是一个cobra.Command类型的对象。执行命令`kubeadm join`时运行的函数是位于`app/cmd/join.go`里的`RunJoin()`。 

`RunJoin()`首先会检查是否有命令参数。在节点上执行的`kubeadm join`命令是前面`kubeadm init`生成的结果，包含了令牌及master节点地址作为参数。

然后调用`app/util.go`里的`UseGivenTokenIfValid()`函数检查令牌格式的合法性。输出`“<util/tokens> validating provided token”`

检查无误之后调用`app/node/discovery.go`里的`RetrieveTrustedClusterInfo()`函数获取集群信息。输出`“<node/discovery> created cluster info discovery client, requesting info from”`信息。向master节点的9898端口发送GET请求，请求的URL为http://MASTER-IP:9898/cluster-info/v1/?token-id=TOKENID。返回的结果分解为一个JWS（JsonWebSignature）对象。输出`“<node/discovery> cluster info object received, verifying signature using given token”`后并用该对象的`Verify()`校验节点令牌，校验成功生成集群信息，包含apiServer地址和CA证书，输出`“<node/discovery> cluster info signature and contents are valid, will use API endpoints”`。最后调用`PerformTLSBootstrap()`函数。

`PerformTLSBootstrap()`函数定义在`app/node/csr.go`里。它首先会获取客户端配置信息，并利用主机名作为节点名，生成节点启动配置。据此继续生成可以发送证书签名请求的客户端，输出`“<node/csr> created API client to obtain unique certificate for this node, generating keys and certificate signing request
”`，并向apiServer请求生成该节点的kubelet的配置，输出`“<node/csr> received signed certificate from the API server, generating kubelet configuration”`，并写入文件，输出`“<util/kubeconfig> created /etc/kubernetes/kubelet.conf”`。

最后输出相应的提示信息：`“Node join complete: * Certificate signing request sent to master and response received. * Kubelet informed of new secure connection details. Run 'kubectl get nodes' on the master to see this machine join.”`

概括一下，`kubeadm join`根据令牌获取集群信息，并生成节点kubelet配置文件。下一次节点上的kubelet就可以按已有的配置进行重启，开始生效。

####Tips
在分别执行完`kubeadm init`和`kubeadm join`之后，还需要添加Pod网络组件才能部署应用，否则Pod之间网络无法通信。可以采用`kubectl apply -f https://git.io/weave-kube`启动Weave，也可换用Calico或者Canal。


### 参考资料
[一颗赛艇 | 我们几条命令搞定Kubernetes 1.4的集群部署【视频】](http://mp.weixin.qq.com/s?__biz=MzI0NjI4MDg5MQ==&mid=2715290521&idx=1&sn=f1ff46af2b488d8faad35f8964ed8ef6&chksm=cd6d0c4dfa1a855bf647d7870283cb8b495eaa925b7136cdcd5d3e93d223f2063433eb443188&mpshare=1&scene=1&srcid=1001zt5rsQ190C7umy4zTA2L#rd)
[Kubernetes-master/cmd/kubeadm](https://github.com/kubernetes/kubernetes/tree/master/cmd/kubeadm)
[get-started](http://kubernetes.io/docs/getting-started-guides/kubeadm/)
