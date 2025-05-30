/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2012 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

/* $Id: zend_objects.c 321634 2012-01-01 13:15:04Z felipe $ */

#include "zend.h"
#include "zend_globals.h"
#include "zend_variables.h"
#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"

ZEND_API void zend_object_std_init(zend_object *object, zend_class_entry *ce TSRMLS_DC)
{
	ALLOC_HASHTABLE(object->properties);
	zend_hash_init(object->properties, 0, NULL, ZVAL_PTR_DTOR, 0);

	object->ce = ce;	
	object->guards = NULL;
}

ZEND_API void zend_object_std_dtor(zend_object *object TSRMLS_DC)
{
	if (object->guards) {
		zend_hash_destroy(object->guards);
		FREE_HASHTABLE(object->guards);		
	}
	if (object->properties) {
		zend_hash_destroy(object->properties);
		FREE_HASHTABLE(object->properties);
	}
}

ZEND_API void zend_objects_destroy_object(zend_object *object, zend_object_handle handle TSRMLS_DC)
{
	zend_function *destructor = object ? object->ce->destructor : NULL;

	if (destructor) {
		zval *old_exception;
		zval *obj;
		zend_object_store_bucket *obj_bucket;

		if (destructor->op_array.fn_flags & (ZEND_ACC_PRIVATE|ZEND_ACC_PROTECTED)) {
			if (destructor->op_array.fn_flags & ZEND_ACC_PRIVATE) {
				/* Ensure that if we're calling a private function, we're allowed to do so.
				 */
				if (object->ce != EG(scope)) {
					zend_class_entry *ce = object->ce;

					zend_error(EG(in_execution) ? E_ERROR : E_WARNING, 
						"Call to private %s::__destruct() from context '%s'%s", 
						ce->name, 
						EG(scope) ? EG(scope)->name : "", 
						EG(in_execution) ? "" : " during shutdown ignored");
					return;
				}
			} else {
				/* Ensure that if we're calling a protected function, we're allowed to do so.
				 */
				if (!zend_check_protected(destructor->common.scope, EG(scope))) {
					zend_class_entry *ce = object->ce;

					zend_error(EG(in_execution) ? E_ERROR : E_WARNING, 
						"Call to protected %s::__destruct() from context '%s'%s", 
						ce->name, 
						EG(scope) ? EG(scope)->name : "", 
						EG(in_execution) ? "" : " during shutdown ignored");
					return;
				}
			}
		}

		MAKE_STD_ZVAL(obj);
		Z_TYPE_P(obj) = IS_OBJECT;
		Z_OBJ_HANDLE_P(obj) = handle;
		obj_bucket = &EG(objects_store).object_buckets[handle];
		if (!obj_bucket->bucket.obj.handlers) {
			obj_bucket->bucket.obj.handlers = &std_object_handlers;
		}
		Z_OBJ_HT_P(obj) = obj_bucket->bucket.obj.handlers;
		zval_copy_ctor(obj);

		/* Make sure that destructors are protected from previously thrown exceptions.
		 * For example, if an exception was thrown in a function and when the function's
		 * local variable destruction results in a destructor being called.
		 */
		old_exception = NULL;
		if (EG(exception)) {
			if (Z_OBJ_HANDLE_P(EG(exception)) == handle) {
				zend_error(E_ERROR, "Attempt to destruct pending exception");
			} else {
				old_exception = EG(exception);
				EG(exception) = NULL;
			}
		}
		zend_call_method_with_0_params(&obj, object->ce, &destructor, ZEND_DESTRUCTOR_FUNC_NAME, NULL);
		if (old_exception) {
			if (EG(exception)) {
				zend_exception_set_previous(EG(exception), old_exception TSRMLS_CC);
			} else {
				EG(exception) = old_exception;
			}
		}
		zval_ptr_dtor(&obj);
	}
}

ZEND_API void zend_objects_free_object_storage(zend_object *object TSRMLS_DC)
{
	zend_object_std_dtor(object TSRMLS_CC);
	efree(object);
}

ZEND_API zend_object_value zend_objects_new(zend_object **object, zend_class_entry *class_type TSRMLS_DC)
{	
	zend_object_value retval;

	*object = emalloc(sizeof(zend_object));
	(*object)->ce = class_type;
	retval.handle = zend_objects_store_put(*object, (zend_objects_store_dtor_t) zend_objects_destroy_object, (zend_objects_free_object_storage_t) zend_objects_free_object_storage, NULL TSRMLS_CC);
	retval.handlers = &std_object_handlers;
	(*object)->guards = NULL;
	return retval;
}

ZEND_API zend_object *zend_objects_get_address(const zval *zobject TSRMLS_DC)
{
	return (zend_object *)zend_object_store_get_object(zobject TSRMLS_CC);
}

ZEND_API void zend_objects_clone_members(zend_object *new_object, zend_object_value new_obj_val, zend_object *old_object, zend_object_handle handle TSRMLS_DC)
{
	zend_hash_copy(new_object->properties, old_object->properties, zval_copy_property_ctor(old_object->ce), (void *) NULL /* Not used anymore */, sizeof(zval *));

	if (old_object->ce->clone) {
		zval *new_obj;

		MAKE_STD_ZVAL(new_obj);
		new_obj->type = IS_OBJECT;
		new_obj->value.obj = new_obj_val;
		zval_copy_ctor(new_obj);

		zend_call_method_with_0_params(&new_obj, old_object->ce, &old_object->ce->clone, ZEND_CLONE_FUNC_NAME, NULL);

		zval_ptr_dtor(&new_obj);
	}
}

ZEND_API zend_object_value zend_objects_clone_obj(zval *zobject TSRMLS_DC)
{
	zend_object_value new_obj_val;
	zend_object *old_object;
	zend_object *new_object;
	zend_object_handle handle = Z_OBJ_HANDLE_P(zobject);

	/* assume that create isn't overwritten, so when clone depends on the 
	 * overwritten one then it must itself be overwritten */
	old_object = zend_objects_get_address(zobject TSRMLS_CC);
	new_obj_val = zend_objects_new(&new_object, old_object->ce TSRMLS_CC);

	ALLOC_HASHTABLE(new_object->properties);
	zend_hash_init(new_object->properties, 0, NULL, ZVAL_PTR_DTOR, 0);

	zend_objects_clone_members(new_object, new_obj_val, old_object, handle TSRMLS_CC);

	return new_obj_val;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
