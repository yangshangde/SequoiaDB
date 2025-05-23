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

   Source File Name = rtnCommandDef.hpp

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who Description
   ====== =========== === ==============================================
          13/12/2012  Xu Jianhui  Initial Draft

   Last Changed =

*******************************************************************************/

#ifndef RTN_COMMAND_DEF_HPP_
#define RTN_COMMAND_DEF_HPP_

#include "core.hpp"
#include "msgDef.h"

namespace engine
{

/*
   Command Name Define
*/
#define NAME_BACKUP_OFFLINE                  CMD_NAME_BACKUP_OFFLINE
#define NAME_CREATE_COLLECTION               CMD_NAME_CREATE_COLLECTION
#define NAME_CREATE_COLLECTIONSPACE          CMD_NAME_CREATE_COLLECTIONSPACE
#define NAME_CREATE_INDEX                    CMD_NAME_CREATE_INDEX
#define NAME_CANCEL_TASK                     CMD_NAME_CANCEL_TASK
#define NAME_DROP_COLLECTION                 CMD_NAME_DROP_COLLECTION
#define NAME_DROP_COLLECTIONSPACE            CMD_NAME_DROP_COLLECTIONSPACE
#define NAME_DROP_INDEX                      CMD_NAME_DROP_INDEX
#define NAME_COPY_INDEX                      CMD_NAME_COPY_INDEX
#define NAME_LOAD_COLLECTIONSPACE            CMD_NAME_LOAD_COLLECTIONSPACE
#define NAME_UNLOAD_COLLECTIONSPACE          CMD_NAME_UNLOAD_COLLECTIONSPACE
#define NAME_GET_COUNT                       CMD_NAME_GET_COUNT
#define NAME_GET_INDEXES                     CMD_NAME_GET_INDEXES
#define NAME_GET_DATABLOCKS                  CMD_NAME_GET_DATABLOCKS
#define NAME_GET_QUERYMETA                   CMD_NAME_GET_QUERYMETA
#define NAME_GET_CL_DETAIL                   CMD_NAME_GET_CL_DETAIL
#define NAME_GET_CL_STAT                     CMD_NAME_GET_CL_STAT
#define NAME_GET_INDEX_STAT                  CMD_NAME_GET_INDEX_STAT
#define NAME_LIST_COLLECTIONS                CMD_NAME_LIST_COLLECTIONS
#define NAME_LIST_COLLECTIONSPACES           CMD_NAME_LIST_COLLECTIONSPACES
#define NAME_LIST_CONTEXTS                   CMD_NAME_LIST_CONTEXTS
#define NAME_LIST_CONTEXTS_CURRENT           CMD_NAME_LIST_CONTEXTS_CURRENT
#define NAME_LIST_SESSIONS                   CMD_NAME_LIST_SESSIONS
#define NAME_LIST_SESSIONS_CURRENT           CMD_NAME_LIST_SESSIONS_CURRENT
#define NAME_LIST_STORAGEUNITS               CMD_NAME_LIST_STORAGEUNITS
#define NAME_LIST_BACKUPS                    CMD_NAME_LIST_BACKUPS
#define NAME_LIST_INDEXES                    CMD_NAME_LIST_INDEXES
#define NAME_RENAME_COLLECTION               CMD_NAME_RENAME_COLLECTION
#define NAME_RENAME_COLLECTIONSPACE          CMD_NAME_RENAME_COLLECTIONSPACE
#define NAME_REORG_OFFLINE                   CMD_NAME_REORG_OFFLINE
#define NAME_REORG_ONLINE                    CMD_NAME_REORG_ONLINE
#define NAME_REORG_RECOVER                   CMD_NAME_REORG_RECOVER
#define NAME_SHUTDOWN                        CMD_NAME_SHUTDOWN
#define NAME_SNAPSHOT_CONTEXTS               CMD_NAME_SNAPSHOT_CONTEXTS
#define NAME_SNAPSHOT_CONTEXTS_CURRENT       CMD_NAME_SNAPSHOT_CONTEXTS_CURRENT
#define NAME_SNAPSHOT_DATABASE               CMD_NAME_SNAPSHOT_DATABASE
#define NAME_SNAPSHOT_RESET                  CMD_NAME_SNAPSHOT_RESET
#define NAME_SNAPSHOT_SESSIONS               CMD_NAME_SNAPSHOT_SESSIONS
#define NAME_SNAPSHOT_SESSIONS_CURRENT       CMD_NAME_SNAPSHOT_SESSIONS_CURRENT
#define NAME_SNAPSHOT_SYSTEM                 CMD_NAME_SNAPSHOT_SYSTEM
#define NAME_SNAPSHOT_COLLECTIONS            CMD_NAME_SNAPSHOT_COLLECTIONS
#define NAME_SNAPSHOT_COLLECTIONSPACES       CMD_NAME_SNAPSHOT_COLLECTIONSPACES
#define NAME_SNAPSHOT_TRANSACTIONS_CUR       CMD_NAME_SNAPSHOT_TRANSACTIONS_CUR
#define NAME_SNAPSHOT_TRANSACTIONS           CMD_NAME_SNAPSHOT_TRANSACTIONS
#define NAME_SNAPSHOT_ACCESSPLANS            CMD_NAME_SNAPSHOT_ACCESSPLANS
#define NAME_SNAPSHOT_HEALTH                 CMD_NAME_SNAPSHOT_HEALTH
#define NAME_SNAPSHOT_CONFIGS                CMD_NAME_SNAPSHOT_CONFIGS
#define NAME_SNAPSHOT_SVCTASKS               CMD_NAME_SNAPSHOT_SVCTASKS
#define NAME_SNAPSHOT_QUERIES                CMD_NAME_SNAPSHOT_QUERIES
#define NAME_SNAPSHOT_LATCHWAITS             CMD_NAME_SNAPSHOT_LATCHWAITS
#define NAME_SNAPSHOT_LOCKWAITS              CMD_NAME_SNAPSHOT_LOCKWAITS
#define NAME_SNAPSHOT_INDEXSTATS             CMD_NAME_SNAPSHOT_INDEXSTATS
#define NAME_SNAPSHOT_TASKS                  CMD_NAME_SNAPSHOT_TASKS
#define NAME_SNAPSHOT_INDEXES                CMD_NAME_SNAPSHOT_INDEXES
#define NAME_SNAPSHOT_TRANSWAITS             CMD_NAME_SNAPSHOT_TRANSWAITS
#define NAME_SNAPSHOT_TRANSDEADLOCK          CMD_NAME_SNAPSHOT_TRANSDEADLOCK
#define NAME_TEST_COLLECTION                 CMD_NAME_TEST_COLLECTION
#define NAME_TEST_COLLECTIONSPACE            CMD_NAME_TEST_COLLECTIONSPACE
#define NAME_SET_PDLEVEL                     CMD_NAME_SET_PDLEVEL
#define NAME_SET_SESSIONATTR                 CMD_NAME_SETSESS_ATTR
#define NAME_GET_SESSIONATTR                 CMD_NAME_GETSESS_ATTR
#define NAME_SPLIT                           CMD_NAME_SPLIT
#define NAME_TRACE_START                     CMD_NAME_TRACE_START
#define NAME_TRACE_STOP                      CMD_NAME_TRACE_STOP
#define NAME_TRACE_STATUS                    CMD_NAME_TRACE_STATUS
#define NAME_EXPORT_CONFIGURATION            CMD_NAME_EXPORT_CONFIG
#define NAME_REMOVE_BACKUP                   CMD_NAME_REMOVE_BACKUP
#define NAME_LINK_COLLECTION                 CMD_NAME_LINK_CL
#define NAME_UNLINK_COLLECTION               CMD_NAME_UNLINK_CL
#define NAME_INVALIDATE_CACHE                CMD_NAME_INVALIDATE_CACHE
#define NAME_INVALIDATE_SEQUENCE_CACHE       CMD_NAME_INVALIDATE_SEQUENCE_CACHE
#define NAME_FORCE_SESSION                   CMD_NAME_FORCE_SESSION
#define NAME_LIST_LOBS                       CMD_NAME_LIST_LOBS
#define NAME_REELECT                         CMD_NAME_REELECT
#define NAME_REELECT_LOCATION                CMD_NAME_REELECT_LOCATION
#define NAME_FORCE_STEP_UP                   CMD_NAME_FORCE_STEP_UP
#define NAME_JSON_LOAD                       CMD_NAME_JSON_LOAD
#define NAME_TRUNCATE                        CMD_NAME_TRUNCATE
#define NAME_ALTER_COLLECTION                CMD_NAME_ALTER_COLLECTION
#define NAME_ALTER_COLLECTION_SPACE          CMD_NAME_ALTER_COLLECTION_SPACE
#define NAME_ALTER_DC                        CMD_NAME_ALTER_DC
#define NAME_SYNC_DB                         CMD_NAME_SYNC_DB
#define NAME_POP                             CMD_NAME_POP
#define NAME_RELOAD_CONFIG                   CMD_NAME_RELOAD_CONFIG
#define NAME_UPDATE_CONFIG                   CMD_NAME_UPDATE_CONFIG
#define NAME_DELETE_CONFIG                   CMD_NAME_DELETE_CONFIG
#define NAME_ANALYZE                         CMD_NAME_ANALYZE
#define NAME_ALTER_GROUP                     CMD_NAME_ALTER_GROUP
#define NAME_MEM_TRIM                        CMD_NAME_MEM_TRIM

// the commands that does not supported by data nodes or standalone mode
#define NAME_CREATE_GROUP                    CMD_NAME_CREATE_GROUP
#define NAME_REMOVE_GROUP                    CMD_NAME_REMOVE_GROUP
#define NAME_CREATE_NODE                     CMD_NAME_CREATE_NODE
#define NAME_REMOVE_NODE                     CMD_NAME_REMOVE_NODE
#define NAME_UPDATE_NODE                     CMD_NAME_UPDATE_NODE
#define NAME_ACTIVE_GROUP                    CMD_NAME_ACTIVE_GROUP
#define NAME_START_NODE                      CMD_NAME_STARTUP_NODE
#define NAME_SHUTDOWN_NODE                   CMD_NAME_SHUTDOWN_NODE
#define NAME_SHUTDOWN_GROUP                  CMD_NAME_SHUTDOWN_GROUP
#define NAME_GET_CONFIG                      CMD_NAME_GET_CONFIG
#define NAME_LIST_TASKS                      CMD_NAME_LIST_TASKS
#define NAME_LIST_USERS                      CMD_NAME_LIST_USERS
#define NAME_LIST_DOMAINS                    CMD_NAME_LIST_DOMAINS
#define NAME_LIST_GROUPS                     CMD_NAME_LIST_GROUPS
#define NAME_LIST_PROCEDURES                 CMD_NAME_LIST_PROCEDURES
#define NAME_LIST_DATASOURCES                CMD_NAME_LIST_DATASOURCES
#define NAME_CREATE_PROCEDURE                CMD_NAME_CRT_PROCEDURE
#define NAME_REMOVE_PROCEDURE                CMD_NAME_RM_PROCEDURE
#define NAME_LIST_CS_IN_DOMAIN               CMD_NAME_LIST_CS_IN_DOMAIN
#define NAME_LIST_CL_IN_DOMAIN               CMD_NAME_LIST_CL_IN_DOMAIN
#define NAME_CREATE_CATAGROUP                CMD_NAME_CREATE_CATA_GROUP
#define NAME_CREATE_DOMAIN                   CMD_NAME_CREATE_DOMAIN
#define NAME_DROP_DOMAIN                     CMD_NAME_DROP_DOMAIN
#define NAME_ALTER_DOMAIN                    CMD_NAME_ALTER_DOMAIN
#define NAME_ADD_DOMAIN_GROUP                CMD_NAME_ADD_DOMAIN_GROUP
#define NAME_REMOVE_DOMAIN_GROUP             CMD_NAME_REMOVE_DOMAIN_GROUP
#define NAME_SNAPSHOT_CATA                   CMD_NAME_SNAPSHOT_CATA
#define NAME_WAITTASK                        CMD_NAME_WAITTASK
#define NAME_GET_DCINFO                      CMD_NAME_GET_DCINFO
#define NAME_LIST_SEQUENCES                  CMD_NAME_LIST_SEQUENCES
#define NAME_SNAP_SEQUENCES                  CMD_NAME_SNAPSHOT_SEQUENCES
#define NAME_CREATE_SEQUENCE                 CMD_NAME_CREATE_SEQUENCE
#define NAME_DROP_SEQUENCE                   CMD_NAME_DROP_SEQUENCE
#define NAME_ALTER_SEQUENCE                  CMD_NAME_ALTER_SEQUENCE
#define NAME_INVALIDATE_DATASOURCE_CACHE     CMD_NAME_INVALIDATE_DATASOURCE_CACHE

#define NAME_LIST_RECYCLEBIN                 CMD_NAME_LIST_RECYCLEBIN
#define NAME_SNAPSHOT_RECYCLEBIN             CMD_NAME_SNAPSHOT_RECYCLEBIN
#define NAME_GET_RECYCLEBIN_DETAIL           CMD_NAME_GET_RECYCLEBIN_DETAIL
#define NAME_GET_RECYCLEBIN_COUNT            CMD_NAME_GET_RECYCLEBIN_COUNT
#define NAME_ALTER_RECYCLEBIN                CMD_NAME_ALTER_RECYCLEBIN
#define NAME_DROP_RECYCLEBIN_ITEM            CMD_NAME_DROP_RECYCLEBIN_ITEM
#define NAME_DROP_RECYCLEBIN_ALL             CMD_NAME_DROP_RECYCLEBIN_ALL
#define NAME_RETURN_RECYCLEBIN_ITEM          CMD_NAME_RETURN_RECYCLEBIN_ITEM
#define NAME_RETURN_RECYCLEBIN_ITEM_TO_NAME  CMD_NAME_RETURN_RECYCLEBIN_ITEM_TO_NAME

#define NAME_INVALIDATE_USER_CACHE           CMD_NAME_INVALIDATE_USER_CACHE
#define NAME_CREATE_ROLE                     CMD_NAME_CREATE_ROLE
#define NAME_DROP_ROLE                       CMD_NAME_DROP_ROLE
#define NAME_GET_ROLE                        CMD_NAME_GET_ROLE
#define NAME_LIST_ROLES                      CMD_NAME_LIST_ROLES
#define NAME_UPDATE_ROLE                     CMD_NAME_UPDATE_ROLE
#define NAME_GRANT_PRIVILEGES                CMD_NAME_GRANT_PRIVILEGES
#define NAME_REVOKE_PRIVILEGES               CMD_NAME_REVOKE_PRIVILEGES
#define NAME_GRANT_ROLES_TO_ROLE             CMD_NAME_GRANT_ROLES_TO_ROLE
#define NAME_REVOKE_ROLES_FROM_ROLE          CMD_NAME_REVOKE_ROLES_FROM_ROLE
#define NAME_GET_USER                        CMD_NAME_GET_USER
#define NAME_GRANT_ROLES_TO_USER             CMD_NAME_GRANT_ROLES_TO_USER
#define NAME_REVOKE_ROLES_FROM_USER          CMD_NAME_REVOKE_ROLES_FROM_USER

