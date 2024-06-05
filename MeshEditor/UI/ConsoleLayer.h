#pragma once

#include "BaseLayer.h"

class ConsoleLayer : public BaseLayer
{
public:
    ConsoleLayer(View* view);

    void render() override;

private:
    void clear();
    void copy();
};