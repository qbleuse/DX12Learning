#pragma once

#include "Camera.hpp"
#include "Demo.hpp"
#include <array>

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

    ID3D12RootSignature*    _rootSignature      = nullptr;
    ID3D12PipelineState*    _pso                = nullptr;

    D3D12_VIEWPORT viewport     = {};
    D3D12_RECT     scissorRect  = {};

    bool MakeShader(D3D12_SHADER_BYTECODE& vertex, D3D12_SHADER_BYTECODE& pixel);
    bool MakeTexture(const DemoInputs& inputs, const DX12Handle& dx12Handle_);
    bool MakePipeline(const DX12Handle& dx12Handle_, D3D12_SHADER_BYTECODE& vertex, D3D12_SHADER_BYTECODE& pixel);

    struct UploadTexture
    {
        /* GPU Texture */
        ID3D12Resource* gpuTexture  = nullptr;
        void*           mapHandle   = nullptr;

        ~UploadTexture();
    };

    std::array<ID3D12DescriptorHeap*, FRAME_BUFFER_COUNT> _descHeaps;
    std::array<UploadTexture, FRAME_BUFFER_COUNT> gpuTextures;

    /* CPU Texture */
    GPM::vec4* cpuTexture = nullptr;
    size_t textureBufferSize = 0;
    int width = 0;
    int height = 0;
};