#include "../JNI/com__0of_adapters_services_impl_NetworkServiceImpl.h"

#include <memory>

#include "../JNI/JNITypes.h"

#include "NetworkServiceListenerAdapter.h"
#include "Exception/Exception.h"
#include "Core/NetworkService.h"

/*
 * Class:     com__0of_adapters_services_impl_NetworkServiceImpl
 * Method:    _freeHandler
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com__0of_adapters_services_impl_NetworkServiceImpl__1freeHandler(JNIEnv *jEnv, jobject jImpl, jlong handle)
{
    auto service = (Core::NetworkService *)handle;
    if (service)
    {
        delete service;
    }
}

/*
 * Class:     com__0of_adapters_services_impl_NetworkServiceImpl
 * Method:    _requestConnect
 * Signature: (JLcom/_0of/adapters/services/NetworkService/EstablishListener;Ljava/lang/String;IIIZZ)I
 */
JNIEXPORT jint JNICALL Java_com__0of_adapters_services_impl_NetworkServiceImpl__1requestConnect(JNIEnv *jEnv, jobject jService, jlong handle, jobject proxy,
       jstring host, jint port, jint id, jint timeout, jboolean isTCP, jboolean isIPv4)
{
    auto service = (Core::NetworkService *)(std::intptr_t)handle;
    if (!service)
    {
        // no reference
        return JNI_RT_UNKNOWN_HANDLE_ERROR;
    }

    AutoUTFCString hostCString{ jEnv, host };
    std::string hostString{ hostCString };
    Core::ConnectionString connectString = { static_cast<bool>(isTCP), static_cast<bool>(isIPv4), static_cast<std::uint16_t>(port), std::move(hostString) };

    try
    {
        // create OnConnection adapter
        JavaVM* jvm = nullptr;
        jEnv->GetJavaVM(&jvm);
        CHECK_JNI_EXCEPT(jEnv)

        auto onConnectInstance = new JNIAdapter::OnConnectionImpl{ jvm, proxy };
        // request connection
        service->request(id, connectString, std::move(onConnectInstance));
    }
    catch (const JNIExcept::JavaObjectRefException& )
    {
        return JNI_RT_OBJECTREF_ERROR;
    }

    // all OK
    return JNI_ALL_OK;
}

/*
 * Class:     com__0of_adapters_services_impl_NetworkServiceImpl
 * Method:    _requestDestroy
 * Signature: (JLcom/_0of/adapters/services/NetworkService/DestroyListener;I)I
 */
JNIEXPORT jint JNICALL Java_com__0of_adapters_services_impl_NetworkServiceImpl__1requestDestroy(JNIEnv *jEnv, jobject jService, jlong handle, jobject onDestroy, jint id)
{
    auto service = (Core::NetworkService *)(std::intptr_t)handle;
    if (!service)
    {
        // no reference
        return JNI_RT_UNKNOWN_HANDLE_ERROR;
    }

    try
    {
        // create OnDisconnect adapter
        JavaVM* jvm = nullptr;
        jEnv->GetJavaVM(&jvm);
        CHECK_JNI_EXCEPT(jEnv)

        auto onDisconnectInstance = new JNIAdapter::OnDisconnectionImpl{ jvm, onDestroy };
        // request connection
        service->request(id, std::move(onDisconnectInstance));
    }
    catch (const JNIExcept::JavaObjectRefException& )
    {
        return JNI_RT_OBJECTREF_ERROR;
    }

    // all OK
    return JNI_ALL_OK;
}


