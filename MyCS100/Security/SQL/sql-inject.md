SQL注入
=======

### MySQL注入
如果你将用户输入的原始数据直接插入数据库，假设用户输入的不是所要求的字符串，而是一句SQL，那么这就造成了极大的安全隐患，即MySQL注入。它会在你不知不觉的情况下完成对数据库的操作。

#### 一个例子
```MySQL&PHP
// a good user's name
$name = "Jimmy";
$query = "SELECT * FROM users WHERE username = '$name'";
SELECT * FROM users WHERE username = "Jimmy";

// user input that uses SQL Injecttion
$name_bad = "'OR 1'";

// our MySQL query builder is not a very safe one
$query_bad = "SELECT * FROM users WHERE username = '$name_bad'";

// display what the query will look like, with injection:
SELECT * FROM users WHERE username = "OR 1";
```

好的SQL当然没什么问题。坏的这个query_bad会让查询变得跟我们的预期不一致。

它们可能会使用一对单引号来结束我们SQL查询中的字符串，然后带上一个恒成立的OR 1。这样，SQL中的WHERE就失去了它的作用。

上面的还算好的，攻击者通过此方法获取大量的信息。更为糟糕的是，攻击者可以通过DELETE来清空这张表：

```MySQL&PHP
$name_evil = "'; DELETE FROM customers WHERE 1 or username ='";
// our MySQL query builder really should check for injection
$query_evil = "SELECT * FROM users WHERE username = '$name_evil'";

// the new evil injection query would include a DELETE statement
SELECT * FROM users WHERE username = ''; DELETE FROM users WHERE 1 or username=''
```

### 如何防止
在PHP里，应用函数*mysql_real_escape_string来防止这些攻击。它会将SQL语句中可能存在的注入攻击剔除掉，然后重新生成SQL语句。基本上，它将带来问题的单引号'替换为转义单引号\'。

前面的SQL语句将会转换为：

SELECT * FROM users WHERE name = '\' OR 1 '\';
SELECT * FROM users WHERE name = '\'; DELETE FROM users WHERE 1 or name = \'';

在golang中，通过[]interface{}类型的变量vals，将每个DOM的成员通过append(vals, u.name, u.age)，然后拼接VALUES(?)，执行stmt.Exec(vals...)的变参来实现bulk。


在写代码里SQL语句的时候需要预编译（？形式），需要使用占位符。如果采用字符串拼接的话，例如故意输入某些值使得一个查询语句变成如下的形式，就完成了一次SQL注入：
```SQL
    SELECT id,name,status FROM users WHERE age > 20 OR 1=1;
```
在这种情况下，该语句一直成立，就会一直执行最终造成某些损害。


### 参考
[Mysql php sql injection](http://www.tizag.com/mysqlTutorial/mysql-php-sql-injection.php)
[Build web application with golang](https://astaxie.gitbooks.io/build-web-application-with-golang/content/zh/09.4.html)

Single Quote and Double Quote's Difference
How golang prevent sql inject using prepare and query?
How could we prevent sql inject while making sql batch ?

* 增加对用户输入值的校验，利用正则工具等
* 将特殊字符转义
