Overview
==========
gorm is one of the most popular golang ORM now. We could operate our database more convienent and effciently with gorm.

Here is the [Homepage](github.com/jinzhu/gorm) and [GoDoc](https://godoc.org/github.com/jinzhu/gorm) of gorm.

### Basic Usage

#### Connection to database

* import the jinzhu/gorm package:
  * import "github.com/jinzhu/gorm"
* import the mysql driver:
  * import _ "github.com/jinzhu/gorm/dialects/mysql"
* connection to database
  * db, err := gorm.Open("mysql", "root:root@tcp(mysql.mycluster:3306)/ormtest?charset=utf8&parsetTime=True&loc=Local")
  
#### Create Table

At first, we indeed need one struct refering to the table will be used. For example:

```golang
type Student struct {
  gorm.Model
  Name string `gorm:unique`
  Age int32
  Number int32 `gorm:AUTO_INCREMENT`
}
```

* db.CreateTable(&Student{})
* db.Set("gorm:table_options","ENGINE=InnoDB").CreateTable(&Student{})

#### Set Table's Name

In default, table's name will be made according to our struct name:

* Student -> student
* AirCraft -> air_craft

We could set table's name #before creating table# like below: 

* gorm.DefaultTableNameHandler = func(db *gorm.DB, defaultTableName string) string {
    return "mush_" + defaultTableName
  }

then the name upper will be looks like:

* Student -> mush_student
* AirCraft -> mush_air_craft

or we can define another function `TableName()`:

* (Student)TableName() string {
    return "students"
  }


#### Drop Table

There are *3* ways to drop one table:

1. db.DropTable(&Student{})
2. db.DropTable("students")
3. db.DropTableIfExists(&Student{}, "students")

#### Make Index

Now we could use `AddIndex()` and `AddUniqueIndex()` to add indexes.

* db.Model(&Student{}).AddIndex("idx_name", "name")
* db.Model(&Student{}).AddUniqueIndex("unique_idx_name", "name")

#### Add Foreign Key

Through `AddForeignKey()` we add foreign key to one column. The description of parameters is listed below:

1. 1st param: foreign key field
2. 2nd param: destination table(id)
3. 3rd param: ONDELETE
4. 4th param: ONUPDATE

* db.Model(&Student{}).AddForeignKey("city_id", "cities(id)", "RESTRICT", "RESTRICT")

#### Create

On Create, we need one instance of this struct. For example:

```golang
  Jack := &Student{
    Name: "Jack",
    Age: 18,
  }
```

* db.Create(&Jack)

Before we create `Jack`, we would like to first check if it's a new record via:

* db.NewRecord(&Jack) 

#### Retrieve

Query always makes up the most dynamic database operations.

We coud get data throught helper function: `Find()`, `First()`, `Last()`, but on `Where()` condition, we can do below:

* db.Where(&Student{Name:"Jack", Age:18}).Find(&who)
* db.Where(map[string]interface{"name":"Jack","age":18}).Find(&who)
* db.Where([]int32{18,19,20}).Find(&who)

but I didn't figure how slice works in where condition.

On the other side, we could use inline condiditon for querying:

* db.Find(&who, "name = ?", "Jack")
* db.Find(&who, "name <> ? AND age > ?", "Jack", "20")
* db.Find(&who, Student{Name:"Jack"})
* db.Find(&who, map[string]interface{"name":"Jack"})

Or:

* db.Find(&who, "name = ?", "Jack").Or("age = ?", "18")

assuming `who` is the instance of data structure `Student`

Sometimes we may need `FirstOrInit()`. It will find the first matched row or init one new record with given conditions

* db.FirstOrInit(&Jack, User{Name:"Jack"})

Note: Init() is just make one new record to the 1st parameter of `FirstOrInit()`. Use `FirstOrCreate()` if you want to insert the new record:

* db.FirstOrCreate(&Jack, User{Name: "Jack"})

And `Attr()` and `Assign()` helps a lot:

* db.Where(Student{Name: "Jack"}).Attr("Age": 18).FirstOrInit(&b)
* db.Where(Student{Name: "Jack"}).Assign(Student{Age: 18}).FirstOrInit(&b)

But for basic operations, `SELECT` is the easier way.

Select: 

* db.Select("name = ?", "Jack").Find(&b)

Order: 

* db.Order("age desc").Find(&d).Order("age", true).Find(&e)

Limit: 

* db.Limit(3).Find(&f)
* db.Limit(10).Find(&g).Limit(-1).Find(&h)

Offsets:

* db.Offset(2).Find(&i)

Count: 

* db.Where("name = ?", "Jack").Or("name = ?", "Justin").Find(&j).Count(&num1)

Pluck, Seriouly I don't know what does it do ?

* db.Find(&Student{}).Pluck("age", &ages)
* db.Model(&Student{}).Pluck("name", &names)
* db.Table("users").Pluck("name", &names)

Scan:

* db.Table("students").Select("name = ?", "Jack").Scan(&m)
* db.Raw("SELECT name, age FROM students WHERE name="Jack").Scan(&n)

#### Update

We could create a new instance of record and using `Save()` to update all fields.

* db.Save(&a)

If we want to update partial fields, we could do below:

update changed fields: 

* db.Model(&c).Where("name = ?", "Jen").Update("name", "Jack")

update multiple attr, only update those changed fields:

* db.Model(&d).Update(map[string]interface{}{"name": "Sam"})

update multiple attr, only update those changed and non blank fields:

* db.Model(&e).Update(Users{Name: "Maxwell"})

Conversely, we could use `Omit()` to update the other columns: 

* db.Model(&g).Omit("name").Updates(map[string]interface{}{"name": "Jack", "age": 18})

We can only update some of these columns:

* db.Model(&h).UpdateColumn("name", "Jay")


#### Delete

Normal delete will actually delete the record from the table:

* db.Delete(&a)

But instead we got soft delete, if the record/structure has one column/member called "deleted_at", the soft delete will be triggered automatically. 

* db.Delete(&a)

Note that we could access this "created_at", "updated_at" and "deleted_at" column/member. Another problem is that if we delete one specific row, we create the same name(anything unique indexed) row, it will makes error.

### Advanced Usage
