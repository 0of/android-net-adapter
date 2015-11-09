#include "NetworkMessages.h"

#include "NetworkEvents.h"

namespace Core
{
    // Messages
    Dispatch::MessageType ConnectionRequestMessage::Type = Message::RegisterType();
    Dispatch::MessageType SendRequestMessage::Type = Message::RegisterType();
    Dispatch::MessageType DisconnectionRequestMessage::Type = Message::RegisterType();

    // Events
    Dispatch::EventType ReceiveResponseEvent::Type = Dispatch::Event::RegisterType();
    Dispatch::EventType ConnectedEvent::Type = Dispatch::Event::RegisterType();
    Dispatch::EventType SentRequestEvent::Type = Dispatch::Event::RegisterType();
    Dispatch::EventType DisconnectedEvent::Type = Dispatch::Event::RegisterType();
}
