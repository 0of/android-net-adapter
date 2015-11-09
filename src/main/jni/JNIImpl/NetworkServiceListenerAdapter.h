#ifndef NETWORKSERVICELISTENERADAPTER_H
#define NETWORKSERVICELISTENERADAPTER_H

#include "../JNI/JNITypes.h"

#include "Core/NetworkService.h"

namespace JNIAdapter
{
    class OnConnectionImpl : public Core::OnConnection
    {
    private:
        JavaVM *_attachedJVM;
        jobject _establishListener;

    public:
        OnConnectionImpl(JavaVM *jvm, jobject instance);
        ~OnConnectionImpl();

    public:
        virtual void OnConnected() override;
        virtual void OnError(Dispatch::ExceptionEvent& exceptionEv) override;
        virtual void OnTimeout(std::int32_t timespan) override;
    };

    class OnDisconnectionImpl : public Core::OnDisconnection
    {
    private:
        JavaVM *_attachedJVM;
        jobject _destroyListenerListener;

     public:
        OnDisconnectionImpl(JavaVM *jvm, jobject instance);
        ~OnDisconnectionImpl();

    public:
        virtual void OnDisconnected();
        virtual void OnError(Dispatch::ExceptionEvent& exceptionEv);
    };
}

#endif  /* NETWORKSERVICELISTENERADAPTER_H */
