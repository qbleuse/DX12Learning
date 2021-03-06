#pragma once

#include "Camera.hpp"
#include "Demo.hpp"
#include <array>

struct ID3D12Resource;
class DX12Handle;

class DemoRayCPUGradiant final : public Demo
{
    protected:
        Camera mainCamera = {};

	public:

    ~DemoRayCPUGradiant() final;
    DemoRayCPUGradiant(const DemoInputs& inputs, const DX12Handle& dx12Handle_);

    void UpdateAndRender(const DemoInputs& inputs) final;

    const char* Name() const final { return typeid(*this).name(); }

    ID3D12RootSignature*    _rootSignature      = nullptr;
    ID3D12PipelineState*    _pso                = nullptr;

    D3D12_VIEWPORT viewport     = {};
    D3D12_RECT     scissorRect  = {};

    bool MakeShader(D3D12_SHADER_BYTECODE& vertex, D3D12_SHADER_BYTECODE& pixel);
    bool MakeTexture(const DemoInputs& inputs, const DX12Handle& dx12Handle_);
    bool MakePipeline(const DX12Handle& dx12Handle_, D3D12_SHADER_BYTECODE& vertex, D3D12_SHADER_BYTECODE& pixel);

    void UpdateInspector();
    void Update(const DemoInputs& inputs_);
    void Render(const DemoInputs& inputs_);

    struct Uniform
    {
        GPM::Vec4 cleanColor { 1.0f, 0.2f, 0.4f, 1.0f };
    } uniform;

    struct UploadTexture
    {
        /* GPU Texture */
        ID3D12Resource* uploadTexture  = nullptr;
        ID3D12Resource* defaultTexture = nullptr;
        void*           mapHandle   = nullptr;

        ~UploadTexture();
    };

    std::array<ID3D12DescriptorHeap*, FRAME_BUFFER_COUNT> _descHeaps;
    std::array<UploadTexture, FRAME_BUFFER_COUNT> gpuTextures;

    /* CPU Texture */
    GPM::vec4*              cpuTexture  = nullptr;
    D3D12_SUBRESOURCE_DATA  data        = {};
    UINT                    width       = 0;
    UINT                    height      = 0;
};