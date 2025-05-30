##名称##

isEmptyDir - 判断指定目录是否为空目录

##语法##

**File.isEmptyDir(\<dirName\>)**

##类别##

File

##描述##

判断指定目录是否为空目录。

##参数##

| 参数名  | 参数类型 | 默认值 | 描述     | 是否必填 |
| ------- | -------- | ------ | -------- | -------- |
| dirName | string   | ---    | 目录路径 | 是       |

##返回值##

如果指定目录为空目录，则返回 true，否则，返回 false。

##错误##

如果出错则抛异常，并输出错误信息，可以通过[getLastErrMsg()](manual/Manual/Sequoiadb_Command/Global/getLastErrMsg.md)获取错误信息或通过[getLastError()](manual/Manual/Sequoiadb_Command/Global/getLastError.md)获取错误码。
关于错误处理可以参考[常见错误处理指南](manual/FAQ/faq_sdb.md)。

常见错误可参考[错误码](manual/Manual/Sequoiadb_error_code.md)。

##版本##

v3.2 及以上版本

##示例##


判断指定目录否为空目录

```lang-javascript
> File.isEmptyDir( "/opt/sequoiadb" )
false
```