#pragma once

#include <string>

class DynamicLibrary
{
public:
    DynamicLibrary(const std::string& name);
    ~DynamicLibrary();

    void* getSymbol(const std::string& symbolName) const;

    template<class T>
    T getSymbol(const std::string& symbolName) const
    {
        return (T)getSymbol(symbolName);
    }
private:
    void* instance;
};

