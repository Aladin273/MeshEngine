#include "DynamicLibrary.h"

#if PLATFORM == PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#if !defined(NOMINMAX) && defined(_MSC_VER)
#define NOMINMAX
#endif
#include <windows.h>
#endif

DynamicLibrary::DynamicLibrary(const std::string& name)
    : instance(nullptr)
{
#if PLATFORM == PLATFORM_WIN32
    instance = (void*)LoadLibrary(name.c_str());
    //instance = (void*)LoadLibrary(name.c_str(), NULL, 0);
#endif
}

DynamicLibrary::~DynamicLibrary()
{
#if PLATFORM == PLATFORM_WIN32
    FreeLibrary((HMODULE)instance);
#endif
}

void* DynamicLibrary::getSymbol(const std::string& symbolName) const
{
#if PLATFORM == PLATFORM_WIN32
    return (void*)GetProcAddress((HMODULE)instance, symbolName.c_str());
#endif
    return nullptr;
}

