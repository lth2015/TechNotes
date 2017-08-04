实战 | 使用Prometheus监控Kubernetes集群和应用
====================================

### 一、环境准备
----------------

必要的环境：

* Kubernetes集群，版本1.4以上

* 相关镜像准备:

	* gcr.io/google_containers/kube-state-metrics:v0.5.0

	* prom/prometheus:v1.7.0

	* prom/node-exporter:v0.14.0

	* giantswarm/tiny-tools

	* dockermuenster/caddy:0.9.3

	* grafana/grafana:4.2.0

	* quay.io/prometheus/alertmanager:v0.7.1

可以在这里[下载](prom.tar)

* 将上述镜像下载到本地后，使用`docker load`命令加载到*Kubernetes*每台Node节点上。


### 二、Prometheus架构介绍
---------------------
