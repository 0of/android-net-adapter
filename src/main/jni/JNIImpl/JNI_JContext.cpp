#include "../JNI/com__0of_adapters_JContext.h"

#include <cstring>

#include "../JNI/JNITypes.h"

#include "Dispatch/Context.h"
#include "Core/NetworkService.h"

static jobject ProvideNetworkService(JNIEnv *jEnv, const Dispatch::SharedContext& context)
{
    // fetch network service
    auto networkService = std::unique_ptr<Core::NetworkService>{ context->ProvideCapability<Core::NetworkService>() };
    if (networkService)
    {
        // acquire successfully
        // so instantiate NetworkServiceImpl
        jclass classNetworkServiceImpl = jEnv->FindClass("com/_0of/adapters/services/impl/NetworkServiceImpl");
        jmethodID acquireInstanceMethod = jEnv->GetStaticMethodID(classNetworkServiceImpl, "acquireInstance", "(J)Ljava/lang/Object;");

        CHECK_JNI_EXCEPT(jEnv)

        // get raw ptr first
        jlong handle = (jlong)(std::intptr_t)networkService.get();
        // if throw any exception, return object should never restore the raw pointer value
        jobject jNetworkService = jEnv->CallStaticObjectMethod(classNetworkServiceImpl, acquireInstanceMethod, handle);
        CHECK_JNI_EXCEPT(jEnv)

        // if all OK, release the managed pointer
        networkService.release();
        return jNetworkService;
    }

    return nullptr;
}

JNIEXPORT void JNICALL Java_com__0of_adapters_JContext__1freeHandler(JNIEnv *jEnv, jobject jContext, jlong handle)
{
    auto context = (Dispatch::SharedContext *)handle;
    if (context)
    {
        delete context;
    }
}

JNIEXPORT jobject JNICALL Java_com__0of_adapters_JContext__1instantiateService(JNIEnv *jEnv, jobject jContext, jstring serviceName, jlong handle)
{
    auto context = (Dispatch::SharedContext *)handle;
    if (context && (*context))
    {
        AutoUTFCString cServiceName{ jEnv, serviceName };
        if (0 == std::strcmp(cServiceName, "com._0of.adapters.services.NetworkService"))
        {
            try
            {
                return ProvideNetworkService(jEnv, *context);
            }
            catch (const Except::Exception& ex)
            {
                // log
                // then quit
            }
        }
    }

    return nullptr;
}