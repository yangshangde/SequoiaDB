package com.sequoiadb.transaction;

import java.util.ArrayList;
import java.util.List;

import org.bson.BSONObject;
import org.bson.BasicBSONObject;
import org.testng.Assert;
import org.testng.annotations.AfterClass;
import org.testng.annotations.BeforeClass;
import org.testng.annotations.Test;

import com.sequoiadb.base.DBCollection;
import com.sequoiadb.base.DBCursor;
import com.sequoiadb.base.Sequoiadb;
import com.sequoiadb.exception.BaseException;
import com.sequoiadb.testcommon.SdbTestBase;
import com.sequoiadb.threadexecutor.ThreadExecutor;
import com.sequoiadb.threadexecutor.annotation.ExecuteOrder;

/**
 * @description seqDB-6001:事务1中更新数据为事务2中插入数据_SD.transaction.012
 * @author wangkexin
 * @date 2019.04.08
 * @review
 */
public class Transaction6001 extends SdbTestBase {
    private String clName = "cl6001";
    private Sequoiadb sdb = null;
    private Sequoiadb db1 = null;
    private Sequoiadb db2 = null;
    private DBCollection cl = null;
    private int insertNum = 10000;

    @BeforeClass
    private void setup() {
        sdb = new Sequoiadb( SdbTestBase.coordUrl, "", "" );
        cl = sdb.getCollectionSpace( SdbTestBase.csName )
                .createCollection( clName );
    }

    @Test
    public void test() throws Exception {
        ThreadExecutor es = new ThreadExecutor( 300000 );
        es.addWorker( new TransInsert6001() );
        es.addWorker( new TransUpdate6001() );
        es.run();

        CheckResult();
    }

    @AfterClass
    private void teardown() {
        try {
            sdb.getCollectionSpace( SdbTestBase.csName )
                    .dropCollection( clName );
        } finally {
            if ( sdb != null )
                sdb.close();
            if ( db1 != null )
                db1.close();
            if ( db2 != null )
                db2.close();
        }
    }

    class TransInsert6001 {
        private DBCollection cl = null;

        public TransInsert6001() {
            db1 = new Sequoiadb( SdbTestBase.coordUrl, "", "" );
            cl = db1.getCollectionSpace( SdbTestBase.csName )
                    .getCollection( clName );
        }

        @ExecuteOrder(step = 1, desc = "开启事务")
        private void beginTrans() {
            db1.beginTransaction();
        }

        @ExecuteOrder(step = 2, desc = "插入数据")
        public void Insert() {
            insertData();
        }

        @ExecuteOrder(step = 4, desc = "提交事务")
        public void Commit() {
            db1.commit();
        }

        private void insertData() {
            List< BSONObject > recs = new ArrayList< BSONObject >();
            for ( int i = 0; i < insertNum; i++ ) {
                BSONObject rec = new BasicBSONObject();
                rec.put( "a", i );
                recs.add( rec );
            }
            cl.insert( recs );
        }
    }

    class TransUpdate6001 {
        private DBCollection cl = null;

        public TransUpdate6001() {
            db2 = new Sequoiadb( SdbTestBase.coordUrl, "", "" );
            cl = db2.getCollectionSpace( SdbTestBase.csName )
                    .getCollection( clName );
        }

        @ExecuteOrder(step = 1)
        private void beginTrans() {
            db2.beginTransaction();
        }

        @ExecuteOrder(step = 3, desc = "更新数据")
        public void Update() {
            BSONObject modifyObj = new BasicBSONObject();
            BSONObject modifier = new BasicBSONObject();
            modifyObj.put( "a", 1 );
            modifier.put( "$inc", modifyObj );
            try {
                // 将集合中所有记录a字段的值增加1
                cl.update( null, modifier, null );
                Assert.fail( "exp fail but found succ." );
            } catch ( BaseException e ) {
                Assert.assertEquals( e.getErrorCode(), -13 );
            }
        }

        @ExecuteOrder(step = 5, desc = "提交事务")
        public void Commit() {
            db2.commit();
        }
    }

    private void CheckResult() {
        int startValue = 0;
        long expCount = insertNum;
        long actCount = cl.getCount();
        Assert.assertEquals( actCount, expCount );
        DBCursor cursor = cl.query( null, null, new BasicBSONObject( "a", 1 ),
                null );
        while ( cursor.hasNext() ) {
            Assert.assertEquals( cursor.getNext().get( "a" ).toString(),
                    String.valueOf( startValue ) );
            startValue++;
        }
    }
}