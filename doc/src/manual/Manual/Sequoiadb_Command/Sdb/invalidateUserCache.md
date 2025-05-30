##名称##

invalidateUserCache - 清除节点的用户权限缓存

##语法##

**db.invalidateUserCache( [username], [options] )**

##类别##

Sdb

##描述##

该函数用于清除节点的用户权限缓存信息。

##参数##

| 参数名 | 参数类型 | 描述 | 是否必填 |
| ------ | ------ | ------ | ------ |
| username | String | 用户名 | 否 |
| options | Json对象 | [命令位置参数](manual/Manual/Sequoiadb_Command/location.md) | 否 |

> **Note:**
>
> 当不指定 options 时，作用域为所有协调节点、所有数据节点、所有编目节点。

##返回值##

函数执行成功时，无返回值。

函数执行失败时，将抛异常并输出错误信息。

##错误##

当异常抛出时，可以通过 [getLastErrMsg()][getLastErrMsg] 获取错误信息或通过 [getLastError()][getLastError] 获取错误码。更多错误处理可以参考[常见错误处理指南][error_guide]。

##版本##

v5.8 及以上版本

##示例##
* 清楚所有节点的所有用户的权限缓存。

    ```lang-javascript
    > db.invalidateUserCache()
    ```

* 清除数据组‘group1’的所有用户的权限缓存。

    ```lang-javascript
    > db.invalidateUserCache( "", { GroupName: 'group1' } )
    ```

* 清除数据组‘group1’的指定用户的权限缓存。

    ```lang-javascript
    > db.invalidateUserCache( "myuser", { GroupName: 'group1' } )
    ```

[^_^]:
     本文使用的所有引用及链接

[list_info]:manual/Manual/List/list.md
[getLastErrMsg]:manual/Manual/Sequoiadb_Command/Global/getLastErrMsg.md
[getLastError]:manual/Manual/Sequoiadb_Command/Global/getLastError.md
[error_guide]:manual/FAQ/faq_sdb.md