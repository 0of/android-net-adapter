#ifndef EXCEPTIONEVENT_H
#define EXCEPTIONEVENT_H

#include <exception>

#include "EventHandler.h"

namespace Dispatch
{
    class ExceptionEvent : public Event
    {
        using Base = Event;

    public:
        static Dispatch::MessageType Type;
    
    private:
        std::exception_ptr _exceptPtr;

    public:
        ExceptionEvent();
        ~ExceptionEvent();

    public:
        void Rethrow();
    };
}

#endif  /* EXCEPTIONEVENT_H */