#pragma once

#include "BaseLayer.h"

class ConsoleLayer : public BaseLayer
{
public:
    ConsoleLayer(View* view);

    virtual void render() override;
};