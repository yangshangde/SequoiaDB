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

   Source File Name = catMainController.cpp

   Descriptive Name =

   When/how to use: this program may be used on binary and text-formatted
   versions of runtime component. This file contains code logic for
   common functions for coordinator node.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================

   Last Changed =

*******************************************************************************/
#include "core.hpp"
#include "catMainController.hpp"
#include "pmdCB.hpp"
#include "pd.hpp"
#include "catDef.hpp"
#include "rtn.hpp"
#include "rtnContextDump.hpp"
#include "dpsLogWrapper.hpp"
#include "msgMessage.hpp"
#include "msgAuth.hpp"
#include "pmdDef.hpp"
#include "pdTrace.hpp"
#include "catTrace.hpp"
#include "catCommon.hpp"
#include "catCMDBase.hpp"

using namespace bson;
namespace engine
{

   // Wait sync retries 3600 times, for each time, waits for 100ms. Plus the
   // waiting time interval 100ms for delay events, totally wait for
   // about 640000ms
   #define CAT_SYNC_MAX_RETRY_TIMES ( 3600 )
   #define CAT_SYNC_FIRST_INTERVAL ( 200 ) // ms
   #define CAT_SYNC_INTERVAL ( 0 ) // ms

   const utilCSUniqueID SYS_CAT_CSUID = UTIL_CSUNIQUEID_CAT_MIN + 0 ;
   const utilCLUniqueID SYS_CAT_NODE_CLUID =
               utilBuildCLUniqueID( SYS_CAT_CSUID, UTIL_CSUNIQUEID_CAT_MIN + 1 ) ;
   const utilCLUniqueID SYS_CAT_CS_CLUID =
               utilBuildCLUniqueID( SYS_CAT_CSUID, UTIL_CSUNIQUEID_CAT_MIN + 2 ) ;
   const utilCLUniqueID SYS_CAT_CL_CLUID =
               utilBuildCLUniqueID( SYS_CAT_CSUID, UTIL_CSUNIQUEID_CAT_MIN + 3 ) ;
   const utilCLUniqueID SYS_CAT_TASKS_CLUID =
               utilBuildCLUniqueID( SYS_CAT_CSUID, UTIL_CSUNIQUEID_CAT_MIN + 4 ) ;
   const utilCLUniqueID SYS_CAT_INDEXES_CLUID =
               utilBuildCLUniqueID( SYS_CAT_CSUID, UTIL_CSUNIQUEID_CAT_MIN + 5 ) ;
   const utilCLUniqueID SYS_CAT_DOMAINS_CLUID =
               utilBuildCLUniqueID( SYS_CAT_CSUID, UTIL_CSUNIQUEID_CAT_MIN + 6 ) ;
   const utilCLUniqueID SYS_CAT_HISTORY_CLUID =
               utilBuildCLUniqueID( SYS_CAT_CSUID, UTIL_CSUNIQUEID_CAT_MIN + 7 ) ;
   const utilCLUniqueID SYS_CAT_DATASOURCES_CLUID =
               utilBuildCLUniqueID( SYS_CAT_CSUID, UTIL_CSUNIQUEID_CAT_MIN + 8 ) ;
   const utilCLUniqueID SYS_CAT_GROUPMODES_CLUID =
               utilBuildCLUniqueID( SYS_CAT_CSUID, UTIL_CSUNIQUEID_CAT_MIN + 9 ) ;

   const utilCSUniqueID SYS_PROCEDURES_CSUID = UTIL_CSUNIQUEID_CAT_MIN + 1 ;
   const utilCLUniqueID SYS_PROCEDURES_CLUID =
               utilBuildCLUniqueID( SYS_PROCEDURES_CSUID, UTIL_CSUNIQUEID_CAT_MIN + 1 ) ;

   const utilCSUniqueID SYS_INFO_CSUID = UTIL_CSUNIQUEID_CAT_MIN + 2 ;
   const utilCLUniqueID SYS_INFO_DCBASE_CLUID =
               utilBuildCLUniqueID( SYS_INFO_CSUID, UTIL_CSUNIQUEID_CAT_MIN + 1 ) ;
   const utilCLUniqueID SYS_INFO_LOG_CLUID[ CAT_SYSLOG_CL_NUM ] =
   {
      utilBuildCLUniqueID( SYS_INFO_CSUID, UTIL_CSUNIQUEID_CAT_MIN + 2 ),
      utilBuildCLUniqueID( SYS_INFO_CSUID, UTIL_CSUNIQUEID_CAT_MIN + 3 ),
      utilBuildCLUniqueID( SYS_INFO_CSUID, UTIL_CSUNIQUEID_CAT_MIN + 4 ),
      utilBuildCLUniqueID( SYS_INFO_CSUID, UTIL_CSUNIQUEID_CAT_MIN + 5 ),
      utilBuildCLUniqueID( SYS_INFO_CSUID, UTIL_CSUNIQUEID_CAT_MIN + 6 ),
   } ;

   const utilCSUniqueID SYS_RECYCLEBIN_CSUID = UTIL_CSUNIQUEID_CAT_MIN + 3 ;
   const utilCLUniqueID SYS_RECYCLEBIN_ITEMS_CLUID =
               utilBuildCLUniqueID( SYS_RECYCLEBIN_CSUID, UTIL_CSUNIQUEID_CAT_MIN + 1 ) ;
   const utilCLUniqueID SYS_RECYCLEBIN_CS_CLUID =
               utilBuildCLUniqueID( SYS_RECYCLEBIN_CSUID, UTIL_CSUNIQUEID_CAT_MIN + 2 ) ;
   const utilCLUniqueID SYS_RECYCLEBIN_CL_CLUID =
               utilBuildCLUniqueID( SYS_RECYCLEBIN_CSUID, UTIL_CSUNIQUEID_CAT_MIN + 3 ) ;
   const utilCLUniqueID SYS_RECYCLEBIN_SEQ_CLUID =
               utilBuildCLUniqueID( SYS_RECYCLEBIN_CSUID, UTIL_CSUNIQUEID_CAT_MIN + 4 ) ;
   const utilCLUniqueID SYS_RECYCLEBIN_INDEXES_CLUID =
               utilBuildCLUniqueID( SYS_RECYCLEBIN_CSUID, UTIL_CSUNIQUEID_CAT_MIN + 5 ) ;

   BEGIN_OBJ_MSG_MAP( catMainController, _pmdObjBase )
      ON_EVENT( PMD_EDU_EVENT_ACTIVE, _onActiveEvent )
      ON_EVENT( PMD_EDU_EVENT_DEACTIVE, _onDeactiveEvent )
   END_OBJ_MSG_MAP()

   /*
      catMainController implement
   */
   catMainController::catMainController ()
   {
      _pEduMgr             = NULL ;
      _pCatCB              = NULL ;
      _pDmsCB              = NULL ;
      _pRtnCB              = NULL ;
      _pAuthCB             = NULL ;
      _pEDUCB              = NULL ;
      _pDpsCB              = NULL ;
      _checkEventTimerID   = NET_INVALID_TIMER_ID ;
      _isDelayed           = FALSE ;
      _delayWithoutSync    = FALSE ;
      _lastCheckDelayTick  = 0 ;

      _changeEvent.signal() ;
   }

   catMainController::~catMainController()
   {
   }

   void catMainController::attachCB( pmdEDUCB * cb )
   {
      _pEDUCB = cb ;

      /// not use rollback segment
      _pEDUCB->getTransExecutor()->setUseRollbackSemgent( FALSE, TRUE ) ;
      /// we don't need to use RC count on catalog ( since only one thread )
      _pEDUCB->getTransExecutor()->setTransRCCount( FALSE, TRUE ) ;

      if ( _pCatCB )
      {
         _pCatCB->getCatGTSMgr()->attachCB( cb ) ;
         _pCatCB->getCatlogueMgr()->attachCB( cb ) ;
         _pCatCB->getCatNodeMgr()->attachCB( cb ) ;
         _pCatCB->getCatDCMgr()->attachCB( cb ) ;
      }
      if ( _pAuthCB )
      {
         _pAuthCB->getRoleManager()->attachCB( cb );
      }

      _attachEvent.signalAll() ;
   }

   void catMainController::detachCB( pmdEDUCB * cb )
   {
      // clear delay event
      _dispatchDelayedOperation( FALSE ) ;

      if ( _pCatCB )
      {
         _pCatCB->getCatDCMgr()->detachCB( cb ) ;
         _pCatCB->getCatlogueMgr()->detachCB( cb ) ;
         _pCatCB->getCatNodeMgr()->detachCB( cb ) ;
         _pCatCB->getCatGTSMgr()->detachCB( cb ) ;
      }
      if ( _pAuthCB )
      {
         _pAuthCB->getRoleManager()->detachCB( cb );
      }
      _pEDUCB = NULL ;
      _changeEvent.signal() ;
   }

   void catMainController::onTimer( UINT64 timerID, UINT32 interval )
   {
      if ( _checkEventTimerID == timerID )
      {
         if ( _needCheckDelay() )
         {
            _dispatchDelayedOperation( TRUE ) ;
            _setCheckDelayTick() ;
         }
      }
      _pmdObjBase::onTimer( timerID, interval ) ;
   }

   BOOLEAN catMainController::canDelayed( UINT32 maxRetryTimes )
   {
      BOOLEAN result       = TRUE ;
      pmdEDUEvent *last    = getLastEvent() ;
      UINT32 handle        = 0 ;
      UINT32 tryTime       = 0 ;

      if ( NULL == _lastDelayEvent._Data &&
           ( PMD_EDU_EVENT_MSG != last->_eventType ||
             NULL == last->_Data ) )
      {
         result = FALSE ;
      }
      else
      {
         // if the msg has already delayed
         if ( _lastDelayEvent._Data )
         {
            ossUnpack32From64( _lastDelayEvent._userData, tryTime, handle ) ;

            if ( tryTime > maxRetryTimes )
            {
               result = FALSE ;
            }
         }
      }
      return result ;
   }

   BOOLEAN catMainController::delayCurOperation( UINT32 maxRetryTimes )
   {
      BOOLEAN result       = TRUE ;
      pmdEDUEvent *last    = getLastEvent() ;
      UINT32 handle        = 0 ;
      UINT32 tryTime       = 0 ;

      if ( NULL == _lastDelayEvent._Data &&
           ( PMD_EDU_EVENT_MSG != last->_eventType ||
             NULL == last->_Data ) )
      {
         result = FALSE ;
      }
      else
      {
         pmdEDUEvent event ;
         event._eventType = PMD_EDU_EVENT_MSG ;

         // if the msg has already delayed
         if ( _lastDelayEvent._Data )
         {
            ossUnpack32From64( _lastDelayEvent._userData, tryTime, handle ) ;

            if ( tryTime > maxRetryTimes )
            {
               result = FALSE ;
               goto done ;
            }
            event = _lastDelayEvent ;
            _lastDelayEvent._Data = NULL ;
            _lastDelayEvent._dataMemType = PMD_EDU_MEM_NONE ;
         }
         else
         {
            CHAR *pData = NULL ;
            MsgHeader *pMsg = ( MsgHeader* )last->_Data ;
            if ( SDB_OK == _pEDUCB->allocBuff( (UINT32)pMsg->messageLength,
                                               &pData, NULL ) )
            {
               ossMemcpy( pData, last->_Data, pMsg->messageLength ) ;
               event._Data = pData ;
               event._dataMemType = PMD_EDU_MEM_SELF ;
               event._eventType = PMD_EDU_EVENT_MSG ;

               handle = last->_userData ;
            }
            else
            {
               result = FALSE ;
               goto done ;
            }
         }

         event._userData = ossPack32To64( ++tryTime, handle ) ;
         _delayEvent( event ) ;
         PD_LOG ( PDDEBUG, "Delay event handle: [%u] type: [%d]",
                  handle, event._eventType ) ;
      }

   done:
      return result ;
   }

   void catMainController::_delayEvent ( pmdEDUEvent &event )
   {
      _vecEvent.push_back( event ) ;
      _isDelayed = TRUE ;

      // Make sure the timer is set
      if ( NET_INVALID_TIMER_ID == _checkEventTimerID )
      {
         _checkEventTimerID = _pCatCB->setTimer( CAT_DEALY_TIME_INTERVAL ) ;
      }
   }

   BOOLEAN catMainController::_needCheckDelay ()
   {
      if ( _vecEvent.size() > 0 &&
           pmdGetTickSpanTime( _lastCheckDelayTick ) >= CAT_DEALY_TIME_INTERVAL )
      {
            return TRUE ;
      }

      return FALSE ;
   }

   void catMainController::_setCheckDelayTick ()
   {
      // Set check timestamp
      _lastCheckDelayTick = pmdGetDBTick() ;
   }

