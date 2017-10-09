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

在本系列的前几个部分，我们向您展示了如何使用`Linkerd`来捕获top-line的服务指标，在服务中透明地添加TLS以及执行蓝绿发布。这些文章展示了如`Kubernetes`
这样的环境中如何使用`Linkerd`作为`Service Mesh`的组件，为内部服务到服务调用中添加了一层弹性和性能的保障，在本篇文章中，我们将这个模型扩展到入口路由。

虽然这篇文章以`Kubernetes`为例，但我们不会使用`Kubernetes`内置的`Ingress`资源对象（参见Sarah的帖子）。虽然`Ingress`提供了一种便捷的基于宿主机和基本路径的路由方法，但在本文撰写时，`Kubernetes Ingress`的功能是相当有限的。在下面的例子中，我们将远远超出`Ingress`提供的范围。

### 1、发布Linkerd Service Mesh
-------------

从以前的文章中的基本的`Linkerd Service Mesh`配置开始，我们进行两个更改来支持`Ingress`：我们将修改`Linker`的配置用以添加一个额外的逻辑路由器，我们在`Kubernetes Service`资源对象中调整VIP在`Linkerd`中的范围，完整的配置请查看[linkerd-ingress.yaml](https://raw.githubusercontent.com/linkerd/linkerd-examples/master/k8s-daemonset/k8s/linkerd-ingress.yml):

在`Linkerd`的实例提供了一个新的逻辑路由器，用于处理入口流量并将其路由到相应的服务：

```yaml
routers:
- protocol: http
  label: ingress
  dtab: |
    /srv                    => /#/io.l5d.k8s/default/http ;
    /domain/world/hello/www => /srv/hello ;
    /domain/world/hello/api => /srv/api ;
    /host                   => /$/io.buoyant.http.domainToPathPfx/domain ;
    /svc                    => /host ;
  interpreter:
    kind: default
    transformers:
    - kind: io.l5d.k8s.daemonset
      namespace: default
      port: incoming
      service: l5d
  servers:
  - port: 4142
    ip: 0.0.0.0
```

在这个配置中，我们使用`Linkerd`的路由语法，`dtabs`将请求从域名传递到服务——在这种情况下从`api.hello.world`传递到`api`服务，从`www.hello.world`到`hello`服务。为了简单起见，我们已经为每个域添加了一个规则，但是对于更复杂的设置，也可以轻松地生成映射规则。

我们已经将这个入口路由器添加到每个`Linkerd`实例中 - 以真正`Service Mesh`的方式，我们将在这些实例中完全分配入口流量，使得应用不存在单点故障。

我们还需要修改我们的`Kubernetes Service`对象，以在端口80上用非入口VIP替换出口的VIP——这将允许我们直接将出口流量发送到`Linkerd`的`Service Mesh`中，主要是用于调试的目的，因为这个流量在到达`Linkerd`之前不会被审查（在下一步，我们将解决这个问题）。

对`Kubernetes Service`修改如下：

```yaml
apiVersion: v1
kind: Service
metadata:
  name: l5d
spec:
  selector:
    app: l5d
  type: LoadBalancer
  ports:
  - name: ingress
    port: 80
    targetPort: 4142
  - name: incoming
    port: 4141
  - name: admin
    port: 9990
```

以上所有的修改都可以通过简单运行一个命令就能生效，细节请查看[`Linkerd Service Mesh plus ingress Kubernetes config`](https://raw.githubusercontent.com/linkerd/linkerd-examples/master/k8s-daemonset/k8s/linkerd-ingress.yml)：

```bash
$ kubectl apply -f https://raw.githubusercontent.com/linkerd/linkerd-examples/master/k8s-daemonset/k8s/linkerd-ingress.yml
```

### 2. 部署服务
------------
