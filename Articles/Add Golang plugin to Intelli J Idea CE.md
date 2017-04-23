## 为Intelli J Idea CE配置Golang开发环境 (Mac)

需要安装：

* Go SDK
* Go Plugin
* Vim Emulation

### 1. 为mac安装Go SDK

前往[Golang官网](https://golang.org/dl/)下载go1.7rc1.darwin-amd64.pkg，并安装。


### 2. 为Intelli J Idea CE安装Go语言插件

安装Go语言插件有三种方法：

* 在线安装

打开Intelli J，然后:

Preferences -> Plugins -> Browse Repositories

![](https://github.com/maxwell92/TechTips/blob/master/pics/preference.png)

![](https://github.com/maxwell92/TechTips/blob/master/pics/plugins.png)

搜索关键字Go，并点击安装。

![](https://github.com/maxwell92/TechTips/blob/master/pics/searchgo.png)

如果下载安装速度太慢，可以添加仓库地址：点击 Manage Repositories ，然后点击 + 添加记录：下面两者中的一条：

[https://plugins.jetbrains.com/plugins/alpha/5047](https://plugins.jetbrains.com/plugins/alpha/5047)

[https://plugins.jetbrains.com/plugins/nightly/5047](https://plugins.jetbrains.com/plugins/nightly/5047)

![](https://github.com/maxwell92/TechTips/blob/master/pics/addrepo.png)


* 官网下载已安装好的包

在刚才的页面Preferences -> Plugins里，可以看到一个选项Install plugin from disk，表示可以从本地选择插件包并进行安装。这里支持的包有两种.jar和.zip。

所以可以去Idealli J官网下载相应的插件包并导入。

[Idealli J官网Golang插件下载地址](https://plugins.jetbrains.com/plugin/5047?pr=idea)

![](https://github.com/maxwell92/TechTips/blob/master/pics/ideapr.png)

* 源码编译

首先需要下载好golang插件的源码，[地址](https://github.com/go-lang-plugin-org/go-lang-idea-plugin)

![](https://github.com/maxwell92/TechTips/blob/master/pics/gitgoplugin.png)

解压后，打开Intelli J，并使用File --> Open打开该项目，并File --> Project Structure

![](https://github.com/maxwell92/TechTips/blob/master/pics/projectstructure.png)

在这里先后设置JDK 和 IntelliJ Platform Plugin SDK，这里试验的JDK版本为1.8，在设置IntelliJ Platform Plugin SDK的时候可以重新设置名字为my-go-plugin-test

设置好后还需要在左侧的Modules这里简单设置，选择IntelliJ Platform Plugin，并在下一页设置里设置名称为my-go-plugin-test

![](https://github.com/maxwell92/TechTips/blob/master/pics/modulenext.png)

这里如果是导入已有的Module(即下载的这个包)，在Build 里不会出现Prepare Plugin Module xxx For Deployment的选项，只有自己新建了Module才会

然后在Modules里设置Dependencies和Paths. 

![](https://github.com/maxwell92/TechTips/blob/master/pics/moduledepedent.png)

![](https://github.com/maxwell92/TechTips/blob/master/pics/modulepath.png)

注意：必须启用插件Gradle，还可能需要一个org.idea.grammar?的包(这个包可以从官网下载)

TODO: *这里有些问题*

设置结束就可以开始编译插件包了: Build --> Prepare Plugin Module "my-go-plugin-test" For Deployment 

![](https://github.com/maxwell92/TechTips/blob/master/pics/build.png)

待编译成功后，可以再从上面的Install plugin from disk进行添加，添加并Apply后要重启IntelliJ，

TODO: *这里可以成功编译和生成插件jar包，但是导入以后并不能生效，应该还是插件配置有问题*


### 3. 为Intelli J Idea CE安装Vim模拟器

如果是喜欢使用Vim环境的同学，可以给Intelli J Idea CE配置Vim模拟器，然后就可以沉溺在“HJKL-上上下下左右左右”的快感中了。我们可以通过简单的在线安装方式完成：

![](https://github.com/maxwell92/TechTips/blob/master/pics/idealli%20j%20vim.png)






