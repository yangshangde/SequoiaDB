/************************************************************************
*@Description:   seqDB-6207:file指定多个文件批量导入
*@Author:           2016-7-14  huangxiaoni
************************************************************************/

main( test );

function test ()
{

   var csName = COMMCSNAME;
   var clName = COMMCLNAME + "_6207";
   var cl = readyCL( csName, clName );

   var imprtFile1 = tmpFileDir + "6207_1.csv";
   var imprtFile2 = tmpFileDir + "6207_2.csv";
   readyData( imprtFile1, imprtFile2 );
   importData( csName, clName, imprtFile1, imprtFile2 );

   checkCLData( cl );
   cleanCL( csName, clName );

}

function readyData ( imprtFile1, imprtFile2 )
{

   var file = fileInit( imprtFile1 );
   file.write( "1\n2" );
   var fileInfo = cmd.run( "cat " + imprtFile1 );
   file.close();

   var file = fileInit( imprtFile2 );
   file.write( "3\n4" );
   var fileInfo = cmd.run( "cat " + imprtFile2 );
   file.close();
}

function importData ( csName, clName, imprtFile1, imprtFile2 )
{

   var imprtOption = installDir + 'bin/sdbimprt -s ' + COORDHOSTNAME + ' -p ' + COORDSVCNAME
      + ' -c ' + csName + ' -l ' + clName
      + ' --type csv --fields a'
      + ' --file ' + imprtFile1 + "," + imprtFile2;
   var rc = cmd.run( imprtOption );

   var rcObj = rc.split( "\n" );
   var expParseRecords = "Parsed records: 4";
   var expImportedRecords = "Imported records: 4";
   var actParseRecords = rcObj[0];
   var actImportedRecords = rcObj[4];
   if( expParseRecords !== actParseRecords
      || expImportedRecords !== actImportedRecords )
   {
      throw new Error( "importData fail,[sdbimprt results]" +
         "[" + expParseRecords + ", " + expImportedRecords + "]" +
         "[" + actParseRecords + ", " + actImportedRecords + "]" );
   }
}

function checkCLData ( cl )
{

   var rc = cl.find( {}, { _id: { $include: 0 } } ).sort( { a: 1 } );
   var recsArray = [];
   while( tmpRecs = rc.next() )
   {
      recsArray.push( tmpRecs.toObj() );
   }

   var expCnt = 4;
   var expRecs = '[{"a":1},{"a":2},{"a":3},{"a":4}]';
   var actCnt = recsArray.length;
   var actRecs = JSON.stringify( recsArray );
   if( actCnt !== expCnt || actRecs !== expRecs )
   {
      throw new Error( "checkCLdata fail,[find]" +
         "[cnt:" + expCnt + ", recs:" + expRecs + "]" +
         "[cnt:" + actCnt + ", recs:" + actRecs + "]" );
   }

}