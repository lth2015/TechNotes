使用prometheus-operator搭建kubernetes集群
=========================================


### 环境准备
----

* 准备好Kubernetes 1.9.3 (>=1.8)

* 下载[Prometheus-Opterator](https://github.com/coreos/prometheus-operator)

* 镜像准备：

	* gcr.io/google_containers/addon-resizer:1.0

	* quay.io/brancz/kube-rbac-proxy:v0.2.0

	* quay.io/coreos/configmap-reload:v0.0.1

	* quay.io/coreos/kube-state-metrics:v1.2.0

	* quay.io/coreos/monitoring-grafana:5.0.0

	* quay.io/coreos/prometheus-config-reloader:v0.0.2

	* quay.io/coreos/prometheus-operator:v0.17.0

	* quay.io/prometheus/alertmanager:v0.14.0

	* quay.io/prometheus/node-exporter:v0.15.2

* 确保这些镜像都已经部署在了每台Node节点上

### 开始部署Prometheus
----

```bash
```


