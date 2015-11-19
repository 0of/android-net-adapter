# android-net-adapter
extendable Android native net adapter

> I wrote these for one week for investigations NOT designed for production

- **highly portability(few codes for platform adaption)**
- **C-style network protocol compatibility**
- **isolated network thread and async request handling**

# Architecture
- Network
- Dispatch
- Core
- App-adaptor
- JNI

## Network Layer
- Socket
- select based Reactor pattern
- Network exceptions

## Dispatch Layer
- Context
- EventDispatcher
- MessageQueue
- Notifier

## Core Layer
- ClientRunnable
- NetworkContext
- NetworkManager
- NetworkMessage
- NetworkMiddleware

## App adaptor Layer
- CommonApplication
- Android Implemented application

# License
  MIT License
