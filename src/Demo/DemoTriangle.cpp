#include "Demo/DemoTriangle.hpp"
#include "DX12Handle.hpp"

DemoTriangle::~DemoTriangle()
{

}

DemoTriangle::DemoTriangle(const DemoInputs& inputs_, const DX12Handle& dx12Handle_)
{
}

void DemoTriangle::UpdateAndRender(const DemoInputs& inputs_)
{

    // Clear the render target by using the ClearRenderTargetView command
    const float clearColor[] = { 0.0f, 0.5f, 0.4f, 1.0f };
    inputs_.renderContext.currCmdList->ClearRenderTargetView(inputs_.renderContext.currBackBufferHandle, clearColor, 0, nullptr);
}