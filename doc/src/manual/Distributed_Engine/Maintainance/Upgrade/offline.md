[^_^]:
    离线升级

离线升级指在升级过程中服务会暂时性不可用，在整个升级过程完成之后恢复服务。本文档将说明在一台主机上的离线升级流程，按照相同步骤，用户可以依次完成所有主机上的软件升级。

##下载安装包##

用户可前往 SequoiaDB 巨杉数据库官网下载中心下载相应版本的[安装包][install]。

##升级##

###升级说明###

- 升级过程需要使用 root 用户权限。
- 升级过程中不支持输入非英文字符。
- 升级过程中如果输入有误，可按 ctrl+退格键进行删除。

###升级步骤###

下述升级过程以 SequoiaDB v3.4.4 升级至 v5.6.1 为例进行说明，其他版本间的升级操作基本一致。

1. 以 root 用户登陆目标主机，解压 SequoiaDB 巨杉数据库产品包，并为解压得到的 `sequoiadb-5.6.1-linux_x86_64-installer.run` 安装包赋可执行权限

    ```lang-bash
    # tar -zxvf sequoiadb-5.6.1-linux_x86_64-installer.tar.gz
    # chmod u+x sequoiadb-5.6.1-linux_x86_64-installer.run
    ```

2. 运行 `sequoiadb-5.6.1-linux_x86_64-installer.run` 包

   ```lang-bash
   $ ./sequoiadb-5.6.1-linux_x86_64-installer.run --mode text
   ```

3. 提示选择向导语言，输入 2，选择中文
 
   ```lang-text
    Language Selection
    
    Please select the installation language
    [1] English - English
    [2] Simplified Chinese - 简体中文
    Please choose an option [1] : 2
    ```

4. 显示安装协议，输入回车，忽略阅读并同意协议；输入 2 后按回车表示读取全部文件

    ```lang-text   
    ----------------------------------------------------------------------------
    欢迎使用 SequoiaDB 安装向导。
    
    ----------------------------------------------------------------------------
    重要信息：请仔细阅读
    
    下面提供了两个许可协议。
    
    1. SequoiaDB 评估程序的最终用户许可协议
    2. SequoiaDB 最终用户许可协议
    
    如果被许可方为了生产性使用目的（而不是为了评估、测试、试用“先试后买”或演示）获得本程序，单击下面的“接受”按钮即表示被许可方接受 SequoiaDB 最终用户许可协议，且不作任何修改。
    
    如果被许可方为了评估、测试、试用“先试后买”或演示（统称为“评估”）目的获得本程序：单击下面的“接受”按钮即表示被许可方同时接受（i）SequoiaDB 评估程序的最终用户许可协议（“评估许可”），且不作任何修改；和（ii）SequoiaDB 最终用户程序许可协议（SELA），且不作任何修改。
    
    在被许可方的评估期间将适用“评估许可”。
    
    如果被许可方通过签署采购协议在评估之后选择保留本程序（或者获得附加的本程序副本供评估之后使用），SequoiaDB 评估程序的最终用户许可协议将自动适用。
    
    “评估许可”和 SequoiaDB 最终用户许可协议不能同时有效；两者之间不能互相修改，并且彼此独立。
    
    这两个许可协议中每个协议的完整文本如下。
    
    评估程序的最终用户许可协议
    
    
    
    [1] 同意以上协议: 了解更多的协议内容，可以在安装后查看协议文件
    [2] 查看详细的协议内容
    请选择选项 [1] : 
    ```

5. 提示指定待升级的 SequoiaDB 所在路径，输入回车，选择默认路径 `/opt/sequoiadb`；输入路径后按回车表示选择自定义路径

    ```lang-text
    ----------------------------------------------------------------------------
    请选择存在的版本或者进行新的安装
    
    请选择存在的版本或者进行新的安装 [/opt/sequoiadb]: 
    ```

6. 提示切换到升级模式，输入回车，选择升级模式；输入 2 后按回车表示选择覆盖模式

    ```lang-text
    ----------------------------------------------------------------------------
    检测到/opt/sequoiadb下已有安装版本
    
    是否切换到升级模式[upgrade/cover]？
    
    [1] upgrade
    [2] cover
    请选择选项 [1] : 
    ```

    >**Note:**
    >
    > 当指定 upgrade 模式时，如果版本不兼容则无法升级；指定 cover 模式时，将忽略版本兼容性，强制覆盖安装。版本兼容性可参考[兼容性列表][compatibility]。

7. 提示是否强制安装，输入回车，选择不强制安装；输入 y 后按回车表示选择强制安装

    ```lang-text
    ----------------------------------------------------------------------------
    是否强制安装？强制安装时可能会强杀残留进程
    
    是否强制安装 [y/N]: 
    ```

8. 输入回车，确认继续

    ```lang-text
    ----------------------------------------------------------------------------
    安装程序已经准备好将 SequoiaDB 安装到您的电脑。
    
    您确定要继续吗？ [Y/n]: 
    ```

9. 升级完成，可通过 `sequoiadb --version` 检查版本号，并通过 `sdblist` 检查节点是否均已正常启动

    ```lang-text
    ----------------------------------------------------------------------------
    正在安装 SequoiaDB Server 至您的电脑中，请稍候。
    
    正在安装
    0% ______________ 50% ______________ 100%
    #########################################
    
    ----------------------------------------------------------------------------
    安装程序已经将 SequoiaDB Server 安装于您的电脑中。
    
    安装成功，安装报告可查看：/opt/sequoiadb/install_report
    ```

在集群中所有主机完成软件升级后，如果 SequoiaDB 是由 v3.6/5.0.3 以下版本升级至 v3.6/5.0.3 及以上版本，需要手动执行 [sdbupgradeidx][upgrade_index] 工具进行索引升级。

[^_^]:
    本文中用到的所有链接
[install]:http://download.sequoiadb.com/cn/
[upgrade_index]:manual/Distributed_Engine/Maintainance/Mgmt_Tools/upgrade_index.md
[report]:manual/Distributed_Engine/Maintainance/Mgmt_Tools/upgrade_index.md#示例
[compatibility]:manual/Distributed_Engine/Maintainance/Upgrade/compatibility.md