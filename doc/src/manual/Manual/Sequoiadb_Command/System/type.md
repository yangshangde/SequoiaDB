##名称##

type - 获取操作系统类别

##语法##

**System.type()**

##类别##

System

##描述##

获取操作系统类别

##参数##

无

##返回值##

返回操作系统的类别

##错误##

如果出错则抛异常，并输出错误信息，可以通过 [getLastErrMsg()](manual/Manual/Sequoiadb_Command/Global/getLastErrMsg.md) 获取错误信息或通过 [getLastError()](manual/Manual/Sequoiadb_Command/Global/getLastError.md) 获取错误码。关于错误处理可以参考[常见错误处理指南](manual/FAQ/faq_sdb.md)。

常见错误可参考[错误码](manual/Manual/Sequoiadb_error_code.md)。

##版本##

v3.2 及以上版本

##示例##

返回操作系统的类别

```lang-javascript
> System.type()
LINUX
```