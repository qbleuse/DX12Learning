#ifndef __IMGUIHANDLE__
#define __IMGUIHANDLE__

#include <d3d12.h>
#include <imgui.h>
#include <vector>

struct GLFWwindow;
class D3D12Handle;

class ImGuiHandle
{
	public:

		/* Dx12 Objects */
		ID3D12DescriptorHeap*						_imguiHeapDesc;

		std::vector<ID3D12GraphicsCommandList4*>	_cmdLists;
		std::vector<ID3D12CommandAllocator*>		_cmdAllocators;
		D3D12_RESOURCE_BARRIER						_barrier = {};

		const std::vector<ID3D12Resource*>*			_backbuffers;
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>	_backbufferCPUHandles;

		/* imgui object */
		ImGuiIO& _io;


		ImGuiHandle();
		~ImGuiHandle();

		bool Init(GLFWwindow* window, const D3D12Handle& dx12Handle);
		bool NewFrame(UINT& currFrameIndex_);
		bool Render(UINT& currFrameIndex_);
		void Terminate();


	private:

		ImGuiIO& GetIO();
};

#endif /* __IMGUIHANDLE__ */