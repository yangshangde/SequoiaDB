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

   Source File Name = rtnMsg.cpp

   Descriptive Name = Runtime Message

   When/how to use: this program may be used on binary and text-formatted
   versions of runtime component. This file contains code logic for
   message processing.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================

   Last Changed =

*******************************************************************************/

#include "core.hpp"
#include "rtn.hpp"
#include "pd.hpp"


namespace engine
{

   INT32 rtnMsg ( MsgOpMsg *pMsg )
   {
      INT32 rc = SDB_OK ;

      if ( (UINT32)pMsg->header.messageLength < sizeof( MsgHeader ) )
      {
         PD_LOG( PDERROR, "Recieve invalid msg[length: %d]",
                 pMsg->header.messageLength ) ;
         rc = SDB_INVALIDARG ;
      }
      else
      {
         CHAR *message = &pMsg->msg[0] ;
         message[ pMsg->header.messageLength - sizeof(MsgHeader) - 1 ] = 0 ;
         PD_LOG ( getPDLevel(), "%s", message ) ;
      }
      return rc ;
   }

}

