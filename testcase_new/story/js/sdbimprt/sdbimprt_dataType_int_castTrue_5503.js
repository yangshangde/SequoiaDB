/************************************************************************
*@Description:    seqDB-5503:自定义允许精度丢失/数值溢出（如cast取值为true）
*@Author:           2016-7-14  huangxiaoni
************************************************************************/

main( test );

function test ()
{

   var csName = COMMCSNAME;
   var clName = COMMCLNAME + "_5503";
   var cl = readyCL( csName, clName );

   var imprtFile = tmpFileDir + "5503.csv";
   readyData( imprtFile );
   importData( csName, clName, imprtFile );

   checkCLData( cl );
   cleanCL( csName, clName );

}

function readyData ( imprtFile )
{
   var file = fileInit( imprtFile );
   file.write( "1,-2147483649\n"
      + "2,2147483648\n"
      + "3,-9223372036854775809\n"
      + "4, 9223372036854775808\n"
      + "5,-1.8E+308\n"
      + "6, 1.8E+308\n" );
   var fileInfo = cmd.run( "cat " + imprtFile );
   file.close();
}

function importData ( csName, clName, imprtFile )
{

   var imprtOption = installDir + 'bin/sdbimprt -s ' + COORDHOSTNAME + ' -p ' + COORDSVCNAME
      + ' -c ' + csName + ' -l ' + clName
      + ' --type csv --fields "a int,v1 int" --cast true'
      + ' --file ' + imprtFile;
   var rc = cmd.run( imprtOption );
   var rcObj = rc.split( "\n" );

   var expParseRecords = "Parsed records: 6";
   var expParseFailure = "Parsed failure: 0";
   var expImportedRecords = "Imported records: 6";
   var actParseRecords = rcObj[0];
   var actParseFailure = rcObj[1];
   var actImportedRecords = rcObj[4];
   if( expParseRecords !== actParseRecords || expParseFailure !== actParseFailure
      || expImportedRecords !== actImportedRecords )
   {
      throw new Error( "importData fail,[sdbimprt results]" +
         "[" + expParseRecords + ", " + expParseFailure + ", " + expImportedRecords + "]" +
         "[" + actParseRecords + ", " + actParseFailure + ", " + actImportedRecords + "]" );
   }
}

function checkCLData ( cl )
{

   var rc = cl.find( { v1: { $type: 1, $et: 16 } }, { _id: { $include: 0 } } ).sort( { a: 1 } );
   var recsArray = [];
   while( tmpRecs = rc.next() )
   {
      recsArray.push( tmpRecs.toObj() );
   }

   var expCnt = 6;
   var expRecs = '[{"a":1,"v1":2147483647},{"a":2,"v1":-2147483648},{"a":3,"v1":0},{"a":4,"v1":0},{"a":5,"v1":0},{"a":6,"v1":0}]';
   var actCnt = recsArray.length;
   var actRecs = JSON.stringify( recsArray );
   if( actCnt !== expCnt || actRecs !== expRecs )
   {
      throw new Error( "checkCLdata fail,[find]" +
         "[cnt:" + expCnt + ", recs:" + expRecs + "]" +
         "[cnt:" + actCnt + ", recs:" + actRecs + "]" );
   }

}