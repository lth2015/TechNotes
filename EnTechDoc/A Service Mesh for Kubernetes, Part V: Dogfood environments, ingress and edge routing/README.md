# Service Mesh: Environments, Ingress and Edge routing
=================

原文链接：https://buoyant.io/2016/11/18/a-service-mesh-for-kubernetes-part-v-dogfood-environments-ingress-and-edge-routing/

在这篇文章中，我们将向您展示如何使用`Linkerd`的实现的`Service Mesh`来处理`Kubernetes`上的入口流量，在`Service Mesh`中的每个实例上分发流量。

我们还将通过一个完整的例子来介绍`Linkerd`的高级路由功能：如何将特定的请求路由到较新版本的应用实例上，比如用于内部测试的，预发布的应用版本。

更新2017-04-19：这篇文章是关于使用`Linkerd`作为到`Kubernetes`网络流量的入口点。 从0.9.1开始，`Linkerd`直接支持`Kubernetes`的Ingress资源，这对于本文中的一些用例来说是一个可替代的，也是更简单的起点。 有关如何使用`Linkerd`作为[`Kubernetes Ingress`控制器](https://kubernetes.io/docs/concepts/services-networking/ingress/#ingress-controllers)的资料，请参阅[Sarah的博文](https://buoyant.io/2017/04/06/a-service-mesh-for-kubernetes-part-viii-linkerd-as-an-ingress-controller/)，`Linkerd`作为`Kubernetes Ingress`的控制器。


注意： 这是关于Linkerd、Kubernetes和service mesh的系列文章其中一篇，其余部分包括：

1. [Top-line service metrics](https://buoyant.io/a-service-mesh-for-kubernetes-part-i-top-line-service-metrics/)

2. [Pods are great, until they’re not](https://buoyant.io/a-service-mesh-for-kubernetes-part-ii-pods-are-great-until-theyre-not/)

3. [Encrypting all the things (本文)(https://buoyant.io/a-service-mesh-for-kubernetes-part-iii-encrypting-all-the-things/)

4. [Continuous deployment via traffic shifting](https://buoyant.io/a-service-mesh-for-kubernetes-part-iv-continuous-deployment-via-traffic-shifting/)

5. [Dogfood environments, ingress, and edge routing](https://buoyant.io/2016/11/18/a-service-mesh-for-kubernetes-part-v-dogfood-environments-ingress-and-edge-routing/)

6. [Staging microservices without the tears](https://buoyant.io/a-service-mesh-for-kubernetes-part-vi-staging-microservices-without-the-tears)

7. [Distributed tracing made easy](https://buoyant.io/a-service-mesh-for-kubernetes-part-vii-distributed-tracing-made-easy/)

8. [Linkerd as an ingress controller](https://buoyant.io/a-service-mesh-for-kubernetes-part-viii-linkerd-as-an-ingress-controller/)

9. [gRPC for fun and profit](https://buoyant.io/a-service-mesh-for-kubernetes-part-ix-grpc-for-fun-and-profit/)

10. [The Service Mesh API](https://buoyant.io/a-service-mesh-for-kubernetes-part-x-the-service-mesh-api/)

11. [Egress](https://buoyant.io/a-service-mesh-for-kubernetes-part-xi-egress/)

12. [Retry budgets, deadline propagation, and failing gracefully]()

13. [Autoscaling by top-line metrics]()


