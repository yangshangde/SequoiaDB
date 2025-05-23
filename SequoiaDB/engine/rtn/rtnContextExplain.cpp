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

   Source File Name = rtnContextExplain.cpp

   Descriptive Name = RunTime Explain Context

   When/how to use: this program may be used on binary and text-formatted
   versions of Runtime component. This file contains structure for Runtime
   Context.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          11/15/2017  HGM Split from rtnContextData.cpp

   Last Changed =

*******************************************************************************/

#include "rtnContextExplain.hpp"
#include "rtnContextMain.hpp"
#include "pdTrace.hpp"
#include "rtnTrace.hpp"
#include "rtnCB.hpp"
#include "rtn.hpp"
#include "dmsCB.hpp"

namespace engine
{

   /*
      _rtnExplainBase implement
    */
   _rtnExplainBase::_rtnExplainBase ()
   : _rtnSubContextHolder(),
     _explainStarted( FALSE ),
     _explainRunned( FALSE ),
     _explainPrepared( FALSE ),
     _explained( FALSE )
   {
   }

   _rtnExplainBase::~_rtnExplainBase ()
   {
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB_RTNEXPBASE__OPENEXP, "_rtnExplainBase::_openExplain" )
   INT32 _rtnExplainBase::_openExplain ( const rtnQueryOptions & options,
                                         pmdEDUCB * cb )
   {
      INT32 rc = SDB_OK ;

      PD_TRACE_ENTRY( SDB_RTNEXPBASE__OPENEXP ) ;

      BSONObj explainOptions, realHint ;
      rtnContextPtr queryContext ;
      rtnQueryOptions subOptions = options ;

      _queryOptions = options ;

      rc = _extractExplainOptions( options.getHint(), explainOptions,
                                   realHint ) ;
      PD_RC_CHECK( rc, PDERROR, "Failed to extract explain options, "
                   "rc: %d", rc ) ;

      rc = _parseExplainOptions( explainOptions ) ;
      PD_RC_CHECK( rc, PDERROR, "Failed to parse explain options, "
                   "rc: %d", rc ) ;

      // Reset query hint
      _queryOptions.setHint( realHint ) ;

      // Reset query flags
      if ( _queryOptions.isQueryAndModify() &&
           !_queryOptions.isOrderByEmpty() )
      {
         // Tell the optimizer to use index by sort
         _queryOptions.setFlag( FLG_QUERY_FORCE_IDX_BY_SORT ) ;
      }

      _queryOptions.clearFlag( FLG_QUERY_EXPLAIN ) ;

      rc = _queryOptions.getOwned() ;
      PD_RC_CHECK( rc, PDERROR, "Failed to get query options owned, "
                   "rc: %d", rc ) ;

      if ( _needResetExplainOptions() )
      {
         subOptions = _queryOptions ;
         subOptions.clearFlag( FLG_QUERY_MODIFY ) ;
      }

      // Open query context
      rc = _openSubContext( subOptions, cb, &queryContext ) ;
      PD_RC_CHECK( rc, PDERROR, "Failed to open query context, rc: %d", rc ) ;

      SDB_ASSERT( queryContext, "query context is invalid" ) ;

      // Disable setting query activity
      queryContext->setEnableQueryActivity( FALSE ) ;

      // Log start timestamp
      if ( cb->getMonConfigCB()->timestampON )
      {
         queryContext->getMonCB()->recordStartTimestamp() ;
      }

      _setSubContext( queryContext, cb ) ;

   done :
      PD_TRACE_EXITRC( SDB_RTNEXPBASE__OPENEXP, rc ) ;
      return rc ;

   error :
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB_RTNEXPBASE__PREPAREEXP, "_rtnExplainBase::_prepareExplain" )
   INT32 _rtnExplainBase::_prepareExplain ( rtnContext * explainContext,
                                            pmdEDUCB * cb )
   {
      INT32 rc = SDB_OK ;

      PD_TRACE_ENTRY( SDB_RTNEXPBASE__PREPAREEXP ) ;

      SDB_ASSERT( NULL != explainContext, "explain context is invalid" ) ;

      rtnContext *queryContext = _getSubContext() ;

      if ( _explained )
      {
         rc = SDB_DMS_EOC ;
         goto done ;
      }

      PD_CHECK( NULL != queryContext,
                SDB_SYS, error, PDERROR, "Failed to get query context" ) ;

      if ( !_explainStarted )
      {
         rc = getExplainPath()->setExplainStart( cb ) ;
         PD_RC_CHECK( rc, PDERROR, "Failed to set explain start, "
                      "rc: %d", rc ) ;
         _explainStarted = TRUE ;
      }

      if ( !_explainRunned &&
           ( _expOptions.isNeedRun() ||
             _needCollectSubExplains() ) )
      {
         for ( ; ; )
         {
            rtnContextBuf contextBuf ;
            rc = queryContext->getMore( -1, contextBuf, cb ) ;
            if ( SDB_DMS_EOC == rc )
            {
               rc = SDB_OK ;
               break ;
            }
            else if ( SDB_OK == rc )
            {
               if ( _expOptions.isNeedRun() &&
                    _needReturnDataInRun() )
               {
                  rc = explainContext->appendObjs( contextBuf.data(),
                                                   contextBuf.size(),
                                                   contextBuf.recordNum() ) ;
                  PD_RC_CHECK( rc, PDERROR, "Failed process data in run mode, "
                               "rc: %d", rc ) ;

                  if ( queryContext->eof() )
                  {
                     _explainRunned = TRUE ;
                  }
                  goto done ;
               }
            }
            else
            {
               PD_RC_CHECK( rc, PDERROR, "Failed to get data from query "
                            "context [%lld], rc: %d",
                            queryContext->contextID(), rc ) ;
            }
         }

         _explainRunned = TRUE ;
      }

      if ( !_explainPrepared )
      {
         // Set the end of explain
         rc = getExplainPath()->setExplainEnd( queryContext, cb ) ;
         PD_RC_CHECK( rc, PDERROR, "Failed to set explain end, rc: %d", rc ) ;

         // Finish the query context
         rc = _finishSubContext( queryContext, cb ) ;
         PD_RC_CHECK( rc, PDERROR, "Failed to finish query context [%lld], "
                   "rc: %d", queryContext->contextID(), rc ) ;

         // Prepare explain paths, must called after finish query context
         // which will collect sub-explain results
         rc = _prepareExplainPath( queryContext, cb ) ;
         PD_RC_CHECK( rc, PDERROR, "Failed to prepare explain path, rc: %d", rc ) ;

         _explainPrepared = TRUE ;
      }

      try
      {
         BOOLEAN hasMore = FALSE ;

         rc = _buildExplain( explainContext, hasMore ) ;
         PD_RC_CHECK( rc, PDERROR, "Failed to build explain result, "
                      "rc: %d", rc ) ;

         if ( !hasMore )
         {
            _explained = TRUE ;
         }
      }
      catch ( std::exception & e )
      {
         PD_LOG( PDERROR, "Failed to prepare explain, received unexpected "
                 "error: %s", e.what() ) ;
         rc = SDB_INVALIDARG ;
         goto error ;
      }

      // Reset total records for context data processor
      if ( _needResetTotalRecords() )
      {
         explainContext->_resetTotalRecords( RTN_CTX_EXPLAIN_PROCESSOR +
                                             explainContext->numRecords() ) ;
      }

      if ( _explained )
      {
         explainContext->_hitEnd = TRUE ;
      }

   done :
      PD_TRACE_EXITRC( SDB_RTNEXPBASE__PREPAREEXP, rc ) ;
      return rc ;

   error :
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB_RTNEXPBASE__EXTRACTEXPOPTS, "_rtnExplainBase::_extractExplainOptions" )
   INT32 _rtnExplainBase::_extractExplainOptions ( const BSONObj & hint,
                                                   BSONObj & explainOptions,
                                                   BSONObj & realHint )
   {
      INT32 rc = SDB_OK ;

      PD_TRACE_ENTRY( SDB_RTNEXPBASE__EXTRACTEXPOPTS ) ;

      try
      {
         BSONElement element ;

         // Extract explain options
         element = hint.getField( FIELD_NAME_OPTIONS ) ;
         if ( Object == element.type() )
         {
            explainOptions = element.embeddedObject() ;
         }

         // Extract hint
         element = hint.getField( FIELD_NAME_HINT ) ;
         if ( Object == element.type() )
         {
            realHint = element.embeddedObject() ;
         }
      }
      catch ( std::exception & e )
      {
         PD_LOG( PDERROR, "Occur exception: %s", e.what() ) ;
         rc = SDB_INVALIDARG ;
         goto error ;
      }

   done :
      PD_TRACE_EXITRC( SDB_RTNEXPBASE__EXTRACTEXPOPTS, rc ) ;
      return rc ;

   error :
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB_RTNEXPBASE__PARSEEXPOPTS, "_rtnExplainBase::_parseExplainOptions" )
   INT32 _rtnExplainBase::_parseExplainOptions ( const BSONObj & options )
   {
      INT32 rc = SDB_OK ;

      PD_TRACE_ENTRY( SDB_RTNEXPBASE__PARSEEXPOPTS ) ;

      BOOLEAN hasMask = TRUE ;
      BOOLEAN hasOption = FALSE ;

      UINT16 showMask = OPT_NODE_EXPLAIN_MASK_NONE ;
      BOOLEAN needDetail = FALSE ;
      BOOLEAN needEstimate = FALSE ;
      BOOLEAN needRun = FALSE ;
      BOOLEAN needSearch = FALSE ;
      BOOLEAN needEvaluate = FALSE ;
      BOOLEAN needExpand = FALSE ;
      BOOLEAN needFlatten = FALSE ;
      BOOLEAN needAbbrev = FALSE ;

      // Run option
      rc = _parseBoolOption( options, FIELD_NAME_RUN, needRun,
                             hasOption, FALSE ) ;
      PD_RC_CHECK( rc, PDERROR, "Failed to parse %s option, rc: %d",
                   FIELD_NAME_RUN, rc ) ;

      // Detail option
      rc = _parseBoolOption( options, FIELD_NAME_DETAIL, needDetail,
                             hasOption, FALSE ) ;
      PD_RC_CHECK( rc, PDERROR, "Failed to parse %s option, rc: %d",
                   FIELD_NAME_DETAIL, rc ) ;

      // Expand option
      rc = _parseBoolOption( options, FIELD_NAME_EXPAND, needExpand,
                             hasOption, FALSE ) ;
      PD_RC_CHECK( rc, PDERROR, "Failed to parse %s option, rc: %d",
                   FIELD_NAME_EXPAND, rc ) ;

      if ( hasOption )
      {
         needDetail = TRUE ;
      }

      // Flatten option
      rc = _parseBoolOption( options, FIELD_NAME_FLATTEN, needFlatten,
                             hasOption, FALSE ) ;
      PD_RC_CHECK( rc, PDERROR, "Failed to parse %s option, rc: %d",
                   FIELD_NAME_FLATTEN, rc ) ;

      if ( hasOption )
      {
         needExpand = TRUE ;
         needDetail = TRUE ;
      }

      // Search option
      rc = _parseBoolOption( options, FIELD_NAME_SEARCH, needSearch,
                             hasOption, FALSE ) ;
      PD_RC_CHECK( rc, PDERROR, "Failed to parse %s option, rc: %d",
                   FIELD_NAME_SEARCH, rc ) ;

      if ( hasOption )
      {
         needDetail = TRUE ;
         needExpand = TRUE ;
      }

      // Evaluate option
      rc = _parseBoolOption( options, FIELD_NAME_EVALUATE, needEvaluate,
                             hasOption, FALSE ) ;
      PD_RC_CHECK( rc, PDERROR, "Failed to parse %s option, rc: %d",
                   FIELD_NAME_EVALUATE, rc ) ;

      if ( hasOption )
      {
         needDetail = TRUE ;
         needExpand = TRUE ;
         needSearch = TRUE ;
      }

      // Filter option, convert to mask
      rc = _parseMaskOption( options, FIELD_NAME_FILTER, hasMask,
                             showMask ) ;
      PD_RC_CHECK( rc, PDERROR, "Failed to parse %s option, rc: %d",
                   FIELD_NAME_FILTER, rc ) ;

      if ( hasMask )
      {
         needDetail = TRUE ;
      }

      // Location option
      rc = _parseLocationOption ( options, hasOption ) ;
      PD_RC_CHECK( rc, PDERROR, "Failed to parse %s option, rc: %d",
                   FIELD_NAME_LOCATION, rc ) ;

      if ( hasOption )
      {
         needDetail = TRUE ;
      }

      // Estimate option
      rc = _parseBoolOption ( options, FIELD_NAME_ESTIMATE, needEstimate,
                              hasOption, needDetail ) ;
      PD_RC_CHECK( rc, PDERROR, "Failed to parse %s option, rc: %d",
                   FIELD_NAME_ESTIMATE, rc ) ;

      if ( hasOption )
      {
         needDetail = TRUE ;
      }

      // Reset explain mask
      if ( needDetail )
      {
         // no mask is explicit given, set all
         if ( !hasMask )
         {
            OSS_BIT_SET( showMask, OPT_NODE_EXPLAIN_MASK_ALL ) ;
         }
      }

      // Abbreviation option
      rc = _parseBoolOption( options, FIELD_NAME_ABBREV, needAbbrev,
                             hasOption, FALSE ) ;
      PD_RC_CHECK( rc, PDERROR, "Failed to parse %s option, rc: %d",
                   FIELD_NAME_ABBREV, rc ) ;

      _expOptions.setShowMask( showMask ) ;
      _expOptions.setNeedDetail( needDetail ) ;
      _expOptions.setNeedEstimate( needEstimate ) ;
      _expOptions.setNeedRun( needRun ) ;
      _expOptions.setNeedSearch( needSearch ) ;
      _expOptions.setNeedEvaluate( needEvaluate ) ;
      _expOptions.setNeedExpand( needExpand ) ;
      _expOptions.setNeedFlatten( needFlatten ) ;
      _expOptions.setNeedAbbrev( needAbbrev ) ;

   done :
      PD_TRACE_EXITRC( SDB_RTNEXPBASE__PARSEEXPOPTS, rc ) ;
      return rc ;

   error :
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB_RTNEXPBASE__PARSELOCFILTER, "_rtnExplainBase::_parseLocationOption" )
   INT32 _rtnExplainBase::_parseLocationOption ( const BSONObj & explainOptions,
                                                 BOOLEAN & hasOption )
   {
      INT32 rc = SDB_OK ;

      PD_TRACE_ENTRY( SDB_RTNEXPBASE__PARSELOCFILTER ) ;

      // We doesn't need "Location" option
      // but it need to make sure "Detail" option is enabled
      if ( explainOptions.hasField( FIELD_NAME_SUB_COLLECTIONS ) ||
           explainOptions.hasField( FIELD_NAME_CMD_LOCATION ) ||
           explainOptions.hasField( FIELD_NAME_LOCATION ) )
      {
         hasOption = TRUE ;
      }
      else
      {
         hasOption = FALSE ;
      }

      PD_TRACE_EXITRC( SDB_RTNEXPBASE__PARSELOCFILTER, rc ) ;

      return rc ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB_RTNEXPBASE__PARSEBOOLOPT, "_rtnExplainBase::_parseBoolOption" )
   INT32 _rtnExplainBase::_parseBoolOption ( const BSONObj & options,
                                             const CHAR * optionName,
                                             BOOLEAN & option,
                                             BOOLEAN & hasOption,
                                             BOOLEAN defaultValue )
   {
      INT32 rc = SDB_OK ;

      PD_TRACE_ENTRY( SDB_RTNEXPBASE__PARSEBOOLOPT ) ;

      hasOption = FALSE ;

      try
      {
         BSONElement e = options.getField( optionName ) ;

         if ( e.eoo() )
         {
            option = defaultValue ;
            goto done ;
         }
         else if ( e.isNumber() )
         {
            option = e.numberInt() == 0 ? FALSE : TRUE ;
         }
         else if ( e.isBoolean() )
         {
            option = e.booleanSafe() ;
         }
         else
         {
            option = defaultValue ;
         }

         hasOption = TRUE ;
      }
      catch ( std::exception & e )
      {
         PD_LOG( PDERROR, "Occur exception: %s", e.what() ) ;
         rc = SDB_INVALIDARG ;
         goto error ;
      }

   done :
      PD_TRACE_EXITRC( SDB_RTNEXPBASE__PARSEBOOLOPT, rc ) ;
      return rc ;

   error :
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB_RTNEXPBASE__PARSEMASKOPT, "_rtnExplainBase::_parseMaskOption" )
   INT32 _rtnExplainBase::_parseMaskOption ( const BSONObj & options,
                                             const CHAR * optionName,
                                             BOOLEAN & hasMask,
                                             UINT16 & mask )
   {
      INT32 rc = SDB_OK ;

      PD_TRACE_ENTRY( SDB_RTNEXPBASE__PARSEMASKOPT ) ;

      hasMask = FALSE ;
      mask = OPT_NODE_EXPLAIN_MASK_NONE ;

      try
      {
         BSONElement e = options.getField( optionName ) ;

         if ( e.eoo() )
         {
            mask = OPT_NODE_EXPLAIN_MASK_NONE ;
            goto done ;
         }
         else if ( e.isNull() )
         {
            mask = OPT_NODE_EXPLAIN_MASK_NONE ;
         }
         else if ( String == e.type() )
         {
            _parseMask( e.valuestrsafe(), mask ) ;
         }
         else if ( Array == e.type() )
         {
            BSONObjIterator iter( e.embeddedObject() ) ;
            while ( iter.more() )
            {
               BSONElement subElement = iter.next() ;
               if ( String == subElement.type() )
               {
                  UINT16 subMask = OPT_NODE_EXPLAIN_MASK_NONE ;
                  rc = _parseMask( subElement.valuestrsafe(), subMask ) ;
                  if ( SDB_OK != rc )
                  {
                     break ;
                  }
                  else if ( OPT_NODE_EXPLAIN_MASK_NONE == subMask ||
                            OPT_NODE_EXPLAIN_MASK_ALL == subMask )
                  {
                     mask = subMask ;
                     break ;
                  }
                  else
                  {
                     mask |= subMask ;
                  }
               }
               else
               {
                  break ;
               }
            }
         }
         else
         {
            mask = OPT_NODE_EXPLAIN_MASK_NONE ;
         }

         hasMask = TRUE ;
      }
      catch ( std::exception & e )
      {
         PD_LOG( PDERROR, "Occur exception: %s", e.what() ) ;
         rc = SDB_INVALIDARG ;
         goto error ;
      }

   done :
      PD_TRACE_EXITRC( SDB_RTNEXPBASE__PARSEMASKOPT, rc ) ;
      return rc ;

   error :
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB_RTNEXPBASE__PARSEMASK, "_rtnExplainBase::_parseMask" )
   INT32 _rtnExplainBase::_parseMask ( const CHAR * maskName,
                                       UINT16 & mask )
   {
      INT32 rc = SDB_OK ;

      PD_TRACE_ENTRY( SDB_RTNEXPBASE__PARSEMASK ) ;

      if ( 0 == ossStrcmp( maskName, OPT_EXPLAIN_MASK_NONE_NAME ) )
      {
         mask = OPT_NODE_EXPLAIN_MASK_NONE ;
      }
      else if ( 0 == ossStrcmp( maskName, OPT_EXPLAIN_MASK_INPUT_NAME ) )
      {
         mask = OPT_NODE_EXPLAIN_MASK_INPUT ;
      }
      else if ( 0 == ossStrcmp( maskName, OPT_EXPLAIN_MASK_FILTER_NAME ) )
      {
         mask = OPT_NODE_EXPLAIN_MASK_FILTER ;
      }
      else if ( 0 == ossStrcmp( maskName, OPT_EXPLAIN_MASK_OUTPUT_NAME ) )
      {
         mask = OPT_NODE_EXPLAIN_MASK_OUTPUT ;
      }
      else if ( 0 == ossStrcmp( maskName, OPT_EXPLAIN_MASK_ALL_NAME ) )
      {
         mask = ( OPT_NODE_EXPLAIN_MASK_INPUT |
                  OPT_NODE_EXPLAIN_MASK_FILTER |
                  OPT_NODE_EXPLAIN_MASK_OUTPUT ) ;
      }
      else
      {
         rc = SDB_INVALIDARG ;
      }

      PD_TRACE_EXITRC( SDB_RTNEXPBASE__PARSEMASK, rc ) ;

      return rc ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB_RTNEXPBASE__BLDBSONNODEINFO, "_rtnExplainBase::_buildBSONNodeInfo" )
   INT32 _rtnExplainBase::_buildBSONNodeInfo ( BSONObjBuilder & builder ) const
   {
      INT32 rc = SDB_OK ;

      PD_TRACE_ENTRY( SDB_RTNEXPBASE__BLDBSONNODEINFO ) ;

      const CHAR* hostName = NULL ;
      stringstream ss ;

      hostName = pmdGetKRCB()->getHostName() ;
      ss << hostName << ":" << pmdGetOptionCB()->getServiceAddr() ;

      builder.append( OPT_FIELD_NODE_NAME, ss.str() ) ;
      builder.append( OPT_FIELD_GROUP_NAME, pmdGetKRCB()->getGroupName() ) ;
      builder.append( OPT_FIELD_ROLE, pmdGetOptionCB()->dbroleStr() ) ;

      PD_TRACE_EXITRC( SDB_RTNEXPBASE__BLDBSONNODEINFO, rc ) ;

      return rc ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB_RTNEXPBASE__BLDBSONOPTS, "_rtnExplainBase::_buildBSONQueryOptions" )
   INT32 _rtnExplainBase::_buildBSONQueryOptions ( BSONObjBuilder & builder,
                                                   const rtnExplainOptions &expOptions ) const
   {
      INT32 rc = SDB_OK ;

      PD_TRACE_ENTRY( SDB_RTNEXPBASE__BLDBSONOPTS ) ;

      builder.append( OPT_FIELD_COLLECTION,
                      NULL != _queryOptions.getCLFullName() ?
                      _queryOptions.getCLFullName() :
                      "" ) ;
      builder.appendEx( OPT_FIELD_QUERY,
                        _queryOptions.getQuery(),
                        expOptions.getBuilderOption() ) ;

      if ( expOptions.isNeedDetail() )
      {
         builder.appendEx( OPT_FIELD_SORT,
                           _queryOptions.getOrderBy(),
                           expOptions.getBuilderOption() ) ;
         builder.appendEx( OPT_FIELD_SELECTOR,
                           _queryOptions.getSelector(),
                           expOptions.getBuilderOption() ) ;
         builder.appendEx( OPT_FIELD_HINT,
                           _queryOptions.getHint(),
                           expOptions.getBuilderOption() ) ;

         builder.append( OPT_FIELD_SKIP, _queryOptions.getSkip() ) ;
         builder.append( OPT_FIELD_RETURN, _queryOptions.getLimit() ) ;
         builder.append( OPT_FIELD_FLAG, _queryOptions.getFlag() ) ;
      }

      PD_TRACE_EXITRC( SDB_RTNEXPBASE__BLDBSONOPTS, rc ) ;

      return rc ;
   }

