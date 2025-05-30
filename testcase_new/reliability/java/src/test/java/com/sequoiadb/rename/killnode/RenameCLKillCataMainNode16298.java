package com.sequoiadb.rename.killnode;

import java.util.ArrayList;
import java.util.List;

import org.bson.BasicBSONObject;
import org.testng.Assert;
import org.testng.SkipException;
import org.testng.annotations.AfterClass;
import org.testng.annotations.BeforeClass;
import org.testng.annotations.Test;

import com.sequoiadb.base.CollectionSpace;
import com.sequoiadb.base.DBCollection;
import com.sequoiadb.base.Sequoiadb;
import com.sequoiadb.commlib.GroupMgr;
import com.sequoiadb.commlib.GroupWrapper;
import com.sequoiadb.commlib.NodeWrapper;
import com.sequoiadb.commlib.SdbTestBase;
import com.sequoiadb.exception.BaseException;
import com.sequoiadb.exception.ReliabilityException;
import com.sequoiadb.fault.KillNode;
import com.sequoiadb.rename.RenameUtils;
import com.sequoiadb.task.FaultMakeTask;
import com.sequoiadb.task.OperateTask;
import com.sequoiadb.task.TaskMgr;

/**
 * @Description RenameKillMainNode16297.java seqDB-16298:执行renameCL过程中，编目主节点故障
 * @author luweikang
 * @date 2018年11月7日
 */
public class RenameCLKillCataMainNode16298 extends SdbTestBase {

    private List< String > oldCLNameList = new ArrayList<>();
    private List< String > newCLNameList = new ArrayList<>();
    private String csName = "cs_16298_A";
    private String oldCLName = "oldCL_16298_A";
    private String newCLName = "newCL_16298_A";
    private GroupMgr groupMgr = null;
    private String groupName = null;
    private Sequoiadb sdb = null;
    private int clNum = 20;
    private int completeTimes = 0;

    @BeforeClass
    public void setUp() throws ReliabilityException {
        groupMgr = GroupMgr.getInstance();

        // CheckBusiness(true),检测当前集群环境，若存在异常返回false，
        if ( !groupMgr.checkBusinessWithLSN( 20 ) ) {
            throw new SkipException( "checkBusiness return false" );
        }
        groupName = groupMgr.getAllDataGroupName().get( 0 );

        sdb = new Sequoiadb( SdbTestBase.coordUrl, "", "" );
        CollectionSpace cs = sdb.createCollectionSpace( csName );
        for ( int i = 0; i < clNum; i++ ) {
            cs.createCollection( oldCLName + i,
                    new BasicBSONObject( "Group", groupName ) );
            oldCLNameList.add( oldCLName + i );
            newCLNameList.add( newCLName + i );
        }
        sdb.sync();
    }

    @Test
    public void test() throws ReliabilityException {
        GroupWrapper cataGroup = groupMgr.getGroupByName( "SYSCatalogGroup" );
        NodeWrapper cataMaster = cataGroup.getMaster();

        // 建立并行任务
        TaskMgr task = new TaskMgr();
        FaultMakeTask faultTask = KillNode.getFaultMakeTask(
                cataMaster.hostName(), cataMaster.svcName(), 0 );

        Rename renameTask = new Rename();

        task.addTask( faultTask );
        task.addTask( renameTask );
        task.execute();

        Assert.assertTrue( task.isAllSuccess(), faultTask.getErrorMsg() );
        Assert.assertTrue( groupMgr.checkBusinessWithLSN( 120 ) );

        // 继续执行rename将剩下未修改的cl修改,然后再进行检查结果
        for ( int i = 0; i < oldCLNameList.size(); i++ ) {
            if ( completeTimes < i + 1 ) {
                RenameUtils.retryRenameCL( csName, oldCLNameList.get( i ),
                        newCLNameList.get( i ) );
            }
            RenameUtils.checkRenameCLResult( sdb, csName,
                    oldCLNameList.get( i ), newCLNameList.get( i ) );
        }

        // 插入数据
        for ( int i = 0; i < newCLNameList.size(); i++ ) {
            DBCollection cl = sdb.getCollectionSpace( csName )
                    .getCollection( newCLNameList.get( i ) );
            RenameUtils.insertData( cl, 1000 );
            long actNum = cl.getCount();
            Assert.assertEquals( actNum, 1000, "check record num" );
        }

        Assert.assertTrue( groupMgr.checkBusinessWithLSN( 120 ) );
    }

    @AfterClass
    public void tearDown() {
        try {
            sdb.dropCollectionSpace( csName );
        } finally {
            if ( sdb != null ) {
                sdb.close();
            }
        }
    }

    class Rename extends OperateTask {

        @Override
        public void exec() throws Exception {
            try ( Sequoiadb db = new Sequoiadb( SdbTestBase.coordUrl, "",
                    "" )) {
                CollectionSpace cs = db.getCollectionSpace( csName );
                for ( int i = 0; i < oldCLNameList.size(); i++ ) {
                    cs.renameCollection( oldCLNameList.get( i ),
                            newCLNameList.get( i ) );
                    completeTimes++;
                }
            } catch ( BaseException e ) {
                e.printStackTrace();
            }
        }
    }
}
