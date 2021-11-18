#include "DemoRayCPU.hpp"
#include "D3D12Handle.hpp"

DemoRayCPU::~DemoRayCPU()
{

}

DemoRayCPU::DemoRayCPU()
{

}

DemoRayCPU::DemoRayCPU(const DemoInputs& inputs)
{
    mainCamera.position = { 0.f, 0.f, 2.f };
}

void DemoRayCPU::UpdateAndRender(const DemoInputs& inputs)
{

    // Clear the render target by using the ClearRenderTargetView command
    const float clearColor[] = { 1.0f, 0.2f, 0.4f, 1.0f };
    inputs.renderContext->currCmdList->ClearRenderTargetView(inputs.renderContext->currBackBufferHandle, clearColor, 0, nullptr);    
}