   void catMainController::_dispatchDelayedOperation( BOOLEAN dispatch )
   {
      UINT32 handle  = 0 ;
      UINT32 tryTime = 0 ;

      VEC_EVENT tmpVecEvent = _vecEvent ;
      _vecEvent.clear() ;

      // Initialize the flag, we need to wait sync
      _delayWithoutSync = FALSE ;

      VEC_EVENT::iterator it = tmpVecEvent.begin() ;
      while ( it != tmpVecEvent.end() )
      {
         pmdEDUEvent &event = *it ;
         ++it ;
         _lastDelayEvent = event ;

         if ( dispatch )
         {
            ossUnpack32From64( event._userData, tryTime, handle ) ;
            MsgHeader *msg = ( MsgHeader * )event._Data ;
            _defaultMsgFunc( ( NET_HANDLE )handle, msg ) ;
         }

         pmdEduEventRelease( _lastDelayEvent, _pEDUCB ) ;
      }
      tmpVecEvent.clear() ;

      // reset last delay event
      _lastDelayEvent.reset() ;
   }

   void catMainController::_deleteDelayedOperation ( UINT32 handle )
   {
      UINT32 tryTime = 0 ;
      UINT32 savedHandle = 0;

      PD_LOG ( PDDEBUG, "Removing event for handle %u", handle ) ;

      VEC_EVENT::iterator itEvent = _vecEvent.begin() ;
      while ( itEvent != _vecEvent.end() )
      {
         pmdEDUEvent &event = ( *itEvent ) ;
         ossUnpack32From64( event._userData, tryTime, savedHandle ) ;
         if ( savedHandle == handle )
         {
            PD_LOG ( PDDEBUG, "Removed event %u %d", savedHandle, event._eventType ) ;
            pmdEduEventRelease( event, _pEDUCB ) ;
            itEvent = _vecEvent.erase( itEvent ) ;
         }
         else
         {
            PD_LOG ( PDDEBUG, "Keep event %u %d", savedHandle, event._eventType ) ;
            ++itEvent ;
         }
      }
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB_CATMAINCT_HANDLEMSG, "catMainController::handleMsg" )
   INT32 catMainController::handleMsg( const NET_HANDLE &handle,
                                       const _MsgHeader *header,
                                       const CHAR *msg )
   {
      SDB_ASSERT ( _pEduMgr && _pCatCB && _pDmsCB,
                   "all of the members must be initialized before init "
                   "netfram" ) ;
      SDB_ASSERT ( NULL != header, "message-header should not be NULL" ) ;
      INT32 rc = SDB_OK ;
      PD_TRACE_ENTRY ( SDB_CATMAINCT_HANDLEMSG ) ;
      PD_TRACE1 ( SDB_CATMAINCT_HANDLEMSG,
                  PD_PACK_INT ( header->opCode ) ) ;

      if ( (UINT32)MSG_SYSTEM_INFO_LEN == (UINT32)header->messageLength )
      {
         MsgSysInfoReply reply ;
         MsgSysInfoReply *pReply = &reply ;
         INT32 replySize = sizeof(reply) ;

         rc = msgBuildSysInfoReply ( (CHAR**)&pReply, &replySize,
                                     pmdGetStartTime() ) ;
         if ( rc )
         {
            PD_LOG( PDERROR, "Failed to build sys info reply, rc: %d", rc ) ;
            // set SDB_NET_BROKEN_MSG, so netFrame will close the connection
            rc = SDB_NET_BROKEN_MSG ;
         }
         else
         {
            rc = _pCatCB->netWork()->syncSendRaw( handle,
                                                  (const CHAR *)pReply,
                                                  (UINT32)replySize ) ;
         }
      }
      else if ( MSG_GTS_BEGIN < header->opCode &&
                header->opCode < MSG_GTS_END )
      {
         rc = _pCatCB->getCatGTSMgr()->handleMsg( handle, header ) ;
         if ( SDB_OK != rc )
         {
            PD_LOG( PDERROR, "Failed to handle GTS msg, rc: %d", rc ) ;
            // set SDB_NET_BROKEN_MSG, so netFrame will close the connection
            rc = SDB_NET_BROKEN_MSG ;
         }
      }
      else
      {
         rc = _postMsg( handle, header ) ;
         if ( SDB_OK != rc )
         {
            PD_LOG( PDERROR, "Failed to post catalog msg, rc: %d", rc ) ;
            // set SDB_NET_BROKEN_MSG, so netFrame will close the connection
            rc = SDB_NET_BROKEN_MSG ;
         }
      }

      PD_TRACE_EXITRC ( SDB_CATMAINCT_HANDLEMSG, rc ) ;
      return rc ;
   }

   void catMainController::handleClose( const NET_HANDLE & handle,
                                        _MsgRouteID id )
   {
      MsgOpReply msg ;
      msg.contextID = -1 ;
      msg.flags = SDB_NETWORK_CLOSE ;
      msg.header.messageLength = sizeof( MsgOpReply ) ;
      msg.header.opCode = MSG_COM_REMOTE_DISC ;
      msg.header.requestID = 0 ;
      msg.header.routeID.value = id.value ;
      msg.header.TID = 0 ;
      msg.numReturned = 0 ;
      msg.startFrom = 0 ;

      PD_LOG ( PDDEBUG, "posting event handle close %u", (UINT32)handle ) ;

      _postMsg( handle, (_MsgHeader *)&msg ) ;
   }

   void catMainController::handleTimeout( const UINT32 &millisec,
                                          const UINT32 &id )
   {
      PMD_EVENT_MESSAGES *eventMsg = NULL ;

      if ( !_pEDUCB )
      {
         PD_LOG( PDERROR, "Catalog Mgr cb is NULL" ) ;
         goto done ;
      }

      // memory will be freed in the event consumer thread
      // PMD_EDU_MEM_ALLOC will be passed into pmdEDUEvent, so that the
      // consumer knows whether to free the memory
      eventMsg = ( PMD_EVENT_MESSAGES * )SDB_THREAD_ALLOC(
                 sizeof (PMD_EVENT_MESSAGES ) ) ;

      if ( NULL == eventMsg )
      {
         // if unable to allocate memory, let's simply return
         PD_LOG ( PDWARNING, "Failed to allocate memory for PDM "
                  "timeout Event for %d bytes",
                  sizeof (PMD_EVENT_MESSAGES ) ) ;
      }
      else
      {
         ossTimestamp ts;
         ossGetCurrentTime(ts);

         eventMsg->timeoutMsg.interval = millisec ;
         eventMsg->timeoutMsg.occurTime = ts.time ;
         eventMsg->timeoutMsg.timerID = id ;

         // post the timeout event of current timestamp
         _pEDUCB->postEvent( pmdEDUEvent ( PMD_EDU_EVENT_TIMEOUT,
                                           PMD_EDU_MEM_THREAD,
                                           (void*)eventMsg ) ) ;
      }

   done:
      return ;
   }

   INT32 catMainController::_processInterruptMsg( const NET_HANDLE & handle,
                                                  MsgHeader * header )
   {
      PD_LOG( PDINFO, "Recieve interrupt msg[handle: %u, tid: %u]",
              handle, header->TID ) ;
      // release the ' handle + tid ' all context
      _delContext( handle, header->TID ) ;
      // not reply
      return SDB_OK ;
   }

   INT32 catMainController::_processDisconnectMsg( const NET_HANDLE & handle,
                                                   MsgHeader * header )
   {
      PD_LOG( PDEVENT, "Recieve disconnect msg[handle: %u, tid: %u]",
              handle, header->TID ) ;
      // release the ' handle + tid ' all context
      _delContext( handle, header->TID ) ;
      // not reply
      return SDB_OK ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB_CATMAINCT_POSTMSG, "catMainController::_postMsg" )
   INT32 catMainController::_postMsg( const NET_HANDLE &handle,
                                      const MsgHeader *header )
   {
      INT32 rc = SDB_OK;
      PD_TRACE_ENTRY ( SDB_CATMAINCT_POSTMSG ) ;
      PD_TRACE1 ( SDB_CATMAINCT_POSTMSG,
                  PD_PACK_INT ( header->opCode ) ) ;

      pmdEDUEvent event ;

      if ( NULL == _pEDUCB )
      {
         rc = SDB_SYS ;
         goto error ;
      }
      rc = _catBuildMsgEvent( handle, header, event ) ;
      PD_RC_CHECK( rc, PDERROR, "Failed to build the event, rc: %d", rc ) ;
      _pEDUCB->postEvent( event ) ;

   done:
      PD_TRACE_EXITRC ( SDB_CATMAINCT_POSTMSG, rc ) ;
      return rc;
   error:
      PD_LOG ( PDERROR, "Failed to process message(MessageType = %d, rc=%d)",
               header->opCode, rc ) ;
      goto done;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB_CATMAINCT_INIT, "catMainController::init" )
   INT32 catMainController::init()
   {
      INT32 rc             = SDB_OK ;
      pmdKRCB *pKrcb       = pmdGetKRCB() ;
      _pEduMgr             = pKrcb->getEDUMgr () ;
      _pDmsCB              = pKrcb->getDMSCB() ;
      _pRtnCB              = pKrcb->getRTNCB() ;
      _pAuthCB             = pKrcb->getAuthCB() ;
      _pCatCB              = pKrcb->getCATLOGUECB() ;
      _pDpsCB              = pKrcb->getDPSCB() ;

      PD_TRACE_ENTRY ( SDB_CATMAINCT_INIT ) ;

      // after initializing, let's attempt to create collectionspace and
      // collections
      rc = _ensureMetadata () ;
      PD_RC_CHECK ( rc, PDERROR, "Failed to create metadata "
                    "collections/indexes, rc = %d", rc ) ;

      _pCatCB->regEventHandler( this ) ;

      // Set timer
      _checkEventTimerID = _pCatCB->setTimer( CAT_DEALY_TIME_INTERVAL ) ;

   done :
      PD_TRACE_EXITRC ( SDB_CATMAINCT_INIT, rc ) ;
      return rc ;
   error :
      goto done ;
   }

   INT32 catMainController::fini ()
   {
      // Check the pointer in case that it is not initialized
      // before unregister the handler
      if ( _pCatCB )
      {
         if ( NET_INVALID_TIMER_ID != _checkEventTimerID )
         {
            _pCatCB->killTimer( _checkEventTimerID ) ;
            _checkEventTimerID = NET_INVALID_TIMER_ID ;
         }
         _pCatCB->unregEventHandler( this ) ;
      }
      return SDB_OK ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB_CATMAINCT__CREATESYSIDX, "catMainController::_createSysIndex" )
   INT32 catMainController::_createSysIndex ( const CHAR *pCollection,
                                              const CHAR *pIndex,
                                              pmdEDUCB *cb )
   {
      INT32 rc = SDB_OK ;
      BSONObj indexDef ;
      PD_TRACE_ENTRY ( SDB_CATMAINCT__CREATESYSIDX ) ;
      PD_TRACE2 ( SDB_CATMAINCT__CREATESYSIDX,
                  PD_PACK_STRING ( pCollection ),
                  PD_PACK_STRING ( pIndex ) ) ;

      rc = fromjson ( pIndex, indexDef ) ;
      PD_RC_CHECK ( rc, PDERROR, "Failed to build index object, rc = %d",
                    rc ) ;

      rc = rtnTestAndCreateIndex( pCollection, indexDef, cb, _pDmsCB,
                                  NULL, TRUE ) ;
      if ( rc )
      {
         goto error ;
      }

   done :
      PD_TRACE_EXITRC ( SDB_CATMAINCT__CREATESYSIDX, rc ) ;
      return rc ;
   error :
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB_CATMAINCT__CREATESYSCOL, "catMainController::_createSysCollection" )
   INT32 catMainController::_createSysCollection ( const CHAR *pCollection,
                                                   utilCLUniqueID clUID,
                                                   pmdEDUCB *cb )
   {
      INT32 rc = SDB_OK ;
      PD_TRACE_ENTRY ( SDB_CATMAINCT__CREATESYSCOL ) ;
      PD_TRACE1 ( SDB_CATMAINCT__CREATESYSCOL,
                  PD_PACK_STRING ( pCollection ) ) ;

      rc = rtnTestAndCreateCL( pCollection, cb, _pDmsCB, NULL, clUID, TRUE ) ;
      if ( rc )
      {
         goto error ;
      }

   done :
      PD_TRACE_EXITRC ( SDB_CATMAINCT__CREATESYSCOL, rc ) ;
      return rc ;
   error :
      goto done ;
   }

