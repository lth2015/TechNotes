Kubernetes 1.5新特性| 使用kubefed快速部署Kubernetes联邦
==========================================================

Kubernetes 1.5包括一个名为*kubefed*的新命令行工具，来帮助您管理联邦。 *kubefed*帮助您部署一个新的Kubernetes联邦的控制面板，通过控制面板可以向现有联邦添加或删除集群。

本文主要介绍如何通过*kubefed*命令来管理联邦。

注意：kubefed是kubernetes1.5的新特性，还是alpha版。

#### 1. 先决条件
---------------------

本文假设您有一个正在运行的Kubernetes集群。

#### 2. 获取kubefed
---------------------

从[官网首页](https://github.com/kubernetes/kubernetes/blob/master/CHANGELOG.md)下载Kubernetes1.5或以上的Client压缩包(tar.gz)。将压缩包里的二进制文件解压到￥PATH下的某个目录中，并对这些二进制文件设置可执行权限。

```bash
curl -O https://storage.googleapis.com/kubernetes-release/release/v1.5.0/kubernetes-client-linux-amd64.tar.gz

tar -xzvf kubernetes-client-linux-amd64.tar.gz

sudo cp kubernetes/client/bin/kubefed /usr/local/bin

sudo chmod +x /usr/local/bin/kubefed

sudo cp kubernetes/client/bin/kubectl /usr/local/bin

sudo chmod +x /usr/local/bin/kubectl
```

#### 3. 选择一个主机集群
------------------------------

您需要一个Kubernetes集群作为主要集群，主要集群联邦的控制面板。确保您在本地kubeconfig种具有与主要集群对应的kubeconfig条目。您可以通过运行一下命令来验证您是否具有所需的kubeconfig条目：

```bash
kubectl config get-context
```

输出应包含与主要集群对应的条目，类似于以下内容：

```bash 
CURRENT   NAME CLUSTER AUTHINFO NAMESPACE 
gke_myproject_asia-east1-b_gce-asia-east1 gke_myproject_asia-east1-b_gce-asia-east1 gke_myproject_asia-east1-b_gce-asia-east1

```
