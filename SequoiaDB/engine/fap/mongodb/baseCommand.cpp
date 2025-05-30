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

   Source File Name = baseCommand.cpp

   Descriptive Name =

   When/how to use: this program may be used on binary and text-formatted
   versions of PMD component. This file contains functions for agent processing.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          06/01/2015  LZ  Initial Draft

   Last Changed =

*******************************************************************************/
#include "baseCommand.hpp"

baseCommand::baseCommand( const CHAR *name, const CHAR *secondName )
{
   commandMgr *mgr = commandMgr::instance() ;
   if ( NULL != mgr )
   {
      mgr->addCommand( name, this ) ;
      _name = name ;
      if ( NULL != secondName )
      {
         mgr->addCommand( secondName, this ) ;
      }
   }
}

commandMgr* commandMgr::instance()
{
   static commandMgr mgr ;
   return &mgr ;
}