   // NO NEED TO SYNC LOG FOR ANY CREATION
   // PD_TRACE_DECLARE_FUNCTION ( SDB_CATMAINCT__ENSUREMETADATA, "catMainController::_ensureMetadata" )
   INT32 catMainController::_ensureMetadata()
   {
      INT32 rc = SDB_OK ;
      pmdEDUCB *cb = pmdGetThreadEDUCB() ;
      PD_TRACE_ENTRY ( SDB_CATMAINCT__ENSUREMETADATA ) ;

      // create SYSCAT.SYSNODES
      rc = _createSysCollection( CAT_NODE_INFO_COLLECTION,
                                 SYS_CAT_NODE_CLUID, cb ) ;
      if ( rc )
      {
         goto error ;
      }
      rc = _createSysIndex ( CAT_NODE_INFO_COLLECTION,
                             CAT_NODEINFO_GROUPNAMEIDX, cb ) ;
      if ( rc )
      {
         goto error ;
      }
      rc = _createSysIndex ( CAT_NODE_INFO_COLLECTION,
                             CAT_NODEINFO_GROUPIDIDX, cb ) ;
      if ( rc )
      {
         goto error ;
      }

      // create SYSCAT.SYSCOLLECTIONSPACES
      rc = _createSysCollection ( CAT_COLLECTION_SPACE_COLLECTION,
                                  SYS_CAT_CS_CLUID, cb ) ;
      if ( rc )
      {
         goto error ;
      }
      rc = _createSysIndex ( CAT_COLLECTION_SPACE_COLLECTION,
                             CAT_COLLECTION_SPACE_NAMEIDX, cb ) ;
      if ( rc )
      {
         goto error ;
      }
      rc = _createSysIndex ( CAT_COLLECTION_SPACE_COLLECTION,
                             CAT_COLLECTION_SPACE_IDIDX, cb ) ;
      if ( rc )
      {
         goto error ;
      }

      // create SYSCAT.SYSCOLLECTIONS
      rc = _createSysCollection ( CAT_COLLECTION_INFO_COLLECTION,
                                  SYS_CAT_CL_CLUID, cb ) ;
      if ( rc )
      {
         goto error ;
      }
      rc = _createSysIndex ( CAT_COLLECTION_INFO_COLLECTION,
                             CAT_COLLECTION_NAMEIDX, cb ) ;
      if ( rc )
      {
         goto error ;
      }
      rc = _createSysIndex ( CAT_COLLECTION_INFO_COLLECTION,
                             CAT_COLLECTION_IDIDX, cb ) ;
      if ( rc )
      {
         goto error ;
      }
      rc = _createSysIndex ( CAT_COLLECTION_INFO_COLLECTION,
                             CAT_COLLECTION_MAINCLIDX, cb ) ;
      if ( rc )
      {
         goto error ;
      }

      // create SYSCAT.SYSTASKS
      rc = _createSysCollection ( CAT_TASK_INFO_COLLECTION,
                                  SYS_CAT_TASKS_CLUID, cb ) ;
      if ( rc )
      {
         goto error ;
      }
      rc = _createSysIndex ( CAT_TASK_INFO_COLLECTION,
                             CAT_TASK_INFO_TASKIDIDX, cb ) ;
      if ( rc )
      {
         goto error ;
      }
      rc = _createSysIndex ( CAT_TASK_INFO_COLLECTION,
                             CAT_TASK_INFO_MAINTASKIDIDX, cb ) ;
      if ( rc )
      {
         goto error ;
      }
      rc = _createSysIndex ( CAT_TASK_INFO_COLLECTION,
                             CAT_TASK_INFO_NAMEIDX, cb ) ;
      if ( rc )
      {
         goto error ;
      }

      // create SYSCAT.SYSINDEXES
      rc = _createSysCollection ( CAT_INDEX_INFO_COLLECTION,
                                  SYS_CAT_INDEXES_CLUID, cb ) ;
      if ( rc )
      {
         goto error ;
      }
      rc = _createSysIndex ( CAT_INDEX_INFO_COLLECTION,
                             CAT_INDEX_INFO_NAMEIDX, cb ) ;
      if ( rc )
      {
         goto error ;
      }

      // create SYSCAT.SYSDOMAINS
      rc = _createSysCollection ( CAT_DOMAIN_COLLECTION,
                                  SYS_CAT_DOMAINS_CLUID, cb ) ;
      if ( rc )
      {
         goto error ;
      }
      rc = _createSysIndex ( CAT_DOMAIN_COLLECTION,
                             CAT_DOMAIN_NAMEIDX, cb ) ;
      if ( rc )
      {
         goto error ;
      }

      // create SYSCAT.SYSHISTORY
      rc = _createSysCollection( CAT_HISTORY_COLLECTION,
                                 SYS_CAT_HISTORY_CLUID, cb ) ;
      if ( rc )
      {
         goto error ;
      }
      rc = _createSysIndex( CAT_HISTORY_COLLECTION, CAT_HISTORY_BUCKETID_IDX,
                            cb ) ;
      if ( rc )
      {
         goto error ;
      }

      /// procedures
      rc = _createSysCollection ( CAT_PROCEDURES_COLLECTION,
                                  SYS_PROCEDURES_CLUID, cb ) ;
      if ( rc )
      {
         goto error ;
      }
      rc = _createSysIndex ( CAT_PROCEDURES_COLLECTION,
                             CAT_PROCEDURES_COLLECTION_INDEX, cb ) ;
      if ( rc )
      {
         goto error ;
      }

      /// SYSINFO
      rc = _createSysCollection( CAT_SYSDCBASE_COLLECTION_NAME,
                                 SYS_INFO_DCBASE_CLUID, cb ) ;
      if ( rc )
      {
         goto error ;
      }
      rc = _createSysIndex( CAT_SYSDCBASE_COLLECTION_NAME,
                            CAT_DCBASEINFO_TYPE_INDEX, cb ) ;
      if ( rc )
      {
         goto error ;
      }

      /// SYSDATASOURCES
      rc = _createSysCollection( CAT_DATASOURCE_COLLECTION,
                                 SYS_CAT_DATASOURCES_CLUID, cb ) ;
      if ( rc )
      {
         goto error ;
      }
      rc = _createSysIndex( CAT_DATASOURCE_COLLECTION,
                            CAT_DATASOURCE_IDIDX, cb ) ;
      if ( rc )
      {
         goto error ;
      }
      rc = _createSysIndex( CAT_DATASOURCE_COLLECTION,
                            CAT_DATASOURCE_NAMEIDX, cb ) ;
      if ( rc )
      {
         goto error ;
      }

      /// SYSGROUPMODES
      rc = _createSysCollection( CAT_GROUP_MODE_COLLECTION,
                                 SYS_CAT_GROUPMODES_CLUID, cb ) ;
      if ( rc )
      {
         goto error ;
      }
      rc = _createSysIndex( CAT_GROUP_MODE_COLLECTION,
                            CAT_GROUP_MODE_GROUPID_IDX, cb ) ;
      if ( rc )
      {
         goto error ;
      }

      /// SYSLOG
      for ( UINT32 i = 0 ; i < CAT_SYSLOG_CL_NUM ; ++i )
      {
         CHAR clName[ DMS_COLLECTION_FULL_NAME_SZ + 1 ] = { 0 } ;
         ossSnprintf( clName, sizeof( clName ), "%s%d",
                      CAT_SYSLOG_COLLECTION_NAME, i ) ;
         rc = _createSysCollection( clName, SYS_INFO_LOG_CLUID[ i ], cb ) ;
         if ( rc )
         {
            goto error ;
         }
         rc = _createSysIndex( clName, CAT_SYSLOG_TYPE_LSNVER, cb ) ;
         if ( rc )
         {
            goto error ;
         }
         rc = _createSysIndex( clName, CAT_SYSLOG_TYPE_LSNOFF, cb ) ;
         if ( rc )
         {
            goto error ;
         }
      }

      // collection for recycle bin item
      rc = _createSysCollection( CAT_SYSRECYCLEBIN_ITEM_COLLECTION,
                                 SYS_RECYCLEBIN_ITEMS_CLUID, cb ) ;
      PD_RC_CHECK( rc, PDERROR, "Failed to create system collection [%s], "
                   "rc: %d", CAT_SYSRECYCLEBIN_ITEM_COLLECTION, rc ) ;

      // index for recycle name
      rc = _createSysIndex( CAT_SYSRECYCLEBIN_ITEM_COLLECTION,
                            UTIL_RECYCLEBIN_ITEM_NAME_INDEX, cb ) ;
      PD_RC_CHECK( rc, PDERROR, "Failed to create system index [%s] "
                   "on system collection [%s], rc: %d",
                   UTIL_RECYCLEBIN_ITEM_NAME_INDEX,
                   CAT_SYSRECYCLEBIN_ITEM_COLLECTION, rc ) ;

      // index for origin name
      rc = _createSysIndex( CAT_SYSRECYCLEBIN_ITEM_COLLECTION,
                            UTIL_RECYCLEBIN_ORIGNAME_INDEX, cb ) ;
      PD_RC_CHECK( rc, PDERROR, "Failed to create system index [%s] "
                   "on system collection [%s], rc: %d",
                   UTIL_RECYCLEBIN_ORIGNAME_INDEX,
                   CAT_SYSRECYCLEBIN_ITEM_COLLECTION, rc ) ;

      // index for recycle ID
      rc = _createSysIndex( CAT_SYSRECYCLEBIN_ITEM_COLLECTION,
                            UTIL_RECYCLEBIN_RECYID_INDEX, cb ) ;
      PD_RC_CHECK( rc, PDERROR, "Failed to create system index [%s] "
                   "on system collection [%s], rc: %d",
                   UTIL_RECYCLEBIN_RECYID_INDEX,
                   CAT_SYSRECYCLEBIN_ITEM_COLLECTION, rc ) ;

      // index for origin ID
      rc = _createSysIndex( CAT_SYSRECYCLEBIN_ITEM_COLLECTION,
                            UTIL_RECYCLEBIN_ORIGID_INDEX, cb ) ;
      PD_RC_CHECK( rc, PDERROR, "Failed to create system index [%s] "
                   "on system collection [%s], rc: %d",
                   UTIL_RECYCLEBIN_ORIGID_INDEX,
                   CAT_SYSRECYCLEBIN_ITEM_COLLECTION, rc ) ;

      // collection for recycled collection space
      rc = _createSysCollection( CAT_SYSRECYCLEBIN_CS_COLLECTION,
                                 SYS_RECYCLEBIN_CS_CLUID, cb ) ;
      PD_RC_CHECK( rc, PDERROR, "Failed to create system collection [%s], "
                   "rc: %d", CAT_SYSRECYCLEBIN_CS_COLLECTION, rc ) ;

      // index for recycle ID of recycled collection space
      rc = _createSysIndex( CAT_SYSRECYCLEBIN_CS_COLLECTION,
                            UTIL_RECYCLEBIN_RECYID_INDEX, cb ) ;
      PD_RC_CHECK( rc, PDERROR, "Failed to create system index [%s] "
                   "on system collection [%s], rc: %d",
                   UTIL_RECYCLEBIN_RECYID_INDEX,
                   CAT_SYSRECYCLEBIN_CS_COLLECTION, rc ) ;

      // collection for recycled collection
      rc = _createSysCollection( CAT_SYSRECYCLEBIN_CL_COLLECTION,
                                 SYS_RECYCLEBIN_CL_CLUID, cb ) ;
      PD_RC_CHECK( rc, PDERROR, "Failed to create system collection [%s], "
                   "rc: %d", CAT_SYSRECYCLEBIN_CL_COLLECTION, rc ) ;

      // index for recycle ID of recycled collection
      rc = _createSysIndex( CAT_SYSRECYCLEBIN_CL_COLLECTION,
                            UTIL_RECYCLEBIN_RECYID_INDEX, cb ) ;
      PD_RC_CHECK( rc, PDERROR, "Failed to create system index [%s] "
                   "on system collection [%s], rc: %d",
                   UTIL_RECYCLEBIN_RECYID_INDEX,
                   CAT_SYSRECYCLEBIN_CL_COLLECTION, rc ) ;

      // index for unique ID of recycled collection
      rc = _createSysIndex( CAT_SYSRECYCLEBIN_CL_COLLECTION,
                            UTIL_RECYCLEBIN_UID_INDEX, cb ) ;
      PD_RC_CHECK( rc, PDERROR, "Failed to create system index [%s] "
                   "on system collection [%s], rc: %d",
                   UTIL_RECYCLEBIN_UID_INDEX,
                   CAT_SYSRECYCLEBIN_CL_COLLECTION, rc ) ;

      // collection for recycled sequence
      rc = _createSysCollection( CAT_SYSRECYCLEBIN_SEQ_COLLECTION,
                                 SYS_RECYCLEBIN_SEQ_CLUID, cb ) ;
      PD_RC_CHECK( rc, PDERROR, "Failed to create system collection [%s], "
                   "rc: %d", CAT_SYSRECYCLEBIN_SEQ_COLLECTION, rc ) ;

      // index for recycle ID of recycled sequence
      rc = _createSysIndex( CAT_SYSRECYCLEBIN_SEQ_COLLECTION,
                            UTIL_RECYCLEBIN_RECYID_INDEX, cb ) ;
      PD_RC_CHECK( rc, PDERROR, "Failed to create system index [%s] "
                   "on system collection [%s], rc: %d",
                   UTIL_RECYCLEBIN_RECYID_INDEX,
                   CAT_SYSRECYCLEBIN_SEQ_COLLECTION, rc ) ;

      // collection for recycled index
      rc = _createSysCollection( CAT_SYSRECYCLEBIN_IDX_COLLECTION,
                                 SYS_RECYCLEBIN_INDEXES_CLUID, cb ) ;
      PD_RC_CHECK( rc, PDERROR, "Failed to create system collection [%s], "
                   "rc: %d", CAT_SYSRECYCLEBIN_IDX_COLLECTION, rc ) ;

      // index for recycle ID of recycled index
      rc = _createSysIndex( CAT_SYSRECYCLEBIN_IDX_COLLECTION,
                            UTIL_RECYCLEBIN_RECYID_INDEX, cb ) ;
      PD_RC_CHECK( rc, PDERROR, "Failed to create system index [%s] "
                   "on system collection [%s], rc: %d",
                   UTIL_RECYCLEBIN_RECYID_INDEX,
                   CAT_SYSRECYCLEBIN_IDX_COLLECTION, rc ) ;

   done :
      PD_TRACE_EXITRC ( SDB_CATMAINCT__ENSUREMETADATA, rc ) ;
      return rc ;
   error :
      goto done ;
   }

