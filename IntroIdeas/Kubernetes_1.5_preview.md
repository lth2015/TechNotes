Kubernetes 1.5新特性预览
==============
9月底我们喜迎Kubernetes 1.4版本，现在仍能清楚记得`kubeadm`带来的震撼。时隔70多天，我们再次怀着激动的心情等候Kubernetes 1.5版本的发布。

在Github上，Kubernetes已拥有接近20,000颗星、6,314次Fork，吸引了国内外众多开发者的青睐，社区异常活跃。那么这次升级Kubernetes又会为我们带来什么样的惊喜？根据官方文档的显示，本次升级将主要围绕以下三项内容进行：

#### Federation(联邦)
目前每个Kubernetes集群还处于一个相对独立的阶段，它们分别运行在指定的数据中心里。不少用户都表示他们希望有个办法能让他们统一管理这些分散开的集群，支持比如跨数据中心这样的场景。集群联邦就提供了这样的功能。

联邦的概念在早期的版本里就有提到，它面临的主要挑战是：本地亲和性、跨集群服务发现、跨集群调度、跨集群迁移等等。社区正在逐步完善对联邦的支持。在Kubernetes 1.5中将会增加对Daemonsets、Deployments和ConfigMaps的支持。

#### 简化集群部署 
Kubernetes集群的部署曾经一直成为诸多初学者最大障碍。而在Kubernetes 1.4里引入了`kubeadm init`和`kubeadm join`两个命令，让Kubernetes可以在任何地方轻松部署。在即将发布的1.5中会除了继续加强`kubeadm`的能力，还会引入新命令`kubefed`，以简化集群联邦的相关操作，最后提供了对于Master节点的HA设置。

#### 节点鲁棒性
Kubernetes可以通过`kubeadm join`轻而易举地为集群添加新的计算节点，而过去对这些节点的认识是它们都必须是Linux操作系统。从1.5开始，提供对于Windows Server 2016节点的支持，同时还可以调度Windows Server Containers，这样用户就可以在Kubernetes上同时运行Linux应用和Windows应用了。同时在1.5版本中，还实现了CRI(容器运行时接口)，以及添加了`kubelet`API调用时的认证和授权。

下面是一些升级之后将会比较显著的改变：
* 节点控制器不再从apiServer强行删除pods
    * 对于有状态的应用StatefulSet(先前为PetSet)而言，这个改动意味着创建替换的Pods被阻塞，直到旧的Pods确实没有在运行了(意味着kubelet从分区返回，Node对象的删除，云服务商里实例的删除，或强行从apiServer中删除了Pod)。这里通过保证不可达的Pod不会被推测为已死来帮助阻止集群应用“脑裂”的场景，除非一些“包围”操作提供了上述之一的情况。
    * 对于其他已有的除StatefulSet外的控制器，因为控制器不会重用Pods命名(使用生成的名字generate-name)，这对于控制器替换Pods的能力没有影响。
    * 用户编写的控制器会重用Pod对象的名字，所以需要考虑这个变化。

* 允许匿名API服务器的访问，通过授权组系统设置认证的用户
    * kube-apiserver添加了--anonymous-auth 参数，默认为true。当它启用时，访问安全端口的请求不会被其他配置的认证方法所拒绝，这些请求被当做匿名请求，并且用户名为`system:anonymous`，组织为`system:unauthenticated`。
    * 认证的用户被设为`system:authenticated`组。
    * 注意：匿名访问默认启用。如果你只依赖认证来授权访问，选择一个授权模式而不是AlwaysAllow，或者将`--anonymous-auth`设为`false`。


下面列出新特性的目前的版本及说明：
* 应用
    * [Stable]当ReplicaSets不能创建Pods时，它们会通过API报告失败的底层原因。
    * [Stable]`kubectl apply`现可通过`--prune`参数删除不再需要的资源
    * [Beta]无法通过滚动升级到新版本的Deployments现可通过API说明它们已被阻塞。
    * [Beta]StatefulSets允许要求持久化identity或单实例存储的的工作负载通过Kubernetes创建和管理。
    * [Beta]为了提供安全保障，集群不会强行删除未响应节点上的Pods，用户通过CLI强行删除Pods会收到警告。
* 认证
    * [Alpha]改进了包含一组默认的集群角色的RBAC API，
    * [Beta]添加了对于Kubelet API认证/授权机制。
* AWS
    * [Stable]角色出现在`kubectl get nodes`的结果里。
