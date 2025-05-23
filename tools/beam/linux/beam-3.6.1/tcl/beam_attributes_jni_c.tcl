# -*- Tcl -*-
#
# Licensed Materials - Property of IBM - RESTRICTED MATERIALS OF IBM
#
# IBM Confidential - OCO Source Materials
#
# Copyright (C) 2007-2010 IBM Corporation. All rights reserved.
#
# US Government Users Restricted Rights - Use, duplication or disclosure
# restricted by GSA ADP Schedule Contract with IBM Corp
#
# The source code for this program is not published or otherwise divested
# of its trade secrets, irrespective of what has been deposited within
# the U.S. Copyright Office.
#
#
#    AUTHOR:
#
#        Goh Kondoh
#
#    DESCRIPTION:
#
#        Attributes that relate to Java Native Interface.
#        
#    MODIFICATIONS:
#
#        Date      UserID   Remark (newest to oldest)
#        --------  -------  ----------------------------------------------------
#        See ChangeLog for recent modifications

beam::propinfo_create {               
    name = "jni exception state",  # cleared of exception, with unchecked exception or with thrown exception
    invariance = "none",    # only the object generated by a constructor has the property
    dependence = "calls",   # only some calls can change the state of a JNI environment
}

beam::field_attribute {
    property ( index = 1,
               property_type = requires,
               type = input,
               property_name = "jni exception state",
               property_value = "cleared of exception" ),
    property ( index = 1,
               property_type = provides,
               type = output,
               property_name = "jni exception state",
               property_value = "with unchecked exception" ),
} JNINativeInterface_::CallVoidMethod \
  JNINativeInterface_::CallVoidMethodV \
  JNINativeInterface_::CallVoidMethodA \
  JNINativeInterface_::CallObjectMethod \
  JNINativeInterface_::CallObjectMethodV \
  JNINativeInterface_::CallObjectMethodA \
  JNINativeInterface_::CallBooleanMethod \
  JNINativeInterface_::CallBooleanMethodV \
  JNINativeInterface_::CallBooleanMethodA \
  JNINativeInterface_::CallByteMethod \
  JNINativeInterface_::CallByteMethodV \
  JNINativeInterface_::CallByteMethodA \
  JNINativeInterface_::CallCharMethod \
  JNINativeInterface_::CallCharMethodV \
  JNINativeInterface_::CallCharMethodA \
  JNINativeInterface_::CallShortMethod \
  JNINativeInterface_::CallShortMethodV \
  JNINativeInterface_::CallShortMethodA \
  JNINativeInterface_::CallIntMethod \
  JNINativeInterface_::CallIntMethodV \
  JNINativeInterface_::CallIntMethodA \
  JNINativeInterface_::CallLongMethod \
  JNINativeInterface_::CallLongMethodV \
  JNINativeInterface_::CallLongMethodA \
  JNINativeInterface_::CallFloatMethod \
  JNINativeInterface_::CallFloatMethodV \
  JNINativeInterface_::CallFloatMethodA \
  JNINativeInterface_::CallDoubleMethod \
  JNINativeInterface_::CallDoubleMethodV \
  JNINativeInterface_::CallDoubleMethodA \
  JNINativeInterface_::CallNonvirtualVoidMethod \
  JNINativeInterface_::CallNonvirtualVoidMethodV \
  JNINativeInterface_::CallNonvirtualVoidMethodA \
  JNINativeInterface_::CallNonvirtualObjectMethod \
  JNINativeInterface_::CallNonvirtualObjectMethodV \
  JNINativeInterface_::CallNonvirtualObjectMethodA \
  JNINativeInterface_::CallNonvirtualBooleanMethod \
  JNINativeInterface_::CallNonvirtualBooleanMethodV \
  JNINativeInterface_::CallNonvirtualBooleanMethodA \
  JNINativeInterface_::CallNonvirtualByteMethod \
  JNINativeInterface_::CallNonvirtualByteMethodV \
  JNINativeInterface_::CallNonvirtualByteMethodA \
  JNINativeInterface_::CallNonvirtualCharMethod \
  JNINativeInterface_::CallNonvirtualCharMethodV \
  JNINativeInterface_::CallNonvirtualCharMethodA \
  JNINativeInterface_::CallNonvirtualShortMethod \
  JNINativeInterface_::CallNonvirtualShortMethodV \
  JNINativeInterface_::CallNonvirtualShortMethodA \
  JNINativeInterface_::CallNonvirtualIntMethod \
  JNINativeInterface_::CallNonvirtualIntMethodV \
  JNINativeInterface_::CallNonvirtualIntMethodA \
  JNINativeInterface_::CallNonvirtualLongMethod \
  JNINativeInterface_::CallNonvirtualLongMethodV \
  JNINativeInterface_::CallNonvirtualLongMethodA \
  JNINativeInterface_::CallNonvirtualFloatMethod \
  JNINativeInterface_::CallNonvirtualFloatMethodV \
  JNINativeInterface_::CallNonvirtualFloatMethodA \
  JNINativeInterface_::CallNonvirtualDoubleMethod \
  JNINativeInterface_::CallNonvirtualDoubleMethodV \
  JNINativeInterface_::CallNonvirtualDoubleMethodA \
  JNINativeInterface_::CallStaticVoidMethod \
  JNINativeInterface_::CallStaticVoidMethodV \
  JNINativeInterface_::CallStaticVoidMethodA \
  JNINativeInterface_::CallStaticObjectMethod \
  JNINativeInterface_::CallStaticObjectMethodV \
  JNINativeInterface_::CallStaticObjectMethodA \
  JNINativeInterface_::CallStaticBooleanMethod \
  JNINativeInterface_::CallStaticBooleanMethodV \
  JNINativeInterface_::CallStaticBooleanMethodA \
  JNINativeInterface_::CallStaticByteMethod \
  JNINativeInterface_::CallStaticByteMethodV \
  JNINativeInterface_::CallStaticByteMethodA \
  JNINativeInterface_::CallStaticCharMethod \
  JNINativeInterface_::CallStaticCharMethodV \
  JNINativeInterface_::CallStaticCharMethodA \
  JNINativeInterface_::CallStaticShortMethod \
  JNINativeInterface_::CallStaticShortMethodV \
  JNINativeInterface_::CallStaticShortMethodA \
  JNINativeInterface_::CallStaticIntMethod \
  JNINativeInterface_::CallStaticIntMethodV \
  JNINativeInterface_::CallStaticIntMethodA \
  JNINativeInterface_::CallStaticLongMethod \
  JNINativeInterface_::CallStaticLongMethodV \
  JNINativeInterface_::CallStaticLongMethodA \
  JNINativeInterface_::CallStaticFloatMethod \
  JNINativeInterface_::CallStaticFloatMethodV \
  JNINativeInterface_::CallStaticFloatMethodA \
  JNINativeInterface_::CallStaticDoubleMethod \
  JNINativeInterface_::CallStaticDoubleMethodV \
  JNINativeInterface_::CallStaticDoubleMethodA

