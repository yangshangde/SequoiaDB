##NAME##

getDomainName - get the domain name of the collectionspace

##SYNOPSIS##

**db.collectionspace.getDomainName()**

##CATEGORY##

SdbCS

##DESCRIPTION##

This function is used to get the domain name of the collectionspace.

##PARAMETERS##

None

##RETURN VALUE##

When the function executes successfully, it will return an object of type String. 

When the function fails, an exception will be thrown and an error message will be printed.

##ERRORS##

When the exception happens, use [getLastErrMsg()][getLastErrMsg] to get the error message or use [getLastError()][getLastError] to get the [error code][error_code]. For more details, refer to [Troubleshooting][faq].

##VERSION##

v3.2 and above

##EXAMPLES##

Get the domain name of the collectionspace "sample".

```lang-javascript
> db.sample.getDomainName()
mydomain
```

[^_^]:
   links
[getLastErrMsg]:manual/Manual/Sequoiadb_Command/Global/getLastErrMsg.md
[getLastError]:manual/Manual/Sequoiadb_Command/Global/getLastError.md
[faq]:manual/FAQ/faq_sdb.md
[error_code]:manual/Manual/Sequoiadb_error_code.md