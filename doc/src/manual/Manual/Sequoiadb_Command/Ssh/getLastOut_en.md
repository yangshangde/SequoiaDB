
##NAME##

getLastOut - Get the result of the last command execution.

##SYNOPSIS##

***getLastOut()***

##CATEGORY##

Ssh

##DESCRIPTION##

Get the result of the last command execution.

##PARAMETERS##

NULL

##RETURN VALUE##

On success, return the object information of the Command object.

On error, exception will be thrown.

##ERRORS##

when exception happen, use [getLastError()](manual/Manual/Sequoiadb_command/Global/getLastError.md) to get the [error code](manual/Manual/Sequoiadb_error_code.md)  and use [getLastErrMsg()](manual/Manual/Sequoiadb_command/Global/getLastErrMsg.md) to get [error message](manual/Manual/Sequoiadb_command/Global/getLastErrMsg.md). For more detial, please reference to [Troubleshooting](manual/FAQ/faq_sdb.md).

##EXAMPLES##

* Connect to the host using ssh.

```lang-javascript
> var ssh = new Ssh( "192.168.20.71", "sdbadmin", "sdbadmin", 22 )
```

* Executed command.

```lang-javascript
> ssh.exec( "ls", "/opt/sequoiadb/file" )
file1
file2
file3
```

* Get the result of the last command execution.

```lang-javascript
> ssh.getLastOut()
file1
file2
file3
```
