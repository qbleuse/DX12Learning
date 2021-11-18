#pragma once

#include <typeinfo>

#include "types.hpp"
#include "camera.hpp"

struct ImGuiContext;
typedef void* (*GLADloadproc)(const char* name);

struct DemoInputs
{
    float deltaTime;
    float2 windowSize;
    CameraInputs cameraInputs;
};

class Demo
{
public:
    virtual ~Demo() {}
    virtual void UpdateAndRender(const DemoInputs& inputs) = 0;
    virtual const char* Name() const { return typeid(*this).name(); }
};
