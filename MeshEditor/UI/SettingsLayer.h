#pragma once

#include "BaseLayer.h"

class SettingsLayer : public PropertiesLayer
{
public:
    SettingsLayer(View* view);

    virtual void render() override;
};