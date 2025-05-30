/****************************************************
@description:      split, range, base case
@testlink cases:   seqDB-7676
@input:        1 createCL 
               2 insert records
               3 syncSplit, range, cover required parameter[$sourceGroup, $targetGroup, $condition]
               4 connect sourceGroup, and check results
               5 connect targetGroup, and check results 
@output:     success
@modify list:
        2016-4-29 XiaoNi Huang init
****************************************************/
<?php

include_once dirname(__FILE__).'/../func.php';

class SyncSplitOper03 extends BaseOperator 
{  
   public function __construct()
   {
      parent::__construct();
   }
   
   function getErrno()
   {
      $this -> err = $this -> db -> getError();
      return $this -> err['errno'];
   }
   
   function getGroup ( $groupNames, $type )
   {
      if( $type === 'sourceGroup' )
      {
         $group = $groupNames[0];
      }else if( $type === 'targetGroup' )
      {
         $group = $groupNames[1];
      }
      return $this -> db -> getGroup( $group );
   }
   
   function getMaster ( $rgDB )
   {
      return $rgDB -> getMaster();
   }
   
   function connect( $nodeObj ) 
   {
      return $nodeObj -> connect();
   }
   
   function createCL( $csName, $clName, $groupNames )
   {
      $options = array( 'ShardingKey' => array( 'a' => 1 ), 'ShardingType' => 'range', 
                        'ReplSize' => 0, 'Group' => $groupNames[0] );
      return $this -> commCreateCL( $csName, $clName, $options, true );
   }
   
   function insert( $clDB )
   {
      for( $i = 0; $i < 100; $i++ )
      {
         $clDB -> insert( array( 'a' => $i ) );
      }
   }
   
   function splitAsync( $clDB, $groupNames )
   {
      $condition = array( 'a' => 50 );
      $rc = $clDB -> splitAsync( $groupNames[0], $groupNames[1], $condition );
      echo "   sourceGroup = ".$groupNames[0].", targetGroup = ".$groupNames[1];
      
      echo "\n   waiting for splitAsync task to finish.\n";
      $taskID = $rc['taskID'];
      $this -> db -> waitTask( $taskID );
   }
   
   function getCS( $nodeDB, $csName )
   {
      return $nodeDB -> getCS( $csName );
   } 
   
   function getCL( $csDB, $clName )
   {
      return $csDB -> getCL( $clName ); 
   } 
   
   function find( $rgClDB, $rgType )
   {
      if( $rgType === 'sourceGroup' )
      {
         $condition = '{a:{$lt:50}}';
      }
      else if( $rgType === 'targetGroup' )
      {
         $condition = '{a:{$gte:50}}';
      }
      
      $cursor = $rgClDB -> find( $condition );
      $recsArray = array();
      while( $recs = $cursor -> next() )
      {
         array_push( $recsArray, $recs );
      }
      
      return $recsArray;
   }
   
   function dropCL( $csName, $clName,$ignoreNotExist )
   {
      $this -> commDropCL( $csName, $clName, $ignoreNotExist );
   }
   
   function dropCS( $csName, $ignoreNotExist )
   {
      $this -> commDropCS( $csName, $ignoreNotExist );
   }
   
}

class TestSyncSplit03 extends PHPUnit_Framework_TestCase
{
   protected static $dbh;
   private static $groupNames;
   private static $csName;
   private static $clName;
   private static $clDB;
   
   public static function setUpBeforeClass()
   {
      self::$dbh = new SyncSplitOper03();
      
      if( self::$dbh -> commIsStandlone() === false )
      {
         self::$groupNames = self::$dbh -> commGetGroupNames();
         
         echo "\n---Begin to ready parameter.\n";
         self::$csName = self::$dbh -> COMMCSNAME."7676_03_must";
         self::$clName = self::$dbh -> COMMCLNAME;
         echo "   cl = ".self::$csName.".".self::$clName."\n";
         
         echo "\n---Begin to drop cl in the begin.\n";
         self::$dbh -> dropCL( self::$csName, self::$clName, true );
         
         echo "\n---Begin to create cl.\n";
         self::$clDB = self::$dbh -> createCL( self::$csName, self::$clName, self::$groupNames );
      }
   }
   
