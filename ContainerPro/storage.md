Docker Storage Study
----------
keywords: Aufs(btrfs, devicemapper, vfs, overlayFS), Copy-on-Write, whiteout

Storage operations: 
* New
* Delete
* Modify
* Find
* Diff

More precisely, protoDriver interface:
* String()
* Create()
* Remove()
* Get()
* Put()
* Exists()
* Status()
* Cleanup()

navieDiffDriver

GraphDriver -> drivers list(driver name, driver init method)

Creation of driver object:
1. inspect if environment variables DOCKER_DRIVER and variables DefaultDrivers reading from Docker Daemon -storage-driver configuration  provide legal driver name
2. if they are null, GraphDriver will find a available driver in priority queue. available means this driver registry itself and its underlying realiaztion is supported by operating system.
3. if can not find one available, GraphDriver find the first drivers in  all drivers registed. but it is just for future extensions, all 5 drivers must regist themselves.

