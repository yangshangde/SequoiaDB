/****************************************************
@description:      random write lob
@testlink cases:   seqDB-13458
@modify list:
        2018-3-13 huangxiaoni init
****************************************************/
<?php

include_once dirname(__FILE__).'/../commlib/LobUtils.php';
include_once dirname(__FILE__).'/../global.php';
class LobTest13458 extends PHPUnit_Framework_TestCase
{
   private static $LobUtils;
   private static $skipTestCase = false;
   
   private static $db;
   private static $cl;
   private static $csName = "php13458";
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
   
   public function test_writeLob()
   {
      echo "\n---Begin to write lob[ lock, seek ].\n"; 
      
      echo "   Begin to open lob.\n"; 
      $lobObj = self::$cl -> openLob( self::$oid, SDB_LOB_WRITE );
      $this -> assertEquals( 0, self::$db -> getError()['errno'] );
      
      echo "   Begin to lockAndSeek lob[ lock[ 1, 50], seek1Offset = 1 ].\n"; 
      $offset1 = 1;
      $len1 = 50;
      $err = $lobObj -> lockAndSeek( $offset1, $len1 );
      $this -> assertEquals( 0, $err['errno'] );
      
      echo "   Begin to lock lob[ 10, -1 ].\n"; 
      $offset2 = 10;
      $len2 = -1;
      $err = $lobObj -> lock( $offset2, $len2 );
      $this -> assertEquals( 0, $err['errno'] );
      
      echo "   Begin to seek lob[ seek2Offset = 1, mode = SDB_LOB_SET ].\n"; 
      $whence = SDB_LOB_SET;
      $err = $lobObj -> seek( $offset1, $whence );
      $this -> assertEquals( 0, $err['errno'] );
      
      echo "   Begin to write lob again.\n"; 
      //var_dump(self::$writeStr);
      $writeLen2 = self::$writeLen;
      $writeStr2 = self::$LobUtils -> getRandomStr( $writeLen2 );
      //var_dump($writeStr2);
      $err = $lobObj -> write( $writeStr2 );
      $this -> assertEquals( 0, $err['errno'] );
      
      echo "   Begin to close lob.\n"; 
      $err = $lobObj -> close();
      $this -> assertEquals( 0, $err['errno'] );
      
      echo "   Begin to check the lob.\n";
      $readStr = substr(self::$writeStr, 0, $offset1).$writeStr2;
      //var_dump($readStr);
      self::$LobUtils -> checkLobContent( self::$oid, $offset1 + $writeLen2 + 1, $readStr );
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