#ifndef NETWORKMIDDLEWARE_H
#define NETWORKMIDDLEWARE_H

#include "NetworkTransportObject.h"

namespace Core
{
     // intercept read event
    class IncomingMiddleware
    {
    public:
        virtual ~IncomingMiddleware() = default;

    public:
        // do NOT delete or restore readStream
        // you can add interception code here
        // the calling chain is blocked
        virtual void OnIncoming(std::int32_t id, DirectAccessibleStream *readStream, IncomingMiddleware *next) = 0;
    };

    // intercept write event
    class OutcomingMiddleware
    {
    public:
        virtual ~OutcomingMiddleware() = default;

    public:
        virtual void OnOutcoming(std::int32_t id, const RequestObject *requestObject, RequestStream *& /* allocate on heap */readStream, OutcomingMiddleware *next) = 0;
    };
}

#endif  /* NETWORKMIDDLEWARE_H */