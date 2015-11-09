#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H

#include <type_traits>
#include <memory>

#include "globals.h"
#include "Context.h"

namespace Dispatch
{
    using MessageType = std::uint16_t;

    class Message
    {
    protected:
        /* not thread safe */
        /* static initialize */
        static MessageType RegisterType(std::uint16_t hint = 0);

    public:
        enum MessageCategory : std::uint16_t
        {
            MessageCategory_User = 0,	/*!< user message. \n�û���Ϣ */
            MessageCategory_Event = 1,	/*!< event. \n�¼���Ϣ */
            MessageCategory_Notifier = 2	/*!< notifier . \n֪ͨ�� */
        };

    private:
        MessageCategory _msgCategory;

    protected:
        MessageType _msgType;

    protected:
        explicit Message(MessageCategory category, std::uint16_t type)
            : _msgCategory{ category }
            , _msgType{ type }
        {}

        Message(const Message& message) = delete ;
        Message& operator = (const Message& message) = delete;

    public:
        virtual ~Message() = default;

    public:
        inline MessageCategory Category() const{ return _msgCategory; }
        inline MessageType GetType() const { return _msgType; }
    };

    class Context;

    /* abstract message queue */
    class MessageQueue
    {
    public:
        virtual ~MessageQueue() {}

    public:
        /* take message ownership */
        virtual void Post(const SharedContext& context, Message *&&msg) = 0;
        virtual ThreadID AttachedThreadID() const = 0;
    };
}

/* global */
template<typename ConcreateMessage>
typename std::add_pointer<ConcreateMessage>::type message_cast(Dispatch::Message *msg)
{
    if (msg->GetType() == ConcreateMessage::Type)
    {
        return static_cast<typename std::add_pointer<ConcreateMessage>::type>(msg);
    }
    return nullptr;
}

#endif  /* MESSAGEQUEUE_H */