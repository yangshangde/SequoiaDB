package com.sequoiadb.transaction.jdbc2mysql.fault;

import java.sql.SQLException;
import java.util.List;

import org.testng.Assert;
import org.testng.annotations.Test;

import com.sequoiadb.commlib.CommLib;
import com.sequoiadb.commlib.GroupMgr;
import com.sequoiadb.commlib.GroupWrapper;
import com.sequoiadb.commlib.NodeWrapper;
import com.sequoiadb.exception.ReliabilityException;
import com.sequoiadb.fault.KillNode;
import com.sequoiadb.task.FaultMakeTask;
import com.sequoiadb.task.TaskMgr;
import com.sequoiadb.transaction.common.TransUtil;
import com.sequoiadb.transaction.jdbc2mysql.common.TransJDBCBase;
import com.sequoiadb.transaction.jdbc2mysql.common.TransferJDBCTh;

/**
 * @Description seqDB-18520: hash分区表/主子表，转账的过程中异常重启所有数据节点主节点
 * @author yinzhen
 * @date 2019-8-14
 *
 */
public class TransactionJDBC18520 extends TransJDBCBase {
    private String clName = "cl18520";
    private GroupMgr groupMgr;
    private List< String > groupNames;

    @Override
    protected void beforeSetUp() throws ReliabilityException {
        initCL( clName, 10000 );
        groupMgr = GroupMgr.getInstance();
        groupNames = CommLib.getDataGroupNames( sdb );
    }

    @Test
    public void test()
            throws ReliabilityException, InterruptedException, SQLException {
        // 异常重启所有数据节点的主节点
        TaskMgr taskMgr = new TaskMgr();
        FaultMakeTask task = null;
        for ( String groupName : groupNames ) {
            GroupWrapper group = groupMgr.getGroupByName( groupName );
            NodeWrapper node = group.getMaster();
            task = KillNode.getFaultMakeTask( node, 60 );
            taskMgr.addTask( task );
        }
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
