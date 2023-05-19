#pragma once

class IGuiLayer
{
public:
	virtual void render() = 0;           // Here is your gui description (buttons, sliders and more)
	virtual void context(void*) = 0;     // You must set UI contex, because dll cannot share this, 
};										 // also, you can use contructor (ImGui::SetCurrentContext example)

class IGuiSystem
{
public:
	virtual void init() = 0;                // You can call render(), also you can call begin() -> content -> end()

	virtual void begin() = 0;
	virtual void end() = 0;
	virtual void render() = 0;              // Render all pushed layers
	virtual void render(IGuiLayer&) = 0;    // Render arg

	virtual void setContext(void*) = 0;
	virtual void* getContext() = 0;

	virtual void pushLayer(uint32_t, IGuiLayer&) = 0;
	virtual void popLayer(uint32_t) = 0;

	virtual void turnDocking(bool enable) = 0;
	virtual void turnVieports(bool enable) = 0;

	virtual bool wantCapture() = 0;
};
