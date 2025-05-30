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

#ifndef CLASS_NODE_H__
#define CLASS_NODE_H__

#include "php_driver.h"

PHP_METHOD( SequoiaNode, __construct ) ;

//e.g. Rename getNodeName
PHP_METHOD( SequoiaNode, getName ) ;
PHP_METHOD( SequoiaNode, getHostName ) ;
PHP_METHOD( SequoiaNode, getServiceName ) ;
PHP_METHOD( SequoiaNode, connect ) ;
PHP_METHOD( SequoiaNode, start ) ;
PHP_METHOD( SequoiaNode, stop ) ;

#endif