beam::field_attribute {
    property ( index = 1,
               property_type = provides,
               type = output,
               property_name = "jni exception state",
               property_value = "with thrown exception" )
    if (index = return,
   type  = provides,
   test_type = not_equal,
   test_value = 0),
    property ( index = 1,
               property_type = provides,
               type = output,
               property_name = "jni exception state",
               property_value = "cleared of exception" )
    if (index = return,
   type  = provides,
   test_type = equal,
   test_value = 0),
} JNINativeInterface_::ExceptionCheck JNINativeInterface_::ExceptionOccurred

beam::field_attribute {
    property ( index = 1,
               property_type = provides,
               type = output,
               property_name = "jni exception state",
               property_value = "cleared of exception" ),
} JNINativeInterface_::ExceptionClear

beam::field_attribute {
    property ( index = 1,
               property_type = requires,
               type = input,
               property_name = "jni exception state",
               property_value = "cleared of exception" ),
    property ( index = 1,
               property_type = provides,
               type = output,
               property_name = "jni exception state",
               property_value = "with thrown exception" )
    if (index = return,
        type  = provides,
        test_type = equal,
        test_value = 0),
    property ( index = 1,
               property_type = provides,
               type = output,
               property_name = "jni exception state",
               property_value = "cleared of exception" )
    if (index = return,
        type  = provides,
        test_type = not_equal,
        test_value = 0),
} JNINativeInterface_::FindClass