   /*
      Command Type Define
   */
   enum RTN_COMMAND_TYPE
   {
      CMD_BACKUP_OFFLINE                     = 1,

      CMD_CREATE_COLLECTION                  = 10,
      CMD_CREATE_COLLECTIONSPACE             = 11,
      CMD_CREATE_INDEX                       = 12,
      CMD_ALTER_COLLECTION                   = 13,
      CMD_ALTER_COLLECTIONSPACE              = 14,

      CMD_DROP_COLLECTION                    = 20,
      CMD_DROP_COLLECTIONSPACE               = 21,
      CMD_DROP_INDEX                         = 22,

      CMD_LOAD_COLLECTIONSPACE               = 25,
      CMD_UNLOAD_COLLECTIONSPACE             = 26,

      CMD_GET_COUNT                          = 30,
      CMD_GET_INDEXES                        = 31,  // deprecated
      CMD_GET_DATABLOCKS                     = 32,
      CMD_GET_QUERYMETA                      = 33,
      CMD_GET_DCINFO                         = 34,
      CMD_GET_CL_DETAIL                      = 35,
      CMD_GET_INDEX_STAT                     = 36,
      CMD_GET_CL_STAT                        = 37,

      CMD_LIST_COLLECTIONS                   = 40,
      CMD_LIST_COLLECTIONSPACES              = 41,
      CMD_LIST_CONTEXTS                      = 42,
      CMD_LIST_CONTEXTS_CURRENT              = 43,
      CMD_LIST_SESSIONS                      = 44,
      CMD_LIST_SESSIONS_CURRENT              = 45,
      CMD_LIST_STORAGEUNITS                  = 46,
      CMD_LIST_GROUPS                        = 47,
      CMD_LIST_DOMAINS                       = 48,
      CMD_LIST_BACKUPS                       = 49,
      CMD_LIST_TASKS                         = 50,
      CMD_LIST_USERS                         = 51,
      CMD_LIST_PROCEDURES                    = 52,
      CMD_LIST_CS_IN_DOMAIN                  = 53,
      CMD_LIST_CL_IN_DOMAIN                  = 54,
      CMD_LIST_TRANS                         = 55,
      CMD_LIST_TRANS_CURRENT                 = 56,
      CMD_CREATE_PROCEDURE                   = 57,
      CMD_REMOVE_PROCEDURE                   = 58,
      CMD_LIST_SVCTASKS                      = 59,