   // when we activate the main controller, we should always assume there's
   // metadata collections exist
   // PD_TRACE_DECLARE_FUNCTION ( SDB_CATMAINCT_ACTIVE, "catMainController::_onActiveEvent" )
   INT32 catMainController::_onActiveEvent( pmdEDUEvent *event )
   {
      INT32 rc = SDB_OK ;
      PD_TRACE_ENTRY ( SDB_CATMAINCT_ACTIVE ) ;

      rc = _pCatCB->getCatGTSMgr()->active() ;
      PD_RC_CHECK( rc, PDERROR, "Failed to active catalog GTS manager, rc: %d",
                   rc ) ;

      rc = _pCatCB->getCatNodeMgr()->active() ;
      PD_RC_CHECK( rc, PDERROR, "Failed to active catalog node manager, rc: %d",
                   rc ) ;

      rc = _pCatCB->getCatDCMgr()->active() ;
      PD_RC_CHECK( rc, PDERROR, "Failed to active cata dc manager, rc: %d",
                   rc ) ;

      rc = _pCatCB->getCatlogueMgr()->active() ;
      PD_RC_CHECK( rc, PDERROR, "Failed to active catalog manager, rc: %d",
                   rc ) ;

      rc = _pCatCB->getRecycleBinMgr()->active(
            _pCatCB->getCatDCMgr()->getDCInfo()->getRecycleBinConf() ) ;
      PD_RC_CHECK( rc, PDERROR, "Failed to active recycle bin manager, "
                   "rc: %d", rc ) ;

      rc = _pAuthCB->getRoleManager()->active();
      PD_RC_CHECK( rc, PDERROR, "Failed to active role manager, rc: %d", rc ) ;

      // all managers are active, now we can perform upgrade
      rc = _pCatCB->checkUpgrade() ;
      PD_RC_CHECK( rc, PDERROR, "Failed to upgrade CATALOG, rc: %d", rc ) ;

   done:
      _changeEvent.signal() ;
      PD_TRACE_EXITRC ( SDB_CATMAINCT_ACTIVE, rc ) ;
      return rc ;
   error:
      PMD_RESTART_DB( rc ) ;
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB_CATMAINCT_DEACTIVE, "catMainController::_onDeactiveEvent" )
   INT32 catMainController::_onDeactiveEvent( pmdEDUEvent *event )
   {
      INT32 rc = SDB_OK ;
      PD_TRACE_ENTRY ( SDB_CATMAINCT_DEACTIVE ) ;

      _pCatCB->getCatDCMgr()->deactive() ;
      _pCatCB->getCatNodeMgr()->deactive() ;
      _pCatCB->getCatlogueMgr()->deactive() ;
      _pCatCB->getCatGTSMgr()->deactive() ;
      _pAuthCB->getRoleManager()->deactive() ;

      _changeEvent.signal() ;

      PD_TRACE_EXITRC ( SDB_CATMAINCT_DEACTIVE, rc ) ;
      return rc ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB_CATMAINCT_BUILDMSGEVENT, "catMainController::_catBuildMsgEvent" )
   INT32 catMainController::_catBuildMsgEvent ( const NET_HANDLE &handle,
                                                const MsgHeader *pMsg,
                                                pmdEDUEvent &event )
   {
      INT32 rc = SDB_OK ;
      CHAR *pEventData = NULL ;
      PD_TRACE_ENTRY ( SDB_CATMAINCT_BUILDMSGEVENT ) ;

      pEventData = (CHAR *)SDB_THREAD_ALLOC( pMsg->messageLength ) ;
      if ( NULL == pEventData )
      {
         rc = SDB_OOM ;
         PD_LOG ( PDERROR, "malloc failed(size = %d)", pMsg->messageLength ) ;
         goto error ;
      }
      ossMemcpy( (void *)pEventData, pMsg, pMsg->messageLength ) ;

      event._Data = pEventData ;
      event._dataMemType = PMD_EDU_MEM_THREAD ;
      event._eventType = PMD_EDU_EVENT_MSG ;
      event._userData = (UINT64)handle ;

   done :
      PD_TRACE_EXITRC ( SDB_CATMAINCT_BUILDMSGEVENT, rc ) ;
      return rc ;
   error :
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB_CATMAINCT_GETMOREMSG, "catMainController::_processGetMoreMsg" )
   INT32 catMainController::_processGetMoreMsg ( const NET_HANDLE &handle,
                                                 MsgHeader *pMsg )
   {
      INT32 rc               = SDB_OK ;
      PD_TRACE_ENTRY ( SDB_CATMAINCT_GETMOREMSG ) ;

      MsgOpGetMore *pGetMore = (MsgOpGetMore*)pMsg ;
      rtnContextBuf buffObj ;
      SINT32 msgLen          = 0 ;

      MsgOpReply reply ;
      MsgOpReply *pReply     = NULL ;

      rtnContextPtr pContext ;
      BOOLEAN bIsDelay = FALSE ;
      BOOLEAN rtnDel = FALSE ;

      rc = _pRtnCB->contextFind ( pGetMore->contextID, pContext, _pEDUCB ) ;
      if ( rc )
      {
         PD_LOG ( PDERROR, "Context %lld does not exist, rc: %d",
                  pGetMore->contextID, rc ) ;
         goto error ;
      }

      if ( pContext->isWrite() )
      {
         rc = _pCatCB->primaryCheck( _pEDUCB, FALSE, bIsDelay ) ;
         if ( rc )
         {
            rtnDel = TRUE ;
            PD_LOG( PDERROR, "Failed to check primary, rc: %d", rc );
            goto error ;
         }
      }

      rc = rtnGetMore( pContext, pGetMore->numToReturn, buffObj,
                       _pEDUCB, _pRtnCB ) ;
      if ( rc )
      {
         if ( SDB_DMS_EOC != rc )
         {
            PD_LOG ( PDERROR, "Failed to get more, rc: %d", rc ) ;
         }
         goto error ;
      }

   done:
      {
         msgLen =  sizeof(MsgOpReply) + buffObj.size() ;

         // free by end of function
         pReply = (MsgOpReply *)SDB_THREAD_ALLOC( msgLen ) ;
         if ( NULL == pReply )
         {
            PD_LOG ( PDERROR, "Malloc error( size = %d )", msgLen ) ;
            rc = SDB_OOM ;
            pReply = &reply ;
            msgLen = sizeof( reply ) ;
            buffObj.release() ;
         }

         pReply->header.messageLength = msgLen ;
         pReply->header.opCode        = MSG_BS_GETMORE_RES ;
         pReply->header.TID           = pGetMore->header.TID ;
         pReply->header.routeID.value = 0 ;
         pReply->header.requestID     = pGetMore->header.requestID ;
         pReply->header.globalID      = pGetMore->header.globalID ;
         pReply->contextID            = pGetMore->contextID ;
         pReply->startFrom            = (INT32)buffObj.getStartFrom() ;
         pReply->numReturned          = buffObj.recordNum() ;
         pReply->flags                = rc ;
         /// copy data
         if ( buffObj.size() > 0 )
         {
            ossMemcpy( (CHAR *)pReply + sizeof(MsgOpReply), buffObj.data(),
                       buffObj.size() ) ;
         }

         /// send result
         rc = _pCatCB->sendReply( handle, pReply, rc ) ;
         if ( rc )
         {
            PD_LOG ( PDERROR, "Failed to syncSend, rc = %d", rc ) ;
         }

         if ( pReply && pReply != &reply )
         {
            SDB_THREAD_FREE( pReply ) ;
         }
      }
      PD_TRACE_EXITRC ( SDB_CATMAINCT_GETMOREMSG, rc ) ;
      return rc ;
   error:
      delContextByID( pGetMore->contextID, rtnDel ) ;
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB_CATMAINCT_ADVANCEMSG, "catMainController::_processAdvanceMsg" )
   INT32 catMainController::_processAdvanceMsg( const NET_HANDLE &handle,
                                                MsgHeader * pMsg)
   {
      INT32 rc               = SDB_OK ;
      PD_TRACE_ENTRY ( SDB_CATMAINCT_ADVANCEMSG ) ;

      INT64 contextID        = -1 ;
      const CHAR *pOption    = NULL ;
      const CHAR *pBackData  = NULL ;
      INT32 backDataSize     = 0 ;
      MsgOpReply reply ;

      // fill reply
      reply.header.messageLength   = sizeof( MsgOpReply ) ;
      reply.header.opCode          = MSG_BS_ADVANCE_RES ;
      reply.header.TID             = pMsg->TID ;
      reply.header.routeID.value   = 0 ;
      reply.header.requestID       = pMsg->requestID ;
      reply.header.globalID        = pMsg->globalID ;
      reply.contextID              = contextID ;
      reply.startFrom              = 0 ;
      reply.numReturned            = 0 ;

      // extrace msg
      rc = msgExtractAdvanceMsg( (const CHAR *)pMsg, &contextID,
                                 &pOption, &pBackData, &backDataSize ) ;
      if ( rc )
      {
         PD_LOG( PDERROR, "Extrace advance message failed, rc: %d", rc ) ;
         goto error ;
      }

      // send the reply whether successful or not
      try
      {
         BSONObj option( pOption ) ;
         rc = rtnAdvance( contextID, option, pBackData, backDataSize,
                          _pEDUCB, _pRtnCB ) ;
         if ( rc )
         {
            goto error ;
         }
      }
      catch( std::exception &e )
      {
         PD_LOG( PDERROR, "Occur exception: %s", e.what() ) ;
         rc = SDB_INVALIDARG ;
         goto error ;
      }

   done :
      reply.flags = rc ;
      rc = _pCatCB->sendReply( handle, &reply, rc ) ;
      if ( rc )
      {
         PD_LOG ( PDERROR, "Failed to syncSend, rc: %d", rc ) ;
      }

      PD_TRACE_EXITRC ( SDB_CATMAINCT_ADVANCEMSG, rc ) ;
      return rc ;
   error :
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB_CATMAINCT_KILLCONTEXT, "catMainController::_processKillContext" )
   INT32 catMainController::_processKillContext( const NET_HANDLE &handle,
                                                 MsgHeader *pMsg )
   {
      INT32 rc = SDB_OK ;
      INT32 contextNum = 0 ;
      const INT64 *pContextIDs = NULL ;
      MsgOpReply msgReply;
      MsgOpKillContexts *pReq = (MsgOpKillContexts *)pMsg;
      msgReply.contextID = -1;
      msgReply.flags = SDB_OK;
      msgReply.numReturned = 0;
      msgReply.startFrom = 0;
      msgReply.header.messageLength = sizeof(MsgOpReply);
      msgReply.header.opCode = MSG_BS_KILL_CONTEXT_RES;
      msgReply.header.requestID = pReq->header.requestID;
      msgReply.header.routeID.value = 0;
      msgReply.header.TID = pReq->header.TID;
      msgReply.header.globalID = pReq->header.globalID ;

      PD_TRACE_ENTRY ( SDB_CATMAINCT_KILLCONTEXT ) ;
      do
      {
         rc = msgExtractKillContexts ( (const CHAR *)pMsg,
                                       &contextNum, &pContextIDs ) ;
         if ( SDB_OK != rc )
         {
            PD_LOG ( PDERROR, "Failed to parse the killcontexts request, "
                     "rc: %d", rc ) ;
            break;
         }

         if ( contextNum > 0 )
         {
            PD_LOG ( PDDEBUG,
                     "KillContext: contextNum: %d contextID: %lld",
                     contextNum, pContextIDs[0] ) ;
         }

         for ( INT32 i = 0 ; i < contextNum ; ++i )
         {
            PD_LOG( PDDEBUG,
                    "Kill context %lld",
                    pContextIDs[i] ) ;
            delContextByID( pContextIDs[ i ], TRUE ) ;
         }
      }while ( FALSE ) ;
      msgReply.flags = rc;
      PD_TRACE1 ( SDB_CATMAINCT_KILLCONTEXT,
                  PD_PACK_INT ( rc ) ) ;
      rc = _pCatCB->sendReply( handle, &msgReply, rc );
      if ( rc != SDB_OK )
      {
         PD_LOG ( PDERROR, "Failed to send the message "
                  "( groupID=%d, nodeID=%d, serviceID=%d )",
                  pReq->header.routeID.columns.groupID,
                  pReq->header.routeID.columns.nodeID,
                  pReq->header.routeID.columns.serviceID );
      }
      PD_TRACE_EXITRC ( SDB_CATMAINCT_KILLCONTEXT, rc ) ;
      return rc;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB_CATMAINCT_REMOTEDISC, "catMainController::_processRemoteDisc" )
   INT32 catMainController::_processRemoteDisc( const NET_HANDLE &handle,
                                                MsgHeader *pMsg )
   {
      INT32 rc = SDB_OK ;
      PD_TRACE_ENTRY ( SDB_CATMAINCT_REMOTEDISC ) ;
      PD_LOG ( PDDEBUG, "Killing handle contexts %u", handle ) ;
      _delContextByHandle( handle ) ;
      _deleteDelayedOperation( handle ) ;
      PD_TRACE_EXITRC ( SDB_CATMAINCT_REMOTEDISC, rc ) ;
      return rc ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB_CATMAINCT_PRCSCMD, "catMainController::_processCommand" )
   INT32 catMainController::_processCommand( const NET_HANDLE &handle,
                                             MsgHeader *pMsg )
   {
      PD_TRACE_ENTRY ( SDB_CATMAINCT_PRCSCMD ) ;
      INT32 rc             = SDB_OK ;
      SINT32 flags         = 0 ;
      SINT64 numToSkip     = 0 ;
      SINT64 numToReturn   = -1 ;
      const CHAR *pName    = NULL ;
      const CHAR *pQuery   = NULL ;
      const CHAR *pSelector= NULL ;
      const CHAR *pOrderBy = NULL ;
      const CHAR *pHint    = NULL ;
      catCMDBase *pCommand = NULL ;
      rtnContextBuf ctxBuff ;
      INT64 contextID = -1 ;

      // extract message
      rc = msgExtractQuery ( (const CHAR *)pMsg, &flags, &pName,
                             &numToSkip, &numToReturn, &pQuery,
                             &pSelector, &pOrderBy, &pHint ) ;
      PD_RC_CHECK ( rc, PDERROR,
                    "Failed to extract query message, rc: %d",
                    rc ) ;

      if ( !rtnIsCommand( pName ) )
      {
         rc = SDB_INVALIDARG ;
         PD_LOG( PDERROR, "%s is not command", pName ) ;
         goto error ;
      }

      // build command
      rc = getCatCmdBuilder()->create( &pName[1], pCommand ) ;
      PD_RC_CHECK ( rc, PDERROR,
                    "Failed to create command[%s], rc: %d",
                    pName, rc ) ;

      // primary check
      if ( pCommand->needCheckPrimary() )
      {
         BOOLEAN isDelay = FALSE ;
         rc = _pCatCB->primaryCheck( _pEDUCB, TRUE, isDelay ) ;
         if ( isDelay )
         {
            goto done ;
         }
         PD_RC_CHECK( rc, PDWARNING, "Service deactive "
                      "but received command: %s, opCode: %d, rc: %d",
                      pCommand->name(), pMsg->opCode, rc ) ;
      }

      if ( pCommand->needCheckDCStatus() && _pCatCB->isDCReadonly() )
      {
         rc = SDB_CAT_CLUSTER_IS_READONLY ;
         goto error ;
      }

      // do command
      rc = pCommand->init( pQuery, pSelector, pOrderBy, pHint,
                           flags, numToSkip, numToReturn ) ;
      PD_RC_CHECK ( rc, PDERROR,
                    "Failed to init command[%s], rc: %d",
                    pCommand->name(), rc ) ;
      _pEDUCB->setCurProcessName( pCommand->getProcessName() ) ;

      rc = pCommand->doit( _pEDUCB, ctxBuff, contextID ) ;
      if ( SDB_LOCK_FAILED == rc )
      {
         SDB_ASSERT( -1 == contextID, "ContextID must be -1" ) ;
         goto lock_failed ;
      }
      PD_RC_CHECK ( rc, PDERROR,
                    "Failed to do command[%s], rc: %d",
                    pCommand->name(), rc ) ;

      addContext( handle, pMsg->TID, contextID ) ;

   done:
      if ( !_pCatCB->isDelayed() )
      {
         // send reply
         MsgOpReply replyHeader ;
         replyHeader.header.messageLength = sizeof( MsgOpReply ) ;
         replyHeader.header.opCode = MAKE_REPLY_TYPE( pMsg->opCode ) ;
         replyHeader.header.TID = pMsg->TID ;
         replyHeader.header.routeID.value = 0 ;
         replyHeader.header.requestID = pMsg->requestID ;
         replyHeader.header.globalID = pMsg->globalID ;
         replyHeader.contextID = contextID ;
         replyHeader.flags = rc ;
         replyHeader.startFrom = 0 ;
         replyHeader.numReturned = 0 ;
         if( SDB_CLS_NOT_PRIMARY == rc )
         {
            replyHeader.startFrom = _pCatCB->getPrimaryNode() ;
         }
         if ( 0 == ctxBuff.size() )
         {
            rc = _pCatCB->sendReply( handle, &replyHeader, rc ) ;
         }
         else
         {
            replyHeader.header.messageLength += ctxBuff.size() ;
            replyHeader.numReturned = ctxBuff.recordNum() ;
            rc = _pCatCB->sendReply( handle, &replyHeader, rc,
                                     (void *)ctxBuff.data(), ctxBuff.size() ) ;
         }
      }
      if ( pCommand )
      {
         getCatCmdBuilder()->release( pCommand ) ;
      }
      PD_TRACE_EXITRC ( SDB_CATMAINCT_PRCSCMD, rc ) ;
      return rc ;

   lock_failed:
      // Lock failed, then try to delay operation
      if ( !delayCurOperation() )
      {
         rc = SDB_LOCK_FAILED ;
         goto error ;
      }
      else
      {
         // Ignore the lock error
         rc = SDB_OK ;
      }
      goto done ;

   error:
      if ( -1 != contextID )
      {
         catDeleteContext( contextID, _pEDUCB ) ;
         contextID = -1 ;
      }
      goto done ;
   }