   /*
      _rtnContextExplain implement
    */
   RTN_CTX_AUTO_REGISTER( _rtnContextExplain, RTN_CONTEXT_EXPLAIN, "EXPLAIN" )

   _rtnContextExplain::_rtnContextExplain ( INT64 contextID,
                                            UINT64 eduID )
   : _rtnContextBase( contextID, eduID ),
     _fromLocal( FALSE ),
     _explainScanPath()
   {
   }

   _rtnContextExplain::~_rtnContextExplain ()
   {
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB_RTNCONTEXTEXPLAIN_OPEN, "_rtnContextExplain::open" )
   INT32 _rtnContextExplain::open ( const rtnQueryOptions &options,
                                    pmdEDUCB *cb )
   {
      INT32 rc = SDB_OK ;

      PD_TRACE_ENTRY( SDB_RTNCONTEXTEXPLAIN_OPEN ) ;

      if ( _isOpened )
      {
         rc = SDB_DMS_CONTEXT_IS_OPEN ;
         goto error ;
      }

      rc = _openExplain( options, cb ) ;
      PD_RC_CHECK( rc, PDERROR, "Failed to open explain, rc: %d", rc ) ;

      _isOpened = TRUE ;
      _hitEnd = FALSE ;

   done :
      PD_TRACE_EXITRC( SDB_RTNCONTEXTEXPLAIN_OPEN, rc ) ;
      return rc ;

   error :
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB_RTNCONTEXTEXPLAIN__PREPAREDATA, "_rtnContextExplain::_prepareData" )
   INT32 _rtnContextExplain::_prepareData ( pmdEDUCB * cb )
   {
      INT32 rc = SDB_OK ;

      PD_TRACE_ENTRY( SDB_RTNCONTEXTEXPLAIN__PREPAREDATA ) ;

      rc = _prepareExplain( this, cb ) ;
      if ( SDB_DMS_EOC != rc &&
           SDB_OK != rc )
      {
         PD_LOG( PDERROR, "Failed to prepare explain, rc: %d", rc ) ;
         goto error ;
      }

   done:
      PD_TRACE_EXITRC( SDB_RTNCONTEXTEXPLAIN__PREPAREDATA, rc ) ;
      return rc ;

   error:
      goto done ;
   }

