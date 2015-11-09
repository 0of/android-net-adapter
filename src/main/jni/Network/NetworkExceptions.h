#ifndef NETWORKEXCEPTIONS_H
#define NETWORKEXCEPTIONS_H

#include "../Exception/Exception.h"

namespace Net
{
    EXCEPTION_DEF(SocketException, Except::IOException)
    EXCEPTION_DEF(NetException, Except::IOException)

    EXCEPTION_DEF(ConnectionAbortedException, NetException)
    EXCEPTION_DEF(ConnectionResetException, NetException)
    EXCEPTION_DEF(ConnectionRefusedException, NetException)

    using InvalidSocketException = std::logic_error;
}

#endif  /* NETWORKEXCEPTIONS_H */