   INT32 catMainController::_processQueryMsg( const NET_HANDLE &handle,
                                              MsgHeader *pMsg )
   {
      INT32 rc = SDB_OK ;

      const CHAR *pCollectionName = ((MsgOpQuery*)pMsg)->name ;

      if ( rtnIsCommand( pCollectionName ) )
      {
         rc = _processCommand( handle, pMsg ) ;
      }
      else
      {
         rc = _processQueryRequest( handle, pMsg, NULL ) ;
      }

      return rc ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB_CATMAINCT_QUERYREQUEST, "catMainController::_processQueryRequest" )
   INT32 catMainController::_processQueryRequest ( const NET_HANDLE &handle,
                                                   MsgHeader *pMsg,
                                                   const CHAR *pCollectionName )
   {
      INT32 rc              = SDB_OK ;
      MsgOpReply msgReply ;
      MsgHeader *pMsgHeader = (MsgHeader *)( pMsg ) ;
      SINT64 contextID      = 0 ;
      SINT32 flags          = 0 ;
      SINT64 numToSkip      = -1 ;
      SINT64 numToReturn    = -1 ;
      const CHAR *pCN       = NULL ;
      const CHAR *pQuery    = NULL ;
      const CHAR *pFieldSelector  = NULL ;
      const CHAR *pOrderByBuffer  = NULL ;
      const CHAR *pHintBuffer     = NULL ;
      rtnContextBuf buffObj ;
      BOOLEAN bIsDelay      = FALSE ;

      PD_TRACE_ENTRY ( SDB_CATMAINCT_QUERYREQUEST ) ;

      rc = msgExtractQuery ( (const CHAR *)pMsgHeader, &flags, &pCN,
                             &numToSkip, &numToReturn, &pQuery,
                             &pFieldSelector, &pOrderByBuffer,
                             &pHintBuffer ) ;
      if ( rc != SDB_OK )
      {
         PD_LOG ( PDERROR, "Failed to read query packet, rc = %d", rc ) ;
         rc = SDB_INVALIDARG ;
         goto error ;
      }

      if ( NULL == pCollectionName )
      {
         pCollectionName = pCN ;
      }

      // check primary
      rc = _pCatCB->primaryCheck( _pEDUCB, TRUE, bIsDelay ) ;
      if ( bIsDelay )
      {
         goto done ;
      }
      else if ( rc )
      {
         PD_LOG( PDWARNING, "it is not primary node but received query "
                 "request, rc: %d", rc );
         goto error ;
      }

      try
      {
         BSONObj matcher ( pQuery ) ;
         BSONObj selector ( pFieldSelector ) ;
         BSONObj orderBy ( pOrderByBuffer ) ;
         BSONObj hint ( pHintBuffer ) ;
         rc = rtnQuery( pCollectionName, selector, matcher, orderBy,
                        hint, flags, _pEDUCB, numToSkip, numToReturn,
                        _pDmsCB, _pRtnCB, contextID ) ;
         if ( rc != SDB_OK )
         {
            if ( rc != SDB_DMS_EOC )
            {
               PD_LOG ( PDERROR, "Failed to query on collection[%s], rc: %d",
                        pCollectionName, rc ) ;
            }
            goto error ;
         }
         else if ( flags & FLG_QUERY_WITH_RETURNDATA )
         {
            rtnContextDump contextDump( 0, _pEDUCB->getID() ) ;
            rc = contextDump.open( BSONObj(), BSONObj(), -1, 0 ) ;
            PD_RC_CHECK( rc, PDERROR, "Open dump context failed, rc: %d",
                         rc ) ;

            while ( TRUE )
            {
               rc = rtnGetMore( contextID, -1, buffObj, _pEDUCB, _pRtnCB ) ;
               if ( rc )
               {
                  contextID = -1 ;
                  if ( SDB_DMS_EOC != rc )
                  {
                     PD_LOG( PDERROR, "Get more failed, rc: %d", rc ) ;
                     goto error ;
                  }
                  rc = SDB_OK ;
                  break ;
               }
               // add to dump context
               rc = contextDump.appendObjs( buffObj.data(), buffObj.size(),
                                            buffObj.recordNum(), TRUE ) ;
               PD_RC_CHECK( rc, PDERROR, "Append objs to dump context failed, "
                            "rc: %d", rc ) ;
               /// check result buffer
               if ( contextDump.buffEndOffset() >= (INT32)DMS_RECORD_MAX_SZ )
               {
                  break ;
               }
            }

            rc = contextDump.getMore( -1, buffObj, _pEDUCB ) ;
            if ( SDB_DMS_EOC == rc )
            {
               rc = SDB_OK ;
            }
            PD_RC_CHECK( rc, PDERROR, "Get more from dump context failed, "
                         "rc: %d", rc ) ;
         }
      }
      catch ( std::exception &e )
      {
         PD_LOG ( PDERROR, "Failed to create arg1 and arg2 for command: %s",
                  e.what() ) ;
         rc = SDB_INVALIDARG ;
         goto error ;
      }

   done :
      if ( !isDelayed() )
      {
         msgReply.header.messageLength = sizeof( MsgOpReply ) +
                                         buffObj.size() ;
         msgReply.header.opCode = MAKE_REPLY_TYPE( pMsgHeader->opCode );
         msgReply.header.TID = pMsgHeader->TID;
         msgReply.header.routeID.value = 0;
         msgReply.header.requestID = pMsgHeader->requestID;
         msgReply.header.globalID = pMsgHeader->globalID ;
         msgReply.contextID = contextID ;
         msgReply.startFrom = (INT32)buffObj.getStartFrom() ;
         msgReply.numReturned = buffObj.recordNum() ;

         if ( rc != SDB_OK )
         {
            msgReply.flags = rc ;
            if ( SDB_PERM == rc )
            {
               msgReply.flags = SDB_CLS_NOT_PRIMARY ;
            }
            else if ( SDB_CLS_NOT_PRIMARY == rc )
            {
               msgReply.startFrom = _pCatCB->getPrimaryNode() ;
            }
         }
         else
         {
            addContext( handle, pMsgHeader->TID, contextID );
            msgReply.flags = SDB_OK ;
         }
         PD_TRACE1 ( SDB_CATMAINCT_QUERYREQUEST,
                     PD_PACK_INT ( msgReply.flags ) ) ;

         if ( 0 == buffObj.size() )
         {
            rc = _pCatCB->sendReply( handle, &msgReply, rc ) ;
         }
         else
         {
            rc = _pCatCB->sendReply( handle, &msgReply, rc,
                                     (void *)buffObj.data(),
                                     (UINT32)buffObj.size() ) ;
         }
         if ( rc != SDB_OK )
         {
            PD_LOG ( PDERROR, "failed to send the message(routeID=%lld)",
                     pMsgHeader->routeID.value ) ;
         }
      }
      PD_TRACE_EXITRC ( SDB_CATMAINCT_QUERYREQUEST, rc ) ;
      return rc ;
   error :
      if ( -1 != contextID )
      {
         _pRtnCB->contextDelete( contextID, _pEDUCB ) ;
         contextID = -1 ;
      }
      goto done ;
   }

