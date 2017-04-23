Role Based Access Controls
---------------------------
—David F, D.Richard
-NIST

### Abstract

在当时，具有多层安全级别的军队采用的是MAC(Mandatory Access Control)访问控制策略，政府和一般企业采用的是比较随意的DAC(Discretionary Access Control)。这篇论文提出对于一般的商业和政府组织使用DAC是没有根据的，同时也不够恰当，因此提出了RBAC模型。

### Introduction
由TCSEC: Trusted Computer Security Evaluatation Criteria提出了计算机安全标准，目的是防止对机密文件未授权的访问。主要提出了前面所说的DAC和MAC。


### Aspects of Security Policies

对可信数据(Confidentiality)和完整性(Integrity)的顾虑

DAC is an access control mechanism that permits system users to allow / disallow other users access to objects under their control: A means of restricting access to objects based on identity of subjects and / or groups which they belong.

很多时候对于数据的权限取决于员工的角色而不是这些数据的归属。

RBAC policy bases access control decisions on the functions a user is allowed to perform within an organization. 

RBAC本质上是一种MAC，但又不同于TCSEC的多级MAC，它更专注于对功能(function)和信息的访问，而不是只严格限制对信息的访问。

Who can perform what act of what information

Roles are group oriented. Each role has a associated set of individual members.

![](https://github.com/maxwell92/TechTips/blob/master/MyCS100/Security/RBAC/pics/role-based.png)

### Formal description of RBAC
![](https://github.com/maxwell92/TechTips/blob/master/MyCS100/Security/RBAC/pics/3-basic-rules-of-rbac.png)
![](https://github.com/maxwell92/TechTips/blob/master/MyCS100/Security/RBAC/pics/formal-description-of-rbac.png)

### Centrally Administering Security Using RBAC

![](https://github.com/maxwell92/TechTips/blob/master/MyCS100/Security/RBAC/pics/multi-role-relationships.png)


### The Least Privilege Principle

### Seperation of Duties
Static policy: checking only roles for users
Dynamic policy: use both role and UserID in checking access to transactions

### Summary and Conclusion
