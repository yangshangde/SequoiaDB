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

   Source File Name = sptUsrCmd.cpp

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          31/03/2014  YW  Initial Draft

   Last Changed =

*******************************************************************************/

#include "sptUsrCmd.hpp"

using namespace bson ;

namespace engine
{
   JS_MEMBER_FUNC_DEFINE( _sptUsrCmd, exec )
   JS_CONSTRUCT_FUNC_DEFINE( _sptUsrCmd, construct )
   JS_DESTRUCT_FUNC_DEFINE( _sptUsrCmd, destruct )
   JS_MEMBER_FUNC_DEFINE( _sptUsrCmd, toString )
   JS_MEMBER_FUNC_DEFINE( _sptUsrCmd, getLastRet )
   JS_MEMBER_FUNC_DEFINE( _sptUsrCmd, getLastOut )
   JS_MEMBER_FUNC_DEFINE( _sptUsrCmd, start )
   JS_MEMBER_FUNC_DEFINE( _sptUsrCmd, getCommand )
   JS_MEMBER_FUNC_DEFINE( _sptUsrCmd, getInfo )

   JS_BEGIN_MAPPING( _sptUsrCmd, "Cmd" )
      JS_ADD_CONSTRUCT_FUNC( construct )
      JS_ADD_DESTRUCT_FUNC( destruct )
      JS_ADD_MEMBER_FUNC_WITHATTR( "_getLastRet", getLastRet, 0 )
      JS_ADD_MEMBER_FUNC_WITHATTR( "_getLastOut", getLastOut, 0 )
      JS_ADD_MEMBER_FUNC_WITHATTR( "_run", exec, 0 )
      JS_ADD_MEMBER_FUNC_WITHATTR( "_start", start, 0 )
      JS_ADD_MEMBER_FUNC_WITHATTR( "_getCommand", getCommand, 0 )
      JS_ADD_MEMBER_FUNC_WITHATTR( "_getInfo", getInfo, 0 )
      JS_ADD_MEMBER_FUNC( "toString", toString )
   JS_MAPPING_END()


   _sptUsrCmd::_sptUsrCmd()
   {
   }

   _sptUsrCmd::~_sptUsrCmd()
   {
   }

   INT32 _sptUsrCmd::construct( const _sptArguments &arg,
                                _sptReturnVal &rval,
                                bson::BSONObj &detail )
   {
      return SDB_OK ;
   }

   INT32 _sptUsrCmd::destruct()
   {
      return SDB_OK ;
   }

   INT32 _sptUsrCmd::toString( const _sptArguments & arg,
                               _sptReturnVal & rval,
                               BSONObj & detail )
   {
      rval.getReturnVal().setValue( "CommandRunner" ) ;
      return SDB_OK ;
   }

   INT32 _sptUsrCmd::getInfo( const _sptArguments & arg,
                              _sptReturnVal & rval,
                              BSONObj & detail )
   {
      INT32 rc = SDB_OK ;
      BSONObj remoteInfo ;
      BSONObjBuilder builder ;
      if ( 0 < arg.argc() )
      {
         rc = arg.getBsonobj( 0, remoteInfo ) ;
         if ( SDB_OK != rc )
         {
            detail = BSON( SPT_ERR << "remoteInfo must be obj" ) ;
            goto error ;
         }
      }
      builder.append( "type", "Cmd" ) ;
      builder.appendElements( remoteInfo ) ;

      rval.getReturnVal().setValue( builder.obj() ) ;
   done:
      return rc ;
   error:
      goto done ;

   }

   INT32 _sptUsrCmd::getLastRet( const _sptArguments & arg,
                                 _sptReturnVal & rval,
                                 BSONObj & detail )
   {
      INT32 rc = SDB_OK ;
      string err ;
      UINT32 lastRet ;

      rc = _cmdCommon.getLastRet( err, lastRet ) ;
      if( SDB_OK != rc )
      {
         detail = BSON( SPT_ERR << err.c_str() ) ;
         goto error ;
      }
      rval.getReturnVal().setValue( lastRet ) ;
   done:
      return rc ;
   error:
      goto done ;
   }

   INT32 _sptUsrCmd::getLastOut( const _sptArguments & arg,
                                 _sptReturnVal & rval,
                                 BSONObj & detail )
   {
      INT32 rc = SDB_OK ;
      string err ;
      string lastOut ;

      rc = _cmdCommon.getLastOut( err, lastOut ) ;
      if( SDB_OK != rc )
      {
         detail = BSON( SPT_ERR << err.c_str() ) ;
         goto error ;
      }
      rval.getReturnVal().setValue( lastOut ) ;
   done:
      return rc ;
   error:
      goto done ;
   }

