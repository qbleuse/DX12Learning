#pragma once

#include "Camera.hpp"
#include "Demo.hpp"

struct ID3D12Resource;
class DX12Handle;

class DemoRayCPU final : public Demo
{
    protected:
        Camera mainCamera = {};

	public:

    ~DemoRayCPU() final;
    DemoRayCPU(const DemoInputs& inputs, const DX12Handle& dx12Handle_);

    void UpdateAndRender(const DemoInputs& inputs) final;

    const char* Name() const final { return typeid(*this).name(); }


    bool MakeTexture(const DemoInputs& inputs, const DX12Handle& dx12Handle_);

    /* GPU Texture */
    ID3D12Resource* gpuTexture;

    /* CPU Texture */
    BYTE* cpuTexture;
};