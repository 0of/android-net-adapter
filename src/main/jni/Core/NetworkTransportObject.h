#ifndef NETWORKTRANSPORTOBJECT_H
#define NETWORKTRANSPORTOBJECT_H

#include "globals.h"

namespace Core
{
    class NOVTABLE RequestStream
    {
    public:
        virtual ~RequestStream() = default;

    public:
        virtual std::uint32_t Read(std::uint8_t *buffer, std::uint32_t len) = 0;
    };

    class ResponseStream
    {
    public:
        virtual ~ResponseStream() = default;

    public:
        virtual std::uint32_t Read(std::uint8_t *buffer, std::uint32_t len) = 0;
    };

    class NOVTABLE DirectAccessibleStream : public RequestStream
    {
    public:
        // read only
        virtual const std::uint8_t *ConstData(std::uint32_t& len) = 0;
    };

    // request object
    class RequestObject
    {
    private:
        enum HoldType{
            Stream, Object
        };

    private:
        HoldType _holdType;
        // if not an object, this value is undefined
        ObjectType _objectType;

        union {
            RequestStream *_stream;
            void *_object;
        } _hold;

    public:
        explicit RequestObject(RequestStream *&& reqStream);
        RequestObject(void *object, ObjectType type);
        ~RequestObject();

    public:
        bool As(RequestStream *& stream) const;
        bool As(void *& object, ObjectType& type) const;
    };

}

#endif /* NETWORKTRANSPORTOBJECT_H */