#include "NetworkServiceListenerAdapter.h"

#include "../JNI/JNITypes.h"

#include "Dispatch/ExceptionEvent.h"

namespace JNIAdapter
{
    OnConnectionImpl::OnConnectionImpl(JavaVM *jvm, jobject instance)
        : _attachedJVM{ jvm }
        , _establishListener{ nullptr }
    {
        INIT_JENV(jEnv, _attachedJVM);
        auto establishListener = jEnv->NewGlobalRef(instance);
        if (!establishListener)
            throw JNIExcept::JavaObjectRefException();
    }

    OnConnectionImpl::~OnConnectionImpl()
    {
        if (_establishListener)
        {
            INIT_JENV(jEnv, _attachedJVM);
            jEnv->DeleteGlobalRef(_establishListener);
        }
    }

    void OnConnectionImpl::OnConnected()
    {
        INIT_JENV(jEnv, _attachedJVM)
        jclass classEstablishListener = jEnv->GetObjectClass(_establishListener);
        CHECK_JNI_EXCEPT(jEnv)

        jmethodID onEstablishedMethod = jEnv->GetMethodID(classEstablishListener, "OnEstablished", "()V");

        jEnv->CallVoidMethod(_establishListener, onEstablishedMethod);
        CHECK_JNI_EXCEPT(jEnv)
    }

    void OnConnectionImpl::OnError(Dispatch::ExceptionEvent& exceptionEv)
    {
        INIT_JENV(jEnv, _attachedJVM)
        // find onException method
        jclass classEstablishListener = jEnv->GetObjectClass(_establishListener);
        CHECK_JNI_EXCEPT(jEnv)

        jmethodID onErrorMethod = jEnv->GetMethodID(classEstablishListener, "OnError", "(I)V");
        // TODO exception to error
        int error = 0;

        jEnv->CallVoidMethod(_establishListener, onErrorMethod, error /* error */);
        CHECK_JNI_EXCEPT(jEnv)
    }

    void OnConnectionImpl::OnTimeout(std::int32_t timespan)
    {
        INIT_JENV(jEnv, _attachedJVM)
        // find onException method
        jclass classEstablishListener = jEnv->GetObjectClass(_establishListener);
        CHECK_JNI_EXCEPT(jEnv)

        jmethodID onTimeoutMethod = jEnv->GetMethodID(classEstablishListener, "OnTimeout", "(I)V");

        jEnv->CallVoidMethod(_establishListener, onTimeoutMethod,timespan);
        CHECK_JNI_EXCEPT(jEnv)
    }


    OnDisconnectionImpl::OnDisconnectionImpl(JavaVM *jvm, jobject instance)
        : _attachedJVM{ jvm }
        , _destroyListenerListener{ nullptr }
    {
       INIT_JENV(jEnv, _attachedJVM);
       auto _destroyListenerListener = jEnv->NewGlobalRef(instance);
       if (!_destroyListenerListener)
           throw JNIExcept::JavaObjectRefException();
    }

     OnDisconnectionImpl::~OnDisconnectionImpl()
     {
         if (_destroyListenerListener)
         {
             INIT_JENV(jEnv, _attachedJVM);
             jEnv->DeleteGlobalRef(_destroyListenerListener);
         }
     }

     void OnDisconnectionImpl::OnDisconnected()
     {
        INIT_JENV(jEnv, _attachedJVM)
        jclass classDestroyListener = jEnv->GetObjectClass(_destroyListenerListener);
        CHECK_JNI_EXCEPT(jEnv)

        jmethodID onDestroyedMethod = jEnv->GetMethodID(classDestroyListener, "OnDestroyed", "()V");

        jEnv->CallVoidMethod(_destroyListenerListener, onDestroyedMethod);
        CHECK_JNI_EXCEPT(jEnv)
     }

    void OnDisconnectionImpl::OnError(Dispatch::ExceptionEvent& exceptionEv)
    {
        INIT_JENV(jEnv, _attachedJVM)
        jclass classDestroyListener = jEnv->GetObjectClass(_destroyListenerListener);
        CHECK_JNI_EXCEPT(jEnv)

        jmethodID onErrorMethod = jEnv->GetMethodID(classDestroyListener, "OnError", "(I)V");
        // TODO exception to error
        int error = 0;

        jEnv->CallVoidMethod(_destroyListenerListener, onErrorMethod, error /* error */);
        CHECK_JNI_EXCEPT(jEnv)
    }
}