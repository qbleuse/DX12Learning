#pragma once

#include "Camera.hpp"
#include "Demo.hpp"

class DX12Handle;

class DemoTriangle final : public Demo
{

public:

    ~DemoTriangle() final;
    DemoTriangle(const DemoInputs& inputs_, const DX12Handle& d3dhandle_);

    void UpdateAndRender(const DemoInputs& inputs_) final;

    inline const char* Name() const final { return typeid(*this).name(); }
};