#pragma once

#include <string>
#include "BaseLayer.h"

class Base;

class PropertiesLayer : public BaseLayer
{
public:
    PropertiesLayer(View* view);

    virtual void render() override;

protected:
    void renderProperties(Base* base, const std::string& name, uint32_t level);

private:
    const uint32_t m_levelOpened = 3;
};