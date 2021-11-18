#pragma once

#include <typeinfo>

#include "camera.hpp"
#include "GPM/Vector3.hpp"

struct D3D12Contextual;
struct ImGuiContext;
typedef void* (*GLADloadproc)(const char* name);

struct DemoInputs
{
    float deltaTime;
    GPM::Vec3 windowSize;
    CameraInputs cameraInputs;
    D3D12Contextual* renderContext;
};

class Demo
{
public:

    virtual ~Demo() {}
    virtual void UpdateAndRender(const DemoInputs& inputs) = 0;
    virtual const char* Name() const { return typeid(*this).name(); }
};
