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

   Source File Name = qgmPlReturn.hpp

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

#ifndef QGMPLRETURN_HPP_
#define QGMPLRETURN_HPP_

#include "qgmPlan.hpp"
#include "qgmUtil.hpp"
#include <sstream>

namespace engine
{
   class _qgmPlReturn : public _qgmPlan
   {
   public:
      _qgmPlReturn() ;
      virtual ~_qgmPlReturn() ;

   public:
     virtual string toString() const
     {
        std::stringstream ss ;
        ss << "Type:" << qgmPlanType( _type ) << '\n' ;
        return ss.str() ;
     }

   private:
      virtual INT32 _execute( _pmdEDUCB *eduCB ) ;

      virtual INT32 _fetchNext ( qgmFetchOut &next ) ;
   } ;
   typedef class _qgmPlReturn qgmPlReturn ;
}

#endif

