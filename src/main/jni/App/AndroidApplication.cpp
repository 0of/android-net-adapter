#include "Application_p.h"

#include "globals.h"

#ifdef OS_ANDROID

#include <thread>
#include <memory>

#include "../JNI/JNITypes.h"

#include "Exception/Exception.h"
#include "Dispatch/Context.h"
#include "Core/ClientRunnable.h"
#include "Core/InitSynchronizer.h"
#include "Core/NetworkEvents.h"

#if defined(COMPILER_GCC) && (__GNUC__ * 10 + __GNUC_MINOR__ <= 48)
// C++14 features
namespace std
{
    template<typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args&&... args)
    {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }
}
#endif

Application::ApplicationImpl::ApplicationImpl() = default;
Application::ApplicationImpl::~ApplicationImpl() = default;

namespace Runtime
{
    class AndroidMainMessageQueue : public Dispatch::MessageQueue
    {
    private:
        ThreadID _threadID;
        JavaVM *_attachedJVM;
        jobject _jMessageQueue;

        jmethodID _postMethod;

    public:
        AndroidMainMessageQueue(JavaVM *jvm, jobject instance, jmethodID postMethod)
            : _threadID{ std::this_thread::get_id() }
            , _attachedJVM{ jvm }
            , _jMessageQueue{ nullptr }
            , _postMethod{ postMethod }
        {
            INIT_JENV(jEnv, _attachedJVM);
            auto messageQueue = jEnv->NewGlobalRef(instance);
            if (!messageQueue)
                throw JNIExcept::JavaObjectRefException();

            _jMessageQueue = messageQueue;
        }

        ~AndroidMainMessageQueue()
        {
            if (_jMessageQueue)
            {
                INIT_JENV(jEnv, _attachedJVM);
                jEnv->DeleteGlobalRef(_jMessageQueue);
            }
        }

    public:
         virtual void Post(const Dispatch::SharedContext& context, Dispatch::Message *&&msg) override
         {
            auto messageContainer = new MessageContainer{context, std::move(msg)};
            if (messageContainer)
            {
                INIT_JENV(jEnv, _attachedJVM)

                // instantiate a JMessage
                jclass classJMessage = jEnv->FindClass("com/_0of/adapters/JMessage");
                jmethodID ctorJMessage = jEnv->GetMethodID(classJMessage, "<init>", "(J)V");

                CHECK_JNI_EXCEPT(jEnv)

                // in case of implicitly conversion to other type
                void *handle = messageContainer;
                jobject jMessage = jEnv->NewObject(classJMessage, ctorJMessage, handle);
                if (jMessage)
                {
                    // call post method
                    jEnv->CallVoidMethod(_jMessageQueue, _postMethod, jMessage);
                    CHECK_JNI_EXCEPT(jEnv)
                }
            }
         }

        virtual ThreadID AttachedThreadID() const override
        {
            return _threadID;
        }
    };

    class JVMContext
    {
    private:
        JavaVM *_jvm;

    public:
        JVMContext(JavaVM *jvm, JNIEnv **outJEnv)
            : _jvm { jvm }
        {
            _jvm->AttachCurrentThread(outJEnv, nullptr);
        }

        ~JVMContext()
        {
            _jvm->DetachCurrentThread();
        }
    };

    // when client(network thread) initialized successfully
    class NetworkThreadInitNotifier : public Core::InitNotifier
    {
    public:
        Dispatch::SharedContext _clientContext;
        Dispatch::QueueIdentifier _id;

    public:
        explicit NetworkThreadInitNotifier(Core::InitSynchronizer *sync)
            : InitNotifier(sync)
            , _clientContext()
            , _id()
        {}

    protected:
        virtual void OnInitialized(Core::ClientRunnable& runnable)
        {
            _clientContext = runnable.Context();
            _id = runnable.RegisteredQueueIdentifier();
        }
    };
}

void Application::ApplicationImpl::InitMainThreadQueue(void *hostContext)
{
    // init global thread manager
    auto& queueMgr = Dispatch::MessageQueueManager::GetInstance();

    auto jEnv = static_cast<JNIEnv *>(hostContext);
    if (!jEnv)
        throw Except::InvalidArgumentException("no android JNIEnv");

    // acquire JVM object
    JavaVM* jvm = nullptr;
    jEnv->GetJavaVM(&jvm);

    CHECK_JNI_EXCEPT(jEnv)

    // instantiate main thread queue for native callings
    jclass classUIMessageQueue = jEnv->FindClass("com/_0of/adapters/UIMessageQueue");
    jmethodID ctorUIMessageQueue = jEnv->GetMethodID(classUIMessageQueue, "<init>", "()V");
    jmethodID postMethod = jEnv->GetMethodID(classUIMessageQueue, "post", "(Lcom/_0of/adapters/JMessage;)V");

    CHECK_JNI_EXCEPT(jEnv)

    jobject uiMessageQueue = jEnv->NewObject(classUIMessageQueue, ctorUIMessageQueue);
    auto mainQueue = std::make_shared<Runtime::AndroidMainMessageQueue>(jvm, uiMessageQueue, postMethod);
    // clean local reference
    jEnv->DeleteLocalRef(uiMessageQueue);

    if (mainQueue)
    {
        auto tid = mainQueue->AttachedThreadID();
        _mainThreadID = tid;

        // register to global queue manager
        queueMgr.RegisterMessageQueue(tid, mainQueue);

        _dispatcher = new Dispatch::EventDispatcher{ tid };

        // set host as JVM
        _host = jvm;
    }
}

void Application::ApplicationImpl::LoadConfig()
{
    // do nothing
}

void Application::ApplicationImpl::InitThreadQueues()
{
    auto jvm = static_cast<JavaVM *>(_host);

    /* wait for all thread quit */
    auto sync = std::make_unique<Core::InitSynchronizer>(1 /* thread count */);
    if (!sync)
        throw Except::OutOfMemoryException{};

    Dispatch::SharedContext networkClientContext;
    Dispatch::QueueIdentifier networkClientID;

    auto initNotifier = std::make_unique<Runtime::NetworkThreadInitNotifier>(sync.get());
    // init network thread
    // TODO
    //   sync has no timeout property
    //   the UI thread will be blocked infinitely
    auto networkThread = new std::thread([](JavaVM* jvm, Runtime::NetworkThreadInitNotifier *notifier)
    {
        // init JNI context
        JNIEnv *jEnv = nullptr;
        Runtime::JVMContext jvmContext{ jvm, &jEnv };

        if (!jEnv)
        {
            // init JEnv failed
            // just quit
            return;
        }

        // JEnv is OK, then initialize network runnable
        Core::ClientRunnable runnable;
        runnable.Run(*notifier);

    }, jvm, initNotifier.get());

    networkThread->detach();

    // wait threads initialization
    sync->Wait();

    // failed
    if (!networkClientContext)
    {
        throw Except::Exception("Init network thread failed");
    }

    // update network thread infos
    _clientContext = initNotifier->_clientContext;
    _clientQueueId = initNotifier->_id;

    _thread = networkThread;
}

void Application::ApplicationImpl::OnMainThreadQuit()
{
    auto& queueMgr = Dispatch::MessageQueueManager::GetInstance();
    if (_thread)
    {
        queueMgr.UnregisterMessageQueue(_clientQueueId);

        _clientContext = nullptr;
        _clientQueueId = ThreadID();

        auto networkThread = static_cast<std::thread *>(_thread);
        delete networkThread;

        _thread = nullptr;
    }

    queueMgr.UnregisterMessageQueue(_mainThreadID);
    if (_dispatcher) delete _dispatcher;
    _host = nullptr;
}

#endif