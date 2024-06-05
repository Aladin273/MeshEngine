#pragma once

#include "BaseLayer.h"

class DockpaneLayer : public BaseLayer
{
public:
    DockpaneLayer(View* view);

    virtual void render() override;
};