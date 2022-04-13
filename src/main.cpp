/* system */
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
#include "DX12Helper.hpp"
#include "DX12Handle.hpp"

/* Demo */
#include "Demo.hpp"
#include "Demo/DemoTriangle.hpp"
#include "Demo/DemoRayCPUGradiant.hpp"
#include "Demo/DemoRayCPUSphere.hpp"
#include "Demo/DemoQuad.hpp"
#include "Demo/DemoModel.hpp"

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void glfw_WindowSize_callback(GLFWwindow* window, int width, int height)
{
	DX12Handle* handle = (DX12Handle*)glfwGetWindowUserPointer(window);

	if (handle && width > 0 && height > 0)
		handle->ResizeBuffer(width, height);
}

// ImGui/GLFW implementation of camera inputs
CameraInputs getCameraInputs(bool mouseCaptured, float mouseDX, float mouseDY)
{
	CameraInputs cameraInputs = {};
	if (!mouseCaptured)
		return cameraInputs;

	cameraInputs.mouseDX = mouseDX;
	cameraInputs.mouseDY = mouseDY;
	cameraInputs.deltaTime = ImGui::GetIO().DeltaTime;

	const struct CameraKeyMapping { int key; int flag; } cameraKeyMapping[]
	{
		{ GLFW_KEY_W,            CAM_MOVE_FORWARD  },
		{ GLFW_KEY_S,            CAM_MOVE_BACKWARD },
		{ GLFW_KEY_A,            CAM_STRAFE_LEFT   },
		{ GLFW_KEY_D,            CAM_STRAFE_RIGHT  },
		{ GLFW_KEY_LEFT_SHIFT,   CAM_MOVE_FAST     },
		{ GLFW_KEY_SPACE,        CAM_MOVE_UP       },
		{ GLFW_KEY_LEFT_CONTROL, CAM_MOVE_DOWN     },
	};

	cameraInputs.keyInputsFlags = 0;
	for (int i = 0; i < ARRAYSIZE(cameraKeyMapping); ++i)
	{
		CameraKeyMapping map = cameraKeyMapping[i];
		cameraInputs.keyInputsFlags |= ImGui::IsKeyDown(map.key) ? map.flag : 0;
	}

	return cameraInputs;
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
	demos.push_back(std::make_unique<DemoModel>(demoInputs, dx12handle));
	demos.push_back(std::make_unique<DemoRayCPUGradiant>(demoInputs, dx12handle));
	demos.push_back(std::make_unique<DemoRayCPUSphere>(demoInputs, dx12handle));

	/* Loop Var */
	bool		mouseCaptured = false;
	double prevMouseX = 0.0;
	double prevMouseY = 0.0;

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

		// Compute mouse deltas
		float mouseDX;
		float mouseDY;
		{
			double mouseX;
			double mouseY;
			glfwGetCursorPos(window, &mouseX, &mouseY);
			mouseDX = (float)(mouseX - prevMouseX);
			mouseDY = (float)(mouseY - prevMouseY);
			prevMouseX = mouseX;
			prevMouseY = mouseY;
		}

		demoInputs.deltaTime = ImGui::GetIO().DeltaTime;
		demoInputs.windowSize = { ImGui::GetIO().DisplaySize.x,ImGui::GetIO().DisplaySize.y };
		demoInputs.cameraInputs = getCameraInputs(mouseCaptured, mouseDX, mouseDY);

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

	dx12handle.YieldGPU();
	demos.clear();
	imGuiHandle.Terminate();

	glfwDestroyWindow(window);
	glfwTerminate();
}