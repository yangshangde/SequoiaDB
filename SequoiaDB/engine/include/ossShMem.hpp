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

   Source File Name = ossShMem.hpp

   Descriptive Name = share memory

   When/how to use: this program may be used on binary and text-formatted
   versions of PMD component. This file contains main function for sdbcm,
   which is used to do cluster managing.

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          10/09/2013  JHL  Initial Draft

   Last Changed =

*******************************************************************************/
#include "ossTypes.h"
#if defined (_LINUX)
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#elif defined (_WINDOWS)

#endif


#if defined (_LINUX)
// to  create  a new segment.
// If this flag is not used, then ossSHMAlloc() will find
// the segment associated with key and check to see
// if the user  has  permission to access the segment.
#define OSS_SHM_CREATE        IPC_CREAT
// used with OSS_SHM_CREATE to ensure failure if the segment already exists
#define OSS_SHM_EXCL          IPC_EXCL

typedef INT32     ossSHMMid;
typedef key_t     ossSHMKey;

#elif defined (_WINDOWS)
typedef HANDLE    ossSHMMid;
typedef CHAR*     ossSHMKey;

// to  create  a new segment.
// If this flag is not used, then ossSHMAlloc() will find
// the segment associated with key and check to see
// if the user  has  permission to access the segment.
#define OSS_SHM_CREATE        0X01
// used with OSS_SHM_CREATE to ensure failure if the segment already exists
#define OSS_SHM_EXCL          0X02

#endif


CHAR *ossSHMAlloc( ossSHMKey shmKey, UINT32 bufSize, INT32 shmFlag,
                   ossSHMMid &shmMid );

void ossSHMFree( ossSHMMid &shmMid, CHAR **ppBuf );

CHAR *ossSHMAttach( ossSHMKey shmKey, UINT32 bufSize, ossSHMMid &shmMid );

void ossSHMDetach( ossSHMMid & shmMid, CHAR **ppBuf );

