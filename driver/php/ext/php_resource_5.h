/*******************************************************************************
   Copyright (C) 2012-2018 SequoiaDB Ltd.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*******************************************************************************/

#ifndef PHP_RESOURCE5_H__
#define PHP_RESOURCE5_H__

#include "php_driver.h"

//supper php7 code
#define zend_resource zend_rsrc_list_entry

#define PHP_REGISTER_RESOURCE( destroy, longDestroy, resourceName, resourceId )\
{\
   resourceId = zend_register_list_destructors_ex( destroy,\
                                                   longDestroy,\
                                                   resourceName,\
                                                   module_number ) ;\
}

//save resource
#define PHP_SAVE_RESOURCE( thisObj, name, resource, resourceId )\
{\
   zval *pZvalResource = NULL ;\
   void *pResource = (void *)resource ;\
   MAKE_STD_ZVAL( pZvalResource ) ;\
   if( pResource == NULL )\
   {\
      ZVAL_NULL( pZvalResource ) ;\
   }\
   else\
   {\
      ZEND_REGISTER_RESOURCE( pZvalResource, pResource, resourceId ) ;\
   }\
   zend_update_property( Z_OBJCE_P( thisObj ),\
                         thisObj,\
                         ZEND_STRL( name ),\
                         pZvalResource TSRMLS_CC ) ;\
}

//save sdb handle resource
#define PHP_SAVE_HANDLE( thisObj, handle, resourceId )\
{\
   PHP_SAVE_RESOURCE( thisObj, "_handle", handle, resourceId ) ;\
}

//read resource
#define PHP_READ_RESOURCE( thisObj, name, resource, resourceType, resourceName, resourceId )\
{\
   zval *pZvalResource = NULL ;\
   PHP_READ_VAR( thisObj, name, pZvalResource ) ;\
   if( Z_TYPE_P( pZvalResource ) == IS_NULL )\
   {\
      resource = 0 ;\
   }\
   else\
   {\
      ZEND_FETCH_RESOURCE_NO_RETURN( resource,\
                                     resourceType,\
                                     &pZvalResource,\
                                     -1,\
                                     resourceName,\
                                     resourceId ) ;\
   }\
}

//delete resource
#define PHP_DEL_RESOURCE( thisObj, name )\
{\
   zval *pZvalResource = NULL ;\
   PHP_READ_VAR( thisObj, name, pZvalResource ) ;\
   zend_list_delete( Z_LVAL_P( pZvalResource ) ) ;\
}

//read handle resource
#define PHP_READ_HANDLE( thisObj, handle, handleType, resourceName, resourceId )\
{\
   PHP_READ_RESOURCE( thisObj, "_handle", handle, handleType, resourceName, resourceId ) ;\
}

//delete handle resource
#define PHP_DEL_HANDLE( thisObj )\
{\
   PHP_DEL_RESOURCE( thisObj, "_handle" ) ;\
}

#endif