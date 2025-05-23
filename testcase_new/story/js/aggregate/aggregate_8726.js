﻿
main( test );
function test ()
{
   var cl = new collection( db, COMMCSNAME, COMMCLNAME );
   cl.create();
   loadData( cl );
   var cursor = cl.execAggregate( { $match: { $and: [{ no: { $gt: 1001, $lt: 1012 } }, { dep: "计算机学院" }] } }, { $project: { no: 1, "info.name": 1, major: 1, dep: 1 } } );

   var expectResult = [{ "no": 1002, "info.name": "Holiday", "major": "计算机软件与理论", "dep": "计算机学院" },
   { "no": 1003, "info.name": "Sam", "major": "计算机软件与理论", "dep": "计算机学院" },
   { "no": 1004, "info.name": "Coll", "major": "计算机工程", "dep": "计算机学院" },
   { "no": 1006, "info.name": "Jim", "major": "计算机工程", "dep": "计算机学院" }];
   var parameters = "{$match:{$and:[{no:{$gt:1001, $lt:1012}}, {dep:'计算机学院'}]}}, {$project:{no:1, 'info.name':1, major:1, dep:1}}";
   checkResult( cursor, expectResult, parameters );

   cl.drop();

}

function loadData ( cl )
{
   var docs = [{ no: 1000, major: "计算机科学与技术", dep: "计算机学院", info: { name: "Tom", age: 25, sex: "男" } },
   { no: 1001, major: "计算机科学与技术", dep: "计算机学院", info: { name: "Json", age: 20, sex: "男" } },
   { no: 1002, major: "计算机软件与理论", dep: "计算机学院", info: { name: "Holiday", age: 22, sex: "女" } },
   { no: 1003, major: "计算机软件与理论", dep: "计算机学院", info: { name: "Sam", age: 30, sex: "男" } },
   { no: 1004, major: "计算机工程", dep: "计算机学院", info: { name: "Coll", age: 26, sex: "男" } },
   { no: 1006, major: "计算机工程", dep: "计算机学院", info: { name: "Jim", age: 24, sex: "女" } },
   { no: 1007, major: "物理学", dep: "物电学院", info: { name: "Lily", age: 28, sex: "女" } },
   { no: 1008, major: "物理学", dep: "物电学院", info: { name: "Lucy", age: 36, sex: "女" } },
   { no: 1009, major: "光学", dep: "物电学院", info: { name: "Coco", age: 27, sex: "女" } },
   { no: 1010, major: "光学", dep: "物电学院", info: { name: "Jack", age: 30, sex: "男" } },
   { no: 1011, major: "电学", dep: "物电学院", info: { name: "Mike", age: 28, sex: "男" } },
   { no: 1012, major: "电学", dep: "物电学院", info: { name: "Jaden", age: 20, sex: "男" } }
   ];
   cl.bulkInsert( docs );
}