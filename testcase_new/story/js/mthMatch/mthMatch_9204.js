/************************************
*@Description: find condition include function,arrfunction and matches; 
*@author:      zhaoyu
*@createdate:  2016.11.7
*@testlinkCase: 
**************************************/
main( test );
function test ()
{
   //clean environment before test
   commDropCL( db, COMMCSNAME, COMMCLNAME, true, true, "drop CL in the beginning" );

   //create cl
   var dbcl = commCreateCL( db, COMMCSNAME, COMMCLNAME );

   //insert data 
   var doc = [{ No: 1, a: [1, 2, 3] },
   { No: 2, a: { 0: 1, 1: 2, 2: 3 } }];
   dbcl.insert( doc );

   //check result
   var findCondition1 = { a: { $size: 1, $returnMatch: 0, $in: [4, 3] } };
   var expRecs1 = [{ No: 1, a: [1, 2, 3] },
   { No: 2, a: { 0: 1, 1: 2, 2: 3 } }];
   checkResult( dbcl, findCondition1, null, expRecs1, { No: 1 } );

   var findCondition2 = { a: { $returnMatch: 0, $size: 1, $expand: 1, $in: [4, 3] } };
   var expRecs2 = [{ No: 1, a: 1 },
   { No: 1, a: 2 },
   { No: 1, a: 3 },
   { No: 2, a: { 0: 1, 1: 2, 2: 3 } }];
   checkResult( dbcl, findCondition2, null, expRecs2, { No: 1 } );
}
