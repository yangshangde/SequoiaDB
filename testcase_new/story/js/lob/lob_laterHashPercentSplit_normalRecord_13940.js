/******************************************************************************
*@Description : the collection do hash percent split after input data.test
*               input normal record and lob data into collection
*@Modify list :
*               2014-12-18  xiaojun Hu  Init
******************************************************************************/
testConf.skipStandAlone = true;
main( test );

function test ()
{
   var testFile = CHANGEDPREFIX + "lobTest.file";
   var getTestFile = CHANGEDPREFIX + "lobTestGet.file";
   var putNum = 50;
   var partitionNum = 2048;

   var names = lobGetAllGroupNames( db );
   if( 1 == names.length )
   {
      return;
   }

   lobGenerateFile( testFile ); // auto file
   var originMd5 = getMd5ForFile( testFile );
   // create collection
   var optionObj = {
      "ShardingKey": { "no": 1 }, "ShardingType": "hash", "ReplSize": 0,
      "Partition": partitionNum, "Compressed": true
   };
   var clName = COMMCLNAME + "_13940";
   commDropCL( db, COMMCSNAME, clName, true, true );
   var cl = commCreateCL( db, COMMCSNAME, clName, optionObj, true, true );
   // put normal data and lob data
   try
   {
      lobInsertDoc( cl, putNum );
      var oids = lobPutLob( cl, testFile, putNum );
      var FULLCLNAME = COMMCSNAME + "." + clName;
      var clRg = commGetCLGroups( db, FULLCLNAME );
      var cond = Math.floor( 100 / names.length );
      for( var i = 0; i < names.length; ++i )
      {
         if( clRg[0] != names[i] )
         {
            var firstCond = cond;
            lobSplit( cl, clRg[0], names[i], firstCond, "percent" );
            firstCond += cond;
         }
      }
      for( var i = 0; i < cl.count(); ++i )
      {
         var count = cl.find( { "no": i } ).count();
         assert.equal( 1, count );
      }
      for( var i = 0; i < oids.length; ++i )// will split error
      {
         cl.getLob( oids[i], getTestFile, true );
         var curMd5 = getMd5ForFile( getTestFile );
         assert.equal( originMd5, curMd5 );
      }
      commDropCL( db, COMMCSNAME, clName, true, true );
   }
   catch( e )
   {
      throw e;
   }
   finally
   {
      var cmd = new Cmd();
      cmd.run( "rm -rf " + testFile );
      if( lobFileIsExist( getTestFile ) )
      {
         cmd.run( "rm -rf " + getTestFile );
      }
   }
}
