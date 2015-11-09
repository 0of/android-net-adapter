#ifndef REQUESTPACKETQUEUE_H
#define REQUESTPACKETQUEUE_H

#include <queue>
#include <mutex>
#include <array>

#include "globals.h"
#include "MessageQueue.h"

namespace Dispatch
{
    using UnreadMessage = std::pair<SharedContext, Message *>;

    class RequestPacketQueue : public MessageQueue
    {
        using Messages = std::deque<UnreadMessage>;
        using RecursiveMutex = std::recursive_mutex;

    private:
        RecursiveMutex _lock;
        Messages _unreadMessages;
        ThreadID _attachedThreadID;

    public:
        explicit RequestPacketQueue(ThreadID tid);
        virtual ~RequestPacketQueue();

    public:
        virtual void Post(const SharedContext& context, Message *&&msg) override;

        virtual ThreadID AttachedThreadID() const { return _attachedThreadID; }

        template<typename FirstForwardIterator, typename LastForwardIterator>
        FirstForwardIterator /* fetched count */ TakeSome(FirstForwardIterator first, LastForwardIterator last)
        {
            std::lock_guard<decltype(_lock)> guard{ _lock };
            (void)guard;

            auto movedLast = _unreadMessages.begin();
            auto iter = first;

            while (iter != last && movedLast != _unreadMessages.end())
                *iter++ = *movedLast++;

            _unreadMessages.erase(_unreadMessages.begin(), movedLast);

            return iter;
        }
    };
}

#endif /* REQUESTPACKETQUEUE_H */