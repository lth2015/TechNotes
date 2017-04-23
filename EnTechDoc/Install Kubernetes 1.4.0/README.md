Install Kubernete 1.4.0 on Ubuntu 16.04
===========================================================

#### 1. Installing kubelet and kubeadm on your hosts
-----------------------------------------------------------

```bash
# curl https://packages.cloud.google.com/apt/doc/apt-key.gpg | apt-key add -

# cat <<EOF > /etc/apt/sources.list.d/kubernetes.list
deb http://apt.kubernetes.io/ kubernetes-xenial main
EOF

# apt-get update

# apt-get install -y docker.io kubelet kubeadm kubectl kubernetes-cni
```

#### 2. Initializing your master
-----------------------------------------------------------

```bash
# kubeadm init
```

```bash
# kubectl taint nodes --all dedicated-
```

#### 3. Joining your nodes
-----------------------------------------------------------

```bash
# kubeadm join --token <token> <master-ip>
```

#### 4. Installing a pod network
-----------------------------------------------------------

```bash

# kubectl apply -f https://git.io/weave-kube
```

#### 5. Installing add-ons
-----------------------------------------------------------

##### 5.1 Install heapster
-----------------------------------------------------------

```bash
# git clone https://github.com/lth2015/heapster.git

# cd heapster/deploy

#  ./kube.sh
```


##### 5.2 Install heapster
-----------------------------------------------------------

```bash
# kubectl create -f https://rawgit.com/kubernetes/dashboard/master/src/deploy/kubernetes-dashboard.yaml
```

##### 5.3 Watch the process of the Insatallation
-----------------------------------------------------------

```bash
# watch "kubectl get pods --namespace=kube-system"
```

#### (Optional) Deploy a mircoservice demo
-----------------------------------------------------------

```bash
# git clone https://github.com/microservices-demo/microservices-demo

# kubectl apply -f microservices-demo/deploy/kubernetes/manifests
```

