/* system include */
#include <system_error>
#include <cstdio>

/* ImGui */
#include <imgui_impl_glfw.h>
#include <imgui_impl_dx12.h>

/* GLFW */
#include <GLFW/glfw3.h>

/* Demos */
#include "Demo.hpp"

#include "DX12Handle.hpp"
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

bool ImGuiHandle::Init(GLFWwindow* window, const DX12Handle& dx12Handle)
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


    ImGui_ImplGlfw_InitForOther(window, true);
    ImGui_ImplDX12_Init(dx12Handle._device, bufferCount, DXGI_FORMAT_R8G8B8A8_UNORM, _imguiHeapDesc, _imguiHeapDesc->GetCPUDescriptorHandleForHeapStart(), _imguiHeapDesc->GetGPUDescriptorHandleForHeapStart());

    return true;
}

/*===== Runtime Methods =====*/

void ImGuiHandle::NewFrame()
{
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiHandle::ChooseDemo(const std::vector<std::unique_ptr<Demo>>& demos_, int& demoId_)
{
    {
        if (ImGui::Button("<"))
            demoId_ = demoId_ > 0 ? (demoId_ - 1) % (int)demos_.size() : (int)demos_.size() - 1;
        ImGui::SameLine();
        ImGui::Text("%d/%d", demoId_ + 1, (int)demos_.size());
        ImGui::SameLine();
        if (ImGui::Button(">"))
            demoId_ = (demoId_ + 1) % (int)demos_.size();
        ImGui::SameLine();
        if (demos_.size() > 0)
            ImGui::Text("[%s]", demos_[demoId_]->Name());

        ImGui::Checkbox("ImGui demo window", &_demo_window);
        if (_demo_window)
            ImGui::ShowDemoWindow(&_demo_window);
    }
}

void ImGuiHandle::Render(DX12Contextual& contextual_)
{
    ImGui::Render();

    contextual_.currCmdList->SetDescriptorHeaps(1, &_imguiHeapDesc);
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), contextual_.currCmdList);
}