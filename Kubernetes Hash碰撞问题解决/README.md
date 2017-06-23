Kubernetes Hash碰撞问题解决
=================================

#### 缘起
-----------

在生产环境中使用*Kubernetes Deployment*进行滚动升级时，经常会出现升级失败的情况。具体的原因是由于在滚动升级的过程中，*Deployment Controller*会使用*Adler32*算法计算*Deployemt.Template*的*Hash*值，这个*Hash*值和*Deployment*的名称共同组成了新的*ReplicaSet*的名称。但是由于滚动升级中，我们只是对*PodTemplate*部分*Containers[0].image*即镜像名称进行了修改，例如从*img.reg.3g:15000/yce-alpha:201703151133*修改成*img.reg.3g:15000/yce-alpha:201703251023*，由于内容变化不大，所以经常会出现创建新*ReplicaSet*失败的情况，随着向*Kubernetes*集群迁移的应用越来越多，这种状况出现的越来越频繁。


#### 解决方法：修改Hash算法
-----------

社区中关于这个问题也有很多讨论[#issue 29735](https://github.com/kubernetes/kubernetes/issues/29735)，大家提出了各种各样的解决办法，由于不能破坏产生新的*ReplicaSet*操作的幂等性，所以不能改变ReplicaSet的名称产生的方式。为了减少Hash碰撞，只能在Hash算法上下功夫。

在理论上，任何Hash算法都存在碰撞。

注：我们在文章[《从Kubernetes的一个Bug说开去》]()中详细描述了*Kubernetes Deployment*滚动升级出现Hash碰撞的定位过程。
