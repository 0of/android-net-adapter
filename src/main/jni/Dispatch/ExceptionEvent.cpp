#include "ExceptionEvent.h"

namespace Dispatch
{
    Dispatch::MessageType ExceptionEvent::Type = ExceptionEvent::RegisterType();

    ExceptionEvent::ExceptionEvent()
        : Base(ExceptionEvent::Type)
        , _exceptPtr(std::current_exception())
    {}

    ExceptionEvent::~ExceptionEvent()
    {

    }

    void ExceptionEvent::Rethrow()
    {
        std::rethrow_exception(_exceptPtr);
    }
}