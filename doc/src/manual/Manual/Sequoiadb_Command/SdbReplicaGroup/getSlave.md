##名称##

getSlave - 获取当前复制组的备节点

##语法##

**rg.getSlave([positions])**

##类别##

SdbReplicaGroup

##  描述

获取当前复制组的备节点。

##参数##

* `positions` ( *Int32*， *选填* )

   节点位置。目前，节点位置定义为该节点在 catalog 元数据的 "Group" 数组中的位置。其起始值为1，范围为[1,7]。最多只能输入7个不重复的、有效节点位置。若指定的节点位置大于复制组的节点数，这些节点位置将按照公式 (position - 1) % nodeCount + 1 进行转换。
   

##返回值##

成功：返回 SdbNode 对象。  

失败：抛出异常。

**Note:**

1. 当复制组只有一个节点，不管是否指定节点位置，直接返回唯一的节点对象，即使该节点为主节点。
2. 当复制组有多个节点，在不指定节点位置的情况下，返回的节点必为备节点。
3. 当复制组有多个节点，在指定节点位置的情况下，若节点位置包含备节点，则随机返回包含的备节点；若节点位置只包含主节点，则返回主节点。

##错误##

`getSlave()`函数常见异常如下：

| 错误码 | 错误类型 | 描述 | 解决方法 |
| ------ | ------ | --- | ------ |
| -6 | SDB_INVALIDARG | 参数错误。| 输入的节点位置参数有误。|
| -10 | SDB_SYS | 系统错误。| 一般由 catalog 元素数据变更后，驱动无法解析元数据导致。|
| -154 | SDB_CLS_GRP_NOT_EXIST | 分区组不存在。| 检查分区组是否存在。|
| -158 | SDB_CLS_EMPTY_GROUP | 分区组为空。 | 检查分区组是否包含任何节点。|

当异常抛出时，可以通过[getLastError()](manual/Manual/Sequoiadb_Command/Global/getLastError.md)获取[错误码](manual/Manual/Sequoiadb_error_code.md)，
或通过[getLastErrMsg()](manual/Manual/Sequoiadb_Command/Global/getLastErrMsg.md)获取错误信息。
可以参考[常见错误处理指南](manual/FAQ/faq_sdb.md)了解更多内容。

##版本##

* v2.10加入节点位置参数，用于获取指定位置的备节点。
* v1.0 添加此接口。

##示例##

1. 获取 group1 复制组的备节点。

	```lang-javascript
	> var rg = db.getRG("group1")
	> rg.getSlave()
	hostname1:42000
	```

2. group1 复制组信息如下：

	```lang-javascript
	> db.list(SDB_LIST_GROUPS, {"GroupName":"group1"})
	{
    	"Group": [
    	{
      	"HostName": "hostname1",
      	"Status": 1,
      	"dbpath": "/sequoiadb/database/40000/",
      	"Service": [
        	{
          	"Type": 0,
          	"Name": "40000"
        	},
        	{
          	"Type": 1,
          	"Name": "40001"
        	},
        	{
          	"Type": 2,
          	"Name": "40002"
        	}
      	],
      	"NodeID": 1001
    	},
    	{
      	"HostName": "hostname1",
      	"Status": 1,
      	"dbpath": "/sequoiadb/database/41000/",
      	"Service": [
        	{
          	"Type": 0,
          	"Name": "41000"
        	},
        	{
          	"Type": 1,
          	"Name": "41001"
        	},
        	{
          	"Type": 2,
          	"Name": "41002"
        	}
      	],
      	"NodeID": 1002
    	},
    	{
      	"HostName": "hostname1",
      	"Status": 1,
      	"dbpath": "/sequoiadb/database/42000/",
      	"Service": [
        	{
          	"Type": 0,
          	"Name": "42000"
        	},
        	{
          	"Type": 1,
          	"Name": "42001"
        	},
        	{
          	"Type": 2,
          	"Name": "42002"
        	}
      	],
      	"NodeID": 1003
    	}
    	],
    	"GroupID": 1001,
    	"GroupName": "group1",
    	"PrimaryNode": 1002,
    	"Role": 0,
    	"SecretID": 1425460557,
    	"Status": 1,
    	"Version": 7,
    	"_id": {
    	"$oid": "5a045460c517f3cf06a32976"
    	}
	}
	```
	
	其中，节点为：  
    hostname1:40000(备节点，节点位置为1)；  
    hostname1:41000(主节点，节点位置为2)；  
    hostname1:42000(备节点，节点位置为3)；  
	从 group1 复制组中，随机获取位置1和位置2节点中的备节点：

	```lang-javascript
	> var rg = db.getRG("group1")
	> rg.getSlave(1,2)
	hostname1:40000
	```
