
##名称##

getRootPath - 获取执行当前js脚本的程序（如：sdb shell或者sdbcm）的工作目录。

##语法##

**getRootPath()**

##类别##

Global

##描述##

当运行js脚本时，我们有时候需要获取运行当前js脚本的程序（如：sdb shell或者sdbcm）的工作目录，可通过调用该方法获取。

##参数##

无。

##返回值##

运行当前js脚本的程序的工作目录。

##错误##

无。

##版本##

v3.0及以上版本。

##示例##

假设:  
SequoiaDB安装路径为：/opt/sequoiadb。  
SequoiaDB安装用户信息为：sdbadmin:sdbadmin_group。  
当前工作目录为sdbadmin的Home目录：/home/users/sdbadmin。  
存在/opt/sequoiadb/bin/test/a.js文件，其内容如下：

```lang-javascript
sdbadmin@ubuntu-dev1:~$ pwd
/home/users/sdbadmin
sdbadmin@ubuntu-dev1:~$ cat /opt/sequoiadb/bin/test/a.js
println( 'exePath: ' + getExePath() ) ;
println( 'rootPath:' + getRootPath() ) ;
println( 'selfPath:' + getSelfPath() ) ;
```

启动 sdb shell:

```lang-javascript
sdbadmin@ubuntu-dev1:~$ /opt/sequoiadb/bin/sdb
Welcome to SequoiaDB shell!
help() for help, Ctrl+c or quit to exit
>
```

1. getRootPath()示例。返回当前运行js脚本的程序（即 sdb shell）的工作目录：

	```lang-javascript
	> getRootPath()
	/home/users/sdbadmin
	Takes 0.000122s.
	>
 	```

2. getExePath()示例。返回当前运行js脚本的程序（即 sdb shell）的位置目录：

	```lang-javascript
	> getExePath()
	/opt/sequoiadb/bin
	Takes 0.000122s.
	>
 	```

3. getSelfPath()示例。返回当前运行的js脚本的位置目录：

	```lang-javascript
	> getSelfPath()
	/home/users/sdbadmin
	Takes 0.000297s.
	>
 	```

4. 在import文件中，获取路径信息。需特别注意getSelfPath()的返回值。


	```lang-javascript
	> import( '/opt/sequoiadb/bin/test/a.js')
	exePath: /opt/sequoiadb/bin
	rootPath:/home/users/sdbadmin
	selfPath:/opt/sequoiadb/bin/test
	Takes 0.000401s.
	```