package com.sequoiadb.lob.killnode;

import java.util.Date;
import java.util.List;

import org.bson.BSONObject;
import org.bson.BasicBSONObject;
import org.bson.types.MaxKey;
import org.bson.types.MinKey;
import org.bson.types.ObjectId;
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
import com.sequoiadb.lob.LobUtil;
import com.sequoiadb.task.FaultMakeTask;
import com.sequoiadb.task.OperateTask;
import com.sequoiadb.task.TaskMgr;

/**
 * @Description seqDB-19064 主表进行lob操作过程中，编目主节点异常重启
 * @author luweikang
 * @date 2019年9月4日
 */
public class LobSubCL19064 extends SdbTestBase {
    private String csName = "cs_19064";
    private String mainCLName = "mainCL_19064";
    private String subCLName = "subCL_19064";
    private GroupMgr groupMgr = null;
    private Sequoiadb sdb = null;
    private DBCollection mainCL;
    private int writeLobSize = 1024 * 1024 * 10;
    private byte[] lobBuff;
    private List< ObjectId > lobIds;

    @BeforeClass
    public void setUp() throws ReliabilityException {
        groupMgr = GroupMgr.getInstance();

        // CheckBusiness(true),检测当前集群环境，若存在异常返回false，
        if ( !groupMgr.checkBusinessWithLSN( 120 ) ) {
            throw new SkipException( "checkBusinessWithLSN return false" );
        }

        sdb = new Sequoiadb( SdbTestBase.coordUrl, "", "" );
        if ( sdb.isCollectionSpaceExist( csName ) ) {
            sdb.dropCollectionSpace( csName );
        }
        mainCL = createMainCLAndAttachCL();
        lobBuff = LobUtil.getRandomBytes( writeLobSize );
    }

    @Test
    public void test() throws ReliabilityException {
        GroupWrapper catalogGroup = groupMgr
                .getGroupByName( "SYSCatalogGroup" );
        NodeWrapper cataMaster = catalogGroup.getMaster();

        // 建立并行任务
        FaultMakeTask faultTask = KillNode.getFaultMakeTask(
                cataMaster.hostName(), cataMaster.svcName(), 0 );
        TaskMgr mgr = new TaskMgr( faultTask );

        PutLob puLobTask = new PutLob();
        mgr.addTask( puLobTask );
        mgr.execute();

        Assert.assertTrue( mgr.isAllSuccess(), mgr.getErrorMsg() );
        Assert.assertTrue( groupMgr.checkBusinessWithLSN( 120 ) );

        LobUtil.checkLobMD5( mainCL, lobIds, lobBuff );
        List< ObjectId > lobIds2 = LobUtil.createAndWriteLob( mainCL, lobBuff );
        LobUtil.checkLobMD5( mainCL, lobIds2, lobBuff );
        for ( ObjectId lobId : lobIds2 ) {
            mainCL.removeLob( lobId );
        }
        checkRemoveLobResult( lobIds2 );

        sdb.sync();
    }

    @AfterClass
    public void tearDown() {
        try {
            if ( sdb.isCollectionSpaceExist( csName ) ) {
                sdb.dropCollectionSpace( csName );
            }
        } finally {
            if ( sdb != null ) {
                sdb.close();
            }
        }
    }

    class PutLob extends OperateTask {

        @Override
        public void exec() throws Exception {
            try ( Sequoiadb db = new Sequoiadb( SdbTestBase.coordUrl, "",
                    "" )) {
                DBCollection mainCL = db.getCollectionSpace( csName )
                        .getCollection( mainCLName );
                lobIds = LobUtil.createAndWriteLob( mainCL, lobBuff );
            }
        }
    }

    private DBCollection createMainCLAndAttachCL() {
        CollectionSpace cs = sdb.createCollectionSpace( csName );
        BSONObject options = new BasicBSONObject();
        options.put( "IsMainCL", true );
        options.put( "ShardingKey", new BasicBSONObject( "date", 1 ) );
        options.put( "ShardingType", "range" );
        options.put( "LobShardingKeyFormat", "YYYYMMDD" );
        DBCollection mainCL = cs.createCollection( mainCLName, options );

        cs.createCollection( subCLName );

        BSONObject bound = new BasicBSONObject();
        bound.put( "LowBound", new BasicBSONObject( "date", new MinKey() ) );
        bound.put( "UpBound", new BasicBSONObject( "date", new MaxKey() ) );
        mainCL.attachCollection( csName + "." + subCLName, bound );

        return mainCL;
    }

    private void checkRemoveLobResult( List< ObjectId > lobIds ) {
        for ( ObjectId lobId : lobIds ) {
            try {
                mainCL.openLob( lobId );
                Assert.fail( "the lob: " + lobId
                        + " has been deleted and the read should fail" );
            } catch ( BaseException e ) {
                if ( e.getErrorCode() != -4 ) {
                    throw e;
                }
            }
        }

    }
}
