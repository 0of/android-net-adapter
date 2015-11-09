#include "Context.h"

#include <thread>

#include "Notifier.h"
#include "MessageQueue.h"
#include "MessageQueueManager.h"
#include "Exception/Exception.h"

namespace Dispatch
{
    Context::Context()
        : _contextTID{ std::this_thread::get_id() }
    {

    }

    void Context::GetCapabilityDescs(CapabilityDescs& descs) const
    {
        return;
    }

    void Context::AcquireNotifier(Notifier& notifier)
    {
        throw Except::LogicException{ LOGIC_EXCEPT("Not implemented") };
    }

    void Context::Post(Message* &&msg)
    {
        auto contextMessageQueue = MessageQueueManager::GetInstance().GetMessageQueue(_contextTID);
        if (!contextMessageQueue)
            throw Except::LogicException{ LOGIC_EXCEPT("thread has no message looper") };

        contextMessageQueue->Post(shared_from_this(), std::move(msg));
    }

    void * Context::GetRawCapability(std::uint16_t cap)
    {
       return nullptr;
    }
}
