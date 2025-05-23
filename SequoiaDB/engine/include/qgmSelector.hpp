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

   Source File Name = qgmSelector.hpp

   Descriptive Name =

   When/how to use: this program may be used on binary and text-formatted
   versions of PMD component. This file contains functions for agent processing.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          09/14/2012  YW  Initial Draft

   Last Changed =

******************************************************************************/

#ifndef QGMSELECTOR_HPP_
#define QGMSELECTOR_HPP_

#include "qgmDef.hpp"

namespace engine
{
   class _qgmSelector : public _utilPooledObject
   {
   public:
      _qgmSelector() ;
      virtual ~_qgmSelector() ;

   public:
      OSS_INLINE BOOLEAN empty()const{ return _selector.empty() ;}

      OSS_INLINE BOOLEAN needSelect()const{return _needSelect ;}

      INT32 load( const qgmOPFieldVec &op ) ;

      INT32 select( const BSONObj &src, BSONObj &out ) const;

      INT32 select( const qgmFetchOut &src, BSONObj &out ) const ;

      BSONObj selector() const;

      string toString() const ;

   private:
      INT32 _createValueWithExpr( const BSONElement &e,
                                  const CHAR *fieldName,
                                  const _qgmSelectorExpr &expr,
                                  BSONObjBuilder &builder ) const ;

   private:
      qgmOPFieldVec _selector ;
      BOOLEAN _needSelect ;
   } ;

   typedef class _qgmSelector qgmSelector ;
}

#endif

