#include "MessageQueue.h"

#include <map>

namespace Dispatch
{
    MessageType Message::RegisterType(std::uint16_t hint)
    {
        static std::map<MessageType, MessageType> registered;

        MessageType bigger = 0;

        if (!registered.empty())
        {
            bigger  = registered.rbegin()->first + 1;
        }

        registered.insert(std::make_pair(bigger, 0));

        return bigger;
    }
}