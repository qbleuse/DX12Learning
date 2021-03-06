#ifndef __IMGUIHANDLE__
#define __IMGUIHANDLE__

#include <d3d12.h>
#include <imgui.h>
#include <vector>
#include <memory>

struct GLFWwindow;
class DX12Handle;
struct DX12Contextual;
class Demo;

class ImGuiHandle
{
	public:

		/* Dx12 Objects */
		ID3D12DescriptorHeap*						_imguiHeapDesc;

		/* imgui object */
		ImGuiIO& _io;


		ImGuiHandle();
		~ImGuiHandle();

		bool Init(GLFWwindow* window, const DX12Handle& dx12Handle);
		void NewFrame();
		void ChooseDemo(const std::vector<std::unique_ptr<Demo>>& demos_, int& demoId_);
		void Render(DX12Contextual& currFrameIndex_);
		void Terminate();


	private:
		/* for showing demo window */
		bool	_demo_window = false;

		/* for constructor */
		ImGuiIO& GetIO();
};

#endif /* __IMGUIHANDLE__ */