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

   Source File Name = rtnContextShdOfLob.hpp

   Descriptive Name =

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          07/31/2014  YW  Initial Draft

   Last Changed =

*******************************************************************************/

#ifndef RTN_CONTEXTSHDOFLOB_HPP_
#define RTN_CONTEXTSHDOFLOB_HPP_

#include "rtnContext.hpp"
#include "rtnLobPieces.hpp"
#include "rtnLobAccessManager.hpp"
#include "dmsLobDef.hpp"
#include "ossMemPool.hpp"
#include "monInterface.hpp"

namespace engine
{
   class _dmsMBContext ;
   class _dmsStorageUnit ;
   class _SDB_DMSCB ;

   class _rtnContextShdOfLob : public _rtnContextBase, public _IMonSubmitEvent
   {
      DECLARE_RTN_CTX_AUTO_REGISTER( _rtnContextShdOfLob )
   public:
      _rtnContextShdOfLob( INT64 contextID, UINT64 eduID ) ;
      virtual ~_rtnContextShdOfLob() ;

   public:
      virtual const CHAR*      name() const { return "SHARD_OF_LOB" ; };
      virtual RTN_CONTEXT_TYPE getType() const { return RTN_CONTEXT_SHARD_OF_LOB ; }
      virtual _dmsStorageUnit*  getSU () ;

      virtual BOOLEAN isWrite() const ;

      virtual const CHAR *getProcessName() const
      {
         return getFullName() ;
      }

      virtual UINT32 getSULogicalID() const
      {
         return _suLogicalID ;
      }

      const CHAR *getSubCLName() const
      {
         return _subCLName.empty() ? NULL : _subCLName.c_str() ;
      }

   public:
      INT32 open( const bson::BSONObj &lob,
                  SINT32 flag,
                  SINT32 version,
                  SINT16 w,
                  SDB_DPSCB *dpsCB,
                  _pmdEDUCB *cb,
                  const CHAR **data,
                  UINT32 &read ) ;

      INT32 write( UINT32 sequence,
                   UINT32 offset,
                   UINT32 len,
                   const CHAR *data,
                   _pmdEDUCB *cb,
                   BOOLEAN orUpdate = FALSE ) ;

      INT32 readv( const MsgLobTuple *tuples,
                   UINT32 cnt,
                   _pmdEDUCB *cb,
                   const CHAR **data,
                   UINT32 &read ) ;

      INT32 remove( UINT32 sequence,
                    _pmdEDUCB *cb ) ;

      INT32 update( UINT32 sequence,
                    UINT32 offset,
                    UINT32 len,
                    const CHAR *data,
                    _pmdEDUCB *cb ) ;

      INT32 lock( _pmdEDUCB *cb,
                  INT64 offset,
                  INT64 length ) ;

      INT32 close( _pmdEDUCB *cb ) ;

      INT32 getLobRTDetail( _pmdEDUCB *cb, BSONObj &detail ) ;

   public:
      const CHAR *getFullName() const
      {
         return _fullName.c_str() ;
      }

      INT32 getMode() const
      {
         return _mode ;
      }

      BOOLEAN isMainShard() const
      {
         return _isMainShd ;
      }

      SINT32 getVersion() const
      {
         return _version ;
      }

      INT16 getW() const
      {
         return _w ;
      }

      const bson::OID &getOID() const
      {
         return _oid ;
      }

   public:
      virtual void onSubmit( const monAppCB &delta ) ;

   protected:
      virtual INT32 _prepareData( _pmdEDUCB *cb ) ;
      virtual void  _toString( stringstream &ss ) ;

   private:
      INT32 _open( _pmdEDUCB *cb, _utilSectionMgr &sectionMgr,
                   const CHAR **data, UINT32 &read ) ;

      INT32 _write( UINT32 sequence,
                    UINT32 offset,
                    UINT32 len,
                    const CHAR *data,
                    _pmdEDUCB *cb,
                    BOOLEAN orUpdate = FALSE ) ;

      INT32 _getAccessPrivilege() ;

      INT32 _parseOpenArgs( const bson::BSONObj &lob,
                            _utilSectionMgr &sectionMgr ) ;

      INT32 _meta2Obj( bson::BSONObj &obj ) ;

      INT32 _rollback( _pmdEDUCB *cb ) ;

      INT32 _extendBuf( UINT32 len ) ;

      const CHAR *_getRealCLName() ;

      UINT32 _getDataLen( UINT32 sequence, UINT32 offset, UINT32 len ) ;

      void   _increaseLobOpCount( _pmdEDUCB *cb ) ;

   private:
      std::string          _fullName ;
      std::string          _subCLName ;
      bson::OID            _oid ;
      BSONObj              _metaObj ;
      INT32                _mode ;
      INT32                _flags ;
      BOOLEAN              _isMainShd ;
      SINT16               _w ;
      SINT32               _version ;
      _dmsLobMeta          _meta ;
      SDB_DPSCB*           _dpsCB ;
      BOOLEAN              _closeWithException ;
      CHAR*                _buf ;
      UINT32               _bufLen ;
      const CHAR*          _pData ;
      UINT32               _dataLen ;
      INT64                _offset ;
      ossPoolSet<UINT32>  _written ;
      _rtnLobPiecesInfo    _lobPieces ;
      _rtnLobAccessInfo*   _accessInfo ;

      _dmsStorageUnit*     _su ;
      UINT32               _suLogicalID ;
      _dmsMBContext*       _mbContext ;
      _SDB_DMSCB*          _dmsCB ;
      BOOLEAN              _reopened ;
      BOOLEAN              _isMetaWrote ; // used in CREATEONLY mode
      INT32                _opType ;
      BOOLEAN              _hasSubmitMetric ;
      _monAppCB            _totalDeltaMonApp ; // keep the changes of the session
   } ;
   typedef class _rtnContextShdOfLob rtnContextShdOfLob ;
}

#endif

