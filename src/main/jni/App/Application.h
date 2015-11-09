#ifndef APPLICATION_H
#define APPLICATION_H

#include "globals.h"
#include "Dispatch/Context.h"

namespace Dispatch
{
    class Notifier;
}

/**
 *  main thread context
 *  any adapting host should create an application instance and call Run()
 *
 *  support services:
 *      NetworkService
 */
class Application : public Dispatch::Context
{
private:
    class ApplicationImpl;
    ApplicationImpl *_impl;

private:
    static Application *Instance;

public:
    static Application& GetInstance();

public:
    Application();
    virtual ~Application();

public:
    virtual void AcquireNotifier(Dispatch::Notifier& notifier) override;

public:
    // initialize other contexts and services
    // hostContext for host OS context
    void Run(void *hostContext);

    /* post quit message, not yet quit */
    void Quit();

public:
    /* normally do nothing */
    /* init viewers */
    virtual void OnInit();
    virtual void OnQuit();

protected:
    virtual void * GetRawCapability(std::uint16_t cap) override;
};

#endif  /* APPLICATION_H */