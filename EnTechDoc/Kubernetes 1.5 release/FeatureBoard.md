| Feature Name | Stage | One Line Release Note Description |
| :--: | :--: | :--: |
| Role-based access control | Alpha | Further polishing of the alpha API including a default set of cluster roles. | 
| Dramatically Simplify Kubernetes Cluster Creation | Alpha | Use kubeadm to make it easy to get a cluster running. | 
| Simplify HA Setup for Master | Alpha | Added ability to create/remove clusters w/highly available (replicaed) masters on GCE using kube-up/kube-down scripts. |
| Federated Daemonset | Alpha | Alpha level support for Daemonsets in federation. |
| Federated Deployments | Alpha | Alpha level support for Deployments in federation. |
| Federation: Support generalized deletions, including cascading and ... | Alpha | Cluster federation: Added support for DeleteOptions.OrphanDependents for federation resources. |
| It should be fast and painless to deploy a Federation of Kubernetes | Alpha | Introducing `kubefed`, a new command line tool to simplify federation control plane deployment and cluster registration/deregistration experience. |
| Enable userns support for containers launched by kubelet | Alpha | Added ability to preserve access to host userns when userns remapping is enabled in container runtime. |
| Container Runtime Interface (CRI) | Alpha | Introducing the v1alpha1 CRI API to allow pluggable container runtimes; an experimental docker-CRI integration is ready for testing and feedback. |
| Add support for pod and qos level cgroups | Alpha | Kubelet launches container in a per pod cgroup hiearchy based on quality of service tier | 
| Accounting and feasibility for opaque integer resources in the API | Alpha | Added support for accounting opaque integer resources. |
| Support Windows Server Containers for K8s | Alpha | Added support for Windows Server 2016 nodes and scheduling Windows Server Containers. |
| Allow deployments to correctly indicate they are failing to deploy | Beta | Deployments that cannot make progress in rolling out the newest version will now indicate via the API they are blocked |
| StatefulSets to Beta | Beta | StatefulSets allow workloads that require persistent identity or per-instance storage to be created and managed on Kubernetes. |
| Make StatefulSets safe during cluster failure | Beta | In order to preserve safety guarantees the cluster no longer force deletes pods on un-responsive nodes and users are now warned if they try to force delete pods via the CLI. |
| Authenticated/Authorized access to kubelet API | Beta | Added ability to authenticate/authorize access to the Kubelet API |
| Federated ConfigMap  | Beta | Support for config maps in federation. | Support for config maps in federation. |
| GCP Cloud Provider: Source IP preservation for Virtual IPs | Beta | Opt in source ip preservation for Services with Type NodePort or LoadBalancer | 
| Improve responsiveness of kubelet eviction module | Beta | Kubelet integrates with memcg notification API to detect when a hard eviction threshold is crossed |
| Node Conformance Test | Beta | Introducing the beta version containerized node conformance test gcr.io/google_containers/node-test:0.2 for users to verify node setup. |
| PodDisruptionBudget and /eviction subresource | Beta | Moving PodDisruptionBudget to Beta + Machine drain task doc |
| Communicate replica set and deployment status via conditions | Stable | When replica sets cannot create pods, they will now report detail via the API about the underlying reason | 
| Support deleting previous objects in `kubectl apply` | Stable | `kubectl apply` is now able to delete resources you no longer need with --prune | 
| Show roles in kubectl | Stable | Roles should appear in kubectl get nodes | 
| Support services that reference an external service | Stable | Services can reference another service by DNS name, rather than being hosted in pods. | 
| Dashboard UI: show all user facing resources | Stable | Dashboard UI now shows all user facing objects and their resource usage. |
