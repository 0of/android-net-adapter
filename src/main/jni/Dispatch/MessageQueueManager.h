#ifndef MESSAGEQUEUEMANAGER_H
#define MESSAGEQUEUEMANAGER_H

#include <memory>
#include <map>
#include <mutex>

#include "globals.h"

namespace Dispatch
{
    class MessageQueue;
    using SharedMessageQueue = std::shared_ptr<MessageQueue>;
    using QueueIdentifier = ThreadID;

    class MessageQueueManager
    {
        using Queues = std::map<QueueIdentifier, SharedMessageQueue>;
        using RecursiveMutex = std::recursive_mutex;

    public:
        // not thread safe
        // must be initialized in main thread
        static MessageQueueManager& GetInstance();

    protected:
        Queues _queues;
        RecursiveMutex _lock;

    public:
        virtual ~MessageQueueManager() = default;

    public:
        virtual SharedMessageQueue GetMessageQueue(const QueueIdentifier& id);

    public:
        virtual void RegisterMessageQueue(const QueueIdentifier& id, const SharedMessageQueue& queue);
        virtual void UnregisterMessageQueue(const QueueIdentifier& id);
    };
}

#endif  /* MESSAGEQUEUEMANAGER_H */