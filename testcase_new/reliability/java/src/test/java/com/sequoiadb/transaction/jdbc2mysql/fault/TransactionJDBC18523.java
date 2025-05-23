package com.sequoiadb.transaction.jdbc2mysql.fault;

import java.sql.SQLException;

import org.testng.Assert;
import org.testng.annotations.Test;

import com.sequoiadb.base.Sequoiadb;
import com.sequoiadb.commlib.GroupMgr;
import com.sequoiadb.commlib.NodeWrapper;
import com.sequoiadb.exception.ReliabilityException;
import com.sequoiadb.fault.NodeRestart;
import com.sequoiadb.task.FaultMakeTask;
import com.sequoiadb.task.TaskMgr;
import com.sequoiadb.transaction.common.TransUtil;
import com.sequoiadb.transaction.jdbc2mysql.common.TransJDBCBase;
import com.sequoiadb.transaction.jdbc2mysql.common.TransferJDBCTh;

/**
 * @Description seqDB-18523:hash分区表/主子表，转账的过程中正常重启转账程序执行的coord节点
 * @author yinzhen
 * @date 2019-8-14
 *
 */
public class TransactionJDBC18523 extends TransJDBCBase {
    private String clName = "cl18523";
    private GroupMgr groupMgr;

    @Override
    protected void beforeSetUp() throws ReliabilityException {
        initCL( clName, 10000 );
        groupMgr = GroupMgr.getInstance();
    }

    @Test
    public void test()
            throws ReliabilityException, InterruptedException, SQLException {
        // 正常重启转账程序连接的coord节点
        TaskMgr taskMgr = new TaskMgr();
        NodeWrapper coordNode = TransUtil.getCoordNode(
                new Sequoiadb( TransUtil.getCoordUrl( sdb ), "", "" ) );
        FaultMakeTask task = NodeRestart.getFaultMakeTask( coordNode, 60, 10 );
        taskMgr.addTask( task );
        TransUtil.setTimeTask( taskMgr, task );

        for ( int i = 0; i < 200; i++ ) {
            taskMgr.addTask( new TransferJDBCTh( clName ) );
        }
        taskMgr.execute();

        Assert.assertTrue( taskMgr.isAllSuccess(), taskMgr.getErrorMsg() );
        Assert.assertTrue( groupMgr.checkBusinessWithLSN( 300 ),
                "GROUP ERROR" );

        // 待集群正常后，查询所有账户的金额总和
        TransferJDBCTh.checkTransResult( clName, getInsertNum() * 10000 );
    }
}
