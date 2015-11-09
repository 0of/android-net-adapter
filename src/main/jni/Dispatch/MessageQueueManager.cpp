#include "MessageQueueManager.h"
#include "Exception/Exception.h"

namespace Dispatch
{
    MessageQueueManager& MessageQueueManager::GetInstance()
    {
        static MessageQueueManager mgr;
        return mgr;
    }

    SharedMessageQueue MessageQueueManager::GetMessageQueue(const QueueIdentifier& id)
    {
        std::lock_guard<RecursiveMutex> lock{ _lock };
        auto found = _queues.find(id);
        if (found != _queues.end())
        {
            return found->second;
        }

        return nullptr;
    }

    void MessageQueueManager::RegisterMessageQueue(const QueueIdentifier& id, const SharedMessageQueue& queue)
    {
        if (!queue)
            throw Except::InvalidArgumentException{ LOGIC_EXCEPT("null queue") };

        std::lock_guard<RecursiveMutex> lock{ _lock };
        _queues[id] = queue;
    }

    void MessageQueueManager::UnregisterMessageQueue(const QueueIdentifier& id)
    {
        std::lock_guard<RecursiveMutex> lock{ _lock };
        auto found = _queues.find(id);
        if (found != _queues.end())
        {
            _queues.erase(found);
        }
    }
}

