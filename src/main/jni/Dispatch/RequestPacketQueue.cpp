#include "RequestPacketQueue.h"

#include <cassert>
#include <algorithm>

namespace Dispatch
{

    RequestPacketQueue::RequestPacketQueue(ThreadID tid)
        : _lock{}
        , _unreadMessages{}
        , _attachedThreadID{tid}
    {

    }

    RequestPacketQueue::~RequestPacketQueue()
    {
        /* clear all the message */
        if (!_lock.try_lock())
        {
            /* lock is occupied */
            assert(0);
        }

        /* log messages */
        
        /* free all */
        for (auto each : _unreadMessages)
            delete each.second;
    }

    void RequestPacketQueue::Post(const SharedContext& context, Message *&&msg)
    {
        using LockType = decltype(_lock);

        if (msg)
        {
            std::lock_guard<LockType> guard{ _lock };
            (void)guard;

            _unreadMessages.push_back(std::make_pair(context, msg));
        }
    }
}