      CMD_RENAME_COLLECTION                  = 60,
      CMD_RENAME_COLLECTIONSPACE             = 61,

      CMD_LIST_SEQUENCES                     = 62,
      CMD_LIST_DATASOURCES                   = 63,
      CMD_LIST_INDEXES                       = 64,

      CMD_REORG_OFFLINE                      = 70,
      CMD_REORG_ONLINE                       = 71,
      CMD_REORG_RECOVER                      = 72,

      CMD_SHUTDOWN                           = 80,

      CMD_SNAPSHOT_ALL                       = 89,
      CMD_SNAPSHOT_CONTEXTS                  = 90,
      CMD_SNAPSHOT_CONTEXTS_CURRENT          = 91,
      CMD_SNAPSHOT_DATABASE                  = 92,
      CMD_SNAPSHOT_RESET                     = 93,
      CMD_SNAPSHOT_SESSIONS                  = 94,
      CMD_SNAPSHOT_SESSIONS_CURRENT          = 95,
      CMD_SNAPSHOT_SYSTEM                    = 96,
      CMD_SNAPSHOT_COLLECTIONS               = 97,
      CMD_SNAPSHOT_COLLECTIONSPACES          = 98,
      CMD_SNAPSHOT_CATA                      = 99,
      CMD_SNAPSHOT_TRANSACTIONS_CUR          = 100,
      CMD_SNAPSHOT_TRANSACTIONS              = 101,
      CMD_SNAPSHOT_ACCESSPLANS               = 102,
      CMD_SNAPSHOT_HEALTH                    = 103,
      CMD_SNAPSHOT_CONFIGS                   = 104,
      CMD_SNAPSHOT_VCL_SESSIONINFO           = 105,
      CMD_SNAPSHOT_SVCTASKS                  = 106,
      CMD_SNAPSHOT_SEQUENCES                 = 107,
      CMD_SNAPSHOT_QUERIES                   = 108,
      CMD_SNAPSHOT_LATCHWAITS                = 109,

