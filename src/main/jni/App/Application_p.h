#ifndef APPLICATION_P_H
#define APPLICATION_P_H

#include "Application.h"
#include "Dispatch/MessageQueueManager.h"
#include "Dispatch/EventDispatcher.h"

class Application::ApplicationImpl
{
public:
    // init at
    //  InitMainThreadQueue
    ThreadID _mainThreadID;
    Dispatch::EventDispatcher *_dispatcher;
    void *_host;

    // init at
    //  InitThreadQueues
    Dispatch::SharedContext _clientContext;
    Dispatch::QueueIdentifier _clientQueueId;
    void *_thread;

public:
    ApplicationImpl();
    ~ApplicationImpl();

public:
    /* first called */
    void InitMainThreadQueue(void *hostContext);

    void LoadConfig();

    void InitThreadQueues();

public:
    void OnMainThreadQuit();
};

#endif  /* APPLICATION_P_H */