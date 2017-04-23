Dynamic Provisioning and Storage Classes in Kubernetes
======================

> 导读：
> *Dynamic Provisioning*的目标是完全自动化存储资源的生命周期管理，让用户无需过多的干预存储的管理。


作者注：这是[5天深入理解Kubernetes新特性系列](http://blog.kubernetes.io/2017/03/five-days-of-kubernetes-1.6.html)的第一篇。

**存储**(*Storage*)是运行有状态容器的关键要素，*Kubernetes*提供了强大的原语来管理存储。**动态卷配置**(*Dynamic provisioning*)是*Kubernetes*的独有功能，它可以根据需要动态的创建存储卷。在动态配置之前，集群管理员必须手动调用云/存储服务提供商的接口来配置新的存储卷，然后创建*PersistentVolume*对象以在*Kubernetes*中表示和使用他们。通过动态配置，可以实现两个步骤的自动化，无须集群管理员预先配置存储资源，而是使用*StorageClass*对象制定的供应商来动态配置存储资源，具体请参考[用户指南](https://kubernetes.io/docs/user-guide/persistent-volumes/index#storageclasses)）。*StorageClass*本质上是为底层存储提供者描绘了蓝图，以及各种参数，例如磁盘类型（例如固态和标准磁盘）。

*StorageClasses*使用特定的存储平台或者云提供商为*Kubernetes*提供物理介质。多个存储配置以*in-tree*的形式（[用户手册](https://kubernetes.io/docs/user-guide/persistent-volumes/index#provisioner)），但现在也支持*out-of-tree*配置器（请参阅[*kubernetes-incubator*](https://github.com/kubernetes-incubator/external-storage)）。

在*Kubernetes 1.6*正式版中，动态配置被提升至稳定版（*Kubernetes 1.4*是*Beta*
）。这是完成*Kubernetes*存储自动化愿景的一大重要进步，它允许集群管理员控制资源的配置，也能够让用户更好地专注应用开发。这些所有的有点，在使用*Kubernetes 1.6*之前，这些面向用户的变化都是非常重要的。

### 怎么使用*Storage Classes*
---------------------------------

*StorageClass*是*Dynamic Provisioning*（动态配置）的基础，允许集群管理员位底层存储平台做定义抽象。用户只需在*PersistentVolumeClaim(PVC)*通过名字引用*StorageClass*即可。

```yaml
apiVersion: v1
kind: PersistentVolumeClaim
metadata:
  name: mypvc
  namespace: testns
spec:
  accessModes:
  - ReadWriteOnce
  resources:
    requests:
      storage: 100Gi
  storageClassName: gold
```

为了促进*Dynamic Provisioning*的使用，此功能允许集群管理指定默认的*StorageClass*。当*Dynamic Provisioning*存在时，用户可以创建一个*PVC*而不需要制定一个*StorageClassName*，进一步减少了用户用于关注底层存储提供者所需的精力。当使用默认的*StorageClasses*时，创建*PersistentVolumeClaims(PV)*，这一点尤为重要：

* 在*Kubernetes 1.6*中，已经跟*PVCs*绑定的*PVs*依然保持绑定：

    * 除非用户手动添加他们，否则，他们将不具有与他们相关联的*StorageClass*。

    * 如果*PV*变为“可用”，如果删除的*PVC*和对应的*PV*被回收，则它要接受如下约束：

* 如果*PVC*中未指定*StorageClassName*，则默认的*StorageClass*将用于动态配置(*Dynamic Provisioning*)。
    
    * 如果存在并且“可用”，没有*StorageClass*标签的*PV*将不被考虑用于绑定到*PVC*。

* 如果在*PVC*中将*StorageClassName*设置为空字符串("")，则不会使用存储类。（即：此*PVC*禁止使用动态配置）

    * 如果存在并且“可用”，*PVs*（没有指定*StorageClassName*），将被考虑用于绑定到*PVC*。


* 如果*StorageClassName*设置为特定值，则将使用与之匹配的存储类。

    * 如果存在并且“可用”，匹配到*StorageClassName*的*PV*将被考虑用于绑定到*PVC*。

    * 如果不存在对应的存储类，*PVC*将失败。


为了减轻集群中默认*StorageClasses*的负担，从*Kubernetes 1.6*开始，*Kubernetes*为多个云提供商安装（通过*add-on*管理器）默认的*StorageClasses*。要使用这些默认的*StorageClasses*，用户不需要按名称引用他们，也就是说，不需要在*PVC*中指定*StorageClassName*，便可直接使用。

下面的表格显示不同的云提供商预安装的默认*StorageClasses*：

| 云提供商 | 默认*StorageClasses Name* | 默认存储 |
|----------|---------------------------|----------|
| Amazon Web Services | gp2 | aws-ebs | aws-ebs |
| Microsoft Auzer | standard | azure-disk |
| Google Cloud Platform | standard | gcd-pd |
| OpenStack | standard | cinder |
| VMware vSphere | thin | vsphere-volume |

对于大多数用户来说，选择使用每个云提供商默认的提供的存储是“理智的”，如果想指定自己使用的默认存储方式，请参考[用户手册](https://kubernetes.io/docs/tasks/administer-cluster/change-default-storage-class/)。

### 动态配置卷和回收策略
----------------------

所有的*PVs*都有一个与之关联的回收策略，规定*PV*一旦从声明中解除后会发生什么（请参考[用户指南](https://kubernetes.io/docs/user-guide/persistent-volumes/#reclaiming)）。由于自动化存储资源的生命周期管理，因此动态配置卷(*Dynamic Provisioning Volume*)的默认回收策略为“删除”。这意味着当*PersistentVolumeClaim(PVC)*被释放时，动态配置卷会被相应的删除，并且可能数据无法恢复。如果这不是所预期的行为，则在设置卷后，用户必须在相应的*PersistentVolume(PV)*对象上更改回收策略。


### 如何设置回收策略
----------------------

您可以通过修改*PV*对象中的*persistentVolumeReclaimPolicy*字段的值来修改*PV*的回收策略。，更多的细节和不同的回收策略请参考[用户手册](https://kubernetes.io/docs/user-guide/persistent-volumes/#reclaim-policy)。

### FAQs
----------------------

#### 如何使用默认的*StorageClass*？
----------------------

如果您的集群有一个默认的*StorageClass*能够满足您的需求，那么您剩下所有需要做的就是创建*PersistentVolumeClaim(PVC)*，剩下的都有默认的动态配置搞定，包括您无需去指定*storageClassName*：

```yaml
apiVersion: v1
kind: PersistentVolumeClaim
metadata:
  name: mypvc
  namespace: testns
spec:
  accessModes:
  - ReadWriteOnce
  resources:
    requests:
      storage: 100Gi
```

#### 能添加自己的*StorageClass*吗？
----------------------

当然可以。在添加自己的*StorageClass*之前，需要先确定动态配置能否在集群上工作。然后，创建一个*StorageClass*对象并通过设置参数来定制它。对很多用户来说，最简单的创建对象的方式就是通过编写一个*yaml*文件并通过`kubectl create -f`来创建。

下面的创建*StorageClass*的例子使用了*Google Cloud Platform*，创建了一个*pd-ssd*，名称为*gold*：

```yaml
kind: StorageClass
apiVersion: storage.k8s.io/v1
metadata:
  name: gold
provisioner: kubernetes.io/gce-pd
parameters:
  type: pd-ssd
```

由于集群中可以存在多个类，因此管理员可以为大多数工作负载保存默认值（因为它使用*pd-standard*），为需要额外的工作负载保留*gold*类。


#### 是否已经安装了默认的*StorageClass*？
----------------------

您可以使用`kubectl`命令检查*StorageClass*对象。在下面的例子中有两个*StorageClass*：*gold*和*standard*。*gold*类是用户自定义的，*standard*类由*Kubernetes*默认安装。

```bash
$ kubectl get sc
NAME                 TYPE
gold                 kubernetes.io/gce-pd   
standard (default)   kubernetes.io/gce-pd
```

```bash

$ kubectl describe storageclass standard
Name:       standard
IsDefaultClass: Yes
Annotations: storageclass.beta.kubernetes.io/is-default-class=true
Provisioner: kubernetes.io/gce-pd
Parameters: type=pd-standard
Events:         <none>
```

#### 能过删除/关闭默认的*StorageClass*？
----------------------

您不能删除默认的*StorageClass*，因为它是作为集群的*add-on*安装的，如果它被删除，会被重新安装。

当然，您可以停掉默认的*StorageClass*行为，通过删除*annotation*：`storageclass.beta.kubernetes.io/is-default-class`。

如果没有*StorageClass*对象标记默认的*annotation*，那么*PersistentVolumeClaim*对象（在不指定*StorageClass*情况下）将不自动触发动态配置。相反，它们将回到绑定可用的*PersistentVolume(PV)*的状态。


#### 能否将*PVs*与一个特殊的*StorageClass*绑定？
----------------------

可以。通过改变*PV*对象的*storageClassName*字段，可以将一个*StorageClass*与这个*PV*绑定。


### 当我删除*PersistentVolumeClaim(PVC)*会发生什么？
----------------------

如果一个卷是动态配置的卷，则默认的回收策略为“删除”。这意味着，在默认的情况下，当*PVC*被删除时，基础的*PV*和对应的存储也会被删除。如果需要保留存储在卷上的数据，则必须在*PV*被设置之后将回收策略从*delete*更改为*retain*。


### 作者&&原文链接
----------------------

作者：Saad Ali & Michelle Au, Software Engineers, and Matthew De Lio, Product Manager, Google

原文：http://blog.kubernetes.io/2017/03/dynamic-provisioning-and-storage-classes-kubernetes.html