beam::field_attribute {
    property ( index = 1,
               property_type = requires,
               type = input,
               property_name = "jni exception state",
               property_value = "cleared of exception" ),
    property ( index = 1,
               property_type = provides,
               type = output,
               property_name = "jni exception state",
               property_value = "with thrown exception" ),
} JNINativeInterface_::Throw JNINativeInterface_::ThrowNew

beam::field_attribute {
    allocator ( return_index = return, initial_state = initialized_to_unknown )
} JNINativeInterface_::GetStringChars \
  JNINativeInterface_::GetStringUTFChars \
  JNINativeInterface_::GetBooleanArrayElements \
  JNINativeInterface_::GetByteArrayElements \
  JNINativeInterface_::GetCharArrayElements \
  JNINativeInterface_::GetShortArrayElements \
  JNINativeInterface_::GetIntArrayElements \
  JNINativeInterface_::GetLongArrayElements \
  JNINativeInterface_::GetFloatArrayElements \
  JNINativeInterface_::GetDoubleArrayElements

beam::field_attribute {
    deallocator ( pointer_index = 3 )
} JNINativeInterface_::ReleaseStringChars \
  JNINativeInterface_::ReleaseStringUTFChars \
  JNINativeInterface_::ReleaseBooleanArrayElements \
  JNINativeInterface_::ReleaseByteArrayElements \
  JNINativeInterface_::ReleaseCharArrayElements \
  JNINativeInterface_::ReleaseShortArrayElements \
  JNINativeInterface_::ReleaseIntArrayElements \
  JNINativeInterface_::ReleaseLongArrayElements \
  JNINativeInterface_::ReleaseFloatArrayElements \
  JNINativeInterface_::ReleaseDoubleArrayElements

beam::field_attribute {
    allocator ( return_index = return, initial_state = initialized_to_unknown ),
    property ( index = 1,
               property_type = provides,
               type = output,
               property_name = "jni critical state",
               property_value = "critical" )
} JNINativeInterface_::GetStringCritical JNINativeInterface_::GetPrimitiveArrayCritical

beam::field_attribute {
    deallocator ( pointer_index = 3 ),
    property ( index = 1,
               property_type = provides,
               type = output,
               property_name = "jni critical state",
               property_value = "not critical" )
} JNINativeInterface_::ReleaseStringCritical JNINativeInterface_::ReleasePrimitiveArrayCritical


