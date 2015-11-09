#ifndef GLOBALS_H
#define GLOBALS_H

#include <cstddef>
#include <cstdint>

/*
The OS
*/
#if (defined(WIN64) || defined(_WIN64) || defined(__WIN64__))
#  define OS_WIN32
#  define OS_WIN64
#elif (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__))
#  if defined(WINCE) || defined(_WIN32_WCE)
#    error "Not support WCE for current version"
#  else
#    define OS_WIN32
#  endif
#elif (defined(__APPLE__))
#  ifdef __LP64__
#    define OS_DARWIN64
#    define OS_OSX64
#  else
#    define OS_DARWIN32
#    define OS_OSX32
#  endif
#elif (defined(__ANDROID__))
#  define OS_ANDROID
#elif (defined(__linux__) || defined(__linux))
#  define OS_LINUX
#else
#  error "Not support OS detected!"
#endif

#if (defined(OS_WIN32) || defined(OS_WIN64))
#  define OS_WINDOWS
#endif

#if (defined(OS_ANDROID))
#  define OS_LINUX
#endif

/*
The compilers
not support vendors: Borland, Intel and Watcom
*/

#if (defined(_MSC_VER))
#  define COMPILER_MSVC
#elif (defined(__GNUC__))
#  define COMPILER_GCC
#  if (defined(__MINGW32__))
#    define COMPILER_MINGW32
#  elif defined(__clang__)
#    define COMPILER_CLANG
#  endif
#else
#  error "Not support compiler detected!"
#endif

/*
Compiler features
*/
#ifdef COMPILER_MSVC
#  ifndef DLLEXPORT
#  define DLLEXPORT __declspec(dllexport)
#  endif
#else
#  ifndef DLLEXPORT
#  define DLLEXPORT
#  endif
#endif

#ifdef COMPILER_MSVC
#  ifndef NOVTABLE
#  define NOVTABLE __declspec(novtable)
#  endif
#else
#  ifndef NOVTABLE
#  define NOVTABLE
#  endif
#endif

#ifdef COMPILER_MSVC
#  ifndef NOEXCEPT
#  define NOEXCEPT throw()
#  endif
#elif defined(COMPILER_GCC)
#  ifndef NOEXCEPT
#    if (__GNUC__ * 10 + __GNUC_MINOR__ >= 46)
#    define NOEXCEPT noexcept
#    else
#    define NOVTABLE
#    endif
#  endif
#elif defined(COMPILER_CLANG)
#  ifndef NOEXCEPT
#    if __has_feature(cxx_noexcept)
#    define NOEXCEPT noexcept
#    else
#    define NOEXCEPT throw()
#    endif
#  endif
#else
#  ifndef NOEXCEPT
#  define NOEXCEPT throw()
#  endif
#endif

#ifndef FRIEND_DECL
#define FRIEND_DECL(classname) friend class classname;
#endif  /* FRIEND_DECL */

#ifndef DISABLE_COPY_DECL
#define DISABLE_COPY_DECL(classname) protected: classname(const classname&); \
    classname& operator = (const classname&);
#endif

#ifndef FOREVER
#define FOREVER for(;;)
#endif

/* thread defs */
#include <thread>
typedef std::thread::id ThreadID;

/* object type defs */
typedef std::int32_t ObjectType;

#endif // GLOBALS_H
