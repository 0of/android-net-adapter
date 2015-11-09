#include "Application.h"

#include "Application_p.h"

#include "Exception/Exception.h"
#include "Dispatch/Notifier.h"
#include "Core/NetworkService.h"

Application* Application::Instance = nullptr;

Application& Application::GetInstance()
{
    if (!Instance)
    {
        throw Except::LogicException{ LOGIC_EXCEPT("Application not initialized") };
    }

    return *Instance;
}

Application::~Application()
{
    if (_impl) delete _impl;
    if (Instance) Instance = nullptr;
}

Application::Application()
    : _impl{ new ApplicationImpl }
{
    /* no need for double lock */
    Instance = this;
}

void Application::AcquireNotifier(Dispatch::Notifier& notifier)
{
    if (!_impl) throw Except::LogicException{ LOGIC_EXCEPT("Impl not initialized") };

    if (std::this_thread::get_id() != _impl->_mainThreadID)
    {
        throw Except::LogicException{ LOGIC_EXCEPT("Can not acquire notifier from other threads") };
    }

    Dispatch::Notifier::Make(notifier, _impl->_dispatcher, this);
}

void Application::Run(void *hostContext)
{
    if (!_impl) throw Except::LogicException{ LOGIC_EXCEPT("Impl not initialized") };

    _impl->InitMainThreadQueue(hostContext);
    _impl->LoadConfig();

    _impl->InitThreadQueues();

    OnInit();
}

void Application::Quit()
{
    if (!_impl) throw Except::LogicException{ LOGIC_EXCEPT("Impl not initialized") };

    /* get main thread queue and quit */
    throw "TODO- NOT Implemented";
}

void Application::OnInit()
{
    /* init guest context */
    /* client events handler register list */

}

void Application::OnQuit()
{

}

void * Application::GetRawCapability(std::uint16_t cap)
{
    if (!_impl) throw Except::LogicException{ LOGIC_EXCEPT("Impl not initialized") };

    if (IS_CAPABILITY(cap, Core::NetworkService))
    {
        return _impl->_clientContext->ProvideCapability<Core::NetworkService>();
    }

    return nullptr;
}