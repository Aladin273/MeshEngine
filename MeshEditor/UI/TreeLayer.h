#pragma once

#include "BaseLayer.h"

class TreeLayer : public BaseLayer
{
public:
    TreeLayer(View* view);

    virtual void render() override;
};