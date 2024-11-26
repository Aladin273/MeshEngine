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
    void renderProperties(Base* base, const std::string& name);
};