   INT32 catMainController::_defaultMsgFunc( NET_HANDLE handle,
                                             MsgHeader * msg )
   {
      INT32 rc = SDB_OK ;

      _isDelayed = FALSE ;

      _pEDUCB->clearProcessInfo() ;

      if ( MSG_PACKET == msg->opCode )
      {
         rc = _processPacketMsg( handle, msg ) ;
      }
      else
      {
         _pCatCB->onBeginCommand( msg ) ;

         if ( MSG_CAT_CATALOGUE_BEGIN < (UINT32)msg->opCode &&
              (UINT32)msg->opCode < MSG_CAT_CATALOGUE_END )
         {
            rc = _pCatCB->getCatlogueMgr()->processMsg( handle, msg ) ;
         }
         else if  ( MSG_CAT_NODE_BEGIN < (UINT32)msg->opCode &&
                    (UINT32)msg->opCode < MSG_CAT_NODE_END )
         {
            rc = _pCatCB->getCatNodeMgr()->processMsg( handle, msg ) ;
         }
         else if ( MSG_CAT_DC_BEGIN < (UINT32)msg->opCode &&
                   (UINT32)msg->opCode < MSG_CAT_DC_END )
         {
            rc = _pCatCB->getCatDCMgr()->processMsg( handle, msg ) ;
         }
         else
         {
            rc = _processMsg( handle, msg ) ;
         }

         _pCatCB->onEndCommand( msg, rc ) ;
      }

      _pEDUCB->clearProcessInfo() ;

      return rc ;
   }

   INT32 catMainController::_processMsg( const NET_HANDLE &handle,
                                         MsgHeader *pMsg )
   {
      INT32 rc = SDB_OK ;

      switch ( pMsg->opCode )
      {
      case MSG_BS_QUERY_REQ:
         {
            rc = _processQueryMsg( handle, pMsg );
            break;
         }
      case MSG_BS_GETMORE_REQ :
         {
            rc = _processGetMoreMsg( handle, pMsg ) ;
            break ;
         }
      case MSG_BS_ADVANCE_REQ :
         {
            rc = _processAdvanceMsg( handle, pMsg ) ;
            break ;
         }
      case MSG_BS_KILL_CONTEXT_REQ:
         {
            rc = _processKillContext( handle, pMsg ) ;
            break;
         }
      case MSG_BS_INTERRUPTE :
         {
            rc = _processInterruptMsg( handle, pMsg ) ;
            break ;
         }
      case MSG_BS_INTERRUPTE_SELF :
         {
            rc = SDB_OK ;
            break ;
         }
      case MSG_BS_DISCONNECT :
         {
            rc = _processDisconnectMsg( handle, pMsg ) ;
            break ;
         }
      case MSG_AUTH_VERIFY_REQ :
      case MSG_AUTH_VERIFY1_REQ :
         {
            rc = _processAuthenticate( handle, pMsg ) ;
            break ;
         }
      case MSG_AUTH_CRTUSR_REQ :
         {
            _pCatCB->getCatDCMgr()->setWritedCommand( TRUE ) ;
            rc = _processAuthCrt( handle, pMsg ) ;
            break ;
         }
      case MSG_AUTH_DELUSR_REQ :
         {
            _pCatCB->getCatDCMgr()->setWritedCommand( TRUE ) ;
            rc = _processAuthDel( handle, pMsg ) ;
            break ;
         }
      case MSG_COM_SESSION_INIT_REQ :
         {
            rc = _processSessionInit( handle, pMsg ) ;
            break;
         }
      case MSG_COM_REMOTE_DISC :
         {
            rc = _processRemoteDisc( handle, pMsg ) ;
            break ;
         }
      case CAT_DELAY_EVENT_TYPE :
         {
            // Only process the event is already delayed
            if ( _lastDelayEvent._Data )
            {
               rc = _processDelayReply( handle, pMsg ) ;
               break ;
            }
         }
      default :
         {
            PD_LOG( PDERROR, "Receive unknown msg[opCode:(%d)%d, len: %d, "
                    "tid: %d, reqID: %lld, nodeID: %u.%u.%u]",
                    IS_REPLY_TYPE(pMsg->opCode), GET_REQUEST_TYPE(pMsg->opCode),
                    pMsg->messageLength, pMsg->TID, pMsg->requestID,
                    pMsg->routeID.columns.groupID, pMsg->routeID.columns.nodeID,
                    pMsg->routeID.columns.serviceID ) ;
            rc = SDB_UNKNOWN_MESSAGE ;

            MsgOpReply reply ;
            reply.header.opCode = MAKE_REPLY_TYPE( pMsg->opCode ) ;
            reply.header.messageLength = sizeof( MsgOpReply ) ;
            reply.header.requestID = pMsg->requestID ;
            reply.header.routeID.value = 0 ;
            reply.header.TID = pMsg->TID ;
            reply.header.globalID = pMsg->globalID ;
            reply.flags = rc ;
            reply.contextID = -1 ;
            reply.numReturned = 1 ;
            reply.startFrom = 0 ;

            _pCatCB->sendReply( handle, &reply, rc ) ;
            break ;
         }
      }

      if ( rc && SDB_UNKNOWN_MESSAGE != rc )
      {
         PD_LOG( PDWARNING, "Process msg[opCode:(%d)%d, len: %d, tid: %d, "
                 "reqID: %lld, nodeID: %u.%u.%u] failed, rc: %d",
                 IS_REPLY_TYPE(pMsg->opCode), GET_REQUEST_TYPE(pMsg->opCode),
                 pMsg->messageLength, pMsg->TID, pMsg->requestID,
                 pMsg->routeID.columns.groupID, pMsg->routeID.columns.nodeID,
                 pMsg->routeID.columns.serviceID, rc ) ;
      }

      return rc ;
   }

   INT32 catMainController::_processPacketMsg( const NET_HANDLE &handle,
                                               MsgHeader *pMsg )
   {
      INT32 rc = SDB_OK ;
      INT32 pos = 0 ;
      MsgHeader *pTmpMsg = NULL ;
      MsgOpReply reply ;
      BOOLEAN hasDec = FALSE ;

      /// init reply
      reply.contextID               = -1;
      reply.numReturned             = 0;
      reply.startFrom               = 0;
      reply.header.messageLength    = sizeof( MsgOpReply ) ;
      reply.header.opCode           = MAKE_REPLY_TYPE(pMsg->opCode) ;
      reply.header.requestID        = pMsg->requestID;
      reply.header.routeID.value    = 0 ;
      reply.header.TID              = pMsg->TID ;
      reply.header.globalID         = pMsg->globalID ;

      _pCatCB->incPacketLevel() ;

      pos += sizeof( MsgHeader ) ;
      while( pos < pMsg->messageLength )
      {
         pTmpMsg = ( MsgHeader* )( ( CHAR*)pMsg + pos ) ;

         // copy route ID
         pTmpMsg->routeID.value = pMsg->routeID.value ;

         pos += pTmpMsg->messageLength ;
         reply.header.opCode = MAKE_REPLY_TYPE(pTmpMsg->opCode) ;

         // for GTS message in packet message, post it to GTS manager
         if ( ( MSG_GTS_BEGIN < pTmpMsg->opCode ) &&
              ( pTmpMsg->opCode < MSG_GTS_END ) )
         {
            if ( pos >= pMsg->messageLength )
            {
               // post the message to GTS manager
               rc = _pCatCB->getCatGTSMgr()->handleMsg( handle, pTmpMsg ) ;
               if ( SDB_OK != rc )
               {
                  // failed to post the message, don't decrease the packet level,
                  // let us send the error message back
                  PD_LOG( PDERROR, "Failed to post GTS message, "
                          "rc: %d", rc ) ;
               }
               else
               {
                  // processed by GTS manager, no need to send the reply
                  _pCatCB->decPacketLevel() ;
                  hasDec = TRUE ;
               }
            }
            else
            {
               // if the GTS message is in the middle, return unknown message
               // to peer, peer node will break the packet itself and retry
               PD_LOG( PDWARNING, "Failed to handle packet message with GTS "
                       "message in the middle" ) ;
               rc = SDB_UNKNOWN_MESSAGE ;
            }
            break ;
         }

         /// Is the last
         if ( pos >= pMsg->messageLength )
         {
            _pCatCB->decPacketLevel() ;
            hasDec = TRUE ;
         }

         rc = _defaultMsgFunc( handle, pTmpMsg ) ;
         if ( rc )
         {
            break ;
         }
      }

      if ( !hasDec )
      {
         _pCatCB->decPacketLevel() ;

         reply.flags = rc ;
         _pCatCB->sendReply( handle, &reply, rc ) ;
      }

      return rc ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB_CATMAINCT_AUTHCRT, "catMainController::_processAuthCrt" )
   INT32 catMainController::_processAuthCrt( const NET_HANDLE &handle,
                                             MsgHeader *pMsg )
   {
      INT32 rc = SDB_OK ;
      PD_TRACE_ENTRY ( SDB_CATMAINCT_AUTHCRT ) ;
      MsgAuthCrtUsr *msg = ( MsgAuthCrtUsr * )pMsg ;
      BSONObj obj ;
      BSONObj retObj ;
      MsgAuthCrtReply reply ;
      BOOLEAN bIsDelay = FALSE ;

      /// fill reply header
      reply.header.messageLength = sizeof( MsgAuthCrtReply ) ;
      reply.header.opCode = MAKE_REPLY_TYPE( pMsg->opCode ) ;
      reply.header.requestID = pMsg->requestID ;
      reply.header.routeID.value = 0 ;
      reply.header.TID = pMsg->TID ;
      reply.header.globalID = pMsg->globalID ;
      reply.contextID = -1 ;
      reply.flags = SDB_OK ;
      reply.numReturned = 0 ;
      reply.startFrom = 0 ;

      rc = _pCatCB->primaryCheck( _pEDUCB, TRUE, bIsDelay ) ;
      if ( bIsDelay )
      {
         goto done ;
      }
      else if ( rc )
      {
         goto error ;
      }

      if ( _pCatCB->getCatDCMgr()->isWritedCommand() &&
           _pCatCB->isDCReadonly() )
      {
         rc = SDB_CAT_CLUSTER_IS_READONLY ;
         goto error ;
      }

      rc = extractAuthMsg( &(msg->header), obj ) ;
      if ( SDB_OK != rc )
      {
         goto error ;
      }

      rc = _pAuthCB->createUsr( obj, _pEDUCB, &retObj,
                                _pCatCB->majoritySize() ) ;
      if ( SDB_OK != rc )
      {
         goto error ;
      }

      reply.header.messageLength += retObj.objsize() ;
      reply.numReturned = 1 ;

   done:
      if ( !isDelayed() )
      {
         PD_TRACE1 ( SDB_CATMAINCT_AUTHCRT, PD_PACK_INT ( rc ) ) ;
         _pCatCB->sendReply( handle, &reply, rc,
                             (void*)retObj.objdata(),
                             retObj.objsize() ) ;
      }
      PD_TRACE_EXITRC ( SDB_CATMAINCT_AUTHCRT, rc ) ;
      return rc ;
   error:
      reply.flags = rc ;
      if ( SDB_CLS_NOT_PRIMARY == rc )
      {
         reply.startFrom = _pCatCB->getPrimaryNode() ;
      }
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB_CATMAINCT_AUTHENTICATE, "catMainController::_processAuthenticate" )
   INT32 catMainController::_processAuthenticate( const NET_HANDLE &handle,
                                                  MsgHeader *pMsg )
   {
      INT32 rc = SDB_OK ;
      PD_TRACE_ENTRY ( SDB_CATMAINCT_AUTHENTICATE ) ;
      MsgAuthentication *msg = ( MsgAuthentication * )pMsg ;
      BSONObj obj ;
      BSONObj retObj ;
      MsgAuthReply reply ;
      BOOLEAN bIsDelay = FALSE ;

      /// fill reply header
      reply.header.messageLength = sizeof( MsgAuthReply ) ;
      reply.header.opCode = MAKE_REPLY_TYPE( pMsg->opCode ) ;
      reply.header.requestID = pMsg->requestID ;
      reply.header.routeID.value = 0 ;
      reply.header.TID = pMsg->TID ;
      reply.header.globalID = pMsg->globalID ;
      reply.contextID = -1 ;
      reply.flags = SDB_OK ;
      reply.numReturned = 0 ;
      reply.startFrom = 0 ;

      if ( !_pAuthCB->authEnabled() )
      {
         goto done ;
      }

      // primary check
      rc = _pCatCB->primaryCheck( _pEDUCB, TRUE, bIsDelay ) ;
      if ( bIsDelay )
      {
         goto done ;
      }
      else if ( rc )
      {
         goto error ;
      }

      rc = extractAuthMsg( &(msg->header), obj ) ;
      if ( SDB_OK != rc )
      {
         goto error ;
      }

      if ( MSG_AUTH_VERIFY1_REQ == pMsg->opCode )
      {
         rc = _pAuthCB->SCRAMSHAAuthenticate( obj, _pEDUCB, &retObj ) ;
      }
      else
      {
         rc = _pAuthCB->md5Authenticate( obj, _pEDUCB, &retObj ) ;
      }
      if ( SDB_OK != rc )
      {
         goto error ;
      }

      reply.header.messageLength += retObj.objsize() ;
      reply.numReturned = 1 ;

   done:
      if ( !isDelayed() )
      {
         PD_TRACE1 ( SDB_CATMAINCT_AUTHENTICATE, PD_PACK_INT ( rc ) ) ;
         _pCatCB->sendReply( handle, &reply, rc,
                             (void*)retObj.objdata(),
                             retObj.objsize() ) ;
      }
      PD_TRACE_EXITRC ( SDB_CATMAINCT_AUTHENTICATE, rc ) ;
      return rc ;
   error:
      reply.flags = rc ;
      if ( SDB_CLS_NOT_PRIMARY == rc )
      {
         reply.startFrom = _pCatCB->getPrimaryNode() ;
      }
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB_CATMAINCT_AUTHDEL, "catMainController::_processAuthDel" )
   INT32 catMainController::_processAuthDel( const NET_HANDLE &handle,
                                             MsgHeader *pMsg )
   {
      INT32 rc = SDB_OK ;
      PD_TRACE_ENTRY ( SDB_CATMAINCT_AUTHDEL ) ;
      MsgAuthDelUsr *msg = ( MsgAuthDelUsr * )pMsg ;
      BSONObj obj ;
      MsgAuthDelReply reply ;
      BOOLEAN bIsDelay = FALSE ;

      /// fill reply header
      reply.header.messageLength = sizeof( MsgAuthDelReply ) ;
      reply.header.opCode = MAKE_REPLY_TYPE( pMsg->opCode ) ;
      reply.header.requestID = pMsg->requestID ;
      reply.header.routeID.value = 0 ;
      reply.header.TID = pMsg->TID ;
      reply.header.globalID = pMsg->globalID ;
      reply.contextID = -1 ;
      reply.flags = SDB_OK ;
      reply.numReturned = 0 ;
      reply.startFrom = 0 ;

      // primary check
      rc = _pCatCB->primaryCheck( _pEDUCB, TRUE, bIsDelay ) ;
      if ( bIsDelay )
      {
         goto done ;
      }
      else if ( rc )
      {
         goto error ;
      }

      if ( _pCatCB->getCatDCMgr()->isWritedCommand() &&
           _pCatCB->isDCReadonly() )
      {
         rc = SDB_CAT_CLUSTER_IS_READONLY ;
         goto error ;
      }

      rc = extractAuthMsg( &(msg->header), obj ) ;
      if ( SDB_OK != rc )
      {
         goto error ;
      }

      rc = _pAuthCB->removeUsr( obj, _pEDUCB, _pCatCB->majoritySize() ) ;
      if ( SDB_OK != rc )
      {
         goto error ;
      }

   done:
      if ( !isDelayed() )
      {
         PD_TRACE1 ( SDB_CATMAINCT_AUTHDEL, PD_PACK_INT ( rc ) ) ;
         _pCatCB->sendReply( handle, &reply, rc ) ;
      }
      PD_TRACE_EXITRC ( SDB_CATMAINCT_AUTHDEL, rc ) ;
      return rc ;
   error:
      reply.flags = rc ;
      if ( SDB_CLS_NOT_PRIMARY == rc )
      {
         reply.startFrom = _pCatCB->getPrimaryNode() ;
      }
      goto done ;
   }

