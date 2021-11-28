/* system include */
#include <system_error>
#include <cstdio>

/* d3d */
#include <d3dcompiler.h>
#include "DX12Helper.hpp"
#include "DX12Handle.hpp"

#include "Demo/DemoTriangle.hpp"


DemoTriangle::~DemoTriangle()
{
    if (_rootSignature)
        _rootSignature->Release();
}

DemoTriangle::DemoTriangle(const DemoInputs& inputs_, const DX12Handle& dx12Handle_)
{
    HRESULT hr;
    ID3DBlob* error;
    ID3DBlob* tmp;

    D3D12_ROOT_SIGNATURE_DESC rootDesc = {};
    rootDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;


    hr = D3D12SerializeRootSignature(&rootDesc, D3D_ROOT_SIGNATURE_VERSION_1, &tmp, &error);

    if (FAILED(hr))
    {
        printf("Failing serializing root signature of demo %s: %s, %s\n", Name() , std::system_category().message(hr).c_str(), (char*)error->GetBufferPointer());
        error->Release();
        return;
    }


    hr = dx12Handle_._device->CreateRootSignature(0, tmp->GetBufferPointer(), tmp->GetBufferSize(), IID_PPV_ARGS(&_rootSignature));
    if (FAILED(hr))
    {
        printf("Failing creating root signature of demo %s: %s\n", Name(), std::system_category().message(hr).c_str());
        return;
    }

    std::string source = (const char*)R"(#line 45
    struct VOut
    {
        float4 position : SV_POSITION;
        float3 color : COLOR; 
    };

    
    VOut vert(float3 position : POSITION, float3 color : COLOR)
    {
        VOut output;
    
        output.position = position;
        output.color    = color;
    
        return output;
    }

    float4 frag(float4 position : SV_POSITION, float3 color : COLOR) : SV_TARGET
    {
        return float4(color,1.0f);
    }
    )";

    D3D12_SHADER_BYTECODE vertex;
    D3D12_SHADER_BYTECODE pixel;
    if (!DX12Helper::CompileVertex(source, &tmp, vertex) || !DX12Helper::CompilePixel(source, &tmp, pixel))
        return;


}

void DemoTriangle::UpdateAndRender(const DemoInputs& inputs_)
{

    // Clear the render target by using the ClearRenderTargetView command
    const float clearColor[] = { 0.0f, 0.5f, 0.4f, 1.0f };
    inputs_.renderContext.currCmdList->ClearRenderTargetView(inputs_.renderContext.currBackBufferHandle, clearColor, 0, nullptr);
}