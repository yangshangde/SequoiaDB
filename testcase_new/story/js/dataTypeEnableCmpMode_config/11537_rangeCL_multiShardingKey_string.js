/************************************
*@Description: match range sharding,regex
               data type: int/numberLong/double/decimal/string/bool/date/timestamp/binary/regex/json/array/null/minKey/maxKey
*@author:      liuxiaoxuan
*@createdate:  2017.5.24
*@testlinkCase:seqDB-11537 
**************************************/
main( test );

function test ()
{
   //set find data from master
   db.setSessionAttr( { PreferedInstance: "M" } );
   var hintConf = [{ "": "$shard" }, { "": null }];
   var sortConf = { _id: 1 };

   var clName = COMMCLNAME + "_11537";
   //clean environment before test
   commDropCL( db, COMMCSNAME, clName, true, true, "drop CL in the beginning" );


   //standalone can not split
   if( true == commIsStandalone( db ) )
   {
      return;
   }
   //less two groups,can not split
   var allGroupName = getGroupName( db );
   if( 1 === allGroupName.length )
   {
      return;
   }

   var ClOption = { ShardingKey: { "a": 1, "b": 1, "c": 1 }, ShardingType: "range" };
   var dbcl = commCreateCL( db, COMMCSNAME, clName, ClOption, true, true );

   var hintConf = [{ "": null }, { "": "$shard" }];
   var sortConf = { _id: 1 };

   var startCondition1 = { a: "b", b: "b", c: "b" };
   ClSplitOneTimes( COMMCSNAME, clName, startCondition1, null );

   //insert data 
   var doc = [{ a: { $minKey: 1 }, b: { $minKey: 1 }, c: { $minKey: 1 } },
   { d: 1 },
   { a: null, b: null, c: null },
   { a: -22, b: -22, c: -22 },
   { a: "aa", b: "aa", c: "aa" },
   { a: "Aa", b: "Aa", c: "Aa" },
   { a: "bb", b: "bb", c: "bb" },
   { a: "Bb", b: "Bb", c: "Bb" },
   { a: "cc", b: "cc", c: "cc" },
   { a: "Cc", b: "Cc", c: "Cc" },
   { a: { $binary: "aGVsbG8gd29ybGQ=", $type: "1" }, b: { $binary: "aGVsbG8gd29ybGQ=", $type: "1" }, c: { $binary: "aGVsbG8gd29ybGQ=", $type: "1" } },
   { a: { $oid: "591cf397a54fe50425000000" }, b: { $oid: "591cf397a54fe50425000000" }, c: { $oid: "591cf397a54fe50425000000" } },
   { a: true, b: true, c: true },
   { a: { $date: "2014-01-01" }, b: { $date: "2014-01-01" }, c: { $date: "2014-01-01" } },
   { a: { $timestamp: "2013-06-05-16.10.33.000000" }, b: { $timestamp: "2013-06-05-16.10.33.000000" }, c: { $timestamp: "2013-06-05-16.10.33.000000" } },
   { a: { $regex: "^a", $options: "i" }, b: { $regex: "^a", $options: "i" }, c: { $regex: "^a", $options: "i" } },
   { a: { $maxKey: 1 }, b: { $maxKey: 1 }, c: { $maxKey: 1 } }];
   dbcl.insert( doc );

   //gte
   var findCondition2 = { $and: [{ a: { $gte: "a" } }, { b: { $gte: "a" } }, { c: { $gte: "a" } }] };
   var expRecs2 = [{ a: "aa", b: "aa", c: "aa" },
   { a: "bb", b: "bb", c: "bb" },
   { a: "cc", b: "cc", c: "cc" },
   { a: { $binary: "aGVsbG8gd29ybGQ=", $type: "1" }, b: { $binary: "aGVsbG8gd29ybGQ=", $type: "1" }, c: { $binary: "aGVsbG8gd29ybGQ=", $type: "1" } },
   { a: { $oid: "591cf397a54fe50425000000" }, b: { $oid: "591cf397a54fe50425000000" }, c: { $oid: "591cf397a54fe50425000000" } },
   { a: true, b: true, c: true },
   { a: { $date: "2014-01-01" }, b: { $date: "2014-01-01" }, c: { $date: "2014-01-01" } },
   { a: { $timestamp: "2013-06-05-16.10.33.000000" }, b: { $timestamp: "2013-06-05-16.10.33.000000" }, c: { $timestamp: "2013-06-05-16.10.33.000000" } },
   { a: { $regex: "^a", $options: "i" }, b: { $regex: "^a", $options: "i" }, c: { $regex: "^a", $options: "i" } },
   { a: { $maxKey: 1 }, b: { $maxKey: 1 }, c: { $maxKey: 1 } }];
   checkResult( dbcl, findCondition2, hintConf, sortConf, expRecs2 );

   //gt
   var findCondition2 = { $and: [{ a: { $gt: "a" } }, { b: { $gt: "a" } }, { c: { $gt: "a" } }] };
   var expRecs2 = [{ a: "aa", b: "aa", c: "aa" },
   { a: "bb", b: "bb", c: "bb" },
   { a: "cc", b: "cc", c: "cc" },
   { a: { $binary: "aGVsbG8gd29ybGQ=", $type: "1" }, b: { $binary: "aGVsbG8gd29ybGQ=", $type: "1" }, c: { $binary: "aGVsbG8gd29ybGQ=", $type: "1" } },
   { a: { $oid: "591cf397a54fe50425000000" }, b: { $oid: "591cf397a54fe50425000000" }, c: { $oid: "591cf397a54fe50425000000" } },
   { a: true, b: true, c: true },
   { a: { $date: "2014-01-01" }, b: { $date: "2014-01-01" }, c: { $date: "2014-01-01" } },
   { a: { $timestamp: "2013-06-05-16.10.33.000000" }, b: { $timestamp: "2013-06-05-16.10.33.000000" }, c: { $timestamp: "2013-06-05-16.10.33.000000" } },
   { a: { $regex: "^a", $options: "i" }, b: { $regex: "^a", $options: "i" }, c: { $regex: "^a", $options: "i" } },
   { a: { $maxKey: 1 }, b: { $maxKey: 1 }, c: { $maxKey: 1 } }];
   checkResult( dbcl, findCondition2, hintConf, sortConf, expRecs2 );

   //lte
   var findCondition2 = { $and: [{ a: { $lte: "c" } }, { b: { $lte: "c" } }, { c: { $lte: "c" } }] };
   var expRecs2 = [{ a: { $minKey: 1 }, b: { $minKey: 1 }, c: { $minKey: 1 } },
   { a: null, b: null, c: null },
   { a: -22, b: -22, c: -22 },
   { a: "aa", b: "aa", c: "aa" },
   { a: "Aa", b: "Aa", c: "Aa" },
   { a: "bb", b: "bb", c: "bb" },
   { a: "Bb", b: "Bb", c: "Bb" },
   { a: "Cc", b: "Cc", c: "Cc" }];
   checkResult( dbcl, findCondition2, hintConf, sortConf, expRecs2 );

   //lt
   var findCondition2 = { $and: [{ a: { $lt: "c" } }, { b: { $lt: "c" } }, { c: { $lt: "c" } }] };
   var expRecs2 = [{ a: { $minKey: 1 }, b: { $minKey: 1 }, c: { $minKey: 1 } },
   { a: null, b: null, c: null },
   { a: -22, b: -22, c: -22 },
   { a: "aa", b: "aa", c: "aa" },
   { a: "Aa", b: "Aa", c: "Aa" },
   { a: "bb", b: "bb", c: "bb" },
   { a: "Bb", b: "Bb", c: "Bb" },
   { a: "Cc", b: "Cc", c: "Cc" }];
   checkResult( dbcl, findCondition2, hintConf, sortConf, expRecs2 );

   //regex
   var findCondition2 = { $and: [{ a: { $regex: "^B", $options: "i" } }, { b: { $regex: "^B", $options: "i" } }, { c: { $regex: "^B", $options: "i" } }] };
   var expRecs2 = [{ a: "bb", b: "bb", c: "bb" },
   { a: "Bb", b: "Bb", c: "Bb" }];
   checkResult( dbcl, findCondition2, hintConf, sortConf, expRecs2 );

   var findCondition2 = { $and: [{ a: { $regex: "^B" } }, { b: { $regex: "^B" } }, { c: { $regex: "^B" } }] };
   var expRecs2 = [{ a: "Bb", b: "Bb", c: "Bb" }];
   checkResult( dbcl, findCondition2, hintConf, sortConf, expRecs2 );

   var findCondition2 = { $and: [{ a: { $options: "i", $regex: "^B", } }, { b: { $options: "i", $regex: "^B", } }, { c: { $options: "i", $regex: "^B", } }] };
   var expRecs2 = [{ a: "bb", b: "bb", c: "bb" },
   { a: "Bb", b: "Bb", c: "Bb" }];
   checkResult( dbcl, findCondition2, hintConf, sortConf, expRecs2 );
   commDropCL( db, COMMCSNAME, clName, true, true, "drop CL in the end" );
}

