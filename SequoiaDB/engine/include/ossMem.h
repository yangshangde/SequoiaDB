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

   Source File Name = ossMem.h

   Descriptive Name = Operating System Services Memory Header

   When/how to use: this program may be used on binary and text-formatted
   versions of OSS component. This file contains declares for all memory
   allocation/free operations.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          09/14/2012  TW  Initial Draft

   Last Changed =

*******************************************************************************/
#ifndef OSSMEM_H_
#define OSSMEM_H_
#include "core.h"

/*
 * [x bytes start][8 bytes data size][4 bytes guard size][4 bytes file hash][4
 * bytes line num][data][1 byte end][x-1 bytes stop]*/
#define SDB_MEMDEBUG_MINGUARDSIZE 8
#define SDB_MEMDEBUG_MAXGUARDSIZE 4194304
#define SDB_MEMDEBUG_GUARDSTART ((CHAR)0xBE)
#define SDB_MEMDEBUG_GUARDSTOP  ((CHAR)0xBF)
#define SDB_MEMDEBUG_GUARDEND   ((CHAR)0xBD)
#define SDB_MEMHEAD_EYECATCHER1 0xFABD0538
#define SDB_MEMHEAD_EYECATCHER2 0xFACE7352

#define SDB_OSS_MALLOC(x)       ossMemAlloc(x,__FILE__,__LINE__)
#define SDB_OSS_FREE(x)         ossMemFree(x)
#define SDB_OSS_ORIGINAL_FREE(x) free(x)
#define SDB_OSS_REALLOC(x,y)    ossMemRealloc(x,y,__FILE__,__LINE__)

#define SDB_OSS_MALLOC3(x,y,z)  ossMemAlloc(x,y,z)

#define SAFE_OSS_FREE(p)      \
   do {                       \
      if (p) {                \
         SDB_OSS_FREE(p) ;    \
         p = NULL ;           \
      }                       \
   } while (0)

SDB_EXTERN_C_START

void  ossEnableMemDebug( BOOLEAN debugEnable,
                         UINT32  memDebugSize,
                         BOOLEAN memDebugVerify,
                         BOOLEAN memDebugDetail,
                         UINT32  memDebugMask ) ;

void* ossMemAlloc ( size_t size, const CHAR* file, UINT32 line ) ;

void* ossMemRealloc ( void* pOld, size_t size,
                      const CHAR* file, UINT32 line ) ;

void* ossAlignedAlloc( UINT32 alignment, UINT32 size ) ;

void  ossMemFree ( void *p ) ;

/*
   Mem tools
*/
BOOLEAN ossMemVerify ( void *p ) ;
BOOLEAN ossMemSanityCheck ( void *p ) ;

void    ossMemTrack ( void *p ) ;
void    ossMemUnTrack ( void *p ) ;
INT32   ossMemTrace ( const CHAR *pPath ) ;
INT32   ossMemDump ( const CHAR *pPath ) ;
void    ossOnMemConfigChange( BOOLEAN debugEnable,
                              UINT32  memDebugSize,
                              BOOLEAN memDebugVerify,
                              BOOLEAN memDebugDetail,
                              UINT32  memDebugMask ) ;

void    ossSetSysMemInfo( INT32 mxfast,
                          INT32 trimThreshold,
                          INT32 mmapThreshold,
                          INT32 mmapMax,
                          INT32 topPad ) ;

INT32   ossMemTrim() ;

SDB_EXTERN_C_END
#endif
