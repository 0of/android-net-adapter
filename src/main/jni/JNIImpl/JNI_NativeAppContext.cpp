#include "../JNI/com__0of_adapters_NativeAppContext.h"

#include "../JNI/JNITypes.h"

#include "App/Application.h"

/*
 * Class:     com__0of_adapters_NativeAppContext
 * Method:    getFromNative
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com__0of_adapters_NativeAppContext_getFromNative(JNIEnv *jEnv, jclass j)
{
    return (jlong)(std::intptr_t)(&Application::GetInstance());
}

/*
 * Class:     com__0of_adapters_NativeAppContext
 * Method:    _start
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_com__0of_adapters_NativeAppContext__1start(JNIEnv *jEnv, jobject jappcontext, jlong app)
{
    auto& nativeApp = Application::GetInstance();
    // reference to the same object
    if ((std::intptr_t)&nativeApp != (std::intptr_t)app)
    {
        return JNI_RT_UNKNOWN_HANDLE_ERROR;
    }

    nativeApp.Run(jEnv);

    return JNI_ALL_OK;
}

/*
 * Class:     com__0of_adapters_NativeAppContext
 * Method:    _quit
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_com__0of_adapters_NativeAppContext__1quit(JNIEnv *jEnv, jobject jappcontext, jlong app)
{
    auto& nativeApp = Application::GetInstance();
    // reference to the same object
    if ((std::intptr_t)&nativeApp != (std::intptr_t)app)
    {
        return JNI_RT_UNKNOWN_HANDLE_ERROR;
    }

    nativeApp.OnQuit();

    return JNI_ALL_OK;
}