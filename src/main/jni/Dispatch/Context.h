#ifndef CONTEXT_H
#define CONTEXT_H

#include <type_traits>
#include <vector>
#include <memory>

#include "globals.h"
#include "CapabilityCategory.h"

namespace Dispatch
{
    class Notifier;
    class Context;
    class Message;

    using CapabilityDescs = std::vector<std::uint16_t>;
    using SharedContext = std::shared_ptr<Context>;

    /* thread bound object */
    class Context : public std::enable_shared_from_this<Context>
    {
    protected:
        ThreadID _contextTID;

    public:
        Context();
        virtual ~Context() = default;

    public:
        // service provider
        // the caller should take the ownership of the return pointer
        template<typename CapInterface>
        typename std::add_pointer<CapInterface>::type ProvideCapability()
        {
            return static_cast<typename std::add_pointer<CapInterface>::type>(GetRawCapability(Defs::CapabilityMetaClass<CapInterface>::ID));
        }

        virtual void GetCapabilityDescs(CapabilityDescs& descs) const;

    public:
        // not implemented
        // all the derived class should implement this method if support notifier callings
        virtual void AcquireNotifier(Notifier& notifier);

        // post message to context thread message queue
        // throw exception if thread the context belonged to is not existed
        void Post(Message* &&msg);

    protected:
        // default return nullptr
        virtual void * GetRawCapability(std::uint16_t cap);
    };
}

#endif  /* CONTEXT_H */