package com.sequoiadb.transaction.restartnode;

import java.util.List;

import org.testng.Assert;
import org.testng.SkipException;
import org.testng.annotations.AfterClass;
import org.testng.annotations.BeforeClass;
import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import com.sequoiadb.base.Sequoiadb;
import com.sequoiadb.commlib.CommLib;
import com.sequoiadb.commlib.GroupMgr;
import com.sequoiadb.commlib.GroupWrapper;
import com.sequoiadb.commlib.NodeWrapper;
import com.sequoiadb.commlib.SdbTestBase;
import com.sequoiadb.exception.ReliabilityException;
import com.sequoiadb.fault.NodeRestart;
import com.sequoiadb.task.FaultMakeTask;
import com.sequoiadb.task.TaskMgr;
import com.sequoiadb.transaction.common.TransUtil;
import com.sequoiadb.transaction.common.TransferTh;

/**
 * @Description seqDB-18517: hash分区表/主子表，转账的过程中正常重启所有数据节点主节点
 * @author yinzhen
 * @date 2019-6-19
 *
 */
@Test(groups = { "rc", "rcauto" })
public class Transaction18517 extends SdbTestBase {
    private Sequoiadb sdb;
    private String hashCLName = "cl18517_hash";
    private String mainCLName = "cl18517_main";
    private String subCLName1 = "subcl18517_1";
    private String subCLName2 = "subcl18517_2";
    private GroupMgr groupMgr;
    private List< String > groupNames;
    private int sum = 100000000;

    @BeforeClass
    public void setUp() throws ReliabilityException {
        sdb = new Sequoiadb( SdbTestBase.coordUrl, "", "" );
        if ( CommLib.isStandAlone( sdb ) ) {
            throw new SkipException( "STANDALONE MODE" );
        }
        groupNames = CommLib.getDataGroupNames( sdb );
        if ( groupNames.size() < 2 ) {
            throw new SkipException( "ONE GROUP MODE" );
        }
        groupMgr = GroupMgr.getInstance();
        if ( !groupMgr.checkBusiness( TransUtil.ClusterRestoreTimeOut ) ) {
            throw new SkipException( "GROUP ERROR" );
        }

        // 创建hash分区表/主子表(主表下挂载多个子表，子表覆盖分区表)，replSize设置为1，且已切分到所有组上，切分键为账户字段
        // 并插入数据 10000 个账户，每个账户 10000 元
        TransUtil.createCLsAndInsertData( sdb, csName, hashCLName, mainCLName,
                subCLName1, subCLName2 );
    }

    @AfterClass
    public void tearDown() throws InterruptedException {
        TransUtil.cleanEnv( sdb, csName, hashCLName, mainCLName );
    }

    @DataProvider(name = "getCL")
    private Object[][] getCLName() {
        return new Object[][] { { hashCLName }, { mainCLName } };
    }

    @Test(dataProvider = "getCL")
    public void test( String clName )
            throws ReliabilityException, InterruptedException {
        // 正常重启所有数据节点的主节点
        TaskMgr taskMgr = new TaskMgr();
        FaultMakeTask task = null;
        for ( String groupName : groupNames ) {
            groupMgr.setSdb( new Sequoiadb( SdbTestBase.coordUrl, "", "" ) );
            GroupWrapper group = groupMgr.getGroupByName( groupName );
            NodeWrapper node = group.getMaster();
            task = NodeRestart.getFaultMakeTask( node, 60, 10 );
            taskMgr.addTask( task );
        }
        TransUtil.setTimeTask( taskMgr, task );

        for ( int i = 0; i < 200; i++ ) {
            taskMgr.addTask( new TransferTh( csName, clName ) );
        }
        taskMgr.execute();

        Assert.assertTrue( taskMgr.isAllSuccess(), taskMgr.getErrorMsg() );
        Assert.assertTrue( groupMgr.checkBusinessWithLSN(
                TransUtil.ClusterRestoreTimeOut ), "GROUP ERROR" );

        // 待集群正常后，查询所有账户的金额总和
        TransUtil.checkSum( sdb, csName, clName, sum,
                this.getClass().getName() );
    }
}
