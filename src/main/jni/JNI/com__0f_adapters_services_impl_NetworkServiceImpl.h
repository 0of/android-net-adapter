/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com__0of_adapters_services_impl_NetworkServiceImpl */

#ifndef _Included_com__0of_adapters_services_impl_NetworkServiceImpl
#define _Included_com__0of_adapters_services_impl_NetworkServiceImpl
#ifdef __cplusplus
extern "C" {
#endif
#undef com__0of_adapters_services_impl_NetworkServiceImpl_NULL
#define com__0of_adapters_services_impl_NetworkServiceImpl_NULL 0i64
/*
 * Class:     com__0of_adapters_services_impl_NetworkServiceImpl
 * Method:    _freeHandler
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com__0of_adapters_services_impl_NetworkServiceImpl__1freeHandler
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com__0of_adapters_services_impl_NetworkServiceImpl
 * Method:    _requestConnect
 * Signature: (JLcom/_0of/adapters/services/NetworkService/EstablishListener;Ljava/lang/String;IIIZZ)I
 */
JNIEXPORT jint JNICALL Java_com__0of_adapters_services_impl_NetworkServiceImpl__1requestConnect
  (JNIEnv *, jobject, jlong, jobject, jstring, jint, jint, jint, jboolean, jboolean);

/*
 * Class:     com__0of_adapters_services_impl_NetworkServiceImpl
 * Method:    _requestDestroy
 * Signature: (JLcom/_0of/adapters/services/NetworkService/DestroyListener;I)I
 */
JNIEXPORT jint JNICALL Java_com__0of_adapters_services_impl_NetworkServiceImpl__1requestDestroy
  (JNIEnv *, jobject, jlong, jobject, jint);

#ifdef __cplusplus
}
#endif
#endif