#
# All the JNI functions must not called in a critical region
# which is between GetPrimitiveArrayCritical/GetStringCritical
# and ReleasePrimitiveArrayCritical/ReleaseStringCritical calls.
#
beam::field_attribute {
    property ( index = 1,
               property_type = requires,
               type = input,
               property_name = "jni critical state",
               property_value = "not critical" ),
} JNINativeInterface_::GetVersion \
  JNINativeInterface_::DefineClass \
  JNINativeInterface_::FindClass \
  JNINativeInterface_::FromReflectedMethod \
  JNINativeInterface_::FromReflectedField \
  JNINativeInterface_::ToReflectedMethod \
  JNINativeInterface_::GetSuperclass \
  JNINativeInterface_::IsAssignableFrom \
  JNINativeInterface_::ToReflectedField \
  JNINativeInterface_::Throw \
  JNINativeInterface_::ThrowNew \
  JNINativeInterface_::ExceptionOccurred \
  JNINativeInterface_::ExceptionDescribe \
  JNINativeInterface_::ExceptionClear \
  JNINativeInterface_::FatalError \
  JNINativeInterface_::PushLocalFrame \
  JNINativeInterface_::PopLocalFrame \
  JNINativeInterface_::NewGlobalRef \
  JNINativeInterface_::DeleteGlobalRef \
  JNINativeInterface_::DeleteLocalRef \
  JNINativeInterface_::IsSameObject \
  JNINativeInterface_::NewLocalRef \
  JNINativeInterface_::EnsureLocalCapacity \
  JNINativeInterface_::AllocObject \
  JNINativeInterface_::NewObject \
  JNINativeInterface_::NewObjectV \
  JNINativeInterface_::NewObjectA \
  JNINativeInterface_::GetObjectClass \
  JNINativeInterface_::IsInstanceOf \
  JNINativeInterface_::GetMethodID \
  JNINativeInterface_::CallObjectMethod \
  JNINativeInterface_::CallObjectMethodV \
  JNINativeInterface_::CallObjectMethodA \
  JNINativeInterface_::CallBooleanMethod \
  JNINativeInterface_::CallBooleanMethodV \
  JNINativeInterface_::CallBooleanMethodA \
  JNINativeInterface_::CallByteMethod \
  JNINativeInterface_::CallByteMethodV \
  JNINativeInterface_::CallByteMethodA \
  JNINativeInterface_::CallCharMethod \
  JNINativeInterface_::CallCharMethodV \
  JNINativeInterface_::CallCharMethodA \
  JNINativeInterface_::CallShortMethod \
  JNINativeInterface_::CallShortMethodV \
  JNINativeInterface_::CallShortMethodA \
  JNINativeInterface_::CallIntMethod \
  JNINativeInterface_::CallIntMethodV \
  JNINativeInterface_::CallIntMethodA \
  JNINativeInterface_::CallLongMethod \
  JNINativeInterface_::CallLongMethodV \
  JNINativeInterface_::CallLongMethodA \
  JNINativeInterface_::CallFloatMethod \
  JNINativeInterface_::CallFloatMethodV \
  JNINativeInterface_::CallFloatMethodA \
  JNINativeInterface_::CallDoubleMethod \
  JNINativeInterface_::CallDoubleMethodV \
  JNINativeInterface_::CallDoubleMethodA \
  JNINativeInterface_::CallVoidMethod \
  JNINativeInterface_::CallVoidMethodV \
  JNINativeInterface_::CallVoidMethodA \
  JNINativeInterface_::CallNonvirtualObjectMethod \
  JNINativeInterface_::CallNonvirtualObjectMethodV \
  JNINativeInterface_::CallNonvirtualObjectMethodA \
  JNINativeInterface_::CallNonvirtualBooleanMethod \
  JNINativeInterface_::CallNonvirtualBooleanMethodV \
  JNINativeInterface_::CallNonvirtualBooleanMethodA \
  JNINativeInterface_::CallNonvirtualByteMethod \
  JNINativeInterface_::CallNonvirtualByteMethodV \
  JNINativeInterface_::CallNonvirtualByteMethodA \
  JNINativeInterface_::CallNonvirtualCharMethod \
  JNINativeInterface_::CallNonvirtualCharMethodV \
  JNINativeInterface_::CallNonvirtualCharMethodA \
  JNINativeInterface_::CallNonvirtualShortMethod \
  JNINativeInterface_::CallNonvirtualShortMethodV \
  JNINativeInterface_::CallNonvirtualShortMethodA \
  JNINativeInterface_::CallNonvirtualIntMethod \
  JNINativeInterface_::CallNonvirtualIntMethodV \
  JNINativeInterface_::CallNonvirtualIntMethodA \
  JNINativeInterface_::CallNonvirtualLongMethod \
  JNINativeInterface_::CallNonvirtualLongMethodV \
  JNINativeInterface_::CallNonvirtualLongMethodA \
  JNINativeInterface_::CallNonvirtualFloatMethod \
  JNINativeInterface_::CallNonvirtualFloatMethodV \
  JNINativeInterface_::CallNonvirtualFloatMethodA \
  JNINativeInterface_::CallNonvirtualDoubleMethod \
  JNINativeInterface_::CallNonvirtualDoubleMethodV \
  JNINativeInterface_::CallNonvirtualDoubleMethodA \
  JNINativeInterface_::CallNonvirtualVoidMethod \
  JNINativeInterface_::CallNonvirtualVoidMethodV \
  JNINativeInterface_::CallNonvirtualVoidMethodA \
  JNINativeInterface_::GetFieldID \
  JNINativeInterface_::GetObjectField \
  JNINativeInterface_::GetBooleanField \
  JNINativeInterface_::GetByteField \
  JNINativeInterface_::GetCharField \
  JNINativeInterface_::GetShortField \
  JNINativeInterface_::GetIntField \
  JNINativeInterface_::GetLongField \
  JNINativeInterface_::GetFloatField \
  JNINativeInterface_::GetDoubleField \
  JNINativeInterface_::SetObjectField \
  JNINativeInterface_::SetBooleanField \
  JNINativeInterface_::SetByteField \
  JNINativeInterface_::SetCharField \
  JNINativeInterface_::SetShortField \
  JNINativeInterface_::SetIntField \
  JNINativeInterface_::SetLongField \
  JNINativeInterface_::SetFloatField \
  JNINativeInterface_::SetDoubleField \
  JNINativeInterface_::GetStaticMethodID \
  JNINativeInterface_::CallStaticObjectMethod \
  JNINativeInterface_::CallStaticObjectMethodV \
  JNINativeInterface_::CallStaticObjectMethodA \
  JNINativeInterface_::CallStaticBooleanMethod \
  JNINativeInterface_::CallStaticBooleanMethodV \
  JNINativeInterface_::CallStaticBooleanMethodA \
  JNINativeInterface_::CallStaticByteMethod \
  JNINativeInterface_::CallStaticByteMethodV \
  JNINativeInterface_::CallStaticByteMethodA \
  JNINativeInterface_::CallStaticCharMethod \
  JNINativeInterface_::CallStaticCharMethodV \
  JNINativeInterface_::CallStaticCharMethodA \
  JNINativeInterface_::CallStaticShortMethod \
  JNINativeInterface_::CallStaticShortMethodV \
  JNINativeInterface_::CallStaticShortMethodA \
  JNINativeInterface_::CallStaticIntMethod \
  JNINativeInterface_::CallStaticIntMethodV \
  JNINativeInterface_::CallStaticIntMethodA \
  JNINativeInterface_::CallStaticLongMethod \
  JNINativeInterface_::CallStaticLongMethodV \
  JNINativeInterface_::CallStaticLongMethodA \
  JNINativeInterface_::CallStaticFloatMethod \
  JNINativeInterface_::CallStaticFloatMethodV \
  JNINativeInterface_::CallStaticFloatMethodA \
  JNINativeInterface_::CallStaticDoubleMethod \
  JNINativeInterface_::CallStaticDoubleMethodV \
  JNINativeInterface_::CallStaticDoubleMethodA \
  JNINativeInterface_::CallStaticVoidMethod \
  JNINativeInterface_::CallStaticVoidMethodV \
  JNINativeInterface_::CallStaticVoidMethodA \
  JNINativeInterface_::GetStaticFieldID \
  JNINativeInterface_::GetStaticObjectField \
  JNINativeInterface_::GetStaticBooleanField \
  JNINativeInterface_::GetStaticByteField \
  JNINativeInterface_::GetStaticCharField \
  JNINativeInterface_::GetStaticShortField \
  JNINativeInterface_::GetStaticIntField \
  JNINativeInterface_::GetStaticLongField \
  JNINativeInterface_::GetStaticFloatField \
  JNINativeInterface_::GetStaticDoubleField \
  JNINativeInterface_::SetStaticObjectField \
  JNINativeInterface_::SetStaticBooleanField \
  JNINativeInterface_::SetStaticByteField \
  JNINativeInterface_::SetStaticCharField \
  JNINativeInterface_::SetStaticShortField \
  JNINativeInterface_::SetStaticIntField \
  JNINativeInterface_::SetStaticLongField \
  JNINativeInterface_::SetStaticFloatField \
  JNINativeInterface_::SetStaticDoubleField \
  JNINativeInterface_::NewString \
  JNINativeInterface_::GetStringLength \
  JNINativeInterface_::GetStringChars \
  JNINativeInterface_::ReleaseStringChars \
  JNINativeInterface_::NewStringUTF \
  JNINativeInterface_::GetStringUTFLength \
  JNINativeInterface_::GetStringUTFChars \
  JNINativeInterface_::ReleaseStringUTFChars \
  JNINativeInterface_::GetArrayLength \
  JNINativeInterface_::NewObjectArray \
  JNINativeInterface_::GetObjectArrayElement \
  JNINativeInterface_::SetObjectArrayElement \
  JNINativeInterface_::NewBooleanArray \
  JNINativeInterface_::NewByteArray \
  JNINativeInterface_::NewCharArray \
  JNINativeInterface_::NewShortArray \
  JNINativeInterface_::NewIntArray \
  JNINativeInterface_::NewLongArray \
  JNINativeInterface_::NewFloatArray \
  JNINativeInterface_::NewDoubleArray \
  JNINativeInterface_::GetBooleanArrayElements \
  JNINativeInterface_::GetByteArrayElements \
  JNINativeInterface_::GetCharArrayElements \
  JNINativeInterface_::GetShortArrayElements \
  JNINativeInterface_::GetIntArrayElements \
  JNINativeInterface_::GetLongArrayElements \
  JNINativeInterface_::GetFloatArrayElements \
  JNINativeInterface_::GetDoubleArrayElements \
  JNINativeInterface_::ReleaseBooleanArrayElements \
  JNINativeInterface_::ReleaseByteArrayElements \
  JNINativeInterface_::ReleaseCharArrayElements \
  JNINativeInterface_::ReleaseShortArrayElements \
  JNINativeInterface_::ReleaseIntArrayElements \
  JNINativeInterface_::ReleaseLongArrayElements \
  JNINativeInterface_::ReleaseFloatArrayElements \
  JNINativeInterface_::ReleaseDoubleArrayElements \
  JNINativeInterface_::GetBooleanArrayRegion \
  JNINativeInterface_::GetByteArrayRegion \
  JNINativeInterface_::GetCharArrayRegion \
  JNINativeInterface_::GetShortArrayRegion \
  JNINativeInterface_::GetIntArrayRegion \
  JNINativeInterface_::GetLongArrayRegion \
  JNINativeInterface_::GetFloatArrayRegion \
  JNINativeInterface_::GetDoubleArrayRegion \
  JNINativeInterface_::SetBooleanArrayRegion \
  JNINativeInterface_::SetByteArrayRegion \
  JNINativeInterface_::SetCharArrayRegion \
  JNINativeInterface_::SetShortArrayRegion \
  JNINativeInterface_::SetIntArrayRegion \
  JNINativeInterface_::SetLongArrayRegion \
  JNINativeInterface_::SetFloatArrayRegion \
  JNINativeInterface_::SetDoubleArrayRegion \
  JNINativeInterface_::RegisterNatives \
  JNINativeInterface_::UnregisterNatives \
  JNINativeInterface_::MonitorEnter \
  JNINativeInterface_::MonitorExit \
  JNINativeInterface_::GetJavaVM \
  JNINativeInterface_::GetStringRegion \
  JNINativeInterface_::GetStringUTFRegion \
  JNINativeInterface_::NewWeakGlobalRef \
  JNINativeInterface_::DeleteWeakGlobalRef \
  JNINativeInterface_::ExceptionCheck \
  JNINativeInterface_::NewDirectByteBuffer \
  JNINativeInterface_::GetDirectBufferAddress \
  JNINativeInterface_::GetDirectBufferCapacity
