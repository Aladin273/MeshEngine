#pragma once

#include <cstdint>

class GuiLayer
{
public:
	virtual void render() = 0;
	virtual void update() = 0;
};								

class GuiSystem
{
public:
	virtual void init() = 0;

	virtual void begin() = 0;
	virtual void end() = 0;

	virtual void render() = 0;          
	virtual void render(GuiLayer&) = 0; 

	virtual void pushLayer(uint32_t, GuiLayer&) = 0;
	virtual void popLayer(uint32_t) = 0;

	virtual bool wantCaptureMouse() = 0;
	virtual bool wantCaptureKeyboard() = 0;
};
