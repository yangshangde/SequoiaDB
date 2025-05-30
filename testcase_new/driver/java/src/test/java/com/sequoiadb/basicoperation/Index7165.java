package com.sequoiadb.basicoperation;

import java.util.ArrayList;
import java.util.Date;

import org.bson.BSONObject;
import org.bson.BasicBSONObject;
import org.bson.util.JSON;
import org.testng.Assert;
import org.testng.annotations.AfterClass;
import org.testng.annotations.BeforeClass;
import org.testng.annotations.Test;

import com.sequoiadb.base.CollectionSpace;
import com.sequoiadb.base.DBCollection;
import com.sequoiadb.base.DBCursor;
import com.sequoiadb.base.Sequoiadb;
import com.sequoiadb.exception.BaseException;
import com.sequoiadb.testcommon.SdbTestBase;

/**
 * Copyright (C), 2016-2016, ShenZhen info. Co., Ltd. FileName: Index7165.java
 * TestLink: seqDB-7165/seqDB-7166
 * 
 * @author zhaoyu
 * @Date 2016.9.21
 * @version 1.00
 */
public class Index7165 extends SdbTestBase {
    private Sequoiadb sdb;
    private String coordAddr;
    private String commCSName;

    @BeforeClass
    public void setUp() {
        this.coordAddr = SdbTestBase.coordUrl;
        this.commCSName = SdbTestBase.csName;
        try {
            sdb = new Sequoiadb( coordAddr, "", "" );
            if ( !sdb.isCollectionSpaceExist( commCSName ) ) {
                sdb.createCollectionSpace( commCSName );
            }
        } catch ( BaseException e ) {
            Assert.fail( "prepare env failed" + e.getMessage() );
        }
    }

    @AfterClass
    public void tearDown() {
        try {
            sdb.disconnect();
        } catch ( BaseException e ) {
            Assert.fail( "clear env failed, errMsg:" + e.getMessage() );
        }
    }

    @Test
    public void test() {
        CollectionSpace cs = sdb.getCollectionSpace( commCSName );
        DBCollection cl = null;
        // create cl
        String clName = "cl7165";
        try {
            if ( cs.isCollectionExist( clName ) ) {
                cs.dropCollection( clName );
            }
            cl = cs.createCollection( clName );
        } catch ( BaseException e ) {
            Assert.fail( "create cl:" + clName + " failed, errMsg:"
                    + e.getMessage() );
        }

        // insert data
        int dataNum = 100;
        ArrayList< BSONObject > insertData = new ArrayList< BSONObject >();

        try {
            for ( int i = 0; i < dataNum; i++ ) {
                BSONObject dataObj = new BasicBSONObject();
                dataObj.put( "a", i );
                insertData.add( dataObj );
            }
            cl.bulkInsert( insertData, 0 );
        } catch ( BaseException e ) {
            Assert.fail( "insert data:" + insertData + " failed, errMsg:"
                    + e.getMessage() );
        }

        // create index
        String indexName = "aIndex";
        BSONObject indexObj = new BasicBSONObject();
        boolean isUnique = true;
        boolean enforced = true;
        int sortBufferSize = 0;
        indexObj.put( "a", -1 );
        indexObj.put( "b", 1 );
        try {
            cl.createIndex( indexName, indexObj, isUnique, enforced,
                    sortBufferSize );
        } catch ( BaseException e ) {
            Assert.fail( "create index failed, errMsg:" + e.getMessage() );
        }
        Assert.assertNotNull( cl.getIndex( indexName ),
                "expect index is not null" );
        ;

        // index scan and check result
        ArrayList< BSONObject > expectData = new ArrayList< BSONObject >();
        for ( int i = 0; i < 50; i++ ) {
            expectData.add( insertData.get( i ) );
        }

        BSONObject matcher = ( BSONObject ) JSON.parse( "{a:{$lt:50}}" );
        BSONObject orderBy = ( BSONObject ) JSON.parse( "{a:1}" );
        DBCursor queryCursor = cl.query( matcher, null, orderBy, null );
        ArrayList< BSONObject > actualData = new ArrayList< BSONObject >();
        while ( queryCursor.hasNext() ) {
            actualData.add( queryCursor.getNext() );
        }
        queryCursor.close();
        Assert.assertEquals( actualData, expectData );

        // check explain
        BSONObject explainOption = ( BSONObject ) JSON.parse( "{Run:true}" );
        DBCursor explainCursor = cl.explain( matcher, null, orderBy, null, 0,
                -1, 0, explainOption );
        while ( explainCursor.hasNext() ) {
            BSONObject record = explainCursor.getNext();

            String scanType = ( String ) record.get( "ScanType" );
            Assert.assertEquals( scanType, "ixscan" );

            String actualIndexName = ( String ) record.get( "IndexName" );
            Assert.assertEquals( actualIndexName, indexName );
        }
        explainCursor.close();

        // check index option seqDB-7166
        DBCursor cursorIndex = cl.getIndex( indexName );
        while ( cursorIndex.hasNext() ) {
            BSONObject record = ( BSONObject ) cursorIndex.getNext()
                    .get( "IndexDef" );

            boolean actualUnique = ( boolean ) record.get( "unique" );
            Assert.assertEquals( actualUnique, isUnique );

            boolean actualEnforced = ( boolean ) record.get( "enforced" );
            Assert.assertEquals( actualEnforced, enforced );

            Assert.assertEquals( record.get( "key" ), indexObj );

        }
        cursorIndex.close();

        int i = 0;
        for ( i = 0; i < 2; i++ ) {
            try {
                BSONObject insertData1 = ( BSONObject ) JSON.parse( "{b:1}" );
                cl.insert( insertData1 );
            } catch ( BaseException e ) {
                if ( e.getErrorCode() != -38 ) {
                    Assert.assertTrue( false,
                            "insert data, errMsg:" + e.getMessage() );
                }
            }
        }

        // drop index
        try {
            cl.dropIndex( indexName );
        } catch ( BaseException e ) {
            Assert.fail( "drop index failed, errMsg:" + e.getMessage() );
        }

        // check index drop successfully ;SEQUOIADBMAINSTREAM-1981
        DBCursor cursorDropIndex = cl.getIndex( indexName );
        while ( cursorDropIndex.hasNext() ) {
            Assert.assertNull( cursorDropIndex.getNext() );
        }
        cursorDropIndex.close();

        // clear env
        if ( cs.isCollectionExist( clName ) ) {
            cs.dropCollection( clName );
        }
    }
}
