#include "../JNI/com__0of_adapters_UIMessageHandler.h"

#include <memory>

#include "../JNI/JNITypes.h"

#include "Exception/Exception.h"
#include "Dispatch/EventHandler.h"
#include "Dispatch/MessageQueue.h"
#include "Dispatch/Notifier.h"

JNIEXPORT void JNICALL Java_com__0of_adapters_UIMessageHandler__1onMessageHandle(JNIEnv *jEnv, jobject handler, jlong handle)
{
    // retrieve as managed pointer
    auto messageContainer = std::unique_ptr<MessageContainer>{ (MessageContainer *)(std::intptr_t)(handle) };
    if (messageContainer)
    {
        auto context = messageContainer->first;

        // filter message types
        // ONLY support event container message
        auto eventMessage = message_cast<Dispatch::EventMessage>(messageContainer->second);

        if (context && eventMessage)
        {
            Dispatch::Notifier notifier;

            try
            {
                // acquire notifier to send message
                context->AcquireNotifier(notifier);
                notifier.Notify(*eventMessage);
            }
            catch (const Except::Exception& ex)
            {
                // log
            }
        }
    }
}