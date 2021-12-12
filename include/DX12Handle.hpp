#ifndef __D3D12_HANDLE__
#define __D3D12_HANDLE__

#include <vector>
#include <d3dx12.h>
#include <dxgi1_6.h>

struct GLFWwindow;


struct DX12Contextual
{
	ID3D12GraphicsCommandList4* currCmdList				= nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE currBackBufferHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE depthBufferHandle;
	UINT						currFrameIndex			= 0;
	FLOAT						width					= 0;
	FLOAT						height					= 0;
};

/* class that handles dx12 init and shut down code and keeps all necessary objects */
class DX12Handle
{
	public:
		IDXGIFactory1*								_factory				= nullptr;
		ID3D12Device*								_device					= nullptr;
													
		ID3D12CommandQueue*							_queue					= nullptr;
		IDXGISwapChain4*							_swapchain				= nullptr;
		DXGI_SAMPLE_DESC							_sampleDesc				= {};
													
		ID3D12DescriptorHeap*						_backBufferDescHeap		= nullptr;
		unsigned int								_backbufferDescOffset	= 0;
		std::vector<ID3D12Resource*>				_backbuffers;

		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>	_backbufferCPUHandles;
		std::vector<D3D12_GPU_DESCRIPTOR_HANDLE>	_backbufferGPUHandles;

		ID3D12DescriptorHeap*						_depthBufferDescHeap	= nullptr;
		ID3D12Resource*								_depthBuffer			= nullptr;
		D3D12_CPU_DESCRIPTOR_HANDLE					_depthBufferCPUHandle;

													
		std::vector<ID3D12CommandAllocator*>		_cmdAllocators;
		std::vector<ID3D12GraphicsCommandList4*>	_cmdLists;

		std::vector<ID3D12Fence*>					_fences;
		std::vector<size_t>							_fenceValue;
		HANDLE										_fenceEvent;
		D3D12_RESOURCE_BARRIER						_barrier				= {};


		DX12Contextual _context;

		~DX12Handle();

		bool Init(GLFWwindow* window, unsigned int windowWidth, unsigned int windowHeight, unsigned int bufferCount);

		bool WaitForPrevFrame();
		bool StartDrawing();
		bool EndDrawing();
		bool Render();
		bool ResizeBuffer(unsigned int windowWidth, unsigned int windowHeight);

	private:
		UINT _currFrameIndex = 0;

		bool CreateDevice();
		bool MakeSwapChain(GLFWwindow* window, unsigned int windowWidth, unsigned int windowHeight, unsigned int bufferCount);
		bool CreateBackBuffer(unsigned int bufferCount);
		bool MakeBackBuffer();
		bool MakeDepthBuffer(unsigned int windowWidth, unsigned int windowHeight);
		bool CreateCmdObjects(unsigned int bufferCount);
		bool CreateFenceObjects(unsigned int bufferCount);
};

#endif /* __D3D12_HANDLE__*/