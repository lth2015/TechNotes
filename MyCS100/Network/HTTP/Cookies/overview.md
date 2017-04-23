Cookies
=========
### 背景

背景有二：
1.  potatoHelper需要了解Cookies的基本原理，然后用来编写相应的golang程序。
2.  YCE需要做一些会话管理，也应当补充相应的Cookies知识。

### Cookies的目标
Cookies，不管它是小饼干或者什么你能叫得出名字的东西，它很有用。Cookies的主要目标是管理服务器和客户端之间的状态，这个状态就是Cookies。

所谓状态，尤其是现在常说的微服务里，通常建议采用无状态的服务，这个状态本质上是描述系统的某种状态的数据。

### Cookies相关的标准

Cookies并没有被编入标准化的HTTP/1.1的RFC2616中，而网景公司于1994年前后设计并制定了相关规格标准，除此之外还有RFC2109, RFC2965以及RFC6265。但目前最常见的Cookies并不是属于任何一个RFC，是基于网景公司的业界事实标准进行的扩展（严格来说，cookie0是网景扩展，cookie1是RFC2965，但没有cookie0应用广泛）。

Cookies的工作机制是用户识别及状态管理。网站为了管理用户，会在用户访问时将一些数据写入用户计算机内，用户随后访问网站可以获取到这些Cookies。

因为浏览器端负责存放了数据，因此称为“客户端侧状态”。这个Cookie规范的正式名称为HTTP状态管理机制（HTTP management mechanism）。

### Cookies的首部和方法

Cookies的首部字段：

|首部字段名|说明|首部类型|
|:--------:|:--:|:------:|
|Set-Cookie|开始状态管理所使用的Cookie信息|响应首部|
|Cookie|服务器接收到的Cookie信息|请求首部|

具体来说，Set-Cookie字段的属性：

|属性|说明|
|:--:|:--:|
|NAME=VALUE|Cookie的键和值|
|expires=DATE|过期时间（默认为浏览器关闭），格式为DD-Mon-YY HH:MM:SS GMT|
|domain=域名|Cookie作用的域名，只有这个域才能看到这个Cookie|
|path=PATH|路径（将服务器上的文件目录作为Cookie的适用对象），比域更细的绑定|
|Secure|仅在HTTPS时发送|
|HttpOnly|使得Cookie不能被Javascript脚本访问|


需要注意的是：

* Cookies一旦发送给客户端，服务器端是无法直接删除的，只能将其覆盖。
* domain属性可做到跟结尾匹配一致。所以除了针对具体指定的多个域名发送Cookie之外，不指定domain属性显得更安全。
* HttpOnly用于防止跨站脚本攻击（Cross-site-scripting, XSS）对Cookie信息的窃取。但该特性并非是为了防止XSS而开发的。
* 除了Set-Cookie外，还有个Set-Cookie2

cookie1的Set-Cookie2的部分首部：
|属性|说明|
|:--:|:--:|
|Discard|如果提供了这个属性，就会在客户端程序终止时，指示客户端放弃这个Cookie。|
|Max-Age|用于设置cookie的生存期。单位为秒，如果为0表示立即丢弃|
|Port|同Domain，Path，用于提供更精确的绑定|


用户在调用Cookie时，由于可以校验Cookie的有效期、发送方的域、路径、协议等信息，因此正规发布的Cookie是不会被其他Web站点攻击或泄露。

Cookie的存在也影响了缓存，大多数缓存和浏览器都不允许对任何cookie的内容进行缓存。

### Cookies的类型

会话Cookie和持久Cookie。

* 会话Cookie会用来保存用户本次访问时的一些偏好等，用户退出后失效。
* 持久Cookie会用来保存用户周期性访问站点的配置文件、用户名等，用户退出后仍有效。

它们的唯一区别就是过期时间。如果设置了Discard参数或没有设置Expires或Max-age参数来说明扩展时间，那么这个cookie就是一个会话cookie。

### Cookies的工作原理
用户首次访问网站时，网站对用户一无所知，然后给用户贴上一些标记，这些标记就是Cookie，通过Set-Cookie或Set-Cookie2来确定。用户的浏览器收到这些Cookie后会保存到自身的数据库中，以便用户再次访问该网站时读取使用。

多数浏览器会将Cookie写到本地的一个文件里。往往也称之为“Cookie罐”。

浏览器在发送Cookie的时候不会将全部Cookie发往各个网站，除了隐私问题外，A网站的cookie对B网站是无效的，另外就是实际传输的数据量要大于Cookie里看见的。

### Cookie与缓存

* 文档的所有者最好说明文档是否是可以缓存的，如果无法缓存，要标识出来。
    * 设置了Set-Cookie首部之外的文档是可缓存的，就使用Cache-Control: no-cache="Set-Cookie"。和为可缓存的文档使用Cache-Control: public。
* 缓存Set-Cookie首部是要小心。
* 小心处理带有Cookie首部的请求。一定要将私有内容标识为不可缓存的。

### 拓展

往往我们会在浏览某个技术网站时，看到旁边的小窗内有某购物网站的小广告，上面刚好是我们之前搜索过的推荐。广告商除了可以根据IP来返回推荐外，还可以利用Cookie来实现。广告商通过和这些站点进行合作，将广告内容作为一个持久Cookie。当用户访问到这些站点时，由于是同一个域（同一家广告商），与Referer首部结合，所以会将用户的历史累计数据进行分析后完成推荐。当然现代很多浏览器都可以允许用户保护隐私，限制第三方Cookie的使用。

胖URL是嵌入了状态的URL。

### 遗留问题

* 缓存Cookie时的一些细节
* Amazon.com是如何通过会话cookie来跟踪用户的

### 参考资料
1.  图解HTTP，【日】上野 宣 著。于均良 译。人民邮电出版社
2.  HTTP权威指南，【美】David Gourley 等著。陈涓， 赵振平 译。人民邮电出版社。


