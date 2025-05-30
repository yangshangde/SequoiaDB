/******************************************************************************
 * @Description   : seqDB-24154:源集群设置会话访问属性，多值指定preferedinstance存在的同时设置PreferedStrict
 * @Author        : Wu Yan
 * @CreateTime    : 2021.05.06
 * @LastEditTime  : 2022.11.03
 * @LastEditors   : liuli
 ******************************************************************************/
testConf.skipStandAlone = true;
main( test );

function test ()
{
   var dataSrcName = "datasrc24154";
   var clName = CHANGEDPREFIX + "_datasource24154";
   var srcCSName = "datasrcCS_24154";
   var csNameA = "DS_24154A";
   var csNameB = "DS_24154B";
   commDropCS( datasrcDB, srcCSName );
   clearDataSource( [csNameA, csNameB], dataSrcName );
   commCreateCS( datasrcDB, srcCSName );
   var groups = commGetGroups( datasrcDB );
   var groupName = groups[0][0].GroupName;
   commCreateCL( datasrcDB, srcCSName, clName, { ShardingKey: { a: 1 }, ReplSize: -1, Group: groupName } );

   db.createDataSource( dataSrcName, datasrcUrl, userName, passwd );
   //集合空间级映射
   var csA = db.createCS( csNameA, { DataSource: dataSrcName, Mapping: srcCSName } );
   var dbclA = csA.getCL( clName );
   //集合级映射
   var csB = db.createCS( csNameB );
   var dbclB = csB.createCL( clName, { DataSource: dataSrcName, Mapping: srcCSName + "." + clName } );
   var docs = [{ a: 1, b: 1, c: "testc" }, { a: 2, b: [1, "test"], c: "testsgasdgasdg" }, { a: 3, b: 234.3, c: { a: 1 } }, { a: 4, b: 4, c: "testtess4" }];
   dbclB.insert( docs );

   try
   {
      //数据源集群上配置节点instanceId
      var instanceid = [7, 8, 10];
      var nodes = [];
      for( var i = 0; i < instanceid.length; i++ )
      {
         var hostName = groups[0][i + 1].HostName;
         var svcName = groups[0][i + 1].svcname;
         updateConf( datasrcDB, { instanceid: instanceid[i] }, { NodeName: hostName + ":" + svcName }, [SDB_RTN_CONF_NOT_TAKE_EFFECT, SDB_COORD_NOT_ALL_DONE] );
         nodes.push( hostName + ":" + svcName );
      }
      datasrcDB.getRG( groupName ).stop();
      datasrcDB.getRG( groupName ).start();
      commCheckBusinessStatus( datasrcDB );
      datasrcDB.invalidateCache();

      //设置PreferedStrict: true，指定第一个instanceId对应节点异常
      var optionsA = { PreferedInstance: [instanceid[0], instanceid[1]], PreferedStrict: true };
      db.setSessionAttr( optionsA );
      datasrcDB.getRG( groupName ).getNode( groups[0][1].HostName, groups[0][1].svcname ).stop();
      var expAccessNodes = [nodes[1]];
      setSessionAndcheckAccessNodes( dbclA, expAccessNodes, optionsA );
      setSessionAndcheckAccessNodes( dbclB, expAccessNodes, optionsA );

      //设置PreferedStrict: true，指定所有instanceId对应节点异常
      datasrcDB.getRG( groupName ).getNode( groups[0][2].HostName, groups[0][2].svcname ).stop();
      assert.tryThrow( [SDB_CLS_NODE_BSFAULT], function() 
      {
         dbclA.find().explain();
      } );

      assert.tryThrow( [SDB_CLS_NODE_BSFAULT], function() 
      {
         dbclB.find().explain();
      } );

      //设置PreferedStrict: false，指定instanceId节点不存在时走其它正常节点
      var optionsB = { PreferedInstance: [instanceid[0], instanceid[1]], PreferedStrict: false };
      var expAccessNodes = [nodes[2]];
      setSessionAndcheckAccessNodes( dbclA, expAccessNodes, optionsB );
      setSessionAndcheckAccessNodes( dbclB, expAccessNodes, optionsB );

      //设置PreferedStrict: true，指定instanceId节点存在时访问对应节点
      datasrcDB.getRG( groupName ).getNode( groups[0][1].HostName, groups[0][1].svcname ).start();
      expAccessNodes = [groups[0][1].HostName + ":" + groups[0][1].svcname];
      setSessionAndcheckAccessNodes( dbclA, expAccessNodes, optionsA );
      setSessionAndcheckAccessNodes( dbclB, expAccessNodes, optionsA );
      datasrcDB.getRG( groupName ).getNode( groups[0][2].HostName, groups[0][2].svcname ).start();
      commCheckBusinessStatus( datasrcDB );
   }
   finally
   {
      datasrcDB.getRG( groupName ).start();
      deleteConf( datasrcDB, { instanceid: 1 }, { GroupName: groupName }, [SDB_RTN_CONF_NOT_TAKE_EFFECT, SDB_COORD_NOT_ALL_DONE] );
      datasrcDB.getRG( groupName ).stop();
      datasrcDB.getRG( groupName ).start();
      commCheckBusinessStatus( datasrcDB );
   }

   datasrcDB.dropCS( srcCSName );
   datasrcDB.close();
   clearDataSource( [csNameA, csNameB], dataSrcName );
}

