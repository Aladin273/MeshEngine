#pragma once

#include "Base.h"

struct Texture : public Base
{
    Texture() {};

    Texture(uint32_t id, const std::string& type, const std::string& path)
        : id(id), type(type), path(path) {};

    virtual ~Texture() {};

    virtual void bind() override
    {
        bindProperty(id);
        bindProperty(type);
        bindProperty(path);

        super::bind();
    };

    uint32_t id;
    std::string type;
    std::string path;
};