* 集群生命周期
    * [Alpha]提升了`kubeadm`二进制包的交互和可用性，简化了新建一个运行集群的过程。
* 集群运维
    * [Alpha]在GCE上使用kube-up/kube-down脚本来创建/移除集群高可用（复制）的主节点。
* 联邦
    * [Beta]支持联邦ConfigMaps。
    * [Alpha]支持联邦Daemonsets。
    * [Alpha]支持联邦Deployments。
    * [Alpha]集群联邦添加对于DeleteOptions.OrphanDependents的支持来开启级联删除。
    * [Alpha]引入`kubefed`命令，简化联邦控制台的部署以及集群注册/注销体验。[文档]()
* 网络
    * [Stable]服务可以通过DNS名被其他服务引用，而不是只有在pods里才可以。
    * [Beta]为NodePort类型和LoadBalancer保留源IP的选项。
* 节点
    * [Alpha]支持在容器运行时启用用户命名空间重映射的时候，保留对宿主用户命名空间的访问，
    * [Alpha]引入了v1alpha1版本的CRI(容器运行时接口) API，它允许可插拔的容器运行时。现有一个实验用的已经就绪的docker-CRI集成，请测试和反馈。
    * [Alpha]Kubelet基于QoS层在每个Pod的CGroup层级里启动容器
    * [Beta]Kubelet集成了memcg提示消息API，来检测超过阈值。
    * [Beta]引入了Beta版的容器化节点一致性测试: gcr.io/google_containers/node-test:0.2。用户可以来验证node设置。
* 调度
    * [Alpha]添加了对于不透明整数资源(node级)的审计。
    * [Beta]PodDisruptionBudget升级为Beta版，可以用来在应用的SLO下安全地drain节点。
* UI
    * [Stable]Dashboard UI现在显示面向用户的对象及它们的资源使用情况。
* Windows
    * [Alpha]添加了对Windows Server 2016节点和调度Windows Server Container的支持。

最后是升级须知：
* batch/v2alpha1.ScheduledJob被重命名为batch/v2alpha1.CronJob。
* PetSet被重命名为StatefulSet。如果你现在有PetSets，你要在升级为StatefulSets前后进行一些额外的迁移操作，
* 如果你从v1.4.x升级你的集群联邦组件，请更新你的`federation-apiserver`和`federation-controller-manager`证明为新版本。
* 废弃的kubelet --configure-cbr0参数被移除。经典的网络模式也是。如果你依赖于此模式，请调研其他的网络插件`kubenet`或`cni`是否满足需求。
* 新的client-go结构，根据kubernetes/client-go为版本策略。
* 废弃的kube-scheduler --bind-pods-qps和--bind-pods burst参数被移除，替换--kube-api-qps和--kube-api-burst代替。
* 如果你需要使用1.4的特性:PodDisruptionBudget(例如创建了PodDisruptionBudget对象)，然后在从1.4升级为1.5之前，你一定要删除所有的PodDisruptionBudget对象(policy/v1alpha1/PodDisruptionBudget)。升级之后不可能删除这些对象。它们的存在也会妨碍你使用1.5里Beta版的PodDisruptionBudget特性(policy/v1beta1/PodDisruptionBudget)。如果你已经进行了升级，那么你需要降级到1.4来删除这些policy/v1alpha1/PodDisruptionBudget对象。






主要更新：

联邦(Federation)现已支持
Daemonsets
Deployments
ConfigMaps
简化集群部署
提升kubeadm
新命令kubefed
Master节点高可用(HA)
节点鲁棒性
支持Windows Server Container
容器运行时接口(CRI)实现
kubeletAPI支持认证和授权
新特性：

应用
 Stable 当ReplicaSets不能创建Pods时，它们会通过API报告失败的底层原因。
 Stable kubectl apply现可通过--prune参数删除不再需要的资源
 Beta​ 无法通过滚动升级到新版本的Deployments现可通过API说明它们已被阻塞。
 Beta StatefulSets允许要求持久化identity或单实例存储的的工作负载通过Kubernetes创建和管理。
 Beta 为了提供安全保障，集群不会强行删除未响应节点上的Pods，用户通过CLI强行删除Pods会收到警告。
认证
 Alpha 改进了包含一组默认的集群角色的RBAC API，
 Beta 添加了对于Kubelet API认证/授权机制。
AWS
 Stable 角色出现在kubectl get nodes的结果里。
