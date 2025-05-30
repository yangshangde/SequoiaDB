
##NAME##

isFile - Determine if it is a normal file.

##SYNOPSIS##

***File.isFile( \<filepath\> )***

##CATEGORY##

File

##DESCRIPTION##

Determine if it is a normal file.

##PARAMETERS##

| Name     | Type     | Default | Description | Required or not |
| -------- | -------- | ------- | ----------- | --------------- |
| filepath | string   | ---     | fiel path   | yes             |

##RETURN VALUE##

Return true if the specified file is a normal file, or return false.

##ERRORS##

when exception happen, use [getLastError()](manual/Manual/Sequoiadb_command/Global/getLastError.md) to get the [error code](manual/Manual/Sequoiadb_error_code.md)  and use [getLastErrMsg()](manual/Manual/Sequoiadb_command/Global/getLastErrMsg.md) to get [error message](manual/Manual/Sequoiadb_command/Global/getLastErrMsg.md). For more detial, please  reference to [Troubleshooting](manual/FAQ/faq_sdb.md).

##EXAMPLES##

* Determine if it is a normal file.

```lang-javascript
> File.isFile( "/opt/sequoiadb/file.txt" )
true
```