   void _rtnContextExplain::_toString ( stringstream & ss )
   {
      ss << ",NeedRun:" << _expOptions.isNeedRun() ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB_RTNCONTEXTEXPLAIN__OPENSUBCTX, "_rtnContextExplain::_openSubContext" )
   INT32 _rtnContextExplain::_openSubContext ( rtnQueryOptions & options,
                                               pmdEDUCB * cb,
                                               rtnContextPtr *ppContext )
   {
      INT32 rc = SDB_OK ;

      PD_TRACE_ENTRY( SDB_RTNCONTEXTEXPLAIN__OPENSUBCTX ) ;

      SDB_ASSERT( ppContext, "context pointer is invalid" ) ;

      SDB_RTNCB * rtnCB = sdbGetRTNCB() ;
      SDB_DMSCB * dmsCB = sdbGetDMSCB() ;

      INT64 queryContextID = -1 ;
      rtnContextPtr queryContext ;

      rc = rtnQuery( options, cb, dmsCB, rtnCB, queryContextID, &queryContext,
                     TRUE, &_expOptions ) ;
      PD_RC_CHECK( rc, PDERROR, "Failed to query data, rc: %d", rc ) ;

      PD_CHECK( queryContext, SDB_SYS, error, PDERROR,
                "Failed to get the context of query" ) ;

      _fromLocal = cb->isFromLocal() ;

      if ( NULL != ppContext )
      {
         *ppContext = queryContext ;
      }

   done :
      PD_TRACE_EXITRC( SDB_RTNCONTEXTEXPLAIN__OPENSUBCTX, rc ) ;
      return rc ;

   error :
      if ( -1 != queryContextID )
      {
         rtnCB->contextDelete( queryContextID, cb ) ;
      }
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB_RTNCONTEXTEXPLAIN__PREPAREEXPPATH, "_rtnContextExplain::_prepareExplainPath" )
   INT32 _rtnContextExplain::_prepareExplainPath ( rtnContext * context,
                                                   pmdEDUCB * cb )
   {
      INT32 rc = SDB_OK ;

      PD_TRACE_ENTRY( SDB_RTNCONTEXTEXPLAIN__PREPAREEXPPATH ) ;

      SDB_ASSERT( NULL != context, "query context is invalid" ) ;

      SDB_ASSERT( RTN_CONTEXT_SORT == context->getType() ||
                  RTN_CONTEXT_DATA == context->getType() ||
                  RTN_CONTEXT_PARADATA == context->getType(),
                  "sub context is invalid" ) ;

      try
      {
         optAccessPlanRuntime * planRuntime = NULL ;

         // Generate explain path from plan runtime
         PD_CHECK( NULL != context, SDB_SYS, error, PDERROR,
                   "Failed to explain: data context should not be NULL" ) ;

         planRuntime = context->getPlanRuntime() ;
         PD_CHECK( NULL != planRuntime && planRuntime->hasPlan(),
                   SDB_SYS, error, PDERROR,
                   "Failed to explain: plan should not be NULL" ) ;

         rc = planRuntime->toExplainPath( _explainScanPath, context ) ;
         PD_RC_CHECK( rc, PDERROR, "Failed to generate explain path, "
                      "rc: %d", rc ) ;

         _explainScanPath.setSearchOptions( _expOptions.isNeedSearch(),
                                            _expOptions.isNeedEvaluate() ) ;
      }
      catch ( std::exception & e )
      {
         PD_LOG( PDERROR, "Failed to build explain path, received unexpected "
                 "error: %s", e.what() ) ;
         rc = SDB_SYS ;
         goto error ;
      }

   done :
      PD_TRACE_EXITRC( SDB_RTNCONTEXTEXPLAIN__PREPAREEXPPATH, rc ) ;
      return rc ;

   error :
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB_RTNCONTEXTEXPLAIN__BLDEXP, "_rtnContextExplain::_buildExplain" )
   INT32 _rtnContextExplain::_buildExplain ( rtnContext * explainContext,
                                             BOOLEAN & hasMore )
   {
      INT32 rc = SDB_OK ;

      PD_TRACE_ENTRY( SDB_RTNCONTEXTEXPLAIN__BLDEXP ) ;

      BSONObjBuilder builder ;

      rc = _buildBSONNodeInfo( builder ) ;
      PD_RC_CHECK( rc, PDERROR, "Failed to build BSON for node information, "
                   "rc: %d", rc ) ;

      if ( _expOptions.isNeedDetail() )
      {
         rtnExplainOptions tempOptions( _expOptions ) ;
         tempOptions.setNeedFlatten( FALSE ) ;

         rc = _buildBSONQueryOptions( builder, _expOptions ) ;
         PD_RC_CHECK( rc, PDERROR, "Failed to build BSON for query options, "
                      "rc: %d", rc ) ;

         rc = _explainScanPath.toBSONExplainInfo( builder,
                                                  OPT_EXPINFO_MASK_ALL ) ;
         PD_RC_CHECK( rc, PDERROR, "Failed to build BSON for run information, "
                      "rc: %d", rc ) ;

         rc = _explainScanPath.toBSON( builder, tempOptions ) ;
         PD_RC_CHECK( rc, PDERROR, "Failed to build BSON for explain details, "
                      "rc: %d", rc ) ;
      }
      else
      {
         rc = _explainScanPath.toBSONBasic( builder, _expOptions ) ;
         PD_RC_CHECK( rc, PDERROR, "Failed to build BSON for basic explain, "
                      "rc: %d", rc ) ;

         rc = _explainScanPath.toBSONExplainInfo( builder,
                                                  OPT_EXPINFO_MASK_ALL ) ;
         PD_RC_CHECK( rc, PDERROR, "Failed to build BSON for run information, "
                      "rc: %d", rc ) ;
      }

      rc = explainContext->append( builder.obj() ) ;
      PD_RC_CHECK( rc, PDERROR, "Failed append explain result to "
                   "context [%lld], rc: %d",
                   explainContext->contextID(), rc ) ;

      hasMore = FALSE ;

   done :
      PD_TRACE_EXITRC( SDB_RTNCONTEXTEXPLAIN__BLDEXP, rc ) ;
      return rc ;

   error :
      goto done ;
   }

