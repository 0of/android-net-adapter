#ifndef JNITYPES_H
#define JNITYPES_H
// this file represents some helper classes & macros wrapped some common JNI methods

#include <utility>

#include <jni.h>

#include "Exception/Exception.h"
#include "Dispatch/Context.h"

namespace Dispatch
{
    class Message;
}

// JNI exceptions
namespace JNIExcept
{
    EXCEPTION_DEF(JavaException, Except::Exception)
    EXCEPTION_DEF(JavaObjectRefException, JavaException)

    using ThreadNotAttachedException = std::logic_error;
}

#define JNI_VERSION_DEF JNI_VERSION_1_4
#define INIT_JENV(jenv, jvm) \
     JNIEnv *jenv = nullptr; \
     { \
        int result = jvm->GetEnv((void **)&jenv, JNI_VERSION_DEF); \
        if (JNI_EDETACHED == result) \
        { \
            throw JNIExcept::ThreadNotAttachedException("current thread is not attached"); \
        } \
        else if (JNI_EVERSION == result) \
        { \
            throw Except::NotSupportedException("JNI ver.1.4 not supported"); \
        } \
     }
#define CHECK_JNI_EXCEPT(jenv) \
    { \
        jthrowable throwable = jenv-> ExceptionOccurred(); \
        if (NULL != throwable) \
        { \
            jenv->ExceptionClear(); \
            \
            jclass throwableClass = jenv->FindClass ("java/lang/Throwable"); \
            jmethodID getMessage = jenv->GetMethodID (throwableClass, "getMessage", "()Ljava/lang/String;"); \
            \
            jstring exceptMessage = (jstring)jenv->CallObjectMethod(throwable, getMessage); \
            \
            const char *cStyleMessage = jenv->GetStringUTFChars(exceptMessage, nullptr); \
            std::string stlMessage{ cStyleMessage }; \
            jenv->ReleaseStringUTFChars(exceptMessage, cStyleMessage); \
            \
            throw JNIExcept::JavaException(stlMessage); \
        } \
    }

// error code
#define JNI_ALL_OK 0
#define JNI_RT_OBJECTREF_ERROR      0xFFFF0001
#define JNI_RT_UNKNOWN_HANDLE_ERROR 0xFFFF0002

// share types
using MessageContainer = std::pair<Dispatch::SharedContext, Dispatch::Message *&&>;

// GetStringUTFChars
// ReleaseStringUTFChars
class AutoUTFCString
{
private:
    JNIEnv *_jEnv;
    jstring _source;
    const char *_utfCString;

public:
    AutoUTFCString(JNIEnv *jEnv, jstring jstr)
        : _jEnv{ jEnv }
        , _source{ jstr }
        , _utfCString{ nullptr }
    {
         _utfCString = _jEnv->GetStringUTFChars(_source, nullptr);
    }

    ~AutoUTFCString()
    {
        _jEnv->ReleaseStringUTFChars(_source, _utfCString);
    }

public:
    operator const char *()
    {
        return _utfCString;
    }
};

#endif /* JNITYPES_H  */