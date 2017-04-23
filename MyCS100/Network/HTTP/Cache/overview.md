Cache
========
### 背景
前端HTML更新后，必须手动刷新才能更新页面

解决方法：
* 浏览器设置不缓存
* 前端html代码添加xxx标记
* 后端server返回一些控制头，比如Cache-Control: no-cache, mush-revalidate, max-age=0, no-store等

其他信息
多次刷新是cache-control:max-age=0
返回http code为304 Not Modified
请求的时候会发送If-Modified-Since的时间，服务器收到后与本地文件进行比较，如果不匹配，说明文件有改动。否则返回304.

### 参考
* [](https://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html)
* [](https://developers.google.com/web/fundamentals/performance/optimizing-content-efficiency/http-caching?hl=zh-cn)
* [](http://www.alloyteam.com/2012/03/web-cache-2-browser-cache/)
* [](http://www.procata.com/cachetest/tests/pragma/fail.php)