   /*
      _rtnExplainMainBase implement
    */
   _rtnExplainMainBase::_rtnExplainMainBase ()
   : _IRtnCtxDataProcessor(),
     _tempTimestamp(),
     _mainExplainOutputted( FALSE )
   {
   }

   _rtnExplainMainBase::~_rtnExplainMainBase ()
   {
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB_RTNEXPLAINMAINBASE_PROCESSDATA, "_rtnExplainMainBase::processData" )
   INT32 _rtnExplainMainBase::processData ( INT64 dataID, const CHAR * data,
                                            INT32 dataSize, INT32 dataNum )
   {
      INT32 rc = SDB_OK ;

      PD_TRACE_ENTRY( SDB_RTNEXPLAINMAINBASE_PROCESSDATA ) ;

      SDB_ASSERT( NULL != data, "data is invalid" ) ;
      SDB_ASSERT( dataSize >= 0, "data size is invalid" ) ;
      SDB_ASSERT( dataNum >= 0, "data number is invalid" ) ;

      rtnContextBuf contextBuf( data, dataSize, dataNum ) ;
      UINT32 explainNum = 0 ;

      while ( !contextBuf.eof() )
      {
         try
         {
            BSONObj explainResult ;
            ossTickDelta queryTime, waitTime ;
            BOOLEAN needChildExplain = FALSE ;
            BOOLEAN needParse = FALSE ;

            rc = contextBuf.nextObj( explainResult ) ;
            if ( SDB_DMS_EOC == rc )
            {
               rc = SDB_OK ;
               break ;
            }
            PD_RC_CHECK( rc, PDERROR, "Failed to get explain from buffer, "
                         "rc: %d", rc ) ;

            // check if node ID is needed for output
            needChildExplain = _needChildExplain( dataID, explainResult ) ;

            if ( _explainIDSet.end() == _explainIDSet.find( dataID ) )
            {
               ossTick startTimestamp, endTimestamp ;
               endTimestamp.sample() ;

               // not found in explained list, need parse the whole object
               // for detail mode
               needParse = _expOptions.isNeedDetail() ;

               rtnExplainTimestampList::iterator endIter =
                                          _endTimestampList.find( dataID ) ;
               if ( _endTimestampList.end() != endIter )
               {
                  endTimestamp = endIter->second ;
               }

               rtnExplainTimestampList::iterator startIter =
                                          _startTimestampList.find( dataID ) ;
               if ( _startTimestampList.end() != startIter &&
                    _needParallelProcess() )
               {
                  startTimestamp = startIter->second ;
               }
               else
               {
                  startTimestamp = _tempTimestamp ;
                  if ( !_needParallelProcess() )
                  {
                     // Not in parallel, re-use the start timestamp to save
                     // the start timestamp of the next sub context
                     _tempTimestamp.sample() ;
                  }
               }
               queryTime = endTimestamp - startTimestamp ;

               rtnExplainTimestampList::iterator waitIter =
                                          _waitTimestampList.find( dataID ) ;
               if ( _waitTimestampList.end() != waitIter )
               {
                  waitTime = waitIter->second - startTimestamp ;
               }
               else
               {
                  waitTime = endTimestamp - startTimestamp ;
               }

               PD_CHECK( NULL != getExplainMergePath(), SDB_SYS, error,
                         PDERROR, "Failed to get merge explain path" ) ;

               rc = getExplainMergePath()->addChildExplain(
                        explainResult, queryTime, waitTime, needParse,
                        needChildExplain, _expOptions ) ;
               PD_RC_CHECK( rc, PDERROR, "Failed to add child explain, "
                            "rc: %d", rc ) ;

               _explainIDSet.insert( dataID ) ;
            }
            else
            {
               // found in explained list
               needParse = FALSE ;

               rc = getExplainMergePath()->addChildExplain(
                        explainResult, queryTime, waitTime, needParse,
                        needChildExplain, _expOptions ) ;
               PD_RC_CHECK( rc, PDERROR, "Failed to add child explain, "
                            "rc: %d", rc ) ;
            }

            explainNum ++ ;
         }
         catch ( std::exception & e )
         {
            PD_LOG( PDERROR, "Failed to process explain data, received "
                    "unexpected error: %s", e.what() ) ;
            rc = SDB_INVALIDARG ;
            goto error ;
         }
      }

      PD_CHECK( explainNum == (UINT32)dataNum, SDB_SYS, error, PDERROR,
                "Failed to process explain data" ) ;

   done :
      PD_TRACE_EXITRC( SDB_RTNEXPLAINMAINBASE_PROCESSDATA, rc ) ;
      return SDB_OK ;

   error :
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB_RTNEXPLAINMAINBASE_CHKDATA, "_rtnExplainMainBase::checkData" )
   INT32 _rtnExplainMainBase::checkData ( INT64 dataID, const CHAR * data,
                                          INT32 dataSize, INT32 dataNum )
   {
      INT32 rc = SDB_OK ;

      PD_TRACE_ENTRY( SDB_RTNEXPLAINMAINBASE_CHKDATA ) ;

      if ( _waitTimestampList.end() == _waitTimestampList.find( dataID ) )
      {
         ossTick waitTimestamp ;
         waitTimestamp.sample() ;
         _waitTimestampList[ dataID ] = waitTimestamp ;
      }

      ossTick endTimestamp ;
      endTimestamp.sample() ;
      _endTimestampList[ dataID ] = endTimestamp ;

      PD_TRACE_EXITRC( SDB_RTNEXPLAINMAINBASE_CHKDATA, rc ) ;

      return rc ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB_RTNEXPLAINMAINBASE_CHKSUBCTX, "_rtnExplainMainBase::checkSubContext" )
   INT32 _rtnExplainMainBase::checkSubContext ( INT64 dataID )
   {
      INT32 rc = SDB_OK ;

      PD_TRACE_ENTRY( SDB_RTNEXPLAINMAINBASE_CHKSUBCTX ) ;

      if ( _startTimestampList.end() == _startTimestampList.find( dataID ) )
      {
         ossTick startTimestamp ;
         startTimestamp.sample() ;
         _startTimestampList[ dataID ] = startTimestamp ;
      }

      PD_TRACE_EXITRC( SDB_RTNEXPLAINMAINBASE_CHKSUBCTX, rc ) ;

      return rc ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB_RTNEXPLAINMAINBASE__REGEXPPROC, "_rtnExplainMainBase::_registerExplainProcessor" )
   INT32 _rtnExplainMainBase::_registerExplainProcessor ( rtnContext * context )
   {
      INT32 rc = SDB_OK ;

      PD_TRACE_ENTRY( SDB_RTNEXPLAINMAINBASE__REGEXPPROC ) ;

      rtnContextMain * mainContext = dynamic_cast<rtnContextMain *>( context ) ;

      PD_CHECK( NULL != mainContext, SDB_SYS, error, PDERROR,
                "Failed to get main context" ) ;

      mainContext->registerProcessor( this ) ;

      _tempTimestamp.sample() ;

   done :
      PD_TRACE_EXITRC( SDB_RTNEXPLAINMAINBASE__REGEXPPROC, rc ) ;
      return rc ;

   error :
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB_RTNEXPLAINMAINBASE__UNREGEXPPROC, "_rtnExplainMainBase::_unregisterExplainProcessor" )
   INT32 _rtnExplainMainBase::_unregisterExplainProcessor ( rtnContext * context )
   {
      INT32 rc = SDB_OK ;

      PD_TRACE_ENTRY( SDB_RTNEXPLAINMAINBASE__UNREGEXPPROC ) ;

      rtnContextMain * mainContext = dynamic_cast<rtnContextMain *>( context ) ;

      PD_CHECK( NULL != mainContext, SDB_SYS, error, PDERROR,
                "Failed to get main context" ) ;

      mainContext->unregisterProcessor( this ) ;

   done :
      PD_TRACE_EXITRC( SDB_RTNEXPLAINMAINBASE__UNREGEXPPROC, rc ) ;
      return rc ;

   error :
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB_RTNEXPLAINMAINBASE__FINISUBCTX, "_rtnExplainMainBase::_finishSubContext" )
   INT32 _rtnExplainMainBase::_finishSubContext ( rtnContext * context,
                                                  pmdEDUCB * cb )
   {
      INT32 rc = SDB_OK ;

      PD_TRACE_ENTRY( SDB_RTNEXPLAINMAINBASE__FINISUBCTX ) ;

      rtnContextMain * mainContext = dynamic_cast<rtnContextMain *>( context ) ;

      PD_CHECK( NULL != mainContext, SDB_SYS, error, PDERROR,
                "Failed to get main-context" ) ;

      rc = mainContext->reopenForExplain( 0, -1 ) ;
      PD_RC_CHECK( rc, PDERROR, "Failed to reopen main-context [%lld], "
                   "rc: %d", mainContext->contextID(), rc ) ;

      for ( ; ; )
      {
         rtnContextBuf contextBuffer ;
         rc = mainContext->getMore( -1, contextBuffer, cb ) ;
         if ( SDB_DMS_EOC == rc )
         {
            rc = SDB_OK ;
            break ;
         }
         PD_RC_CHECK( rc, PDERROR, "Failed to get more from main-context "
                      "[%lld], rc: %d", mainContext->contextID(), rc ) ;
      }

      rc = _unregisterExplainProcessor( context ) ;
      PD_RC_CHECK( rc, PDERROR, "Failed to unregister explain processor, "
                   "rc: %d", rc ) ;

   done :
      PD_TRACE_EXITRC( SDB_RTNEXPLAINMAINBASE__FINISUBCTX, rc ) ;
      return rc ;

   error :
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB_RTNEXPLAINMAINBASE__BLDEXP, "_rtnExplainMainBase::_buildExplain" )
   INT32 _rtnExplainMainBase::_buildExplain ( rtnContext * explainContext,
                                              BOOLEAN & hasMore )
   {
      INT32 rc = SDB_OK ;

      PD_TRACE_ENTRY( SDB_RTNEXPLAINMAINBASE__BLDEXP ) ;

      SDB_ASSERT( explainContext, "explain context is invalid" ) ;

      PD_CHECK( NULL != getExplainMergePath(), SDB_SYS, error, PDERROR,
                "Failed to get merge explain path" ) ;

      hasMore = FALSE ;

      if ( _expOptions.isNeedDetail() &&
           _expOptions.isNeedFlatten() )
      {
         if ( !_mainExplainOutputted )
         {
            rc = _buildMainExplain( explainContext, hasMore ) ;
            PD_RC_CHECK( rc, PDERROR, "Failed to build main explain, "
                         "rc: %d", rc ) ;
         }

         rc = _buildSubExplains( explainContext, TRUE, hasMore ) ;
         PD_RC_CHECK( rc, PDERROR, "Failed to build sub explains, "
                      "rc: %d", rc ) ;
      }
      else if ( _expOptions.isNeedDetail() )
      {
         rc = _buildMainExplain( explainContext, hasMore ) ;
         PD_RC_CHECK( rc, PDERROR, "Failed to build main explain, "
                      "rc: %d", rc ) ;
      }
      else
      {
         rc = _buildSimpleExplain( explainContext, hasMore ) ;
         PD_RC_CHECK( rc, PDERROR, "Failed to build simple explain, "
                      "rc: %d", rc ) ;
      }

      _mainExplainOutputted = TRUE ;

   done :
      PD_TRACE_EXITRC( SDB_RTNEXPLAINMAINBASE__BLDEXP, rc ) ;
      return rc ;

   error :
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB_RTNEXPLAINMAINBASE__BLDMAINEXP, "_rtnExplainMainBase::_buildMainExplain" )
   INT32 _rtnExplainMainBase::_buildMainExplain ( rtnContext * explainContext,
                                                  BOOLEAN & hasMore )
   {
      INT32 rc = SDB_OK ;

      PD_TRACE_ENTRY( SDB_RTNEXPLAINMAINBASE__BLDMAINEXP ) ;

      if ( NULL != getExplainMergePath() )
      {
         BSONObjBuilder builder ;

         rc = _buildBSONNodeInfo( builder ) ;
         PD_RC_CHECK( rc, PDERROR, "Failed to build BSON for node info, "
                      "rc: %d", rc ) ;

         rc = _buildBSONQueryOptions( builder, _expOptions ) ;
         PD_RC_CHECK( rc, PDERROR, "Failed to build BSON for query options, "
                      "rc: %d", rc ) ;

         rc = getExplainMergePath()->toBSONExplainInfo( builder,
                                                        _getExplainInfoMask() ) ;
         PD_RC_CHECK( rc, PDERROR, "Failed to build BSON for run information, "
                      "rc: %d", rc ) ;

         rc = getExplainMergePath()->toBSON( builder, _expOptions ) ;
         PD_RC_CHECK( rc, PDERROR, "Failed to output BSON from explain path, "
                      "rc: %d", rc ) ;

         rc = explainContext->append( builder.obj() ) ;
         PD_RC_CHECK( rc, PDERROR, "Failed append explain result to context "
                      "[%lld], rc: %d", explainContext->contextID(), rc ) ;

         hasMore = _expOptions.isNeedFlatten() ;
      }
      else
      {
         PD_LOG( PDERROR, "Failed to get merge explain path" ) ;
         rc = SDB_SYS ;
         goto error ;
      }

   done :
      PD_TRACE_EXITRC( SDB_RTNEXPLAINMAINBASE__BLDMAINEXP, rc ) ;
      return rc ;

   error :
      goto done ;
   }

