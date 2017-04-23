[insert on duplicate](http://dev.mysql.com/doc/refman/5.7/en/insert-on-duplicate.html)

INSERT INTO user(id, name, age) VALUES ("10000", "Jay", 30), ("10001", "Jen", 32) ON DUPLICATE KEY UPDATE age=100;
or
INSERT INTO user(id, name, age) VALUES ("10000", "Jay", 30), ("10001", "Jen", 32) ON DUPLICATE KEY UPDATE age=VALUES(age);

### Insert:


### Update:

through INSERT ON DUPLICATE KEY UPDATE

[](http://dba.stackexchange.com/questions/28282/whats-the-most-efficient-way-to-batch-update-queries-in-mysql)
