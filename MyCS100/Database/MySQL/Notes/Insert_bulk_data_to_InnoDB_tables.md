Insert bulk data to Innodb tables
=================================
向InnoDB引擎的表里插入大量数据。

起因是在yce创建和删除datacenter的时候，需要初始化和删除关联的另一张nodeport表，每次需要操作2768条数据（因为nodeport默认的范围是30000到32767）。这2000多条数据的循环Insert和Update（自己用Update做的软删除Soft-Delete）最终导致HTTP请求的响应时间达到50s左右。从数值上来看，50s秒延迟不算太大，可以在页面上给一个进度条之类的东西告诉用户正在创建/删除，请等待。但是相比其他的API响应时间还是长了点，另外是第一次遇到这样的问题，需要专门研究研究。

### 背景及David的建议

背景在前面已经进行了简单的描述。跟David沟通之后，我整理的解决方案有下面三条：

* 使用MySQL的批量操作进行。这是因为每次单独的操作都会执行一次提交Commitment，不如批量生成记录后只提交一次（或者比较少的次数），从而减少响应时间。 
* 设置异步队列（不知道这样的叫法对不对），将收到的HTTP请求非数据库操作进行响应，数据库操作则放入队列（假设数据库操作都很慢），然后异步操作数据库，而不用阻塞响应的返回。显然，这样对于K8s操作和数据库需要一致的情况是有害的，可能K8s操作成功而数据库操作失败而导致二者不一致。 
* 采用Websocket，利用服务器可以主动向客户端“推”数据，当数据库操作完毕后再返回响应。

另外，David还提到了如果采用批量SQL的方法，形如`INSERT INTO nodeport VALUES(...),(...),(...);`，这些值应该也是有大小的，不是无限地写。对应的内存大小应该小于等于一个页的大小，大小不允许跨页。

### 来自MySQL官方文档的研究

MySQL的容器化版本是5.7.13，除了本文讨论的内容外，它增加了对JSON数据的支持，相应的操作也值得一看。本文参考的文档是MySQL 5.7的官方手册。

在[9.2.2 INSERT语句的执行速度](http://dev.mysql.com/doc/refman/5.7/en/insert-speed.html)里面提到：为了提高INSERT的速度，可能会将几个小的操作合并为一个大的操作。在理想的情况下，你只需连接一次，然后一次性发送多个新行的数据，并且延迟所有的索引更新和一致性检查直到最后才进行。运行时长可能被下列因素所影响（括号里的数字近似看做占比）：

* 连接时长（3）
* 向服务器发送请求（2）
* 分析请求（2）
* 插入一行（1 * 行大小）
* 插入索引（1 * 索引数）
* 关闭连接（1）

这里没有考虑到第一次打开表所花费的时间。这个时间在每个并行的查询运行时就只完成一次。

表的大小会将插入索引的时间减慢为*logN*倍，假设是B-tree索引。

可以使用下面的方法来加速插入：

* 如果你同一时刻从同一个客户端插入多条数据，使用INSERT和多个VALUES来进行插入。这比使用单行的INSERT插入要快很多倍（某些情形下）。如果你向一张非空表里插入数据，你可以将`bulk_insert_buffer_size`进行调整来让插入变得更快。参见[6.1.5 服务器系统变量](http://dev.mysql.com/doc/refman/5.7/en/server-system-variables.html)
* 当从文本文件中加载表时（比如yce导入初始表的时候），利用`LOAD_DATA_INFILE`，这通常能提高20倍相对于INSERT语句的速度。参见[14.2.6 从文件加载数据](http://dev.mysql.com/doc/refman/5.7/en/load-data.html)
* 利用拥有默认值的列，仅在与默认值不同的时候需要显式（explicit）地插入值。这回减少MySQL分析的时间，并提高INSERT的时间。
* 参见[9.5.5 向InnoDB表里导入大量数据](http://dev.mysql.com/doc/refman/5.7/en/optimizing-innodb-bulk-data-loading.html)来获取向InnoDB引擎的表插入数据的建议
* 参见[9.6.2 向MyISAM表里导入大量数据](http://dev.mysql.com/doc/refman/5.7/en/optimizing-myisam-bulk-data-loading.html)来获取向MyISAM引擎的表插入数据的建议

### 向InnoDB引擎驱动的表中插入大量数据

可能的原因及方法有如下：

* 当向InnoDB导入数据时，关掉自动提交模式（autocommit mode）。因为它会为每个INSERT执行一次日志刷新（log flush，便于回滚）。为了在你的导入时关闭自动提交，用SET_autocommit和COMMIT命令将他们包起来：
```
SET autocommit=0;
... SQL import statements ...
COMMIT;
```
	* mysqldump的参数--opt会创建可以快速导入InnoDB表的数据文件，甚至不需要用上面的两条语句进行包围。

* 如果你在第二主键上有唯一索引（UNIQUE），你可以在导入会话时暂时关闭唯一性检查来加速表导入：
```
SET unique_checks=0;
... SQL import statements ...
SET unique_checks=1;
```
	* 对于达标，这可以节省大量的磁盘I/O，因为InnoDB可以使用它的交换缓存来将第二索引记录写入批处理。确定数据没有重复的。

* 如果你的表里有外键，你可以通过在导入会话时暂时关闭外键检查来提高插入速度：
```
SET foreign_key_checks=0;
... SQL import statement ...
SET foreign_key_checks=1;
```

	* 对于大表，这样也能节省磁盘I/O

* 使用多行插入标识可以减少客户端和服务器间的过度通信：
```
INSERT INTO yourtable VALUES (1,2), (5,5), ...;
```

	* 这个贴士对于任何表都是有效的，不只是InnoDB表

* 当对带有自增属性（auto-increment）的列执行大量的插入时，设置innodb_autoinc_lock_mode为2，代替原有的1。参见[15.8.6 InnoDB里的自增操作](http://dev.mysql.com/doc/refman/5.7/en/innodb-auto-increment-handling.html)

* 当执行大量插入的时候，如果按照主键序列进行插入会更快。InnoDB表使用了聚簇索引(clustered index)，它使得按照主键序列的数据使用起来相对更快。对于不完全满足缓冲池（buffer pool）的表，这种插入方法是特别重要的。

* 对于向InnoDB FULLTEXT索引中插入数据时的性能优化，可采用下面的步骤：
	* 在建表时定义一个名为FTS_DOC_ID的列，类型是BIGINT UNSIGNED NOT NULL，带有一个名为FTS_DOC_ID_INDEX的唯一索引，例如：
```
CREATE TABLE t1 (
	FTS_DOC_ID BIGINT unsigned NOT NULL AUTO_INCREMENT,
	title varchar(255) NOT NULL DEFAULT ",
	text mediumtext NOT NULL,
	PRIMARY KEY (`FTS_DOC_ID`)
	) ENGINE=InnoDB DEFAULT CHARSET=latin1;
	CREATE UNIQUE INDEX FTS_DOC_ID_INDEX on t1(FTS_DOC_ID);
)
```
	* 向表内导入数据
	* 数据导入后创建FULLTEXT索引

注意：
当在建表时添加一列FTS_DOC_ID，确保当FULLTEXT索引列更新时，这一列也被更新，FTS_DOC_ID必须随着每个INSERT和UPDATE而更新。如果你选择不在建表时添加FTS_DOC_ID，并且由InnoDB为你管理DOC ID，InnoDB会自动添加FTS_DOC_ID作为隐藏列，使用CREATE_FULLTEXT_INDEX调用。这个方法，要求会影响到性能的表重建。