   // PD_TRACE_DECLARE_FUNCTION ( SDB_RTNEXPLAINMAINBASE__BLDSIMPEXP, "_rtnExplainMainBase::_buildSubExplains" )
   INT32 _rtnExplainMainBase::_buildSubExplains ( rtnContext * explainContext,
                                                  BOOLEAN needSort,
                                                  BOOLEAN & hasMore )
   {
      INT32 rc = SDB_OK ;

      PD_TRACE_ENTRY( SDB_RTNEXPLAINMAINBASE__BLDSIMPEXP ) ;

      PD_CHECK( NULL != getExplainMergePath(), SDB_SYS, error, PDERROR,
                "Failed to get merge explain path" ) ;

      if ( needSort )
      {
         rc = getExplainMergePath()->sortChildExplains() ;
         PD_RC_CHECK( rc, PDERROR, "Failed to sort child explains, "
                      "rc: %d", rc ) ;
      }

      {
         optExplainResultList & childExplainList =
                                 getExplainMergePath()->getChildExplains() ;

         while ( !childExplainList.empty() &&
                 explainContext->buffEndOffset() < RTN_MAX_EXPLAIN_BUFFER_SIZE &&
                 explainContext->buffEndOffset() + DMS_RECORD_MAX_SZ < RTN_RESULTBUFFER_SIZE_MAX )
         {
            rc = explainContext->append( childExplainList.front() ) ;
            PD_RC_CHECK( rc, PDERROR, "Failed append explain result to "
                         "context [%lld], rc: %d",
                         explainContext->contextID(), rc ) ;

            childExplainList.pop_front() ;
         }

         hasMore = !childExplainList.empty() ;
      }

   done :
      PD_TRACE_EXITRC( SDB_RTNEXPLAINMAINBASE__BLDSIMPEXP, rc ) ;
      return rc ;

   error :
      goto done ;
   }
}
