#include "Exception.h"

namespace Except
{
    Exception::Exception() NOEXCEPT
        : _reason{}
    {}

    Exception::Exception(const Exception& ex) NOEXCEPT
        : _reason(ex._reason)
    {}

    Exception & Exception::operator = (const Exception& ex) NOEXCEPT
    {
        _reason = ex._reason;
        return *this;
    }

    Exception::~Exception() NOEXCEPT
    {}
}