   INT32 _sptUsrCmd::getCommand( const _sptArguments & arg,
                                 _sptReturnVal & rval,
                                 BSONObj & detail )
   {
      INT32 rc = SDB_OK ;
      string err ;
      string lastCommand ;

      rc = _cmdCommon.getCommand( err, lastCommand ) ;
      if( SDB_OK != rc )
      {
         detail = BSON( SPT_ERR << err.c_str() ) ;
         goto error ;
      }
      rval.getReturnVal().setValue( lastCommand ) ;
   done:
      return rc ;
   error:
      goto done ;
   }

   INT32 _sptUsrCmd::exec( const _sptArguments &arg,
                           _sptReturnVal &rval,
                           bson::BSONObj &detail )
   {
      INT32 rc = SDB_OK ;
      string ev ;
      UINT32 timeout = 0 ;
      UINT32 useShell = TRUE ;
      string command ;
      string err ;
      string strOut ;

      rc = arg.getString( 0, command ) ;
      if ( SDB_OK != rc )
      {
         rc = SDB_INVALIDARG ;
         detail = BSON( SPT_ERR << "cmd must be config" ) ;
         goto error ;
      }

      rc = arg.getString( 1, ev ) ;
      if ( SDB_OK != rc && SDB_OUT_OF_BOUND != rc )
      {
         rc = SDB_INVALIDARG ;
         detail = BSON( SPT_ERR << "environment should be a string" ) ;
         goto error ;
      }

      rc = arg.getNative( 2, (void*)&timeout, SPT_NATIVE_INT32 ) ;
      if ( SDB_OK != rc && SDB_OUT_OF_BOUND != rc )
      {
         rc = SDB_INVALIDARG ;
         detail = BSON( SPT_ERR << "timeout should be a number" ) ;
         goto error ;
      }
      rc = SDB_OK ;

      // useShell, default : 1
      rc = arg.getNative( 3, (void*)&useShell, SPT_NATIVE_INT32 ) ;
      if ( SDB_OK != rc && SDB_OUT_OF_BOUND != rc )
      {
         rc = SDB_INVALIDARG ;
         detail = BSON( SPT_ERR << "useShell should be a number" ) ;
         goto error ;
      }
      rc = SDB_OK ;

      rc = _cmdCommon.exec( command, ev, timeout, useShell, err, strOut ) ;
      if( SDB_OK != rc )
      {
         if( err.empty() )
         {
            stringstream ss ;
            ss << "Run command(\"" << command << "\") return code is " << rc ;
            detail = BSON( SPT_ERR << ss.str() ) ;
         }
         else
         {
            detail = BSON( SPT_ERR << err.c_str() ) ;
         }
         goto error ;
      }

      rval.getReturnVal().setValue( strOut ) ;
   done:
      return rc ;
   error:
      goto done ;
   }

   INT32 _sptUsrCmd::start( const _sptArguments & arg,
                            _sptReturnVal & rval,
                            BSONObj & detail )
   {
      INT32 rc = SDB_OK ;
      string ev ;
      string command ;
      string err ;
      INT32 pid ;
      UINT32 useShell = 1 ;
      UINT32 timeout  = 100 ;
      string retStr ;

      rc = arg.getString( 0, command ) ;
      if ( SDB_OK != rc )
      {
         rc = SDB_INVALIDARG ;
         detail = BSON( SPT_ERR << "cmd must be config" ) ;
         goto error ;
      }

      rc = arg.getString( 1, ev ) ;
      if ( SDB_OK != rc && SDB_OUT_OF_BOUND != rc )
      {
         rc = SDB_INVALIDARG ;
         detail = BSON( SPT_ERR << "environment should be a string" ) ;
         goto error ;
      }

      // useShell, default : 1
      rc = arg.getNative( 2, (void*)&useShell, SPT_NATIVE_INT32 ) ;
      if ( SDB_OK != rc && SDB_OUT_OF_BOUND != rc )
      {
         rc = SDB_INVALIDARG ;
         detail = BSON( SPT_ERR << "useShell should be a number" ) ;
         goto error ;
      }
      rc = SDB_OK ;

      // timeout, default : 100
      rc = arg.getNative( 3, (void*)&timeout, SPT_NATIVE_INT32 ) ;
      if ( SDB_OK != rc && SDB_OUT_OF_BOUND != rc )
      {
         rc = SDB_INVALIDARG ;
         detail = BSON( SPT_ERR << "timeout should be a number" ) ;
         goto error ;
      }
      rc = SDB_OK ;

      rc = _cmdCommon.start( command, ev, useShell, timeout, err, pid, retStr ) ;
      if( SDB_OK != rc )
      {
         detail = BSON( SPT_ERR << err.c_str() ) ;
         goto error ;
      }
      rval.getReturnVal().setValue( pid ) ;
   done:
      return rc ;
   error:
      goto done ;
   }

}

