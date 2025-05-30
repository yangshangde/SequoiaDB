package com.sequoiadb.location.killnode;

import java.util.ArrayList;
import java.util.Date;
import java.util.List;

import com.sequoiadb.exception.BaseException;
import com.sequoiadb.exception.SDBError;
import org.bson.BSONObject;
import org.bson.BasicBSONObject;
import org.testng.Assert;
import org.testng.SkipException;
import org.testng.annotations.AfterClass;
import org.testng.annotations.BeforeClass;
import org.testng.annotations.Test;

import com.sequoiadb.base.*;
import com.sequoiadb.commlib.CommLib;
import com.sequoiadb.commlib.GroupMgr;
import com.sequoiadb.commlib.SdbTestBase;
import com.sequoiadb.exception.ReliabilityException;
import com.sequoiadb.fault.KillNode;
import com.sequoiadb.location.LocationUtils;
import com.sequoiadb.task.FaultMakeTask;
import com.sequoiadb.task.TaskMgr;

/**
 * @Description seqDB-31796:同城主备中心同时异常，超过MaxKeepTime节点未恢复，自动停止Critical模式
 * @Author liuli
 * @Date 2023.05.30
 * @UpdateAuthor liuli
 * @UpdateDate 2023.05.30
 * @version 1.10
 */
@Test(groups = "location")
public class Critical31796 extends SdbTestBase {

    private Sequoiadb sdb = null;
    private CollectionSpace dbcs = null;
    private GroupMgr groupMgr;
    private String csName = "cs_31796";
    private String clName = "cl_31796_";
    private String primaryLocation = "guangzhou.nansha_31796";
    private String sameCityLocation = "guangzhou.panyu_31796";
    private String offsiteLocation = "shenzhan.nanshan_31796";
    private int recordNum = 10000;
    private String groupName = null;
    private Integer[] replSizes = { 0, 2, 3 };

    @BeforeClass
    public void setUp() throws ReliabilityException {
        sdb = new Sequoiadb( SdbTestBase.coordUrl, "", "" );
        groupName = SdbTestBase.expandGroupNames.get( 0 );
        if ( CommLib.isStandAlone( sdb ) ) {
            throw new SkipException( "is standalone skip testcase" );
        }
        groupMgr = GroupMgr.getInstance();
        if ( !groupMgr.checkBusiness( 120, true, SdbTestBase.coordUrl ) ) {
            throw new SkipException( "checkBusiness return false" );
        }
        LocationUtils.setTwoCityAndThreeLocation( sdb, expandGroupName,
                primaryLocation, sameCityLocation, offsiteLocation );
        sdb.getReplicaGroup( groupName ).setActiveLocation( primaryLocation );

        CommLib.isLSNConsistency( sdb, groupName );

        if ( sdb.isCollectionSpaceExist( csName ) ) {
            sdb.dropCollectionSpace( csName );
        }
        dbcs = sdb.createCollectionSpace( csName );

        // 创建多个集合
        for ( int i = 0; i < replSizes.length; i++ ) {
            BasicBSONObject option = new BasicBSONObject();
            option.put( "Group", groupName );
            option.put( "ReplSize", replSizes[ i ] );
            option.put( "ConsistencyStrategy", 3 );
            dbcs.createCollection( clName + i, option );
        }
    }

    @Test
    public void test() throws ReliabilityException {
        ReplicaGroup group = sdb.getReplicaGroup( groupName );
        ArrayList< BasicBSONObject > cityLocationNodes = new ArrayList<>();
        ArrayList< BasicBSONObject > primaryLocationNodes = LocationUtils
                .getGroupLocationNodes( sdb, groupName, primaryLocation );
        ArrayList< BasicBSONObject > sameCityLocationNodes = LocationUtils
                .getGroupLocationNodes( sdb, groupName, sameCityLocation );
        cityLocationNodes.addAll( primaryLocationNodes );
        cityLocationNodes.addAll( sameCityLocationNodes );

        // 同城主备中心节点异常停止
        TaskMgr mgr = new TaskMgr();
        for ( BasicBSONObject cityLocationNode : cityLocationNodes ) {
            FaultMakeTask faultTask = KillNode.getFaultMakeTask(
                    cityLocationNode.getString( "hostName" ),
                    cityLocationNode.getString( "svcName" ), 0 );
            mgr.addTask( faultTask );
        }

        mgr.execute();
        Assert.assertTrue( mgr.isAllSuccess(), mgr.getErrorMsg() );

        // 等待节点启动后再正常停止节点，模拟节点停止后不会恢复
        int timeout = 30;
        for ( BasicBSONObject cityLocationNode : cityLocationNodes ) {
            String nodeName = cityLocationNode.getString( "hostName" ) + ":"
                    + cityLocationNode.getString( "svcName" );
            LocationUtils.waitNodeStart( sdb, nodeName, timeout );
            Node node = group.getNode( nodeName );
            node.stop();
        }

        // 异地备中心Location启动Critical模式
        BasicBSONObject options = new BasicBSONObject();
        int maxKeepTime = 2;
        options.put( "MinKeepTime", 1 );
        options.put( "MaxKeepTime", maxKeepTime );
        options.put( "Location", offsiteLocation );
        group.startCriticalMode( options );
        Date beginTime = new Date( System.currentTimeMillis() );

        // 等待时间超过MaxKeepTime
        LocationUtils.validateWaitTime( beginTime, maxKeepTime + 1 );

        // 校验自动停止Critical模式
        LocationUtils.checkGroupStopCriticalMode( sdb, groupName );

        // 集合插入数据
        for ( int i = 0; i < replSizes.length; i++ ) {
            DBCollection dbcl = dbcs.getCollection( clName + i );
            BasicBSONObject docs = new BasicBSONObject( "a", 1 );
            try {
                dbcl.insertRecord( docs );
                Assert.fail( "should error but success" );
            } catch ( BaseException e ) {
                System.out.println( "e -- " + e );
                if ( e.getErrorCode() != SDBError.SDB_CLS_NOT_PRIMARY
                        .getErrorCode() ) {
                    throw e;
                }
            }
        }

        // 等待集群恢复
        group.start();
        Assert.assertTrue(
                groupMgr.checkBusiness( 600, true, SdbTestBase.coordUrl ),
                "failed to restore business" );

        // 再次插入数据并校验
        for ( int i = 0; i < replSizes.length; i++ ) {
            DBCollection dbcl = dbcs.getCollection( clName + i );
            List< BSONObject > batchRecords = CommLib.insertData( dbcl,
                    recordNum );
            BasicBSONObject orderBy = new BasicBSONObject( "a", 1 );
            CommLib.checkRecords( dbcl, batchRecords, orderBy );
        }
    }

    @AfterClass
    public void tearDown() throws ReliabilityException {
        sdb.getReplicaGroup( expandGroupNames.get( 0 ) ).start();
        Assert.assertTrue(
                groupMgr.checkBusiness( 600, true, SdbTestBase.coordUrl ),
                "failed to restore business" );
        LocationUtils.cleanLocation( sdb, expandGroupNames.get( 0 ) );
        if ( sdb.isCollectionSpaceExist( csName ) ) {
            sdb.dropCollectionSpace( csName );
        }
        if ( sdb != null ) {
            sdb.close();
        }
    }
}
