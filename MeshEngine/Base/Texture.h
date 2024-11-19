#pragma once

#include "Base.h"

struct Texture : public Base
{
    Texture() { m_name = "Texture"; };

    Texture(uint32_t id, const std::string& type, const std::string& path)
        : id(id), type(type), path(path) { m_name = "Texture"; };

    virtual ~Texture() {};

    virtual void bind() override
    {
        bindProperty(id);
        bindProperty(type);
        bindProperty(path);

        super::bind();
    };

    uint32_t id = 1;
    std::string type = "None";
    std::string path = "None";
};