   //PD_TRACE_DECLARE_FUNCTION ( SDB_CATMAINCT_SESSIONINIT, "catMainController::_processSessionInit" )
   INT32 catMainController::_processSessionInit( const NET_HANDLE &handle,
                                                 MsgHeader *pMsg )
   {
      INT32 rc = SDB_OK;
      PD_TRACE_ENTRY ( SDB_CATMAINCT_SESSIONINIT ) ;
      MsgComSessionInitReq *pMsgReq = (MsgComSessionInitReq*)pMsg ;
      MsgOpReply reply ;

      /// init reply
      reply.contextID               = -1;
      reply.numReturned             = 0;
      reply.startFrom               = 0;
      reply.header.messageLength    = sizeof( MsgOpReply ) ;
      reply.header.opCode           = MSG_COM_SESSION_INIT_RSP ;
      reply.header.requestID        = pMsgReq->header.requestID;
      reply.header.routeID.value    = 0 ;
      reply.header.TID              = pMsgReq->header.TID ;
      reply.header.globalID         = pMsgReq->header.globalID ;

      /// check wether the route id is right
      MsgRouteID localRouteID       = _pCatCB->netWork()->localID() ;
      if ( pMsgReq->dstRouteID.value != localRouteID.value )
      {
         rc = SDB_INVALID_ROUTEID ;
         PD_LOG ( PDERROR, "Session init failed: route id does not match."
                  "Message info: [%s], Local route id: %s",
                  msg2String( pMsg ).c_str(),
                  routeID2String( localRouteID ).c_str() ) ;
      }
      reply.flags = rc ;
      _pCatCB->sendReply( handle, &reply, rc ) ;

      PD_TRACE_EXITRC ( SDB_CATMAINCT_SESSIONINIT, rc ) ;
      return rc ;
   }

   void catMainController::addContext( const UINT32 &handle, UINT32 tid,
                                       INT64 contextID )
   {
      if ( -1 != contextID )
      {
         PD_LOG( PDDEBUG, "add context( handle=%u, contextID=%lld )",
                 handle, contextID );
         ossScopedLock lock( &_contextLatch ) ;
         _contextLst[ contextID ] = ossPack32To64( handle, tid ) ;
      }
   }

   void catMainController::_delContextByHandle( const UINT32 &handle )
   {
      PD_LOG ( PDDEBUG,
               "delete context( handle=%u )",
               handle ) ;
      UINT32 saveTid = 0 ;
      UINT32 saveHandle = 0 ;

      ossScopedLock lock( &_contextLatch ) ;
      CONTEXT_LIST::iterator iterMap = _contextLst.begin() ;
      while ( iterMap != _contextLst.end() )
      {
         ossUnpack32From64( iterMap->second, saveHandle, saveTid ) ;
         if ( handle != saveHandle )
         {
            ++iterMap ;
            continue ;
         }
         // rtn delete
         _pRtnCB->contextDelete( iterMap->first, _pEDUCB );
         _contextLst.erase( iterMap++ ) ;
      }
   }

   void catMainController::_delContext( const UINT32 &handle,
                                        UINT32 tid )
   {
      PD_LOG ( PDDEBUG,
               "delete context( handle=%u, tid=%u )",
               handle, tid ) ;
      UINT32 saveTid = 0 ;
      UINT32 saveHandle = 0 ;

      ossScopedLock lock( &_contextLatch ) ;
      CONTEXT_LIST::iterator iterMap = _contextLst.begin() ;
      while ( iterMap != _contextLst.end() )
      {
         ossUnpack32From64( iterMap->second, saveHandle, saveTid ) ;
         if ( handle != saveHandle || tid != saveTid )
         {
            ++iterMap ;
            continue ;
         }
         // rtn delete
         _pRtnCB->contextDelete( iterMap->first, _pEDUCB ) ;
         _contextLst.erase( iterMap++ ) ;
      }
   }

   void catMainController::delContextByID( INT64 contextID, BOOLEAN rtnDel )
   {
      PD_LOG ( PDDEBUG,
               "delete context( contextID=%lld )", contextID ) ;

      ossScopedLock lock( &_contextLatch ) ;
      CONTEXT_LIST::iterator iterMap = _contextLst.find( contextID ) ;
      if ( iterMap != _contextLst.end() )
      {
         if ( rtnDel )
         {
            _pRtnCB->contextDelete( iterMap->first, _pEDUCB ) ;
         }
         _contextLst.erase( iterMap ) ;
      }
   }

   INT32 catMainController::onBeginCommand ( MsgHeader *pReqMsg )
   {
      catSetSyncW( 0 ) ;
      _pCatCB->setNodeInfoChanged( FALSE ) ;
      return catTransBegin( _pEDUCB ) ;
   }

   INT32 catMainController::onEndCommand ( MsgHeader *pReqMsg, INT32 result )
   {
      INT32 rc = SDB_OK ;

      rc = catTransEnd( result, _pEDUCB, _pDpsCB ) ;
      if ( SDB_OK != result && TRUE == _pCatCB->getNodeInfoChanged() )
      {
         _pCatCB->_catNodeMgr.loadNodeInfo() ;
      }
      _pCatCB->setNodeInfoChanged( FALSE ) ;

      return rc ;
   }

   INT32 catMainController::waitSync ( const NET_HANDLE &handle,
                                       MsgOpReply *pReply,
                                       void *pReplyData, UINT32 replyDataLen )
   {
      INT32 rc = SDB_OK ;

      if ( 0 != _pEDUCB->getLsnCount() )
      {
         INT16 w = catGetSyncW() ;
         rc = _waitSyncInternal( handle, TRUE, _pEDUCB->getEndLsn(), w,
                                 pReply, pReplyData, replyDataLen ) ;
         PD_RC_CHECK( rc, PDERROR, "Wait sync failed, rc: %d", rc ) ;
      }

   done :
      _pEDUCB->resetLsn() ;
      return rc ;

   error :
      goto done ;
   }

   INT32 catMainController::onSendReply ( MsgOpReply *pReply, INT32 result )
   {
      INT32 rc = SDB_OK ;

      rc = catTransEnd( result, _pEDUCB, _pDpsCB ) ;
      if ( SDB_OK != result && TRUE == _pCatCB->getNodeInfoChanged() )
      {
         _pCatCB->_catNodeMgr.loadNodeInfo() ;
      }
      _pCatCB->setNodeInfoChanged( FALSE ) ;

      return rc ;
   }

   //PD_TRACE_DECLARE_FUNCTION ( SDB_CATMAINCT_DELAYREPLY, "catMainController::_processDelayReply" )
   INT32 catMainController::_processDelayReply( const NET_HANDLE &handle,
                                                MsgHeader *pMsg )
   {
      INT32 rc = SDB_OK ;

      PD_TRACE_ENTRY ( SDB_CATMAINCT_DELAYREPLY ) ;

      MsgOpReply *pReply = NULL ;
      CAT_DELAY_REPLY_TYPE msgType = CAT_DELAY_REPLY_UNKNOWN ;
      MsgOpReply errReply ;
      BSONObj boEvent ;

      try
      {
         // Extract delay reply
         rc = _extractDelayReplyEvent( (MsgOpReply *)pMsg, msgType,
                                       &pReply, boEvent ) ;
         PD_CHECK( SDB_OK == rc,
                   SDB_UNKNOWN_MESSAGE, error, PDERROR,
                   "Failed to extract delayed reply message" ) ;

         switch ( msgType )
         {
         case CAT_DELAY_REPLY_SYNC :
         {
            UINT64 syncLsn = 0 ;
            INT32 tmpW = 0 ;
            INT16 w = 0 ;
            rc = rtnGetNumberLongElement( boEvent, CAT_DELAY_SYNC_LSN_NAME,
                                          (INT64 &)syncLsn ) ;
            PD_CHECK( SDB_OK == rc,
                      SDB_UNKNOWN_MESSAGE, error, PDERROR,
                      "Failed to extract delayed reply message, "
                      "failed to extract %s",
                      CAT_DELAY_SYNC_LSN_NAME ) ;

            rc = rtnGetIntElement ( boEvent, CAT_DELAY_SYNC_W_NAME, tmpW ) ;
            if ( SDB_FIELD_NOT_EXIST == rc )
            {
               rc = SDB_OK ;
               tmpW = 0 ;
            }
            PD_CHECK( SDB_OK == rc,
                      SDB_UNKNOWN_MESSAGE, error, PDERROR,
                      "Failed to extract delayed reply message, "
                      "failed to extract %s",
                      CAT_DELAY_SYNC_W_NAME ) ;

            w = (INT16)tmpW ;
            rc = _waitSyncInternal( handle, FALSE, syncLsn, w, pReply ) ;

            PD_RC_CHECK( rc, PDERROR,
                         "Failed to process delayed reply message, failed to wait "
                         "sync failed, rc: %d", rc ) ;
            break ;
         }
         default :
            PD_LOG( PDERROR,
                    "Failed to extract delayed reply message, unknown type: %d",
                    msgType ) ;
            rc = SDB_UNKNOWN_MESSAGE ;
            goto error ;
         }
      }
      catch ( std::exception &e )
      {
         PD_LOG ( PDERROR, "Failed to extract delayed reply message: %s",
                  e.what() ) ;
         rc = SDB_INVALIDARG ;
         goto error ;
      }

   done :
      if ( pReply && !isDelayed() )
      {
         PD_LOG( PDDEBUG,
                 "Finished process delayed reply [rc: %d], "
                 "sending reply message [%d]",
                 rc, pReply->header.opCode ) ;
         rc = _pCatCB->sendReply( handle, pReply, rc, NULL, 0, FALSE ) ;
      }
      PD_TRACE_EXITRC( SDB_CATMAINCT_DELAYREPLY, rc ) ;
      return rc ;

   error :
      {
         MsgOpReply *pTmpReply = ( MsgOpReply *)pMsg ;

         // Delete context if the reply message specifies contextID
         // since Coord will not send KillContext if Catalog reports error
         SINT64 contextID = pTmpReply->contextID ;
         if ( -1 != contextID )
         {
            delContextByID( contextID, TRUE ) ;
         }

         // Replace with error reply
         _pCatCB->fillErrReply( pTmpReply, &errReply, rc ) ;
         pReply = &errReply ;
      }
      goto done ;
   }

