PHP 驱动提供了数据库操作和集群操作接口。

- 数据库操作包括数据库的连接、用户的创建删除、数据的增删改查、索引的创建删除、快照的获取与重置、集合与集合空间的创建删除操作等操作。
- 集群操作包括管理复制组和数据节点的各种操作，例如：启动或停止复制组、启动或停止数据节点、获取主从数据节点、集合分区等。

##PHP 类实例##

PHP 驱动的有两种类实例。一种用于数据库操作，另一种用于集群操作。

###数据库操作实例###

SequoiaDB 巨杉数据库中的数据存放分为三个级别：

- 数据库

- 集合空间

- 集合

在数据库操作中，可用三个类来分别表示连接、集合空间和集合实例，一个类表示游标实例。

| 类                | 名称       | 描述                               |
| ----------------- | ---------- | ---------------------------------- |
| SequoiaDB         | 数据库类   | 连接实例代表一个单独的数据库连接   |
| SequoiaCS         | 集合空间类 | 集合空间实例代表一个单独的集合空间 |
| SequoiaCollection | 集合类     | 集合实例代表一个单独的集合         |
| SequoiaCursor     | 游标类     | 游标实例代表一个查询产生的游标     |

PHP 驱动需要使用不同的实例进行操作，例如：读取数据的操作需要游标实例，而创建表空间则需要数据库实例。

###集群操作实例###

SequoiaDB 巨杉数据库中的集群操作分为两个级别：

- 复制组 

- 节点

> **Note:**
>
> 复制组包括协调复制组、编目复制组和数据复制组三种类型。

复制组实例和节点实例可以用以下两种类的实例表示：

| 类           | 名称       | 描述                               |
| ------------ | ---------- | ---------------------------------- |
| SequoiaGroup | 复制组类   | 复制组实例代表一个单独的复制组     |
| SequoiaNode  | 节点类     | 节点实例代表一个单独的节点         |

- replicagroup 的实例用于管理复制组。其操作包括启动、停止复制组，获取复制组中节点的状态、名称信息、数目信息等。

- replicanode 的实例用于管理节点。其操作包括启动、停止指定的节点，获取指定节点实例、获取主从节点实例、获取数据节点地址信息等。

> **Note:**
>
> 与集群相关的操作需要使用复制组及数据节点实例

##错误信息##

* 一个函数被成功调用则返回 true（或整型 1），否则返回值为 false（或整型 0）。
* 如果用户需要知道详细的错误信息，可以调用 getError() 获取错误信息，如果没有错误，则会输出“No error message”。
