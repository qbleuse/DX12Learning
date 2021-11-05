/* system include */
#include <system_error>
#include <cstdio>

/* ImGui */
#include <imgui_impl_glfw.h>
#include <imgui_impl_dx12.h>

/* GLFW */
#include <GLFW/glfw3.h>

#include "D3D12Handle.hpp"
#include "ImGuiHandle.hpp"

/*==== CONSTRUCTORS =====*/

ImGuiIO& ImGuiHandle::GetIO()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    return ImGui::GetIO();
}

ImGuiHandle::ImGuiHandle():
    _io{GetIO()}
{
   
    _io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    _io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    _io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    _io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (_io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
}

ImGuiHandle::~ImGuiHandle()
{

}

void ImGuiHandle::Terminate()
{
    _imguiHeapDesc->Release();

    ImGui_ImplDX12_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

/*===== INIT Methods =====*/

bool ImGuiHandle::Init(GLFWwindow* window, const D3D12Handle& dx12Handle)
{
    HRESULT hr;
    int bufferCount = dx12Handle._backbuffers.size();

    /* describe an rtv descriptor heapand create */
    D3D12_DESCRIPTOR_HEAP_DESC imguiDesc = {};
    imguiDesc.Type              = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    imguiDesc.NumDescriptors    = 1;
    imguiDesc.Flags             = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    /* create imgui heap desc */
    hr = dx12Handle._device->CreateDescriptorHeap(&imguiDesc, IID_PPV_ARGS(&_imguiHeapDesc));
    if (FAILED(hr))
    {
        printf("Failing creating DX12 imgui descriptor heap: %s\n", std::system_category().message(hr).c_str());
        return false;
    }

    /*===== Create the Command Allocators =====*/
    _cmdAllocators.resize(bufferCount);

    for (int i = 0; i < bufferCount; i++)
    {
        hr = dx12Handle._device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_cmdAllocators[i]));
        if (FAILED(hr))
        {
            printf("Failing creating DX12 command allocator nb %d: %s\n", i, std::system_category().message(hr).c_str());
            return false;
        }
    }

    /*===== Create the Command Lists =====*/
    _cmdLists.resize(bufferCount);

    for (int i = 0; i < bufferCount; i++)
    {
        hr = dx12Handle._device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, dx12Handle._cmdAllocators[i], NULL, IID_PPV_ARGS(&_cmdLists[i]));
        if (FAILED(hr))
        {
            printf("Failing creating DX12 command List nb %d: %s\n", i, std::system_category().message(hr).c_str());
            return false;
        }

        _cmdLists[i]->Close();
    }

    ImGui_ImplGlfw_InitForOther(window, true);
    ImGui_ImplDX12_Init(dx12Handle._device, bufferCount, DXGI_FORMAT_R8G8B8A8_UNORM, _imguiHeapDesc, _imguiHeapDesc->GetCPUDescriptorHandleForHeapStart(), _imguiHeapDesc->GetGPUDescriptorHandleForHeapStart());

    _barrier.Type   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    _barrier.Flags  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    _barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

    _backbuffers            = &(dx12Handle._backbuffers);
    _backbufferCPUHandles   = dx12Handle._backbufferCPUHandles;

    return true;
}

/*===== Runtime Methods =====*/

bool ImGuiHandle::NewFrame(UINT& currFrameIndex_)
{
    HRESULT hr;

    ImGui_ImplDX12_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    _barrier.Transition.pResource   = (*_backbuffers)[currFrameIndex_];
    _barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    _barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;
    
    hr = _cmdLists[currFrameIndex_]->Reset(_cmdAllocators[currFrameIndex_], NULL);

    if (FAILED(hr))
    {
        printf("Failing reset cmdList: %s\n", std::system_category().message(hr).c_str());
        return false;
    }

    _cmdLists[currFrameIndex_]->ResourceBarrier(1, &_barrier);
    _cmdLists[currFrameIndex_]->OMSetRenderTargets(1, &_backbufferCPUHandles[currFrameIndex_], FALSE, nullptr);

    return true;
}

bool ImGuiHandle::Render(UINT& currFrameIndex_)
{
    HRESULT hr;

    ImGui::Render();

    _cmdLists[currFrameIndex_]->SetDescriptorHeaps(1, &_imguiHeapDesc);
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), _cmdLists[currFrameIndex_]);

    _barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    _barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PRESENT;
    _cmdLists[currFrameIndex_]->ResourceBarrier(1, &_barrier);

    hr = _cmdLists[currFrameIndex_]->Close();
    if (FAILED(hr))
    {
        printf("Failing close cmd list: %s\n", std::system_category().message(hr).c_str());
        return false;
    }

    return true;
}