   INT32 catMainController::_waitSyncInternal ( const NET_HANDLE &handle,
                                                BOOLEAN firstTry,
                                                UINT64 syncLsn, INT16 w,
                                                MsgOpReply *pReply,
                                                void *pReplyData,
                                                UINT32 replyDataLen )
   {
      INT32 rc = SDB_OK ;

      replCB *pRepl = sdbGetReplCB() ;
      UINT64 timeout = firstTry ? CAT_SYNC_FIRST_INTERVAL :
                                  CAT_SYNC_INTERVAL ;

      // Recheck w in the delayed event, reset w in below cases
      // 1. w is <= 0, which means not set
      // 2. w is > current majority size, which means the node number of
      //    catalog group has been reduced
      INT16 curSyncW = 0 ;
      if ( w <= 0 ||
           w > _pCatCB->majoritySize( TRUE ) )
      {
         curSyncW = _pCatCB->majoritySize( TRUE ) ;
      }
      else
      {
         // w is 1 or smaller than current majority size,
         // we could have a try
         _delayWithoutSync = FALSE ;
         curSyncW = w ;
      }

      // For the first try, always wait sync before delay
      _delayWithoutSync = firstTry ? FALSE : _delayWithoutSync ;

      // No need to wait
      if ( !_pDpsCB || curSyncW <= 1 )
      {
         goto done ;
      }

      if ( _delayWithoutSync )
      {
         // Delay sync directly
         rc = SDB_TIMEOUT ;
      }
      else
      {
         rc = pRepl->sync( syncLsn, _pEDUCB, curSyncW, timeout ) ;
      }

      if ( SDB_TIMEOUT == rc )
      {
         // The earlier wait sync failed, don't wait for later ones in the
         // delayed list for this round
         _delayWithoutSync = TRUE ;

         rc = _delaySync( handle, firstTry, syncLsn, w,
                          pReply, pReplyData, replyDataLen ) ;
         PD_RC_CHECK( rc, PDERROR, "Wait sync delay failed, "
                      "w: [%d/%d], LSN: [%llu], first: [%s], rc: %d",
                      w, curSyncW, syncLsn, firstTry ? "TRUE" : "FALSE", rc ) ;
         PD_LOG( PDDEBUG,
                 "Wait sync delayed: w: [%d/%d], LSN: [%llu], first: [%s]",
                 w, curSyncW, syncLsn, firstTry ? "TRUE" : "FALSE" ) ;
      }
      else
      {
         PD_RC_CHECK( rc, PDERROR, "Wait sync failed, "
                      "w: [%d/%d], LSN: [%llu], first: [%s], rc: %d",
                      w, curSyncW, syncLsn, firstTry ? "TRUE" : "FALSE", rc ) ;

         PD_LOG( PDDEBUG,
                 "Wait sync finished: w: [%d/%d], LSN: [%llu], first: [%s]",
                 w, curSyncW, syncLsn, firstTry ? "TRUE" : "FALSE" ) ;
      }

   done :
      return rc ;

   error :
      goto done ;
   }

   INT32 catMainController::_delaySync ( const NET_HANDLE &handle,
                                         BOOLEAN firstTry,
                                         UINT64 syncLsn, INT16 w,
                                         MsgOpReply *pReply,
                                         void *pReplyData,
                                         UINT32 replyDataLen )
   {
      INT32 rc = SDB_OK ;

      pmdEDUEvent event ;
      CHAR *pBuffer = NULL ;
      INT32 bufferSize = 0 ;

      if ( !firstTry )
      {
         if ( delayCurOperation( CAT_SYNC_MAX_RETRY_TIMES ) )
         {
            goto done ;
         }
         else
         {
            rc = SDB_CLS_WAIT_SYNC_FAILED ;
            PD_RC_CHECK( rc, PDERROR,
                         "Delay wait sync failed, rc: %d",
                         rc ) ;
         }
      }

      rc = _buildDelaySyncEvent( &pBuffer, &bufferSize, syncLsn, w,
                                 pReply, pReplyData, replyDataLen ) ;
      PD_RC_CHECK( rc, PDERROR,
                   "Build catalog wait sync request failed, rc: %d",
                   rc ) ;

      event._eventType = PMD_EDU_EVENT_MSG ;
      event._Data = pBuffer ;
      event._dataMemType = PMD_EDU_MEM_SELF ;
      event._eventType = PMD_EDU_EVENT_MSG ;
      event._userData = ossPack32To64( 1, handle ) ;

      _delayEvent( event ) ;

      PD_LOG ( PDDEBUG, "Delay event handle: [%u] type: [%d]",
               handle, event._eventType ) ;

   done :
      return rc ;

   error :
      goto done ;
   }

   INT32 catMainController::_buildDelaySyncEvent ( CHAR **ppBuffer,
                                                   INT32 *pBufferSize,
                                                   UINT64 syncLsn, INT16 w,
                                                   MsgOpReply *pReply,
                                                   void *pReplyData,
                                                   UINT32 replyDataLen )
   {
      // For wait sync event, record the sync LSN and w
      BSONObj boEvent = BSON( CAT_DELAY_SYNC_LSN_NAME << (INT64)syncLsn <<
                              CAT_DELAY_SYNC_W_NAME << (INT32)w ) ;
      return _buildDelayReplyEvent( ppBuffer, pBufferSize,
                                    CAT_DELAY_REPLY_SYNC,
                                    pReply, pReplyData, replyDataLen,
                                    boEvent ) ;
   }

   INT32 catMainController::_buildDelayReplyEvent ( CHAR **ppBuffer,
                                                    INT32 *pBufferSize,
                                                    CAT_DELAY_REPLY_TYPE type,
                                                    MsgOpReply *pReply,
                                                    void *pReplyData,
                                                    UINT32 replyDataLen,
                                                    const BSONObj &boInfo )
   {
      INT32 rc = SDB_OK ;

      SDB_ASSERT( NULL != ppBuffer &&
                  NULL != pBufferSize, "invalid pBuffer" ) ;

      SDB_ASSERT( NULL != pReply , "invalid pReply" ) ;

      CHAR *pTmpBuffer = NULL ;
      INT32 tmpBufferSize = 0 ;
      MsgOpReply *pDelayReply = NULL ;
      INT32 delayReplylen = pReply->header.messageLength ;

      // Merge the reply message first
      if ( pReplyData )
      {
         SDB_ASSERT( (UINT32)delayReplylen == sizeof ( MsgOpReply ) + replyDataLen,
                     "mismatched message length" ) ;
         rc = msgCheckBuffer ( &pTmpBuffer, &tmpBufferSize, delayReplylen ) ;
         PD_RC_CHECK ( rc, PDERROR,
                       "Failed to allocate temporary message buffer, rc: %d",
                       rc ) ;
         ossMemcpy( pTmpBuffer, pReply, sizeof( MsgOpReply ) ) ;
         ossMemcpy( pTmpBuffer + sizeof( MsgOpReply ), pReplyData, replyDataLen ) ;
         pDelayReply = (MsgOpReply *)pTmpBuffer ;
      }
      else
      {
         pDelayReply = pReply ;
      }

      try
      {
         BSONObjBuilder boBuilder ;
         BSONObj boEvent ;

         // Format of delay reply event:
         // MsgOpReply header
         // { DELAYEVENT : <type>, DELAYREPLY: <reply>, ... }

         boBuilder.append( CAT_DELAY_REPLY_TYPE_NAME, (INT32)type ) ;
         boBuilder.appendElements( boInfo ) ;
         if ( pDelayReply )
         {
            boBuilder.appendBinData( CAT_DELAY_REPLY_MSG_NAME, delayReplylen,
                                     BinDataGeneral, (CHAR *)pDelayReply ) ;
         }

         boEvent = boBuilder.obj() ;
         UINT32 eventLen = sizeof( MsgOpReply ) + (UINT32)boEvent.objsize() ;

         rc = _pEDUCB->allocBuff( (UINT32)eventLen, ppBuffer,
                                  (UINT32 *)pBufferSize ) ;
         PD_RC_CHECK ( rc, PDERROR,
                       "Failed to allocate event buffer, rc: %d",
                       rc ) ;

         // Fill the event header
         ossMemcpy( (*ppBuffer), pReply, sizeof( MsgOpReply ) ) ;
         MsgHeader *pDelayMsg = (MsgHeader *)(*ppBuffer) ;
         pDelayMsg->opCode = CAT_DELAY_EVENT_TYPE ;
         pDelayMsg->messageLength = eventLen ;

         // Fill the event content
         ossMemcpy( (*ppBuffer) + sizeof( MsgOpReply ), boEvent.objdata(),
                    boEvent.objsize() ) ;
      }
      catch ( std::exception &e )
      {
         PD_LOG ( PDERROR, "Failed to extract delayed reply message: %s",
                  e.what() ) ;
         rc = SDB_INVALIDARG ;
         goto error ;
      }

   done :
      SAFE_OSS_FREE( pTmpBuffer ) ;
      return rc ;

   error :
      goto done ;
   }

   INT32 catMainController::_extractDelayReplyEvent ( const MsgOpReply *pDelayedReply,
                                                      CAT_DELAY_REPLY_TYPE &type,
                                                      MsgOpReply **ppReply,
                                                      BSONObj &boInfo )
   {
      INT32 rc = SDB_OK ;

      CHAR *pOffset = (CHAR *)pDelayedReply ;
      UINT32 msgLen = (UINT32)pDelayedReply->header.messageLength ;

      PD_CHECK( msgLen > sizeof( MsgOpReply ),
                SDB_UNKNOWN_MESSAGE, error, PDERROR,
                "Failed to extract delayed reply message, mismatched "
                "message length" ) ;

      try
      {
         // Format of delay reply event:
         // MsgOpReply header
         // { DELAYEVENT : <type>, DELAYREPLY: <reply>, ... }

         pOffset += sizeof( MsgOpReply ) ;
         boInfo = BSONObj( pOffset ) ;

         rc = rtnGetIntElement( boInfo, CAT_DELAY_REPLY_TYPE_NAME,
                                (INT32 &)type ) ;
         PD_CHECK( SDB_OK == rc,
                   SDB_UNKNOWN_MESSAGE, error, PDERROR,
                   "Failed to extract delayed reply message, "
                   "failed to extract %s",
                   CAT_DELAY_REPLY_TYPE_NAME ) ;

         PD_CHECK ( BinData == boInfo.getField( CAT_DELAY_REPLY_MSG_NAME ).type(),
                    SDB_UNKNOWN_MESSAGE, error, PDERROR,
                    "Failed to extract delayed reply message, "
                    "failed to extract %s",
                    CAT_DELAY_REPLY_MSG_NAME ) ;

         if ( ppReply )
         {
            BSONElement boMsg = boInfo.getField( CAT_DELAY_REPLY_MSG_NAME ) ;
            INT32 replyLen = 0 ;

            (*ppReply) = (MsgOpReply *)boMsg.binData( replyLen ) ;

            PD_CHECK( (*ppReply)->header.messageLength == replyLen,
                      SDB_UNKNOWN_MESSAGE, error, PDERROR,
                      "Failed to extract delayed reply message, "
                      "failed to extract %s",
                      CAT_DELAY_REPLY_MSG_NAME ) ;

         }
      }
      catch ( std::exception &e )
      {
         PD_LOG ( PDERROR, "Failed to extract delayed reply message: %s",
                  e.what() ) ;
         rc = SDB_INVALIDARG ;
         goto error ;
      }

   done :
      return rc ;
   error :
      goto done ;
   }
}

