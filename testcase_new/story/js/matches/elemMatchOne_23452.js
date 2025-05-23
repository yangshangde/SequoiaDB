/******************************************************************************
 * @Description   : seqDB-23452 :: $elemMatchOne条件以非$开头 
 *                  seqDB-23453 :: $elemMatchOne条件是以$开头的选择符，选择数组的非嵌套元素 
 *                  seqDB-23454 :: $elemMatchOne选择字段为数组中的数组 
 *                  seqDB-23455 :: $elemMatchOne选择多个字段，包含数组的非嵌套元素 
 *                  seqDB-23456 :: $elemMatchOne条件是以$开头的选择符，选择嵌套对象 
 * @Author        : Yu Fan
 * @CreateTime    : 2021.01.20
 * @LastEditTime  : 2021.10.14
 * @LastEditors   : Yi Pan
 ******************************************************************************/
testConf.clName = COMMCLNAME + "_23452";

main( test );
function test ( testPara )
{
   var cl = testPara.testCL;
   // 准备数据
   var docs = [{ a: 1, b: [] },
   { a: 2, b: [1, 2, 3] },
   { a: 3, b: [{ b1: 1, b2: 1 }, { b1: 2, b2: 2 }] },
   { a: 4, b: [1, [1, 2, 3]] },
   { a: 5, b: [1, 1, 2, 2, { b1: 1, b2: 2 }, [1, 2, 3]] },
   { a: 6, b: [{ b1: { b2: 1 } }, { b2: 2 }] },
   { a: 7, b: { b1: [1, 2, 3] } },
   { a: 8, b: { b1: 1, b2: { b3: 1 }, b3: [1, 1, 2, 3] } },
   { a: 9, b: [{ "": 1 }, { "": 1 }] },
   { a: 10, b: [1, 1, 2, 3], c: [1, 2, 2, 3] },
   { a: 11, b: [1, 1, 2, 3], c: [1, 2, 2, 3], d: { e: [1, 2, 3] } },
   { a: 12 }]

   cl.insert( docs );

   // $elemMatchOne条件以非$开头  
   cursor = cl.find( { a: { $gte: 9, $lte: 10 } }, { "b": { $elemMatchOne: { "": 1 } } } ).sort( { a: 1 } );
   commCompareResults( cursor, [
      { a: 9, b: [{ "": 1 }] },
      { a: 10, b: [], c: [1, 2, 2, 3] }] );

   // $elemMatchOne条件是以$开头的选择符，选择数组的非嵌套元素 
   cursor = cl.find( { a: { $gte: 10, $lte: 12 } }, { b: { $elemMatchOne: { $et: 1 } } } ).sort( { a: 1 } );
   commCompareResults( cursor, [
      { "a": 10, "b": [1], "c": [1, 2, 2, 3] },
      { "a": 11, "b": [1], "c": [1, 2, 2, 3], "d": { "e": [1, 2, 3] } },
      { a: 12 }] );

   cursor = cl.find( { a: { $gte: 5, $lte: 6 } }, { b: { $elemMatchOne: { $in: [1, 2] } } } ).sort( { a: 1 } );
   commCompareResults( cursor, [
      { "a": 5, "b": [1] },
      { a: 6, b: [] }] );

   cursor = cl.find( { a: { $gte: 4, $lte: 6 } }, { b: { $elemMatchOne: { $gt: 1, $lt: 3 } } } ).sort( { a: 1 } );
   commCompareResults( cursor, [
      { a: 4, b: [[1, 2, 3]] },
      { a: 5, b: [2] },
      { a: 6, b: [] }] );

   //  $elemMatchOne选择字段为数组中的数组  
   //  SEQUOIADBMAINSTREAM-6546
   cursor = cl.find( { a: { $gte: 5, $lte: 6 } }, { "b.$[5]": { $elemMatchOne: { $gt: 1, $lt: 3 } } } ).sort( { a: 1 } );
   commCompareResults( cursor, [
      { a: 5, b: [[2]] },
      { a: 6, b: [] }] );

   // $elemMatchOne选择多个字段，包含数组的非嵌套元素 
   cursor = cl.find( { a: { $gte: 10, $lte: 12 } }, { b: { $elemMatchOne: { $et: 1 } }, c: { $elemMatchOne: { $gt: 1, $lt: 3 } } } ).sort( { a: 1 } );
   commCompareResults( cursor, [
      { a: 10, b: [1], c: [2] },
      { a: 11, b: [1], c: [2], d: { e: [1, 2, 3] } },
      { a: 12 }] );

   // $elemMatchOne条件是以$开头的选择符，选择嵌套对象 
   cursor = cl.find( { a: 8 }, { "b.b3": { $elemMatchOne: { $et: 1 } } } ).sort( { a: 1 } );
   commCompareResults( cursor, [{ a: 8, b: { b1: 1, b2: { b3: 1 }, b3: [1] } }] );
}

