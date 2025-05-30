/************************************
*@Description: index scan,use gt/gte/lt/lte to comapare,
               one field use many times,use and/or/not
*@author:      zhaoyu
*@createdate:  2016.8.5
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
   var doc = [{ No: 1, a: 500 }, { No: 11, a: 1000 }, { No: 21, a: 1500 }, { No: 31, a: 2000 }, { No: 41, a: 2500 }, { No: 51, a: 3000 }, { No: 61, a: 3500 },
   { No: 2, a: 501 }, { No: 12, a: 1001 }, { No: 22, a: 1501 }, { No: 32, a: 2001 }, { No: 42, a: 2501 }, { No: 52, a: 3001 }, { No: 62, a: 3501 },
   { No: 3, a: 502 }, { No: 13, a: 1002 }, { No: 23, a: 1502 }, { No: 33, a: 2002 }, { No: 43, a: 2502 }, { No: 53, a: 3002 }, { No: 63, a: 3502 },
   { No: 4, a: 503 }, { No: 14, a: 1003 }, { No: 24, a: 1503 }, { No: 34, a: 2003 }, { No: 44, a: 2503 }, { No: 54, a: 3003 }, { No: 64, a: 3503 },
   { No: 5, a: 504 }, { No: 15, a: 1004 }, { No: 25, a: 1504 }, { No: 35, a: 2004 }, { No: 45, a: 2504 }, { No: 55, a: 3004 }, { No: 65, a: 3504 },
   { No: 6, a: 505 }, { No: 16, a: 1005 }, { No: 26, a: 1505 }, { No: 36, a: 2005 }, { No: 46, a: 2505 }, { No: 56, a: 3005 }, { No: 66, a: 3505 },
   { No: 7, a: 506 }, { No: 17, a: 1006 }, { No: 27, a: 1506 }, { No: 37, a: 2006 }, { No: 47, a: 2506 }, { No: 57, a: 3006 }, { No: 67, a: 3506 },
   { No: 8, a: 507 }, { No: 18, a: 1007 }, { No: 28, a: 1507 }, { No: 38, a: 2007 }, { No: 48, a: 2507 }, { No: 58, a: 3007 }, { No: 68, a: 3507 },
   { No: 9, a: 508 }, { No: 19, a: 1008 }, { No: 29, a: 1508 }, { No: 39, a: 2008 }, { No: 49, a: 2508 }, { No: 59, a: 3008 }, { No: 69, a: 3508 },
   { No: 10, a: 509 }, { No: 20, a: 1009 }, { No: 30, a: 1509 }, { No: 40, a: 2009 }, { No: 50, a: 2509 }, { No: 60, a: 3009 }, { No: 70, a: 3509 },
   { No: 71, a: { $oid: "123abcd00ef12358902300ef" } },
   { No: 72, a: { $date: "2000-01-01" } },
   { No: 73, a: { $timestamp: "2000-01-01-15.32.18.000000" } },
   { No: 74, a: { $binary: "aGVsbG8gd29ybGQ=", "$type": "1" } },
   { No: 75, a: { $regex: "^z", "$options": "i" } },
   { No: 76, a: null },
   { No: 77, a: "abc" },
   { No: 78, a: MinKey() },
   { No: 79, a: MaxKey() },
   { No: 80, a: true },
   { No: 81, a: false },
   { No: 82, a: { name: "zhang" } },
   { No: 83, a: [1, 2, 3] }];
   dbcl.insert( doc );

   //and
   //gt
   var findCondition1 = { $and: [{ a: { $gt: 1000 } }, { a: { $gt: 2000 } }, { a: { $gt: 3000 } }] };
   var expRecs1 = [{ No: 52, a: 3001 }, { No: 53, a: 3002 }, { No: 54, a: 3003 }, { No: 55, a: 3004 }, { No: 56, a: 3005 },
   { No: 57, a: 3006 }, { No: 58, a: 3007 }, { No: 59, a: 3008 }, { No: 60, a: 3009 }, { No: 61, a: 3500 },
   { No: 62, a: 3501 }, { No: 63, a: 3502 }, { No: 64, a: 3503 }, { No: 65, a: 3504 }, { No: 66, a: 3505 },
   { No: 67, a: 3506 }, { No: 68, a: 3507 }, { No: 69, a: 3508 }, { No: 70, a: 3509 }];
   checkResult( dbcl, findCondition1, null, expRecs1, { No: 1 } );

   var findCondition2 = { $and: [{ a: { $gt: 1000 } }, { a: { $gt: 3000 } }, { a: { $gt: 2000 } }] };
   checkResult( dbcl, findCondition2, null, expRecs1, { No: 1 } );

   var findCondition3 = { $and: [{ a: { $gt: 2000 } }, { a: { $gt: 3000 } }, { a: { $gt: 1000 } }] };
   checkResult( dbcl, findCondition3, null, expRecs1, { No: 1 } );

   var findCondition4 = { $and: [{ a: { $gt: 2000 } }, { a: { $gt: 1000 } }, { a: { $gt: 3000 } }] };
   checkResult( dbcl, findCondition4, null, expRecs1, { No: 1 } );

   var findCondition5 = { $and: [{ a: { $gt: 3000 } }, { a: { $gt: 2000 } }, { a: { $gt: 1000 } }] };
   checkResult( dbcl, findCondition5, null, expRecs1, { No: 1 } );

   var findCondition6 = { $and: [{ a: { $gt: 3000 } }, { a: { $gt: 1000 } }, { a: { $gt: 2000 } }] };
   checkResult( dbcl, findCondition6, null, expRecs1, { No: 1 } );

   //gte and
   var findCondition7 = { $and: [{ a: { $gte: 3000 } }, { a: { $gte: 2000 } }, { a: { $gte: 1000 } }] };
   var expRecs2 = [{ No: 51, a: 3000 }, { No: 52, a: 3001 }, { No: 53, a: 3002 }, { No: 54, a: 3003 }, { No: 55, a: 3004 },
   { No: 56, a: 3005 }, { No: 57, a: 3006 }, { No: 58, a: 3007 }, { No: 59, a: 3008 }, { No: 60, a: 3009 },
   { No: 61, a: 3500 }, { No: 62, a: 3501 }, { No: 63, a: 3502 }, { No: 64, a: 3503 }, { No: 65, a: 3504 },
   { No: 66, a: 3505 }, { No: 67, a: 3506 }, { No: 68, a: 3507 }, { No: 69, a: 3508 }, { No: 70, a: 3509 }];
   checkResult( dbcl, findCondition7, null, expRecs2, { No: 1 } );

   var findCondition8 = { $and: [{ a: { $gte: 1000 } }, { a: { $gte: 2000 } }, { a: { $gte: 3000 } }] };
   checkResult( dbcl, findCondition8, null, expRecs2, { No: 1 } );

   var findCondition9 = { $and: [{ a: { $gte: 1000 } }, { a: { $gte: 3000 } }, { a: { $gte: 2000 } }] };
   checkResult( dbcl, findCondition9, null, expRecs2, { No: 1 } );

   var findCondition10 = { $and: [{ a: { $gte: 2000 } }, { a: { $gte: 1000 } }, { a: { $gte: 3000 } }] };
   checkResult( dbcl, findCondition10, null, expRecs2, { No: 1 } );

   var findCondition11 = { $and: [{ a: { $gte: 2000 } }, { a: { $gte: 3000 } }, { a: { $gte: 1000 } }] };
   checkResult( dbcl, findCondition11, null, expRecs2, { No: 1 } );

   var findCondition12 = { $and: [{ a: { $gte: 3000 } }, { a: { $gte: 1000 } }, { a: { $gte: 2000 } }] };
   checkResult( dbcl, findCondition12, null, expRecs2, { No: 1 } );

   //lt and
   var findCondition13 = { $and: [{ a: { $lt: 1000 } }, { a: { $lt: 2000 } }, { a: { $lt: 3000 } }] };
   var expRecs3 = [{ No: 1, a: 500 }, { No: 2, a: 501 }, { No: 3, a: 502 }, { No: 4, a: 503 },
   { No: 5, a: 504 }, { No: 6, a: 505 }, { No: 7, a: 506 }, { No: 8, a: 507 },
   { No: 9, a: 508 }, { No: 10, a: 509 }, { No: 83, a: [1, 2, 3] }];
   checkResult( dbcl, findCondition13, null, expRecs3, { No: 1 } );

   var findCondition14 = { $and: [{ a: { $lt: 1000 } }, { a: { $lt: 3000 } }, { a: { $lt: 2000 } }] };
   checkResult( dbcl, findCondition14, null, expRecs3, { No: 1 } );

   var findCondition15 = { $and: [{ a: { $lt: 2000 } }, { a: { $lt: 3000 } }, { a: { $lt: 1000 } }] };
   checkResult( dbcl, findCondition15, null, expRecs3, { No: 1 } );

   var findCondition16 = { $and: [{ a: { $lt: 2000 } }, { a: { $lt: 1000 } }, { a: { $lt: 3000 } }] };
   checkResult( dbcl, findCondition16, null, expRecs3, { No: 1 } );

   var findCondition17 = { $and: [{ a: { $lt: 3000 } }, { a: { $lt: 2000 } }, { a: { $lt: 1000 } }] };
   checkResult( dbcl, findCondition17, null, expRecs3, { No: 1 } );

   var findCondition18 = { $and: [{ a: { $lt: 3000 } }, { a: { $lt: 1000 } }, { a: { $lt: 2000 } }] };
   checkResult( dbcl, findCondition18, null, expRecs3, { No: 1 } );

   //lte and
   var findCondition19 = { $and: [{ a: { $lte: 1000 } }, { a: { $lte: 2000 } }, { a: { $lte: 3000 } }] };
   var expRecs4 = [{ No: 1, a: 500 }, { No: 2, a: 501 }, { No: 3, a: 502 }, { No: 4, a: 503 },
   { No: 5, a: 504 }, { No: 6, a: 505 }, { No: 7, a: 506 }, { No: 8, a: 507 },
   { No: 9, a: 508 }, { No: 10, a: 509 }, { No: 11, a: 1000 }, { No: 83, a: [1, 2, 3] }];
   checkResult( dbcl, findCondition19, null, expRecs4, { No: 1 } );

   var findCondition20 = { $and: [{ a: { $lte: 1000 } }, { a: { $lte: 3000 } }, { a: { $lte: 2000 } }] };
   checkResult( dbcl, findCondition20, null, expRecs4, { No: 1 } );

   var findCondition21 = { $and: [{ a: { $lte: 2000 } }, { a: { $lte: 1000 } }, { a: { $lte: 3000 } }] };
   checkResult( dbcl, findCondition21, null, expRecs4, { No: 1 } );

   var findCondition22 = { $and: [{ a: { $lte: 2000 } }, { a: { $lte: 3000 } }, { a: { $lte: 1000 } }] };
   checkResult( dbcl, findCondition22, null, expRecs4, { No: 1 } );

   var findCondition23 = { $and: [{ a: { $lte: 3000 } }, { a: { $lte: 2000 } }, { a: { $lte: 1000 } }] };
   checkResult( dbcl, findCondition23, null, expRecs4, { No: 1 } );

   var findCondition24 = { $and: [{ a: { $lte: 3000 } }, { a: { $lte: 1000 } }, { a: { $lte: 2000 } }] };
   checkResult( dbcl, findCondition24, null, expRecs4, { No: 1 } );

   //or
   //gt
   var findCondition25 = { $or: [{ a: { $gt: 1000 } }, { a: { $gt: 2000 } }, { a: { $gt: 3000 } }] };
   var expRecs5 = [{ No: 12, a: 1001 }, { No: 13, a: 1002 }, { No: 14, a: 1003 }, { No: 15, a: 1004 }, { No: 16, a: 1005 },
   { No: 17, a: 1006 }, { No: 18, a: 1007 }, { No: 19, a: 1008 }, { No: 20, a: 1009 }, { No: 21, a: 1500 },
   { No: 22, a: 1501 }, { No: 23, a: 1502 }, { No: 24, a: 1503 }, { No: 25, a: 1504 }, { No: 26, a: 1505 },
   { No: 27, a: 1506 }, { No: 28, a: 1507 }, { No: 29, a: 1508 }, { No: 30, a: 1509 }, { No: 31, a: 2000 },
   { No: 32, a: 2001 }, { No: 33, a: 2002 }, { No: 34, a: 2003 }, { No: 35, a: 2004 }, { No: 36, a: 2005 },
   { No: 37, a: 2006 }, { No: 38, a: 2007 }, { No: 39, a: 2008 }, { No: 40, a: 2009 }, { No: 41, a: 2500 },
   { No: 42, a: 2501 }, { No: 43, a: 2502 }, { No: 44, a: 2503 }, { No: 45, a: 2504 }, { No: 46, a: 2505 },
   { No: 47, a: 2506 }, { No: 48, a: 2507 }, { No: 49, a: 2508 }, { No: 50, a: 2509 }, { No: 51, a: 3000 },
   { No: 52, a: 3001 }, { No: 53, a: 3002 }, { No: 54, a: 3003 }, { No: 55, a: 3004 }, { No: 56, a: 3005 },
   { No: 57, a: 3006 }, { No: 58, a: 3007 }, { No: 59, a: 3008 }, { No: 60, a: 3009 }, { No: 61, a: 3500 },
   { No: 62, a: 3501 }, { No: 63, a: 3502 }, { No: 64, a: 3503 }, { No: 65, a: 3504 }, { No: 66, a: 3505 },
   { No: 67, a: 3506 }, { No: 68, a: 3507 }, { No: 69, a: 3508 }, { No: 70, a: 3509 }];
   checkResult( dbcl, findCondition25, null, expRecs5, { No: 1 } );

   var findCondition26 = { $or: [{ a: { $gt: 1000 } }, { a: { $gt: 3000 } }, { a: { $gt: 2000 } }] };
   checkResult( dbcl, findCondition26, null, expRecs5, { No: 1 } );

   var findCondition27 = { $or: [{ a: { $gt: 2000 } }, { a: { $gt: 3000 } }, { a: { $gt: 1000 } }] };
   checkResult( dbcl, findCondition27, null, expRecs5, { No: 1 } );

   var findCondition28 = { $or: [{ a: { $gt: 2000 } }, { a: { $gt: 1000 } }, { a: { $gt: 3000 } }] };
   checkResult( dbcl, findCondition28, null, expRecs5, { No: 1 } );

   var findCondition29 = { $or: [{ a: { $gt: 3000 } }, { a: { $gt: 2000 } }, { a: { $gt: 1000 } }] };
   checkResult( dbcl, findCondition29, null, expRecs5, { No: 1 } );

   var findCondition30 = { $or: [{ a: { $gt: 3000 } }, { a: { $gt: 1000 } }, { a: { $gt: 2000 } }] };
   checkResult( dbcl, findCondition30, null, expRecs5, { No: 1 } );

   //gte or
   var findCondition31 = { $or: [{ a: { $gte: 3000 } }, { a: { $gte: 2000 } }, { a: { $gte: 1000 } }] };
   var expRecs6 = [{ No: 11, a: 1000 }, { No: 12, a: 1001 }, { No: 13, a: 1002 }, { No: 14, a: 1003 }, { No: 15, a: 1004 },
   { No: 16, a: 1005 }, { No: 17, a: 1006 }, { No: 18, a: 1007 }, { No: 19, a: 1008 }, { No: 20, a: 1009 },
   { No: 21, a: 1500 }, { No: 22, a: 1501 }, { No: 23, a: 1502 }, { No: 24, a: 1503 }, { No: 25, a: 1504 },
   { No: 26, a: 1505 }, { No: 27, a: 1506 }, { No: 28, a: 1507 }, { No: 29, a: 1508 }, { No: 30, a: 1509 },
   { No: 31, a: 2000 }, { No: 32, a: 2001 }, { No: 33, a: 2002 }, { No: 34, a: 2003 }, { No: 35, a: 2004 },
   { No: 36, a: 2005 }, { No: 37, a: 2006 }, { No: 38, a: 2007 }, { No: 39, a: 2008 }, { No: 40, a: 2009 },
   { No: 41, a: 2500 }, { No: 42, a: 2501 }, { No: 43, a: 2502 }, { No: 44, a: 2503 }, { No: 45, a: 2504 },
   { No: 46, a: 2505 }, { No: 47, a: 2506 }, { No: 48, a: 2507 }, { No: 49, a: 2508 }, { No: 50, a: 2509 },
   { No: 51, a: 3000 }, { No: 52, a: 3001 }, { No: 53, a: 3002 }, { No: 54, a: 3003 }, { No: 55, a: 3004 },
   { No: 56, a: 3005 }, { No: 57, a: 3006 }, { No: 58, a: 3007 }, { No: 59, a: 3008 }, { No: 60, a: 3009 },
   { No: 61, a: 3500 }, { No: 62, a: 3501 }, { No: 63, a: 3502 }, { No: 64, a: 3503 }, { No: 65, a: 3504 },
   { No: 66, a: 3505 }, { No: 67, a: 3506 }, { No: 68, a: 3507 }, { No: 69, a: 3508 }, { No: 70, a: 3509 }];
   checkResult( dbcl, findCondition31, null, expRecs6, { No: 1 } );

   var findCondition32 = { $or: [{ a: { $gte: 1000 } }, { a: { $gte: 2000 } }, { a: { $gte: 3000 } }] };
   checkResult( dbcl, findCondition32, null, expRecs6, { No: 1 } );

   var findCondition33 = { $or: [{ a: { $gte: 1000 } }, { a: { $gte: 3000 } }, { a: { $gte: 2000 } }] };
   checkResult( dbcl, findCondition33, null, expRecs6, { No: 1 } );

   var findCondition34 = { $or: [{ a: { $gte: 2000 } }, { a: { $gte: 1000 } }, { a: { $gte: 3000 } }] };
   checkResult( dbcl, findCondition34, null, expRecs6, { No: 1 } );

   var findCondition35 = { $or: [{ a: { $gte: 2000 } }, { a: { $gte: 3000 } }, { a: { $gte: 1000 } }] };
   checkResult( dbcl, findCondition35, null, expRecs6, { No: 1 } );

   var findCondition36 = { $or: [{ a: { $gte: 3000 } }, { a: { $gte: 1000 } }, { a: { $gte: 2000 } }] };
   checkResult( dbcl, findCondition36, null, expRecs6, { No: 1 } );

   //lt or
   var findCondition37 = { $or: [{ a: { $lt: 1000 } }, { a: { $lt: 2000 } }, { a: { $lt: 3000 } }] };
   var expRecs7 = [{ No: 1, a: 500 }, { No: 2, a: 501 }, { No: 3, a: 502 }, { No: 4, a: 503 }, { No: 5, a: 504 },
   { No: 6, a: 505 }, { No: 7, a: 506 }, { No: 8, a: 507 }, { No: 9, a: 508 }, { No: 10, a: 509 },
   { No: 11, a: 1000 }, { No: 12, a: 1001 }, { No: 13, a: 1002 }, { No: 14, a: 1003 }, { No: 15, a: 1004 },
   { No: 16, a: 1005 }, { No: 17, a: 1006 }, { No: 18, a: 1007 }, { No: 19, a: 1008 }, { No: 20, a: 1009 },
   { No: 21, a: 1500 }, { No: 22, a: 1501 }, { No: 23, a: 1502 }, { No: 24, a: 1503 }, { No: 25, a: 1504 },
   { No: 26, a: 1505 }, { No: 27, a: 1506 }, { No: 28, a: 1507 }, { No: 29, a: 1508 }, { No: 30, a: 1509 },
   { No: 31, a: 2000 }, { No: 32, a: 2001 }, { No: 33, a: 2002 }, { No: 34, a: 2003 }, { No: 35, a: 2004 },
   { No: 36, a: 2005 }, { No: 37, a: 2006 }, { No: 38, a: 2007 }, { No: 39, a: 2008 }, { No: 40, a: 2009 },
   { No: 41, a: 2500 }, { No: 42, a: 2501 }, { No: 43, a: 2502 }, { No: 44, a: 2503 }, { No: 45, a: 2504 },
   { No: 46, a: 2505 }, { No: 47, a: 2506 }, { No: 48, a: 2507 }, { No: 49, a: 2508 }, { No: 50, a: 2509 },
   { No: 83, a: [1, 2, 3] }];
   checkResult( dbcl, findCondition37, null, expRecs7, { No: 1 } );

   var findCondition38 = { $or: [{ a: { $lt: 1000 } }, { a: { $lt: 3000 } }, { a: { $lt: 2000 } }] };
   checkResult( dbcl, findCondition38, null, expRecs7, { No: 1 } );

   var findCondition39 = { $or: [{ a: { $lt: 2000 } }, { a: { $lt: 3000 } }, { a: { $lt: 1000 } }] };
   checkResult( dbcl, findCondition39, null, expRecs7, { No: 1 } );

   var findCondition40 = { $or: [{ a: { $lt: 2000 } }, { a: { $lt: 1000 } }, { a: { $lt: 3000 } }] };
   checkResult( dbcl, findCondition40, null, expRecs7, { No: 1 } );

   var findCondition41 = { $or: [{ a: { $lt: 3000 } }, { a: { $lt: 2000 } }, { a: { $lt: 1000 } }] };
   checkResult( dbcl, findCondition41, null, expRecs7, { No: 1 } );

   var findCondition42 = { $or: [{ a: { $lt: 3000 } }, { a: { $lt: 1000 } }, { a: { $lt: 2000 } }] };
   checkResult( dbcl, findCondition42, null, expRecs7, { No: 1 } );

   //lte or
   var findCondition43 = { $or: [{ a: { $lte: 1000 } }, { a: { $lte: 2000 } }, { a: { $lte: 3000 } }] };
   var expRecs8 = [{ No: 1, a: 500 }, { No: 2, a: 501 }, { No: 3, a: 502 }, { No: 4, a: 503 }, { No: 5, a: 504 },
   { No: 6, a: 505 }, { No: 7, a: 506 }, { No: 8, a: 507 }, { No: 9, a: 508 }, { No: 10, a: 509 },
   { No: 11, a: 1000 }, { No: 12, a: 1001 }, { No: 13, a: 1002 }, { No: 14, a: 1003 }, { No: 15, a: 1004 },
   { No: 16, a: 1005 }, { No: 17, a: 1006 }, { No: 18, a: 1007 }, { No: 19, a: 1008 }, { No: 20, a: 1009 },
   { No: 21, a: 1500 }, { No: 22, a: 1501 }, { No: 23, a: 1502 }, { No: 24, a: 1503 }, { No: 25, a: 1504 },
   { No: 26, a: 1505 }, { No: 27, a: 1506 }, { No: 28, a: 1507 }, { No: 29, a: 1508 }, { No: 30, a: 1509 },
   { No: 31, a: 2000 }, { No: 32, a: 2001 }, { No: 33, a: 2002 }, { No: 34, a: 2003 }, { No: 35, a: 2004 },
   { No: 36, a: 2005 }, { No: 37, a: 2006 }, { No: 38, a: 2007 }, { No: 39, a: 2008 }, { No: 40, a: 2009 },
   { No: 41, a: 2500 }, { No: 42, a: 2501 }, { No: 43, a: 2502 }, { No: 44, a: 2503 }, { No: 45, a: 2504 },
   { No: 46, a: 2505 }, { No: 47, a: 2506 }, { No: 48, a: 2507 }, { No: 49, a: 2508 }, { No: 50, a: 2509 },
   { No: 51, a: 3000 }, { No: 83, a: [1, 2, 3] }];
   checkResult( dbcl, findCondition43, null, expRecs8, { No: 1 } );

   var findCondition44 = { $or: [{ a: { $lte: 1000 } }, { a: { $lte: 3000 } }, { a: { $lte: 2000 } }] };
   checkResult( dbcl, findCondition44, null, expRecs8, { No: 1 } );

   var findCondition45 = { $or: [{ a: { $lte: 2000 } }, { a: { $lte: 1000 } }, { a: { $lte: 3000 } }] };
   checkResult( dbcl, findCondition45, null, expRecs8, { No: 1 } );

   var findCondition46 = { $or: [{ a: { $lte: 2000 } }, { a: { $lte: 3000 } }, { a: { $lte: 1000 } }] };
   checkResult( dbcl, findCondition46, null, expRecs8, { No: 1 } );

   var findCondition47 = { $or: [{ a: { $lte: 3000 } }, { a: { $lte: 2000 } }, { a: { $lte: 1000 } }] };
   checkResult( dbcl, findCondition47, null, expRecs8, { No: 1 } );

   var findCondition48 = { $or: [{ a: { $lte: 3000 } }, { a: { $lte: 1000 } }, { a: { $lte: 2000 } }] };
   checkResult( dbcl, findCondition48, null, expRecs8, { No: 1 } );

   //not
   //gt
   var findCondition49 = { $not: [{ a: { $gt: 1000 } }, { a: { $gt: 2000 } }, { a: { $gt: 3000 } }] };
   var expRecs9 = [{ No: 1, a: 500 }, { No: 2, a: 501 }, { No: 3, a: 502 }, { No: 4, a: 503 }, { No: 5, a: 504 },
   { No: 6, a: 505 }, { No: 7, a: 506 }, { No: 8, a: 507 }, { No: 9, a: 508 }, { No: 10, a: 509 },
   { No: 11, a: 1000 }, { No: 12, a: 1001 }, { No: 13, a: 1002 }, { No: 14, a: 1003 }, { No: 15, a: 1004 },
   { No: 16, a: 1005 }, { No: 17, a: 1006 }, { No: 18, a: 1007 }, { No: 19, a: 1008 }, { No: 20, a: 1009 },
   { No: 21, a: 1500 }, { No: 22, a: 1501 }, { No: 23, a: 1502 }, { No: 24, a: 1503 }, { No: 25, a: 1504 },
   { No: 26, a: 1505 }, { No: 27, a: 1506 }, { No: 28, a: 1507 }, { No: 29, a: 1508 }, { No: 30, a: 1509 },
   { No: 31, a: 2000 }, { No: 32, a: 2001 }, { No: 33, a: 2002 }, { No: 34, a: 2003 }, { No: 35, a: 2004 },
   { No: 36, a: 2005 }, { No: 37, a: 2006 }, { No: 38, a: 2007 }, { No: 39, a: 2008 }, { No: 40, a: 2009 },
   { No: 41, a: 2500 }, { No: 42, a: 2501 }, { No: 43, a: 2502 }, { No: 44, a: 2503 }, { No: 45, a: 2504 },
   { No: 46, a: 2505 }, { No: 47, a: 2506 }, { No: 48, a: 2507 }, { No: 49, a: 2508 }, { No: 50, a: 2509 },
   { No: 51, a: 3000 },
   { No: 71, a: { $oid: "123abcd00ef12358902300ef" } },
   { No: 72, a: { $date: "2000-01-01" } },
   { No: 73, a: { $timestamp: "2000-01-01-15.32.18.000000" } },
   { No: 74, a: { $binary: "aGVsbG8gd29ybGQ=", "$type": "1" } },
   { No: 75, a: { $regex: "^z", "$options": "i" } },
   { No: 76, a: null },
   { No: 77, a: "abc" },
   { No: 78, a: { $minKey: 1 } },
   { No: 79, a: { $maxKey: 1 } },
   { No: 80, a: true },
   { No: 81, a: false },
   { No: 82, a: { name: "zhang" } },
   { No: 83, a: [1, 2, 3] }];
   checkResult( dbcl, findCondition49, null, expRecs9, { No: 1 } );

   var findCondition50 = { $not: [{ a: { $gt: 1000 } }, { a: { $gt: 3000 } }, { a: { $gt: 2000 } }] };
   checkResult( dbcl, findCondition50, null, expRecs9, { No: 1 } );

   var findCondition51 = { $not: [{ a: { $gt: 2000 } }, { a: { $gt: 3000 } }, { a: { $gt: 1000 } }] };
   checkResult( dbcl, findCondition51, null, expRecs9, { No: 1 } );

   var findCondition52 = { $not: [{ a: { $gt: 2000 } }, { a: { $gt: 1000 } }, { a: { $gt: 3000 } }] };
   checkResult( dbcl, findCondition52, null, expRecs9, { No: 1 } );

   var findCondition53 = { $not: [{ a: { $gt: 3000 } }, { a: { $gt: 2000 } }, { a: { $gt: 1000 } }] };
   checkResult( dbcl, findCondition53, null, expRecs9, { No: 1 } );

   var findCondition54 = { $not: [{ a: { $gt: 3000 } }, { a: { $gt: 1000 } }, { a: { $gt: 2000 } }] };
   checkResult( dbcl, findCondition54, null, expRecs9, { No: 1 } );

   //gte
   var findCondition55 = { $not: [{ a: { $gte: 3000 } }, { a: { $gte: 2000 } }, { a: { $gte: 1000 } }] };
   var expRecs10 = [{ No: 1, a: 500 }, { No: 2, a: 501 }, { No: 3, a: 502 }, { No: 4, a: 503 }, { No: 5, a: 504 },
   { No: 6, a: 505 }, { No: 7, a: 506 }, { No: 8, a: 507 }, { No: 9, a: 508 }, { No: 10, a: 509 },
   { No: 11, a: 1000 }, { No: 12, a: 1001 }, { No: 13, a: 1002 }, { No: 14, a: 1003 }, { No: 15, a: 1004 },
   { No: 16, a: 1005 }, { No: 17, a: 1006 }, { No: 18, a: 1007 }, { No: 19, a: 1008 }, { No: 20, a: 1009 },
   { No: 21, a: 1500 }, { No: 22, a: 1501 }, { No: 23, a: 1502 }, { No: 24, a: 1503 }, { No: 25, a: 1504 },
   { No: 26, a: 1505 }, { No: 27, a: 1506 }, { No: 28, a: 1507 }, { No: 29, a: 1508 }, { No: 30, a: 1509 },
   { No: 31, a: 2000 }, { No: 32, a: 2001 }, { No: 33, a: 2002 }, { No: 34, a: 2003 }, { No: 35, a: 2004 },
   { No: 36, a: 2005 }, { No: 37, a: 2006 }, { No: 38, a: 2007 }, { No: 39, a: 2008 }, { No: 40, a: 2009 },
   { No: 41, a: 2500 }, { No: 42, a: 2501 }, { No: 43, a: 2502 }, { No: 44, a: 2503 }, { No: 45, a: 2504 },
   { No: 46, a: 2505 }, { No: 47, a: 2506 }, { No: 48, a: 2507 }, { No: 49, a: 2508 }, { No: 50, a: 2509 },
   { No: 71, a: { $oid: "123abcd00ef12358902300ef" } },
   { No: 72, a: { $date: "2000-01-01" } },
   { No: 73, a: { $timestamp: "2000-01-01-15.32.18.000000" } },
   { No: 74, a: { $binary: "aGVsbG8gd29ybGQ=", "$type": "1" } },
   { No: 75, a: { $regex: "^z", "$options": "i" } },
   { No: 76, a: null },
   { No: 77, a: "abc" },
   { No: 78, a: { $minKey: 1 } },
   { No: 79, a: { $maxKey: 1 } },
   { No: 80, a: true },
   { No: 81, a: false },
   { No: 82, a: { name: "zhang" } },
   { No: 83, a: [1, 2, 3] }];
   checkResult( dbcl, findCondition55, null, expRecs10, { No: 1 } );

   var findCondition56 = { $not: [{ a: { $gte: 1000 } }, { a: { $gte: 2000 } }, { a: { $gte: 3000 } }] };
   checkResult( dbcl, findCondition56, null, expRecs10, { No: 1 } );

   var findCondition57 = { $not: [{ a: { $gte: 1000 } }, { a: { $gte: 3000 } }, { a: { $gte: 2000 } }] };
   checkResult( dbcl, findCondition57, null, expRecs10, { No: 1 } );

   var findCondition58 = { $not: [{ a: { $gte: 2000 } }, { a: { $gte: 1000 } }, { a: { $gte: 3000 } }] };
   checkResult( dbcl, findCondition58, null, expRecs10, { No: 1 } );

   var findCondition59 = { $not: [{ a: { $gte: 2000 } }, { a: { $gte: 3000 } }, { a: { $gte: 1000 } }] };
   checkResult( dbcl, findCondition59, null, expRecs10, { No: 1 } );

   var findCondition60 = { $not: [{ a: { $gte: 3000 } }, { a: { $gte: 1000 } }, { a: { $gte: 2000 } }] };
   checkResult( dbcl, findCondition60, null, expRecs10, { No: 1 } );

   //lt 
   var findCondition61 = { $not: [{ a: { $lt: 1000 } }, { a: { $lt: 2000 } }, { a: { $lt: 3000 } }] };
   var expRecs11 = [{ No: 11, a: 1000 }, { No: 12, a: 1001 }, { No: 13, a: 1002 }, { No: 14, a: 1003 }, { No: 15, a: 1004 },
   { No: 16, a: 1005 }, { No: 17, a: 1006 }, { No: 18, a: 1007 }, { No: 19, a: 1008 }, { No: 20, a: 1009 },
   { No: 21, a: 1500 }, { No: 22, a: 1501 }, { No: 23, a: 1502 }, { No: 24, a: 1503 }, { No: 25, a: 1504 },
   { No: 26, a: 1505 }, { No: 27, a: 1506 }, { No: 28, a: 1507 }, { No: 29, a: 1508 }, { No: 30, a: 1509 },
   { No: 31, a: 2000 }, { No: 32, a: 2001 }, { No: 33, a: 2002 }, { No: 34, a: 2003 }, { No: 35, a: 2004 },
   { No: 36, a: 2005 }, { No: 37, a: 2006 }, { No: 38, a: 2007 }, { No: 39, a: 2008 }, { No: 40, a: 2009 },
   { No: 41, a: 2500 }, { No: 42, a: 2501 }, { No: 43, a: 2502 }, { No: 44, a: 2503 }, { No: 45, a: 2504 },
   { No: 46, a: 2505 }, { No: 47, a: 2506 }, { No: 48, a: 2507 }, { No: 49, a: 2508 }, { No: 50, a: 2509 },
   { No: 51, a: 3000 }, { No: 52, a: 3001 }, { No: 53, a: 3002 }, { No: 54, a: 3003 }, { No: 55, a: 3004 },
   { No: 56, a: 3005 }, { No: 57, a: 3006 }, { No: 58, a: 3007 }, { No: 59, a: 3008 }, { No: 60, a: 3009 },
   { No: 61, a: 3500 }, { No: 62, a: 3501 }, { No: 63, a: 3502 }, { No: 64, a: 3503 }, { No: 65, a: 3504 },
   { No: 66, a: 3505 }, { No: 67, a: 3506 }, { No: 68, a: 3507 }, { No: 69, a: 3508 }, { No: 70, a: 3509 },
   { No: 71, a: { $oid: "123abcd00ef12358902300ef" } },
   { No: 72, a: { $date: "2000-01-01" } },
   { No: 73, a: { $timestamp: "2000-01-01-15.32.18.000000" } },
   { No: 74, a: { $binary: "aGVsbG8gd29ybGQ=", "$type": "1" } },
   { No: 75, a: { $regex: "^z", "$options": "i" } },
   { No: 76, a: null },
   { No: 77, a: "abc" },
   { No: 78, a: { $minKey: 1 } },
   { No: 79, a: { $maxKey: 1 } },
   { No: 80, a: true },
   { No: 81, a: false },
   { No: 82, a: { name: "zhang" } }];
   checkResult( dbcl, findCondition61, null, expRecs11, { No: 1 } );

   var findCondition62 = { $not: [{ a: { $lt: 1000 } }, { a: { $lt: 3000 } }, { a: { $lt: 2000 } }] };
   checkResult( dbcl, findCondition62, null, expRecs11, { No: 1 } );

   var findCondition63 = { $not: [{ a: { $lt: 2000 } }, { a: { $lt: 3000 } }, { a: { $lt: 1000 } }] };
   checkResult( dbcl, findCondition63, null, expRecs11, { No: 1 } );

   var findCondition64 = { $not: [{ a: { $lt: 2000 } }, { a: { $lt: 1000 } }, { a: { $lt: 3000 } }] };
   checkResult( dbcl, findCondition64, null, expRecs11, { No: 1 } );

   var findCondition65 = { $not: [{ a: { $lt: 3000 } }, { a: { $lt: 2000 } }, { a: { $lt: 1000 } }] };
   checkResult( dbcl, findCondition65, null, expRecs11, { No: 1 } );

   var findCondition66 = { $not: [{ a: { $lt: 3000 } }, { a: { $lt: 1000 } }, { a: { $lt: 2000 } }] };
   checkResult( dbcl, findCondition66, null, expRecs11, { No: 1 } );

   //lte
   var findCondition67 = { $not: [{ a: { $lte: 1000 } }, { a: { $lte: 2000 } }, { a: { $lte: 3000 } }] };
   var expRecs12 = [{ No: 12, a: 1001 }, { No: 13, a: 1002 }, { No: 14, a: 1003 }, { No: 15, a: 1004 },
   { No: 16, a: 1005 }, { No: 17, a: 1006 }, { No: 18, a: 1007 }, { No: 19, a: 1008 }, { No: 20, a: 1009 },
   { No: 21, a: 1500 }, { No: 22, a: 1501 }, { No: 23, a: 1502 }, { No: 24, a: 1503 }, { No: 25, a: 1504 },
   { No: 26, a: 1505 }, { No: 27, a: 1506 }, { No: 28, a: 1507 }, { No: 29, a: 1508 }, { No: 30, a: 1509 },
   { No: 31, a: 2000 }, { No: 32, a: 2001 }, { No: 33, a: 2002 }, { No: 34, a: 2003 }, { No: 35, a: 2004 },
   { No: 36, a: 2005 }, { No: 37, a: 2006 }, { No: 38, a: 2007 }, { No: 39, a: 2008 }, { No: 40, a: 2009 },
   { No: 41, a: 2500 }, { No: 42, a: 2501 }, { No: 43, a: 2502 }, { No: 44, a: 2503 }, { No: 45, a: 2504 },
   { No: 46, a: 2505 }, { No: 47, a: 2506 }, { No: 48, a: 2507 }, { No: 49, a: 2508 }, { No: 50, a: 2509 },
   { No: 51, a: 3000 }, { No: 52, a: 3001 }, { No: 53, a: 3002 }, { No: 54, a: 3003 }, { No: 55, a: 3004 },
   { No: 56, a: 3005 }, { No: 57, a: 3006 }, { No: 58, a: 3007 }, { No: 59, a: 3008 }, { No: 60, a: 3009 },
   { No: 61, a: 3500 }, { No: 62, a: 3501 }, { No: 63, a: 3502 }, { No: 64, a: 3503 }, { No: 65, a: 3504 },
   { No: 66, a: 3505 }, { No: 67, a: 3506 }, { No: 68, a: 3507 }, { No: 69, a: 3508 }, { No: 70, a: 3509 },
   { No: 71, a: { $oid: "123abcd00ef12358902300ef" } },
   { No: 72, a: { $date: "2000-01-01" } },
   { No: 73, a: { $timestamp: "2000-01-01-15.32.18.000000" } },
   { No: 74, a: { $binary: "aGVsbG8gd29ybGQ=", "$type": "1" } },
   { No: 75, a: { $regex: "^z", "$options": "i" } },
   { No: 76, a: null },
   { No: 77, a: "abc" },
   { No: 78, a: { $minKey: 1 } },
   { No: 79, a: { $maxKey: 1 } },
   { No: 80, a: true },
   { No: 81, a: false },
   { No: 82, a: { name: "zhang" } }];
   checkResult( dbcl, findCondition67, null, expRecs12, { No: 1 } );

   var findCondition68 = { $not: [{ a: { $lte: 1000 } }, { a: { $lte: 3000 } }, { a: { $lte: 2000 } }] };
   checkResult( dbcl, findCondition68, null, expRecs12, { No: 1 } );

   var findCondition69 = { $not: [{ a: { $lte: 2000 } }, { a: { $lte: 1000 } }, { a: { $lte: 3000 } }] };
   checkResult( dbcl, findCondition69, null, expRecs12, { No: 1 } );

   var findCondition70 = { $not: [{ a: { $lte: 2000 } }, { a: { $lte: 3000 } }, { a: { $lte: 1000 } }] };
   checkResult( dbcl, findCondition70, null, expRecs12, { No: 1 } );

   var findCondition71 = { $not: [{ a: { $lte: 3000 } }, { a: { $lte: 2000 } }, { a: { $lte: 1000 } }] };
   checkResult( dbcl, findCondition71, null, expRecs12, { No: 1 } );

   var findCondition72 = { $not: [{ a: { $lte: 3000 } }, { a: { $lte: 1000 } }, { a: { $lte: 2000 } }] };
   checkResult( dbcl, findCondition72, null, expRecs12, { No: 1 } );
}
