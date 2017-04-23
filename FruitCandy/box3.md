MVC

MVVC

DOM

MySQL的INSERT ON DUPLICATE KEY UPDATE命令，用于具有唯一索引的键或主键上，如果该记录存在，则更新其某一字段；若不存在，则新增一条记录。

CQRS vs CRUD

awk 在打印输出的时候，如果用了,间隔，可能也会转换为空格

或许可以通过github的版本控制思路，整一个音乐歌单版本管理。

1. goroutine在获取runtime.Caller()的时候的开销，有没有上下文切换？或者说goroutine调度是否存在上下文切换？从CPU角度来看，执行的指令不一样是有切换的，但上下文不是进程级概念的么？同一个进程里的goroutine上下文怎么区分？另外PC寄存器里的值也是有多个吧？最后goroutine是否有挂起状态？需要去源码里确认一下goroutine的几个状态。


2. grep -o '字符串' file |wc -l
   awk -v RS="@#$j" '{print gsub(/字符串/,"&")}' file
awk '{s+=gsub(/字符串/,"&")}END{print s}' file