      CMD_TEST_COLLECTION                    = 110,
      CMD_TEST_COLLECTIONSPACE               = 111,

      CMD_SET_PDLEVEL                        = 120,
      CMD_SET_SESSIONATTR                    = 121,
      CMD_GET_SESSIONATTR                    = 122,

      CMD_CREATE_GROUP                       = 130,
      CMD_CREATE_NODE                        = 131,
      CMD_UPDATE_NODE                        = 132,
      CMD_ACTIVE_GROUP                       = 133,
      CMD_GET_CONFIG                         = 134,
      CMD_SPLIT                              = 135,
      CMD_REMOVE_GROUP                       = 136,
      CMD_REMOVE_NODE                        = 137,
      CMD_START_NODE                         = 138,
      CMD_SHUTDOWN_NODE                      = 139,
      CMD_SHUTDOWN_GROUP                     = 140,
      CMD_CREATE_CATAGROUP                   = 141,
      CMD_CREATE_DOMAIN                      = 142,
      CMD_DROP_DOMAIN                        = 143,
      CMD_ADD_DOMAIN_GROUP                   = 144,
      CMD_REMOVE_DOMAIN_GROUP                = 145,
      CMD_WAITTASK                           = 146,
      CMD_CANCEL_TASK                        = 147,
      CMD_ALTER_DOMAIN                       = 148,
      CMD_ALTER_GROUP                        = 149,

