##名称##

count - 查看回收项目的个数

##语法##

**db.getRecycleBin().count([cond])**

##类别##

SdbRecycleBin

##描述##

该函数用于查看回收站项目的个数。

##参数##

cond（ *object，选填* ）

匹配条件，只返回符合 cond 的记录

##返回值##

函数执行成功时，将返回当前回收站项目的个数。

函数执行失败时，将抛异常并输出错误信息。

##错误##

当异常抛出时，可以通过 [getLastErrMsg()][getLastErrMsg] 获取错误信息或通过 [getLastError()][getLastError] 获取[错误码][error_code]。更多错误处理可以参考[常见错误处理指南][faq]。

##版本##

v3.6 及以上版本

##示例##

查看回收站项目的个数

```lang-javascript
> db.getRecycleBin().count()
```

[^_^]:
    本文使用的所有引用及链接
[getLastErrMsg]:manual/Manual/Sequoiadb_Command/Global/getLastErrMsg.md
[getLastError]:manual/Manual/Sequoiadb_Command/Global/getLastError.md
[faq]:manual/FAQ/faq_sdb.md
[error_code]:manual/Manual/Sequoiadb_error_code.md
