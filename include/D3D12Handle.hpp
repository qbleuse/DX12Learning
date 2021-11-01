#ifndef __D3D12_HANDLE__
#define __D3D12_HANDLE__

#include <vector>
#include <d3d12.h>
#include <dxgi1_6.h>

struct GLFWwindow;

/* class that handles dx12 init and shut down code and keeps all necessary objects */
struct D3D12Handle
{
	public:
		IDXGIFactory1*								_factory				= nullptr;
		ID3D12Device*								_device					= nullptr;
													
		ID3D12CommandQueue*							_queue					= nullptr;
		IDXGISwapChain4*							_swapchain				= nullptr;
													
		ID3D12DescriptorHeap*						_backBufferDescHeap		= nullptr;
		unsigned int								_backbufferDescOffset	= 0;
		std::vector<ID3D12Resource*>				_backbuffers;
													
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>	_backbufferCPUHandles;
		std::vector<D3D12_GPU_DESCRIPTOR_HANDLE>	_backbufferGPUHandles;
													
		std::vector<ID3D12CommandAllocator*>		_cmdAllocators;
		std::vector<ID3D12GraphicsCommandList4*>	_cmdLists;

		std::vector<ID3D12Fence*>					_fences;
		std::vector<size_t>							_fenceValue;
		HANDLE										_fenceEvent;

		UINT										_currFrameIndex = 0;


		~D3D12Handle();

		bool Init(GLFWwindow* window, unsigned int windowWidth, unsigned int windowHeight, unsigned int bufferCount);

		bool WaitForPrevFrame();

	private:

		bool CreateDevice();
		bool MakeSwapChain(GLFWwindow* window, unsigned int windowWidth, unsigned int windowHeight, unsigned int bufferCount);
		bool CreateBackBuffer(unsigned int bufferCount);
		bool CreateCmdObjects(unsigned int bufferCount);
		bool CreateFenceObjects(unsigned int bufferCount);
};

#endif /* __D3D12_HANDLE__*/