      CMD_LINK_COLLECTION                    = 150,
      CMD_UNLINK_COLLECTION                  = 151,
      CMD_TRUNCATE                           = 152,

      CMD_ALTER_IMAGE                        = 155,

      // trace commands
      CMD_TRACE_START                        = 160,
      CMD_TRACE_RESUME                       = 161,
      CMD_TRACE_STOP                         = 162,
      CMD_TRACE_STATUS                       = 163,

      CMD_JSON_LOAD                          = 180,

      CMD_EXPORT_CONFIG                      = 200,
      CMD_RELOAD_CONFIG                      = 201,
      CMD_UPDATE_CONFIG                      = 202,
      CMD_DELETE_CONFIG                      = 203,
      CMD_MEM_TRIM                           = 204,

      CMD_REMOVE_BACKUP                      = 210,
      CMD_INVALIDATE_CACHE                   = 220,
      CMD_FORCE_SESSION                      = 221,
      CMD_LIST_LOB                           = 222,
      CMD_REELECT                            = 223,
      CMD_FORCE_STEP_UP                      = 224,
      CMD_REELECT_LOCATION                   = 225,

      CMD_SYNC_DB                            = 251,
      CMD_POP                                = 252,
      CMD_ANALYZE                            = 253,
      CMD_INVALIDATE_SEQUENCE_CACHE          = 254,

