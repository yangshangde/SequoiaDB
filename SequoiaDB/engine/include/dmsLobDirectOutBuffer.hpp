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

   Source File Name = dmsLobDirectOutBuffer.hpp

   Descriptive Name =

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          07/31/2014  YW  Initial Draft

   Last Changed =

*******************************************************************************/

#ifndef DMS_LOBDIRECTOUTBUFFER_HPP_
#define DMS_LOBDIRECTOUTBUFFER_HPP_

#include "dmsLobDirectBuffer.hpp"
#include "utilCache.hpp"

namespace engine
{
   /*
      _dmsLobDirectOutBuffer define
   */
   class _dmsLobDirectOutBuffer : public _dmsLobDirectBuffer 
   {
   public:
      _dmsLobDirectOutBuffer( const CHAR *usrBuf,
                              UINT32 size,
                              UINT32 offset,
                              BOOLEAN needAligned,
                              IExecutor *cb,
                              INT32 pageID,
                              UINT32 newestMask,
                              utilCachFileBase *pFile ) ;
      virtual ~_dmsLobDirectOutBuffer() ;

   public:
      virtual  INT32 doit( const tuple **pTuple ) ;
      virtual  void  done() ;

   private:
      utilCachFileBase     *_pFile ;
      INT32                _pageID ;
      UINT32               _newestMask ;

   } ;
   typedef class _dmsLobDirectOutBuffer dmsLobDirectOutBuffer ;
}

#endif //DMS_LOBDIRECTOUTBUFFER_HPP_