   public function setUp()
   {
      if( self::$dbh -> commIsStandlone() === true )
      {
         $this -> markTestSkipped( "Database is standlone." );
      }
      else if( count(self::$groupNames) < 2 )
      {
         echo "\n---Begin to judge groupNumber.\n";
         $this -> markTestSkipped( "Database is standlone." );
      }
   }
   
   function test_insert()
   {
      echo "\n---Begin to insert records.\n";
      
      self::$dbh -> insert( self::$clDB );
      $errno = self::$dbh -> getErrno();
      $this -> assertEquals( 0, $errno );
   }
   
   function test_splitAsync()
   {
      echo "\n---Begin to splitAsync[range, percent].\n";
      
      self::$dbh -> splitAsync( self::$clDB, self::$groupNames );
      $errno = self::$dbh -> getErrno();
      $this -> assertEquals( 0, $errno );
   }
   
   function test_findOfSourceRG()
   {
      echo "\n-------------------check results of sourceGroup------------------------\n";
      
      //--------------------------connect sourceGroup-------------------------
      echo "\n---Begin to getGroup[souceGroup].\n";
      $rgDB = self::$dbh -> getGroup( self::$groupNames, 'sourceGroup' );
      
      echo "\n---Begin to getMaster.\n";
      $nodeObj = self::$dbh -> getMaster( $rgDB );
      
      echo "\n---Begin to connect masterNode.\n";
      $nodeDB = self::$dbh -> connect( $nodeObj );
      $errno = self::$dbh -> getErrno();
      $this -> assertEquals( 0, $errno );
      
      echo "\n---Begin to getCS of the sourceGroup.\n";
      $csDB  = self::$dbh -> getCS( $nodeDB, self::$csName );
      $errno = self::$dbh -> getErrno();
      $this -> assertEquals( 0, $errno );
      
      echo "\n---Begin to getCL of the sourceGroup.\n";
      $rgClDB = self::$dbh -> getCL( $csDB, self::$clName );
      $errno = self::$dbh -> getErrno();
      $this -> assertEquals( 0, $errno );
      
      //--------------------------check results of sourceGroup-------------------------
      echo "\n---Begin to find of source group.\n";
      $recsArray = self::$dbh -> find( $rgClDB, 'sourceGroup' );
      $errno = self::$dbh -> getErrno();
      $this -> assertEquals( 0, $errno );
      
      $this -> assertCount( 50, $recsArray );
   }
   
   function test_findOfTargetRG()
   {
      echo "\n-------------------check results of targetGroup------------------------\n";
      
      //--------------------------connect targetGroup-------------------------
      echo "\n---Begin to getGroup[targetGroup].\n";
      $rgDB = self::$dbh -> getGroup( self::$groupNames, 'targetGroup' );
      
      echo "\n---Begin to getMaster.\n";
      $nodeObj = self::$dbh -> getMaster( $rgDB );
      
      echo "\n---Begin to connect masterNode.\n";
      $nodeDB = self::$dbh -> connect( $nodeObj );
      $errno = self::$dbh -> getErrno();
      $this -> assertEquals( 0, $errno );
      
      echo "\n---Begin to getCS of the targetGroup.\n";
      $csDB  = self::$dbh -> getCS( $nodeDB, self::$csName );
      $errno = self::$dbh -> getErrno();
      $this -> assertEquals( 0, $errno );
      
      echo "\n---Begin to getCL of the targetGroup.\n";
      $rgClDB = self::$dbh -> getCL( $csDB, self::$clName );
      $errno = self::$dbh -> getErrno();
      $this -> assertEquals( 0, $errno );
      
      //--------------------------check results of targetGroup-------------------------
      echo "\n---Begin to find of source group.\n";
      $recsArray = self::$dbh -> find( $rgClDB, 'targetGroup' );
      $errno = self::$dbh -> getErrno();
      $this -> assertEquals( 0, $errno );
      
      $this -> assertCount( 50, $recsArray );
      
      echo "\n-------------------check results end-----------------------------------\n";
   }
   
   function test_dropCL()
   {
      echo "\n---Begin to drop cl in the end.\n";
      
      self::$dbh -> dropCL( self::$csName, self::$clName, false );
      $errno = self::$dbh -> getErrno();
      $this -> assertEquals( 0, $errno );
      
      self::$dbh -> dropCS( self::$csName, false );
      $errno = self::$dbh -> getErrno();
      $this -> assertEquals( 0, $errno );
   }
}
?>