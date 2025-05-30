/*******************************************************************************


   Copyright (C) 2023-present SequoiaDB Ltd.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Affero General Public License for more details.

   You should have received a copy of the GNU Affero General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   Source File Name = clsMsgHandler.cpp

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          26/11/2012  Xu Jianhui  Initial Draft

   Last Changed =

*******************************************************************************/

#include "clsMsgHandler.hpp"
#include "pdTrace.hpp"
#include "clsTrace.hpp"


namespace engine
{
   /*
      _shdMsgHandler implement
   */
   _shdMsgHandler::_shdMsgHandler ( _pmdAsycSessionMgr *pSessionMgr,
                                    _schedTaskAdapterBase *pTaskAdapter,
                                    _pmdRemoteSessionMgr *pRemoteSessionMgr )
      : _pmdAsyncMsgHandler ( pSessionMgr, pTaskAdapter, pRemoteSessionMgr )
   {
      _pShardCB = NULL ;
   }

   _shdMsgHandler::~_shdMsgHandler ()
   {
      _pShardCB = NULL ;
   }

   void _shdMsgHandler::_postMainMsg( const NET_HANDLE & handle,
                                      MsgHeader * pNewMsg,
                                      pmdEDUMemTypes memType )
   {
      if ( _pShardCB && ( MSG_CAT_NODEGRP_RES == pNewMsg->opCode ||
           MSG_CAT_QUERY_CATALOG_RSP == pNewMsg->opCode ||
           MSG_CAT_QUERY_SPACEINFO_RSP == pNewMsg->opCode ||
           ( MSG_CAT_CATGRP_RES == pNewMsg->opCode &&
             _pShardCB->getTID() != (UINT32)pNewMsg->requestID ) ) )
      {
         _pShardCB->postEvent( pmdEDUEvent( PMD_EDU_EVENT_MSG,
                                            memType,
                                            pNewMsg, (UINT64)handle ) ) ;
      }
      else
      {
         //store type to TID and dispatch restore
         pNewMsg->TID = (UINT32)CLS_SHARD ;
         _pMgrEDUCB->postEvent( pmdEDUEvent( PMD_EDU_EVENT_MSG,
                                             memType,
                                             pNewMsg, (UINT64)handle ) );
      }
   }

   void _shdMsgHandler::handleClose( const NET_HANDLE &handle,
                                     _MsgRouteID id )
   {
      _pmdAsyncMsgHandler::handleClose( handle, id ) ;

      /// post msg to shard edu
      if ( _pShardCB )
      {
         MsgOpReply *pMsg = NULL ;
         pMsg = ( MsgOpReply* )SDB_THREAD_ALLOC( sizeof( MsgOpReply ) ) ;
         if ( !pMsg )
         {
            PD_LOG( PDERROR, "Alloc memory[size: %d] failed",
                    sizeof( MsgOpReply ) ) ;
         }
         else
         {
            pMsg->contextID = -1 ;
            pMsg->flags = SDB_NETWORK_CLOSE ;
            pMsg->header.messageLength = sizeof( MsgOpReply ) ;
            pMsg->header.opCode = MSG_COM_REMOTE_DISC ;
            pMsg->header.requestID = 0 ;
            pMsg->header.routeID.value = id.value ;
            pMsg->header.TID = 0 ;
            pMsg->numReturned = 0 ;
            pMsg->startFrom = 0 ;

            _pShardCB->postEvent( pmdEDUEvent( PMD_EDU_EVENT_MSG,
                                               PMD_EDU_MEM_THREAD,
                                               pMsg, (UINT64)handle ) ) ;
         }
      }
   }

   /*
      _replMsgHandler implement
   */
   _replMsgHandler::_replMsgHandler ( _pmdAsycSessionMgr *pSessionMgr )
      :_pmdAsyncMsgHandler ( pSessionMgr )
   {
   }

   _replMsgHandler::~_replMsgHandler ()
   {
   }

   void _replMsgHandler::_postMainMsg( const NET_HANDLE &handle,
                                       MsgHeader *pNewMsg,
                                       pmdEDUMemTypes memType )
   {
      //store type to TID and dispatch restore
      pNewMsg->TID = (UINT32)CLS_REPL ;
      _pMgrEDUCB->postEvent( pmdEDUEvent( PMD_EDU_EVENT_MSG,
                                          memType,
                                          pNewMsg, (UINT64)handle ) ) ;
   }

}


