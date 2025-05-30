package com.sequoiadb.test.cs;


import com.sequoiadb.base.CollectionSpace;
import com.sequoiadb.base.Sequoiadb;
import com.sequoiadb.exception.BaseException;
import com.sequoiadb.exception.SDBError;
import com.sequoiadb.test.common.Constants;
import org.junit.*;

import java.util.ArrayList;
import java.util.List;

import static org.junit.Assert.*;

public class DropCreateGetCS {
    private static Sequoiadb sdb;
    private static CollectionSpace cs;
    private static CollectionSpace cs2;

    @BeforeClass
    public static void setConnBeforeClass() throws Exception {
    }

    @AfterClass
    public static void DropConnAfterClass() throws Exception {
    }

    @Before
    public void setUp() throws Exception {
        sdb = new Sequoiadb(Constants.COOR_NODE_CONN, "", "");
        if (sdb.isCollectionSpaceExist(Constants.TEST_CS_NAME_1))
            sdb.dropCollectionSpace(Constants.TEST_CS_NAME_1);
        if (sdb.isCollectionSpaceExist(Constants.TEST_CS_NAME_2))
            sdb.dropCollectionSpace(Constants.TEST_CS_NAME_2);
    }

    @After
    public void tearDown() throws Exception {
        if (sdb.isCollectionSpaceExist(Constants.TEST_CS_NAME_1))
            sdb.dropCollectionSpace(Constants.TEST_CS_NAME_1);
        if (sdb.isCollectionSpaceExist(Constants.TEST_CS_NAME_2))
            sdb.dropCollectionSpace(Constants.TEST_CS_NAME_2);
        sdb.disconnect();
    }

    @Test
    public void testCreateCS() {
        // TODO:
        //create cs name "testfoo"
        cs = sdb.createCollectionSpace(Constants.TEST_CS_NAME_1);
        assertNotNull(cs);
        String csName1 = cs.getName();
        assertEquals(Constants.TEST_CS_NAME_1, csName1);
        //create cs name "testCS2" with default pagesize
        cs2 = sdb.createCollectionSpace(Constants.TEST_CS_NAME_2,
            Sequoiadb.SDB_PAGESIZE_DEFAULT);
        assertNotNull(cs2);
        String csName2 = cs2.getName();
        assertEquals(Constants.TEST_CS_NAME_2, csName2);
    }

    @Test
    public void testListCS() {
        // create cs
        cs = sdb.createCollectionSpace(Constants.TEST_CS_NAME_1);
        cs2 = sdb.createCollectionSpace(Constants.TEST_CS_NAME_2);
        // TODO:
        //list collectionspaces
        List<String> listCS = new ArrayList<String>();
        listCS.addAll(sdb.getCollectionSpaceNames());
        // check
        List<String> name = new ArrayList<String>();
        for (String s : listCS) {
            if (s.equals(Constants.TEST_CS_NAME_1) ||
                s.equals(Constants.TEST_CS_NAME_2))
                name.add(s);
        }
        assertEquals(name.size(), 2);
    }

    //	@Test(expected = BaseException.class)
    @Test
    public void testCreateSameCS() {
        // create cs
        cs = sdb.createCollectionSpace(Constants.TEST_CS_NAME_1);
        // TODO:
        //create the same cs name "testCS1"
        try {
            cs2 = sdb.createCollectionSpace(Constants.TEST_CS_NAME_1);
        } catch (BaseException e) {
            int errono = e.getErrorCode();
            assertEquals(SDBError.SDB_DMS_CS_EXIST.getErrorCode(), errono);
            return;
        }
    }

    @Test
    public void testGetCS() {
        // create cs
        cs = sdb.createCollectionSpace(Constants.TEST_CS_NAME_1);
        // TODO:
        //get the cs name "testCS1"
        cs2 = sdb.getCollectionSpace(Constants.TEST_CS_NAME_1);
        assertNotNull(cs);
        //drop cs
        sdb.dropCollectionSpace(Constants.TEST_CS_NAME_1);
    }

    @Test
    public void testIsCSExist() {
        // create cs
        cs = sdb.createCollectionSpace(Constants.TEST_CS_NAME_1);
        // TODO:
        //test if the cs is exist
        assertTrue(sdb.isCollectionSpaceExist(Constants.TEST_CS_NAME_1));
        //drop cs
        sdb.dropCollectionSpace(Constants.TEST_CS_NAME_1);
    }

    @Test(expected = BaseException.class)
    public void testDropCSAndGetCS() {

        sdb.dropCollectionSpace(Constants.TEST_CS_NAME_1);

        cs = sdb.getCollectionSpace(Constants.TEST_CS_NAME_1);
    }

    @Test(expected = BaseException.class)
    public void testCreateIllegalCS() {
        //create illegal cs name
        String arr[] = {".", "SYS", "$"};
        String name = "foo";
        for (int i = 0; i < arr.length; i++) {
            String cs_name = arr[i] + name;
            cs = sdb.createCollectionSpace(cs_name);
        }
    }

    @Test(expected = BaseException.class)
    public void testCreateNullCS() {
        //create the cs name is ""
        cs = sdb.createCollectionSpace("");
    }

    @Test(expected = BaseException.class)
    public void testCreate_128_CS() {
        //create cs name's legth is 128
        String cs_128 = "";
        for (int i = 0; i < 128; i++) {
            cs_128 += "a";
        }
        cs = sdb.createCollectionSpace(cs_128);
    }

    //the cs name's length is 127B
    @Test
    public void testCreate_127_CS() {
        String cs_127 = "";
        for (int i = 0; i < 127; i++) {
            cs_127 += "a";
        }
        if (sdb.isCollectionSpaceExist(cs_127))
            sdb.dropCollectionSpace(cs_127);

        cs = sdb.createCollectionSpace(cs_127);
        assertNotNull(cs);

        sdb.dropCollectionSpace(cs_127);
    }
}

