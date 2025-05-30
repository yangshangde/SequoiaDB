/************************************
*@Description: 分区范围逆序，查询条件带主、子表分区键，
            查询条件满足：被主表的LowBond-UpBound包含同时被子表的部分分区包含，进行查询/更新/删除
*@author:      zengxianquan
*@createDate:  2016.11.28
*@testlinkCase: seqDB-10449
**************************************/
main( test );

function test ()
{
   //check test environment before split
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

   mainCL_Name = CHANGEDPREFIX + "_maincl";
   subCL_Name1 = CHANGEDPREFIX + "_subcl1";
   subCL_Name2 = CHANGEDPREFIX + "_subcl2";
   //在测试前清除环境中冲突的表
   commDropCL( db, COMMCSNAME, mainCL_Name, true, true );
   commDropCL( db, COMMCSNAME, subCL_Name1, true, true );
   commDropCL( db, COMMCSNAME, subCL_Name2, true, true );

   prepareByInvertedSequence( mainCL_Name, subCL_Name1, subCL_Name2 );

   db.setSessionAttr( { PreferedInstance: "M" } );
   var maincl = db.getCS( COMMCSNAME ).getCL( mainCL_Name );
   //检测查找
   checkFind( maincl );
   //检测更新
   checkUpdate( maincl );
   //检测删除
   checkRemove( maincl );
   //清除环境
   db.getCS( COMMCSNAME ).dropCL( mainCL_Name );
}
function checkFind ( maincl )
{
   var realData = maincl.find( { a: { $gt: 14 }, b: { $gte: 0 } } ).sort( { a: 1, b: 1 } );
   //查找数据
   var expectDataArray = [];
   for( var i = 15; i < 20; i++ )
   {
      for( var j = 0; j < 100; j++ )
      {
         expectDataArray.push( { a: i, b: j, test: "testData" + j } );
      }
   }
   //检查插入的数据与预期的数据是否一致
   zxqCheckRec( realData, expectDataArray );
}
function checkUpdate ( maincl )
{
   //更新数据
   for( var i = 15; i < 20; i++ )
   {
      for( var j = 0; j < 100; j++ )
      {
         maincl.update( { $set: { "test": "update" + j } }, { a: i, b: j } );
      }
   }
   var expectDataArray = [];
   for( var i = 15; i < 20; i++ )
   {
      for( var j = 0; j < 100; j++ )
      {
         expectDataArray.push( { a: i, b: j, test: "update" + j } );
      }
   }
   var realData = maincl.find( { a: { $gt: 14 }, b: { $gte: 0 } } ).sort( { a: 1, b: 1 } );
   //更新后对数据的检验
   zxqCheckRec( realData, expectDataArray );
}
function checkRemove ( maincl )
{
   //删除数据
   for( var i = 15; i < 17; i++ )
   {
      maincl.remove( { a: i } );
   }
   var expectDataArray = [];
   for( var i = 17; i < 20; i++ )
   {
      for( var j = 0; j < 100; j++ )
      {
         expectDataArray.push( { a: i, b: j, test: "update" + j } );
      }
   }
   var realData = maincl.find( { a: { $gt: 14 }, b: { $gte: 0 } } ).sort( { a: 1, b: 1 } );
   //删除后对数据的检验
   zxqCheckRec( realData, expectDataArray );
}