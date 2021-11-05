#include <cstdio>
#include <system_error>

/* GLFW */
#include <GLFW/glfw3.h>

/* ImGui */
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_dx12.h>
#include "ImGuiHandle.hpp"

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
    D3D12Handle dx12handle;

    if (!dx12handle.Init(window, WINDOW_WIDTH, WINDOW_HEIGHT, FRAME_BUFFER_COUNT))
        return 1;


    /*==== Setup ImGui =====*/
    ImGuiHandle imGuiHandle;
    if (!imGuiHandle.Init(window, dx12handle))
        return 1;
    

    /* Loop Var */
    UINT    currFrameIndex = 0;
    bool    demo_window = true;

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        /* init dx12 and Imgui For Drawing */
        if (!dx12handle.WaitForPrevFrame(currFrameIndex) 
            || !dx12handle.StartDrawing(currFrameIndex) 
            || !imGuiHandle.NewFrame(currFrameIndex))
            break;

        if (demo_window)
            ImGui::ShowDemoWindow(&demo_window);

        // Clear the render target by using the ClearRenderTargetView command
        const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
        dx12handle._cmdLists[currFrameIndex]->ClearRenderTargetView(dx12handle._backbufferCPUHandles[currFrameIndex], clearColor, 0, nullptr);

        if (!dx12handle.EndDrawing(currFrameIndex) || !imGuiHandle.Render(currFrameIndex))
            break;

        ID3D12CommandList* cmdLists[2] = { dx12handle._cmdLists[currFrameIndex] , imGuiHandle._cmdLists[currFrameIndex] };

        dx12handle._queue->ExecuteCommandLists(2,cmdLists);

        if (imGuiHandle._io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault(NULL, (void*)imGuiHandle._cmdLists[currFrameIndex]);
        }

        hr = dx12handle._queue->Signal(dx12handle._fences[currFrameIndex], dx12handle._fenceValue[currFrameIndex]);
        if (FAILED(hr))
        {
            printf("Failing signal: %s\n", std::system_category().message(hr).c_str());
            break;
        }

        dx12handle._swapchain->Present(0,0);
    }

    imGuiHandle._imguiHeapDesc->Release();

    ImGui_ImplDX12_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}