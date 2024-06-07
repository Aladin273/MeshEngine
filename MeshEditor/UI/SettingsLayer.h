#pragma once

#include "BaseLayer.h"

class SettingsLayer : public BaseLayer
{
public:
    SettingsLayer(View* view);

    virtual void render() override;
};