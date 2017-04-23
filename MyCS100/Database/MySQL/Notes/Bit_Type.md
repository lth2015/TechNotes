Bit Type
=======

[bit-type](http://dev.mysql.com/doc/refman/5.7/en/bit-type.html)
[bit-value-literals](http://dev.mysql.com/doc/refman/5.7/en/bit-value-literals.html)

CREATE TABLE t(b BIT(8) ;
ALTER TABLE t ADD COLUMN name VARCHAR(255) NOT NULL ;
INSERT t VALUES(b'01001100', "mushroom");
*SELECT * FROM t;*
*L | mushroom*
