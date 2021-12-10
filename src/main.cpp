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
#include "DX12Handle.hpp"

/* Demo */
#include "Demo.hpp"
#include "Demo/DemoTriangle.hpp"
#include "Demo/DemoRayCPU.hpp"
#include "Demo/DemoQuad.hpp"

#define WINDOW_WIDTH 1800
#define WINDOW_HEIGHT 900
#define FRAME_BUFFER_COUNT 3

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void glfw_WindowSize_callback(GLFWwindow* window, int width, int height)
{
	DX12Handle* handle = (DX12Handle*)glfwGetWindowUserPointer(window);

	if (handle)
		handle->ResizeBuffer(width, height);
}

int main()
{
	/*===== Setup GLFW window =====*/
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return 1;

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "DX12Example", NULL, NULL);
	glfwSetWindowSizeCallback(window, glfw_WindowSize_callback);

	/*===== Setup Dx12 =====*/
	DX12Handle dx12handle;

	if (!dx12handle.Init(window, WINDOW_WIDTH, WINDOW_HEIGHT, FRAME_BUFFER_COUNT))
		return 1;

	glfwSetWindowUserPointer(window, &dx12handle);

	/*==== Setup ImGui =====*/
	ImGuiHandle imGuiHandle;
	if (!imGuiHandle.Init(window, dx12handle))
		return 1;
	
	/* Demo */
	DemoInputs	demoInputs(dx12handle._context);
	int demoId = 0;
	std::vector<std::unique_ptr<Demo>> demos;
	demos.push_back(std::make_unique<DemoTriangle>(demoInputs,dx12handle));
	demos.push_back(std::make_unique<DemoQuad>(demoInputs, dx12handle));
	demos.push_back(std::make_unique<DemoRayCPU>(demoInputs));

	/* Loop Var */
	bool		mouseCaptured = false;

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		imGuiHandle.NewFrame();

		/* init dx12 and Imgui For Drawing */
		if (!dx12handle.WaitForPrevFrame() 
			|| !dx12handle.StartDrawing())
			break;


		imGuiHandle.ChooseDemo(demos, demoId);

		{
			if (ImGui::IsKeyPressed(GLFW_KEY_ESCAPE) && mouseCaptured)
			{
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				mouseCaptured = false;
			}

			if (!ImGui::GetIO().WantCaptureMouse && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
			{
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				mouseCaptured = true;
			}
		}

		demos[demoId]->UpdateAndRender(demoInputs);

		imGuiHandle.Render(dx12handle._context);

		if (!dx12handle.EndDrawing() )
			break;

		ID3D12CommandList* cmdLists[1] = { dx12handle._context.currCmdList};

		dx12handle._queue->ExecuteCommandLists(1,cmdLists);

		if (imGuiHandle._io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault(NULL, dx12handle._context.currCmdList);
		}

		if (!dx12handle.Render())
			break;
		
	}


	demos.clear();
	imGuiHandle.Terminate();

	glfwDestroyWindow(window);
	glfwTerminate();
}