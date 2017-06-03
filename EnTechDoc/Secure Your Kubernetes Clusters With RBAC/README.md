使用RBAC特性让您的Kubernetes集群更加安全
========================================

原文标题：[Secure Your Kubernetes Clusters With RBAC](http://www.opcito.com/secure-kubernetes-clusters-rbac/)

#### 介绍
---------

*Kubernetes*是在集群环境中管理应用程序的开源容器编排工具。在最新的版本(version 1.6)中，*Kubernetes*引入了诸多特性，如*DaemonSet*, *ETCD v3*, 支持更大规模的扩展，高级的调度特性和*Kubeadmin*的改进。此外，新的*RBAC*授权者是*Kubernetes 1.6*的另一个有趣的功能。在*Kubernetes*内，授权作为一个单独的认证步骤发生。授权检查将请求的属性与访问策略进行比较。一个API调用必须满足至少有一个策略才能发生。

*RBAC(Role-Based Access Controll)*代表基于角色的访问控制，允许*Kubernetes*管理员以细粒度的方式将权限应用与*Kubernetes*集群内的所有内容。它在*Kubernetes*中提供额外的安全性，并允许配置灵活的授权策略，可以在不重启容器集群的情况下进行更新。在深入了解*RBAC*的功能之前，我们来看看在*Kubernetes*如何处理授权。


#### 授权模式
-----------

*Kubernetes*可以在启动*ApiServer*启动的时候通过"标记——授权模式“进行选择。*Kubernetes*具有如下几种实现：

* *AlwaysDeny* —— 阻止所有的请求（主要用于测试）

* *AlwaysAllow* —— 允许所有的请求（不需要授权时使用）

* *ABAC* —— 基于属性的访问控制，这里访问规则是由用户配置的，以文件的形式提供给*Kubernetes*

* *Webhook* —— 允许您使用自定义或外部服务通过*REST*接口授权

* *RBAC* —— 基于角色的访问控制，此处访问规则存储在*Kubernetes API*中，也可以预定义或者用户配置

#### 请求属性
------------

在使用授权的请求中要包含如下属性：

* *user* —— 用于用户认证的用户字符串

* *group* —— 用户是其成员的组名称列表

* *extra* —— 认证层提供的任职字符串秘钥的映射

* *request path* —— 请求的*URI*，有助于提供对非资源断点的访问，如`/healthz`或`/api`

* 该请求**是否**访问*API*资源

* 资源请求方式：

    * *Get, list, create, update, patch, watch, proxy, redirect, delete, deletecollection*

* 非资源请求方式：

    * *Get, post, put, delete*

* 正在访问的资源

* 正在访问的子资源

* 正在访问的*Namespace*

* 正在访问的*API*组

* 正在访问的资源的名称

#### ABAC vs RBAC
------------

|           ABAC          |         RBAC        |
|:-----------------------:|:-------------------:|
|*ABAC*是基于属性的控制|*RBAC*是基于角色的控制|
|任何策略改变都需要主机上的文件(*ssh and root file system*)授权| 无需*ssh*授权即可更改策略|
|用户必须重启集群*ApiServer*才能更新权限|用户不必重启*ApiServer*即可更改权限|
|它允许一个简单的基于本地文件的用户配置的授权策略|它允许授权由Kubernetes API驱动|

#### RBAC
------------

基本上，*RBAC*为用户提供细粒度的访问*Kubernetes API*资源。 *RBAC*使用两个对象，即*Roles*和*RoleBindings*，描述权限并连接用户和适当的资源。

* *Roles* —— 角色是一组权限，它们在*Namespace*中定义。

角色有两种风格：

    * *Role* —— 授予对所选*Namespace*内资源的访问权限

    * *ClusterRole* —— 授予对集群级资源的访问权限


* *RoleBindings* —— 将用户或用户组映射到角色，授予角色*Namespace*下用户的权限。

就像*Roles*一样，*RoleBindings*也有两种风格：

    * *RoleBinding* —— 用于将橘色绑定到用户以访问*Namespace*下的资源。  

    * *ClusterRoleBinding* —— 用于将角色绑定到用户以访问集群中的资源。


RBAC的各个属性之间关系的示意图如下：

[](RBAC.jpg)

接下来我们看几个*Roles*和它们的*RoleBindings*的实例：

##### Role
--------

下面的例子展示了如何定义"Role"：允许该角色读取*default*空间下的所有*Pod*:

```yaml
kind: Role
apiVersion: rbac.authorization.k8s.io/v1beta1
metadata:
  namespace: default
  name: pod-reader
rules:
- apiGroups: [""] # "" indicates the core API group
  resources: ["pods"]
  verbs: ["get", "watch", "list"]
```

##### Role Binding: 
--------

下面的例子给出*RoleBindings*赋予*default*命名空间下的*pod-reader*的角色给用户*jane*。该策略允许用户*jane*可以读取*default*命名空间下的所有*pods*:

```yaml
# This role binding allows "jane" to read pods in the "default" namespace.
kind: RoleBinding
apiVersion: rbac.authorization.k8s.io/v1beta1
metadata:
  name: read-pods
  namespace: default
subjects:
- kind: User
  name: jane
  apiGroup: rbac.authorization.k8s.io
roleRef:
  kind: Role
  name: pod-reader
  apiGroup: rbac.authorization.k8s.io
```

##### Cluster Role
-----------

下面的例子展示该*ClusterRole*赋予读取所有*secret*的权限：

```yaml
kind: ClusterRole
apiVersion: rbac.authorization.k8s.io/v1beta1
metadata:
  # "namespace" omitted since ClusterRoles are not namespaced
  name: secret-reader
rules:
- apiGroups: [""]
  resources: ["secrets"]
  verbs: ["get", "watch", "list"]
```

下面的例子展示了*RoleBinding*指向一个*ClusterRoles*关联的*dave*用户，*dave*只能访问*development*空间下的*secret*。

```yaml
# This role binding allows "dave" to read secrets in the "development" namespace.
kind: RoleBinding
apiVersion: rbac.authorization.k8s.io/v1beta1
metadata:
  name: read-secrets
  namespace: development # This only grants permissions within the "development" 
namespace.
subjects:
- kind: User
  name: dave
  apiGroup: rbac.authorization.k8s.io
roleRef:
  kind: ClusterRole
  name: secret-reader
  apiGroup: rbac.authorization.k8s.io
```

#### ClusterRoleBinding
---------------

*ClusterRoleBinding*允许*manager*组下的任何用户都可以读取任意命名空间下的任何*secret*

```yaml
# This cluster role binding allows anyone in the "manager" group to read secrets in any namespace.
kind: ClusterRoleBinding
apiVersion: rbac.authorization.k8s.io/v1beta1
metadata:
  name: read-secrets-global
subjects:
- kind: Group
  name: manager
  apiGroup: rbac.authorization.k8s.io
roleRef:
  kind: ClusterRole
  name: secret-reader
  apiGroup: rbac.authorization.k8s.io
```

#### 总结
------

*Kubernetes*使得所有操作都很容易，*RBAC*使用户能够访问这些*Kubernetes API*资源。他还允许用户动态的配置集群用户的权限，并定义他们可以交互的资源种类。
