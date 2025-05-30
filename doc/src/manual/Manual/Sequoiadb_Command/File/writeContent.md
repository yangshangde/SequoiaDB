##名称##

writeContent - 将 fileContent 对象中的二进制内容写入文件中

##语法##

**file.writeContent(\<fileContent\>)**

##类别##

File

##描述##

将 fileContent 对象中的二进制内容写入文件中。

##参数##

| 参数名      | 参数类型         | 默认值 | 描述                                    | 是否必填 |
| ----------- | ---------------- | ------ | --------------------------------------- | -------- |
| fileContent | fileContent 对象 | ---    | 往文件中写入的 fileContent 的二进制内容 | 是       |

##返回值##

无返回值。

##错误##

如果出错则抛异常，并输出错误信息，可以通过[getLastErrMsg()](manual/Manual/Sequoiadb_Command/Global/getLastErrMsg.md)获取错误信息或通过[getLastError()](manual/Manual/Sequoiadb_Command/Global/getLastError.md)获取错误码。关于错误处理可以参考[常见错误处理指南](manual/FAQ/faq_sdb.md)。

常见错误可参考[错误码](manual/Manual/Sequoiadb_error_code.md)。

##版本##

v3.2 及以上版本

##示例##

* 打开一个二进制文件，获取文件描述符

    ```lang-javascript
    > var file = new File( "/opt/sequoiadb/file.dump" )
    ```

* 读取二进制文件的内容并存入 fileContent 对象中

    ```lang-javascript
    > var content = file.readContent()
    > content instanceof FileContent
    true
    ```

* 往文件中写入 fileContent 对象中的二进制内容

    ```lang-javascript
    > var file2 = new File( "/opt/sequoiadb/file2.dump" )
    > file2.writeContent( content )
    ```
