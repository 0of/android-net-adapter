#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <string>
#include <exception>
#include <stdexcept>

#include "globals.h"

namespace Except
{
    class Exception
    {
    private:
        std::string _reason;

    public:
        Exception() NOEXCEPT;

        template<typename T>
        explicit Exception(T&& reason) NOEXCEPT
            : _reason(std::forward<T>(reason))
        {}

        Exception(const Exception& ex) NOEXCEPT;
        Exception & operator = (const Exception& ex) NOEXCEPT;
        virtual ~Exception() NOEXCEPT;

    public:
        std::string Reason() const NOEXCEPT{ return _reason; }
    };
}

#define EXCEPTION_DEF(ClassName, ParentClass) \
class ClassName : public ParentClass \
{ \
public:	\
    ClassName() NOEXCEPT \
    : ParentClass() \
    {} \
    template<typename T>            \
    explicit ClassName(T&& reason) NOEXCEPT \
        : ParentClass(std::forward<T>(reason)) \
    {} \
    ClassName(const ClassName& ex) NOEXCEPT \
        : ParentClass(static_cast<const ParentClass&>(ex)) \
    {} \
    virtual ~ClassName() NOEXCEPT \
    {} \
    ClassName& operator = (const ClassName& exc) NOEXCEPT \
    { \
        ParentClass::operator=(exc); \
        return *this; \
    }\
};

namespace Except
{
    EXCEPTION_DEF(IOException, Exception)
    EXCEPTION_DEF(NotSupportedException, Exception)
    EXCEPTION_DEF(OutOfMemoryException, Exception)
    EXCEPTION_DEF(FileNotFoundException, Exception)
    EXCEPTION_DEF(PermissionDeniedException, Exception)
    EXCEPTION_DEF(TimeoutException, Exception)

    using LogicException = std::logic_error;
    using InvalidArgumentException = std::invalid_argument;
}

#ifndef LOGIC_EXCEPT
#define LOGIC_EXCEPT(desc) desc
#endif  /* LOGIC_EXCEPT */

#endif  /* EXCEPTION_H */

