#pragma once

#include "BaseLayer.h"

class PropertiesLayer : public BaseLayer
{
public:
    PropertiesLayer(View* view);

    virtual void render() override;
};