从Google Container Registry(GCR)搜索镜像
=======================

### 第一步: 注册gcloud账号
----------

注册Google Cloude账号：xunmi2004@gmail.com


### 第二步: 安装gcloud命令
----------

在外网机器安装gcloud命令：

#### Red Hat and CentOS
----------

```bash
sudo tee -a /etc/yum.repos.d/google-cloud-sdk.repo << EOM
[google-cloud-sdk]
name=Google Cloud SDK
baseurl=https://packages.cloud.google.com/yum/repos/cloud-sdk-el7-x86_64
enabled=1
gpgcheck=1
repo_gpgcheck=1
gpgkey=https://packages.cloud.google.com/yum/doc/yum-key.gpg
https://packages.cloud.google.com/yum/doc/rpm-package-key.gpg
EOM

yum install google-cloud-sdk
```

#### Debian and Ubuntu only
-----------

```bash
export CLOUD_SDK_REPO="cloud-sdk-$(lsb_release -c -s)"

echo "deb http://packages.cloud.google.com/apt $CLOUD_SDK_REPO main" | sudo tee -a /etc/apt/sources.list.d/google-cloud-sdk.list

curl https://packages.cloud.google.com/apt/doc/apt-key.gpg | sudo apt-key add -

sudo apt-get update && sudo apt-get install google-cloud-sdk
```

### 第三步: 登录
----------

登录glcoude

```bash
#gcloud auth login

>Go to the following link in your browser:
>	 https://accounts.google.com/o/oauth2/auth?redirect_uri=urn%3Aietf%3Awg%3Aoauth%3A2.0%3Aoob&prompt=select_account&response_type=code&client_id=32555940559.apps.googleusercontent.com&scope=https%3A%2F%2Fwww.googleapis.com%2Fauth%2Fuserinfo.email+https%3A%2F%2Fwww.googleapis.com%2Fauth%2Fcloud-platform+https%3A%2F%2Fwww.googleapis.com%2Fauth%2Fappengine.admin+https%3A%2F%2Fwww.googleapis.com%2Fauth%2Fcompute+https%3A%2F%2Fwww.googleapis.com%2Fauth%2Faccounts.reauth&access_type=offline

Enter verification code:
```

在浏览器中输入上面URL后，会返回一串秘钥，拷贝到命令行，回车，登录完成。


### 第四步: 查询镜像
--------

查询项目：

```bash
gcloud alpha container images list --repository=gcr.io/google_containers
```

查询镜像的Tags：

```bash
gcloud alpha container images list-tags gcr.io/google_containers/kubedns-amd64
```


### 附录
---------

kubedns镜像tags列表：

```bash
gcloud alpha container images list-tags gcr.io/google_containers/kubedns-amd64

DIGEST        TAGS      TIMESTAMP
3d3d67f51930  1.9       2016-11-19T01:10:50
39264fd3c998  1.8       2016-09-29T18:44:48
9b811ed606fd  1.7       2016-08-24T23:41:32
ec8d3ddb120d  1.6-test  2016-07-02T04:05:18
3aad94a965b1            2016-07-02T03:58:05
8699d3593b6f            2016-07-02T03:51:36
9d102cc48bbe            2016-07-02T02:37:40
7e63b783cdbf            2016-07-02T02:20:27
ac7672acd351            2016-07-02T00:05:12
2cb02e679acf  1.6       2016-06-30T00:05:57
```

dnsmasq镜像tags列表：

```bash
DIGEST        TAGS  TIMESTAMP
6da81cd706e4  1.1   2016-05-24T20:30:29
6c66fa360516  1.0   2016-05-23T23:44:22
```

exechealthz镜像tags列表:

```bash
gcloud alpha container images list-tags gcr.io/google_containers/exechealthz-amd64
DIGEST        TAGS    TIMESTAMP
7ecf20f93a63  v1.2.0  2016-09-27T01:05:09
503e158c3f65  1.2     2016-09-23T01:15:29
2e2a318b5627  1.1     2016-07-28T20:49:53
a5c595bcc2cd  1.0     2016-03-31T03:30:59
```
```
