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

   Source File Name = rtnSQLFunc.cpp

   Descriptive Name =

   When/how to use: this program may be used on binary and text-formatted
   versions of PMD component. This file contains declare for QGM operators

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          04/09/2013  YW  Initial Draft

   Last Changed =

*******************************************************************************/

#include "rtnSQLFunc.hpp"
#include <sstream>

namespace engine
{
   string _rtnSQLFunc::toString() const
   {
      stringstream ss ;

      ss << "{name:"
         << _name.c_str() ;
      if ( !_alias.empty() )
      {
         ss << ",alias:"
            << _alias.toString() ;
      }
      if ( !_param.empty() )
      {
         ss << ",param:[" ;
         qgmOPFieldVec::const_iterator itr = _param.begin() ;
         for ( ; itr != _param.end(); itr++ )
         {
            ss <<"{" << itr->toString() << "}," ;
         }
         ss.seekp((INT32)ss.tellp()-1 ) ;
         ss << "]" ;
      }

      return ss.str() ;
   }
}
