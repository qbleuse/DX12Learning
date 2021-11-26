#pragma once

#include "Camera.hpp"
#include "Demo.hpp"

class DemoRayCPU final : public Demo
{
    protected:
        Camera mainCamera = {};

	public:

    ~DemoRayCPU() final;
    DemoRayCPU();
    DemoRayCPU(const DemoInputs& inputs);

    void UpdateAndRender(const DemoInputs& inputs) final;

    const char* Name() const final { return typeid(*this).name(); }

    // TODO: Generer GPU texture
};