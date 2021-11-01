#include <cstdio>
#include <system_error>

/* GLFW */
#include <GLFW/glfw3.h>

/* ImGui */
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_dx12.h>

/* Dx12 */
#include "D3D12Handle.hpp"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define FRAME_BUFFER_COUNT 3

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int main()
{
    HRESULT hr;

    /*===== Setup GLFW window =====*/
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "DX12Example", NULL, NULL);

    /*===== Setup Dx12 =====*/
    D3D12Handle handle;

    if (!handle.Init(window, WINDOW_WIDTH, WINDOW_HEIGHT, FRAME_BUFFER_COUNT))
        return 1;


    /*==== Setup ImGui =====*/
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    /* describe an rtv descriptor heapand create */
    D3D12_DESCRIPTOR_HEAP_DESC imguiDesc = {};
    imguiDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    imguiDesc.NumDescriptors = 1;
    imguiDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    /* create imgui heap desc */
    ID3D12DescriptorHeap* imguiHeapDesc;
    hr = handle._device->CreateDescriptorHeap(&imguiDesc, IID_PPV_ARGS(&imguiHeapDesc));
    if (FAILED(hr))
    {
        printf("Failing creating DX12 imgui descriptor heap: %s\n", std::system_category().message(hr).c_str());
        return 1;
    }

    ImGui_ImplGlfw_InitForOther(window, true);
    ImGui_ImplDX12_Init(handle._device,FRAME_BUFFER_COUNT,DXGI_FORMAT_R8G8B8A8_UNORM, imguiHeapDesc, imguiHeapDesc->GetCPUDescriptorHandleForHeapStart(),imguiHeapDesc->GetGPUDescriptorHandleForHeapStart());

    bool demo_window = true;

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplDX12_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (!handle.WaitForPrevFrame())
            break;

        hr = handle._cmdAllocators[handle._currFrameIndex]->Reset();
        if (FAILED(hr))
        {
            printf("Failing reset cmd alloc: %s\n", std::system_category().message(hr).c_str());
            break;
        }

        if (demo_window)
            ImGui::ShowDemoWindow(&demo_window);

        ImGui::Render();

        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = handle._backbuffers[handle._currFrameIndex];
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
        hr = handle._cmdLists[handle._currFrameIndex]->Reset(handle._cmdAllocators[handle._currFrameIndex], NULL);
        if (FAILED(hr))
        {
            printf("Failing reset cmdList: %s\n", std::system_category().message(hr).c_str());
            break;
        }



        handle._cmdLists[handle._currFrameIndex]->ResourceBarrier(1, &barrier);

        // set the render target for the output merger stage (the output of the pipeline)
        handle._cmdLists[handle._currFrameIndex]->OMSetRenderTargets(1, &handle._backbufferCPUHandles[handle._currFrameIndex], FALSE, nullptr);
        
        // Clear the render target by using the ClearRenderTargetView command
        const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
        handle._cmdLists[handle._currFrameIndex]->ClearRenderTargetView(handle._backbufferCPUHandles[handle._currFrameIndex], clearColor, 0, nullptr);
        handle._cmdLists[handle._currFrameIndex]->SetDescriptorHeaps(1, &imguiHeapDesc);
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), handle._cmdLists[handle._currFrameIndex]);
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
        handle._cmdLists[handle._currFrameIndex]->ResourceBarrier(1, &barrier);

        hr = handle._cmdLists[handle._currFrameIndex]->Close();
        if (FAILED(hr))
        {
            printf("Failing close cmd list: %s\n", std::system_category().message(hr).c_str());
            break;
        }

        handle._queue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&handle._cmdLists[handle._currFrameIndex]);

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault(NULL, (void*)handle._cmdLists[handle._currFrameIndex]);
        }

        hr = handle._queue->Signal(handle._fences[handle._currFrameIndex], handle._fenceValue[handle._currFrameIndex]);
        if (FAILED(hr))
        {
            printf("Failing signal: %s\n", std::system_category().message(hr).c_str());
            break;
        }

        handle._swapchain->Present(0,0);
    }

    handle.WaitForPrevFrame();
    imguiHeapDesc->Release();

    ImGui_ImplDX12_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}