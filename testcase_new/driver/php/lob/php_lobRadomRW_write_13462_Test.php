/****************************************************
@description:      random write lob
@testlink cases:   seqDB-13462
@modify list:
        2018-3-13 huangxiaoni init
****************************************************/
<?php

include_once dirname(__FILE__).'/../commlib/LobUtils.php';
include_once dirname(__FILE__).'/../global.php';
class LobTest13462 extends PHPUnit_Framework_TestCase
{
   private static $LobUtils;
   private static $skipTestCase = false;
   
   private static $db;
   private static $cl;
   private static $csName = "php13462";
   private static $clName = "cl";
   
   private static $oid;
   private static $writeLen = 100;
   private static $writeStr;
   
   public static function setUpBeforeClass()
   {        
      // connect sdb
      self::$db = new SequoiaDB();
      $err = self::$db -> connect( globalParameter::getHostName(), globalParameter::getCoordPort() );
      if ( $err['errno'] != 0 )
      {
         echo "Failed to connect database, error code: ".$err['errno'];
         self::$skipTestCase = true;
         return;
      }   
      
      // setSssionAttr['m']
      $err = self::$db -> setSessionAttr(array('PreferedInstance' => 'm' ));
      if ( $err['errno'] != 0 )
      {
         echo "Failed to call setSessionAttr, error code: ".$err['errno'];
         self::$skipTestCase = true;
         return;
      }
      
      // create cs
      $cs = self::$db -> selectCS( self::$csName );
      $err = self::$db -> getError();
      if( $err['errno'] != 0 ) 
      {
         echo "Failed to exec selectCS, error code: ".$err['errno'];
         self::$skipTestCase = true;
         return;
      }
      
      // create cl
      self::$cl = $cs -> selectCL( self::$clName );
      $err = self::$db -> getError();
      if( $err['errno'] != 0 ) 
      {
         echo "Failed to exec selesctCL, error code: ".$err['errno'];
         self::$skipTestCase = true;
         return;
      }
      
      // new LobUtils
      self::$LobUtils = new LobUtils( self::$db, self::$cl );
      self::$oid = self::$LobUtils -> getOid();
      //var_dump("oid = ".self::$oid);
      self::$writeStr = self::$LobUtils -> getRandomStr( self::$writeLen );
      self::$LobUtils -> writeLob( self::$oid, self::$writeStr );
   }
   
   public function setUp()
   {
      if ( self::$skipTestCase == true )
      {
         $this->markTestSkipped( 'init failed' );
      }
   }
   
   public function test_lockOffsetVerify()
   {
      echo "\n---Begin to test lock parameter[ offset ].\n";
      $len = -1; 
            
      echo "   Begin to open lob.\n"; 
      $lobObj = self::$cl -> openLob( self::$oid, SDB_LOB_WRITE );
      $this -> assertEquals( 0, self::$db -> getError()['errno'] );
      
      echo "   Begin to lock lob[ offset=-1 ].\n"; 
      $offset = -1;
      $err = $lobObj -> lock( $offset, $len );
      $this -> assertEquals( -6, $err['errno'] );
      /*
      echo "   Begin to lock lob[ offset='test' ].\n"; 
      $offset = "test";
      $err = $lobObj -> lock( $offset, $len );
      $this -> assertEquals( -6, $err['errno'] );*/  //same: jira:3349
   }
   
   public function test_lockLenVerify()
   {
      echo "\n---Begin to test lock parameter[ len ].\n";
      $offset = 1; 
            
      echo "   Begin to open lob.\n"; 
      $lobObj = self::$cl -> openLob( self::$oid, SDB_LOB_WRITE );
      $this -> assertEquals( 0, self::$db -> getError()['errno'] );
      
      echo "   Begin to lock lob[ len=-2 ].\n"; 
      $len = -2;
      $err = $lobObj -> lock( $offset, $len );
      $this -> assertEquals( -6, $err['errno'] );
      
      echo "   Begin to lock lob[ len=0 ].\n"; 
      $len = 0;
      $err = $lobObj -> lock( $offset, $len );
      $this -> assertEquals( -6, $err['errno'] );
      /*
      echo "   Begin to lock lob[ len='test' ].\n"; 
      $len = "test";
      $err = $lobObj -> lock( $offset, $len );
      $this -> assertEquals( -6, $err['errno'] );*/  //same: jira:3349
   }
   
   public static function tearDownAfterClass()
   {
      echo "\n---Begin to dropCS in the end.\n"; 
      $err = self::$db -> dropCS( self::$csName );
      if ( $err['errno'] != 0 )
      {
         throw new Exception("failed to drop cs, errno=".$err['errno']);
      }
      
      $err = self::$db->close();
   }
   
}
?>