      CMD_SNAPSHOT_LOCKWAITS                 = 255,
      CMD_SNAPSHOT_INDEXSTATS                = 256,

      CMD_INVALIDATE_DATASOURCE_CACHE        = 257,

      CMD_CREATE_SEQUENCE                    = 260,
      CMD_DROP_SEQUENCE                      = 261,
      CMD_ALTER_SEQUENCE                     = 262,

      CMD_SNAPSHOT_TASKS                     = 270,
      CMD_SNAPSHOT_INDEXES                   = 271,
      CMD_SNAPSHOT_TRANSWAITS                = 272,
      CMD_SNAPSHOT_TRANSDEADLOCK             = 273,

      CMD_COPY_INDEX                         = 280,

      // recycle bin commands
      CMD_LIST_RECYCLEBIN                    = 290,
      CMD_SNAPSHOT_RECYCLEBIN                = 291,
      CMD_GET_RECYCLEBIN_DETAIL              = 292,
      CMD_GET_RECYCLEBIN_COUNT               = 293,
      CMD_ALTER_RECYCLEBIN                   = 294,
      CMD_DROP_RECYCLEBIN_ITEM               = 295,
      CMD_DROP_RECYCLEBIN_ALL                = 296,
      CMD_RETURN_RECYCLEBIN_ITEM             = 297,
      CMD_RETURN_RECYCLEBIN_ITEM_TO_NAME     = 298,

      // rbac commands
      CMD_INVALIDATE_USER_CACHE              = 310,
      CMD_CREATE_ROLE                        = 311,
      CMD_DROP_ROLE                          = 312,
      CMD_GET_ROLE                           = 313,
      CMD_LIST_ROLES                         = 314,
      CMD_UPDATE_ROLE                        = 315,
      CMD_GRANT_PRIVILEGES                   = 316,
      CMD_REVOKE_PRIVILEGES                  = 317,
      CMD_GRANT_ROLES_TO_ROLE                = 318,
      CMD_REVOKE_ROLES_FROM_ROLE             = 319,
      CMD_GET_USER                           = 320,
      CMD_GRANT_ROLES_TO_USER                = 321,
      CMD_REVOKE_ROLES_FROM_USER             = 322,

      CMD_UNKNOW                             = 65535
   };

/*
   Command Space Node Role Define
*/
#define CMD_SPACE_NODE_NULL               0x0000
#define CMD_SPACE_NODE_STANDALONE         0x0001
#define CMD_SPACE_NODE_COORD              0x0002
#define CMD_SPACE_NODE_DATA               0x0004
#define CMD_SPACE_NODE_CATA               0x0008
#define CMD_SPACE_NODE_OM                 0x0010

#define CMD_SPACE_NODE_ALL                0xFFFF

/*
   Command Space Service Define
*/
#define CMD_SPACE_SERVICE_LOCAL           0x0010
#define CMD_SPACE_SERVICE_SHARD           0x0020
#define CMD_SPACE_SERVICE_CATA            0x0040
#define CMD_SPACE_SERVICE_REST            0x0080

#define CMD_SPACE_SERVICE_ALL             0xFFFF

}


#endif //RTN_COMMAND_DEF_HPP_

