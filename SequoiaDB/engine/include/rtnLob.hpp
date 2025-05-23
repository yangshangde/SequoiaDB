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

   Source File Name = rtnLob.hpp

   Descriptive Name =

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          07/31/2014  YW  Initial Draft

   Last Changed =

*******************************************************************************/

#ifndef RTN_LOB_HPP_
#define RTN_LOB_HPP_

#include "rtn.hpp"
#include "dmsLobDef.hpp"

using namespace bson ;

// SHARE_READ | WRITE mode
#define SDB_IS_LOBSREADWRITE_MODE( mode ) \
   ( (SDB_LOB_MODE_WRITE | SDB_LOB_MODE_SHAREREAD) == ( mode ) )

#define SDB_HAS_LOBWRITE_MODE( mode ) \
   ( SDB_LOB_MODE_WRITE == ( mode ) || \
     SDB_IS_LOBSREADWRITE_MODE( mode ) )

#define SDB_IS_LOBREADONLY_MODE( mode ) \
   ( SDB_LOB_MODE_READ == ( mode ) || \
     SDB_LOB_MODE_SHAREREAD == ( mode ) )

#define SDB_IS_VALID_LOB_MODE( mode ) \
   ( SDB_IS_LOBREADONLY_MODE( mode ) || \
     SDB_HAS_LOBWRITE_MODE( mode ) || \
     SDB_LOB_MODE_CREATEONLY == ( mode ) || \
     SDB_LOB_MODE_REMOVE == ( mode ) || \
     SDB_LOB_MODE_TRUNCATE == ( mode ) )

namespace engine
{
   class _rtnLobStream ;

   /// interfaces for stream

   /*
      Note: The pStream will be takeover in cases both failed and succed
   */
   INT32 rtnOpenLob( const BSONObj &lob,
                     SINT32 flags,
                     _pmdEDUCB *cb,
                     SDB_DPSCB *dpsCB,
                     _rtnLobStream *pStream,
                     SINT16 w,
                     SINT64 &contextID,
                     rtnContextBuf &buffObj ) ;

   INT32 rtnWriteLob( SINT64 contextID,
                      pmdEDUCB *cb,
                      UINT32 len,
                      const CHAR *buf,
                      INT64 lobOffset,
                      rtnContextBuf *errBuf = NULL ) ;

   INT32 rtnReadLob( SINT64 contextID,
                     pmdEDUCB *cb,
                     UINT32 len,
                     SINT64 offset,
                     const CHAR **buf,
                     UINT32 &read,
                     rtnContextBuf *errBuf = NULL ) ;

   INT32 rtnLockLob( SINT64 contextID,
                     pmdEDUCB *cb,
                     INT64 offset,
                     INT64 length,
                     rtnContextBuf *errBuf = NULL ) ;

   INT32 rtnGetLobRTDetail( SINT64 contextID, pmdEDUCB *cb,
                            rtnContextBuf *bufObj = NULL ) ;

   INT32 rtnCloseLob( SINT64 contextID,
                      pmdEDUCB *cb,
                      rtnContextBuf *bufObj = NULL ) ;

   INT32 rtnRemoveLob( const BSONObj &lob,
                       INT32 flags,
                       SINT16 w,
                       _pmdEDUCB *cb,
                       SDB_DPSCB *dpsCB ) ;

   INT32 rtnTruncateLob( const BSONObj &lob,
                         INT32 flags,
                         SINT16 w,
                         _pmdEDUCB *cb,
                         SDB_DPSCB *dpsCB ) ;

   INT32 rtnGetLobMetaData( SINT64 contextID,
                            pmdEDUCB *cb,
                            BSONObj &meta ) ;

   /// interfaces for non-stream
   INT32 rtnCreateLob( const CHAR *fullName,
                       const bson::OID &oid,
                       _dmsLobMeta &meta,
                       pmdEDUCB *cb,
                       SINT16 w,
                       SDB_DPSCB *dpsCB,
                       dmsStorageUnit *su = NULL,
                       dmsMBContext *mbContext = NULL ) ;

   INT32 rtnGetLobMetaData( const CHAR *fullName,
                            const bson::OID &oid,
                            pmdEDUCB *cb,
                            dmsLobMeta &meta,
                            dmsStorageUnit *su = NULL,
                            dmsMBContext *mbContext = NULL,
                            BOOLEAN allowUncompleted = FALSE ) ;

   INT32 rtnWriteLob( const CHAR *fullName,
                      const bson::OID &oid,
                      UINT32 sequence,
                      UINT32 offset,
                      UINT32 len,
                      const CHAR *data,
                      pmdEDUCB *cb,
                      SINT16 w,
                      SDB_DPSCB *dpsCB,
                      dmsStorageUnit *su = NULL,
                      dmsMBContext *mbContext = NULL ) ;

   INT32 rtnWriteOrUpdateLob( const CHAR *fullName,
                              const bson::OID &oid,
                              UINT32 sequence,
                              UINT32 offset,
                              UINT32 len,
                              const CHAR *data,
                              pmdEDUCB *cb,
                              SINT16 w,
                              SDB_DPSCB *dpsCB,
                              dmsStorageUnit *su = NULL,
                              dmsMBContext *mbContext = NULL,
                              BOOLEAN* hasUpdated = NULL ) ;

   INT32 rtnUpdateLob( const CHAR *fullName,
                       const bson::OID &oid,
                       UINT32 sequence,
                       UINT32 offset,
                       UINT32 len,
                       const CHAR *data,
                       pmdEDUCB *cb,
                       SINT16 w,
                       SDB_DPSCB *dpsCB,
                       dmsStorageUnit *su = NULL,
                       dmsMBContext *mbContext = NULL ) ;

   INT32 rtnReadLob( const CHAR *fullName,
                     const bson::OID &oid,
                     UINT32 sequence,
                     UINT32 offset,
                     UINT32 len,
                     pmdEDUCB *cb,
                     CHAR *data,
                     UINT32 &read,
                     dmsStorageUnit *su = NULL,
                     dmsMBContext *mbContext = NULL ) ;


   INT32 rtnCloseLob( const CHAR *fullName,
                      const bson::OID &oid,
                      const dmsLobMeta &meta,
                      pmdEDUCB *cb,
                      SINT16 w,
                      SDB_DPSCB *dpsCB,
                      dmsStorageUnit *su = NULL,
                      dmsMBContext *mbContext = NULL ) ;

   INT32 rtnRemoveLobPiece( const CHAR *fullName,
                            const bson::OID &oid,
                            UINT32 sequence,
                            pmdEDUCB *cb,
                            SINT16 w,
                            SDB_DPSCB *dpsCB,
                            dmsStorageUnit *su = NULL,
                            dmsMBContext *mbContext = NULL,
                            BOOLEAN onlyRemoveNewPiece = FALSE,
                            const CHAR *pOldData = NULL ) ;

   INT32 rtnQueryAndInvalidateLob( const CHAR *fullName,
                                   const bson::OID &oid,
                                   pmdEDUCB *cb,
                                   SINT16 w,
                                   SDB_DPSCB *dpsCB,
                                   dmsLobMeta &meta,
                                   dmsStorageUnit *su = NULL,
                                   dmsMBContext *mbContext = NULL,
                                   BOOLEAN allowUncompleted = FALSE ) ;

   INT32 rtnCreateLobID( const BSONObj &createLobIDObj, bson::OID &oid ) ;

}

#endif