集群生命周期
 Alpha 提升了kubeadm二进制包的交互和可用性，简化了新建一个运行集群的过程。
集群运维
 Alpha 在GCE上使用kube-up/kube-down脚本来创建/移除集群高可用（复制）的主节点。
联邦
 Beta 支持联邦ConfigMaps。
 Alpha 支持联邦Daemonsets。
 Alpha 支持联邦Deployments。
 Alpha 集群联邦添加对于DeleteOptions.OrphanDependents的支持来开启级联删除。
 Alpha 引入kubefed命令，简化联邦控制台的部署以及集群注册/注销体验。文档
网络
 Stable 服务可以通过DNS名被其他服务引用，而不是只有在pods里才可以。
 Beta 为NodePort类型和LoadBalancer保留源IP的选项。
节点
 Alpha 支持在容器运行时启用用户命名空间重映射的时候，保留对宿主用户命名空间的访问，
 Alpha 引入了v1alpha1版本的CRI(容器运行时接口) API，它允许可插拔的容器运行时。现有一个实验用的已经就绪的docker-CRI集成，请测试和反馈。
 Alpha Kubelet基于QoS层在每个Pod的CGroup层级里启动容器
 Beta Kubelet集成了memcg提示消息API，来检测超过阈值。
 Beta 引入了Beta版的容器化节点一致性测试: gcr.io/google_containers/node-test:0.2。用户可以来验证node设置。
调度
 Alpha 添加了对于不透明整数资源(node级)的审计。
 Beta PodDisruptionBudget升级为Beta版，可以用来在应用的SLO下安全地drain节点。
UI
 Stable Dashboard UI现在显示面向用户的对象及它们的资源使用情况。
Windows
 Alpha 添加了对Windows Server 2016节点和调度Windows Server Container的支持。
显著改变

节点控制器不再从apiServer强行删除pods
对于有状态的应用StatefulSet(先前为PetSet)而言，这个改动意味着创建替换的Pods被阻塞，直到旧的Pods确实没有在运行了(意味着kubelet从分区返回，Node对象的删除，云服务商里实例的删除，或强行从apiServer中删除了Pod)。这里通过保证不可达的Pod不会被推测为已死来帮助阻止集群应用“脑裂”的场景，除非一些“包围”操作提供了上述之一的情况。
对于其他已有的除StatefulSet外的控制器，这对于控制器替换Pods没有影响，因为控制器不会重用Pods命名(使用生成的名字generate-name)
用户编写的控制器会重用Pod对象的名字，所以需要考虑这个变化。
允许匿名API服务器的访问，通过授权组系统设置认证的用户
kube-apiserver添加了--anonymous-auth 参数，默认为true。当它启用时，访问安全端口的请求不会被其他配置的认证方法所拒绝，这些请求被当做匿名请求，并且用户名为system:anonymous，组织为system:unauthenticated。
认证的用户被设为system:authenticated组。
注意：匿名访问默认启用。如果你只依赖认证来授权访问，选择一个授权模式而不是AlwaysAllow，或者将--anonymous-auth设为false。
升级须知

batch/v2alpha1.ScheduledJob被重命名为batch/v2alpha1.CronJob。
PetSet被重命名为StatefulSet。如果你现在有PetSets，你要在升级为StatefulSets前后进行一些额外的迁移操作，
如果你从v1.4.x升级你的集群联邦组件，请更新你的federation-apiserver和federation-controller-manager证明为新版本。
废弃的kubelet --configure-cbr0参数被移除。经典的网络模式也是。如果你依赖于此模式，请调研其他的网络插件kubenet或cni是否满足需求。
新的client-go结构，根据kubernetes/client-go为版本策略。
废弃的kube-scheduler --bind-pods-qps和--bind-pods burst参数被移除，替换--kube-api-qps和--kube-api-burst代替。
如果你需要使用1.4的特性:PodDisruptionBudget(例如创建了PodDisruptionBudget对象)，然后在从1.4升级为1.5之前，你一定要删除所有的PodDisruptionBudget对象(policy/v1alpha1/PodDisruptionBudget)。升级之后不可能删除这些对象。它们的存在也会妨碍你使用1.5里Beta版的PodDisruptionBudget特性(policy/v1beta1/PodDisruptionBudget)。如果你已经进行了升级，那么你需要降级到1.4来删除这些policy/v1alpha1/PodDisruptionBudget对象。

