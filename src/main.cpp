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

/* Demo */
#include "Demo.hpp"
#include "DemoRayCPU.hpp"

#define WINDOW_WIDTH 1800
#define WINDOW_HEIGHT 900
#define FRAME_BUFFER_COUNT 3

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void glfw_WindowSize_callback(GLFWwindow* window, int width, int height)
{
	D3D12Handle* handle = (D3D12Handle*)glfwGetWindowUserPointer(window);

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
	D3D12Handle dx12handle;

	if (!dx12handle.Init(window, WINDOW_WIDTH, WINDOW_HEIGHT, FRAME_BUFFER_COUNT))
		return 1;

	glfwSetWindowUserPointer(window, &dx12handle);

	/*==== Setup ImGui =====*/
	ImGuiHandle imGuiHandle;
	if (!imGuiHandle.Init(window, dx12handle))
		return 1;
	
	/* Demo */
	DemoInputs	demoInputs;
	demoInputs.renderContext = &dx12handle._context;
	int demoId = 0;
	std::vector<Demo*> demos;
	demos.push_back(new DemoRayCPU(demoInputs));

	/* Loop Var */
	UINT		currFrameIndex = 0;
	bool		demo_window = true;
	bool		mouseCaptured = false;

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		/* init dx12 and Imgui For Drawing */
		if (!dx12handle.WaitForPrevFrame(currFrameIndex) 
			|| !dx12handle.StartDrawing(currFrameIndex) 
			|| !imGuiHandle.NewFrame(currFrameIndex))
			break;


		
		{
			if (ImGui::Button("<"))
				demoId = (demoId - 1)%(int)demos.size();
			ImGui::SameLine();
			ImGui::Text("%d/%d", demoId + 1, (int)demos.size());
			ImGui::SameLine();
			if (ImGui::Button(">"))
				demoId = (demoId + 1)%(int)demos.size();
			ImGui::SameLine();
			if (demos.size()>0)
				ImGui::Text("[%s]", demos[demoId]->Name());
		}

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

		ImGui::Checkbox("ImGui demo window", &demo_window);
		if (demo_window)
			ImGui::ShowDemoWindow(&demo_window);

		demos[demoId]->UpdateAndRender(demoInputs);

		if (!dx12handle.EndDrawing(currFrameIndex) || !imGuiHandle.Render(currFrameIndex))
			break;

		ID3D12CommandList* cmdLists[2] = { dx12handle._cmdLists[currFrameIndex] , imGuiHandle._cmdLists[currFrameIndex] };

		dx12handle._queue->ExecuteCommandLists(2,cmdLists);

		if (imGuiHandle._io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault(NULL, (void*)imGuiHandle._cmdLists[currFrameIndex]);
		}

		if (!dx12handle.Render(currFrameIndex))
			break;
		
	}

	imGuiHandle.Terminate();

	glfwDestroyWindow(window);
	glfwTerminate();
}