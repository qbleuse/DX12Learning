/* system include */
#include <system_error>
#include <cstdio>

/* glfw include */
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "DX12Handle.hpp"


/*==== CONSTRUCTORS =====*/
DX12Handle::~DX12Handle()
{

	if (_factory)
		_factory->Release();
	if (_device)
		_device->Release();
	if (_queue)
		_queue->Release();
	if (_swapchain)
		_swapchain->Release();
	if (_backBufferDescHeap)
		_backBufferDescHeap->Release();

	for (int i = 0; i < _backbuffers.size(); i++)
	{
		_backbuffers[i]->Release();
		_cmdAllocators[i]->Release();
		_cmdLists[i]->Release();
		_fences[i]->Release();
	}
}

/*===== INIT Methods =====*/

bool DX12Handle::Init(GLFWwindow* window, unsigned int windowWidth, unsigned int windowHeight, unsigned int bufferCount)
{
	
	return CreateDevice() 
		&& MakeSwapChain(window, windowWidth, windowHeight, bufferCount) 
		&& CreateBackBuffer(bufferCount) 
		&& CreateCmdObjects(bufferCount)
		&& CreateFenceObjects(bufferCount);
}

bool DX12Handle::CreateDevice()
{
	HRESULT hr;

	hr = CreateDXGIFactory1(IID_PPV_ARGS(&_factory));
	if (FAILED(hr))
	{
		printf("Failing creating factory: %s\n", std::system_category().message(hr).c_str());
		return false;
	}

	/*=== Create the device ===*/
	hr = D3D12CreateDevice(
		nullptr,
		D3D_FEATURE_LEVEL_12_0,
		IID_PPV_ARGS(&_device)
	);

	if (FAILED(hr))
	{
		printf("Failing creating DX12 device: %s\n", std::system_category().message(hr).c_str());
		return false;
	}

	return true;
}

bool DX12Handle::MakeSwapChain(GLFWwindow* window, unsigned int windowWidth, unsigned int windowHeight, unsigned int bufferCount)
{
	HRESULT hr;

	/*=== Create the Command Queue ===*/

	/* automatically fills up with default values, and we use those */
	D3D12_COMMAND_QUEUE_DESC cqDesc = {};

	hr = _device->CreateCommandQueue(&cqDesc, IID_PPV_ARGS(&_queue));

	if (FAILED(hr))
	{
		printf("Failing creating DX12 command queue: %s\n", std::system_category().message(hr).c_str());
		return false;
	}

	/*===== Create the Swap Chain =====*/

	/* describe the back buffer(s) */
	DXGI_MODE_DESC backBufferDesc = {};
	backBufferDesc.Width = windowWidth;
	backBufferDesc.Height = windowHeight;
	backBufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	/* describe the multi sample, we don't use it but unfortunately,
	we are forced to set up to default */
	DXGI_SAMPLE_DESC sampleDesc = {};
	sampleDesc.Count = 1;

	/* Describe the swap chain. */
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = bufferCount;
	swapChainDesc.BufferDesc = backBufferDesc;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // dxgi will discard the buffer (data) after we call present
	swapChainDesc.OutputWindow = glfwGetWin32Window(window); // handle to our window
	swapChainDesc.SampleDesc = sampleDesc; // our multi-sampling description
	swapChainDesc.Windowed = true; // we can change to full screen later


	hr = _factory->CreateSwapChain(
		_queue, // the queue will be flushed once the swap chain is created
		&swapChainDesc, // give it the swap chain description we created above
		(IDXGISwapChain**)&_swapchain // store the created swap chain in a temp IDXGISwapChain interface
	);


	if (FAILED(hr))
	{
		printf("Failing creating DX12 swap chain: %s\n", std::system_category().message(hr).c_str());
		return false;
	}


	return true;
}

bool DX12Handle::CreateBackBuffer(unsigned int bufferCount)
{
	HRESULT hr;

	/*===== Create the Descriptor Heap =====*/

   /* describe an rtv descriptor heapand create */
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc	= {};
	rtvHeapDesc.NumDescriptors				= bufferCount;
	rtvHeapDesc.Type						= D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // it is for Render Target View

	hr = _device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&_backBufferDescHeap));
	if (FAILED(hr))
	{
		printf("Failing creating DX12 swap chain descriptor heap: %s\n", std::system_category().message(hr).c_str());
		return false;
	}

	/* get the size of a descriptor in this heap descriptor sizes may vary from device to device,
	 * which is why there is no set size and we must ask the  device to give us the size.
	 * we will use this size to increment a descriptor handle offset */
	_backbufferDescOffset = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	/* get a handle to the first descriptor in the descriptor heap to create render target from swap chain buffer */
	D3D12_CPU_DESCRIPTOR_HANDLE backBufferCPUHandle(_backBufferDescHeap->GetCPUDescriptorHandleForHeapStart());
	D3D12_GPU_DESCRIPTOR_HANDLE backBufferGPUHandle(_backBufferDescHeap->GetGPUDescriptorHandleForHeapStart());


	/*===== Create the Back Buffers Render Targets =====*/

	_backbuffers.resize(bufferCount);
	_backbufferCPUHandles.resize(bufferCount);
	_backbufferGPUHandles.resize(bufferCount);

	/* Create a render target view for each back buffer */
	for (int i = 0; i < bufferCount; i++)
	{
		_backbufferCPUHandles[i] = backBufferCPUHandle;
		_backbufferGPUHandles[i] = backBufferGPUHandle;
		backBufferCPUHandle.ptr += _backbufferDescOffset;
		backBufferGPUHandle.ptr += _backbufferDescOffset;
	}

	MakeBackBuffer();

	return true;
}

bool DX12Handle::CreateCmdObjects(unsigned int bufferCount)
{
	HRESULT hr;

	/*===== Create the Command Allocators =====*/
	_cmdAllocators.resize(bufferCount);

	for (int i = 0; i < bufferCount; i++)
	{
		hr = _device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_cmdAllocators[i]));
		if (FAILED(hr))
		{
			printf("Failing creating DX12 command allocator nb %d: %s\n", i, std::system_category().message(hr).c_str());
			return false;
		}
	}

	/*===== Create the Command Lists =====*/
	_cmdLists.resize(bufferCount);

	for (int i = 0; i < bufferCount; i++)
	{
		hr = _device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdAllocators[i], NULL, IID_PPV_ARGS(&_cmdLists[i]));
		if (FAILED(hr))
		{
			printf("Failing creating DX12 command List nb %d: %s\n", i, std::system_category().message(hr).c_str());
			return false;
		}

		_cmdLists[i]->Close();
	}

	_barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	_barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	_barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;


	return true;
}

bool DX12Handle::CreateFenceObjects(unsigned int bufferCount)
{
	HRESULT hr;

	/* Create the Fences */
	_fences.resize(bufferCount);
	_fenceValue.resize(bufferCount);

	for (int i = 0; i < bufferCount; i++)
	{
		hr = _device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fences[i]));
		if (FAILED(hr))
		{
			printf("Failing creating DX12 Fence nb %d: %s\n", i, std::system_category().message(hr).c_str());
			return false;
		}

		_fenceValue[i] = 0; // set the initial fence value to 0
	}

	/* create a handle to a fence event */
	_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (_fenceEvent == nullptr)
	{
		printf("Failing creating DX12 Fence Event\n");
		return false;
	}

	return true;
}

/*===== Setup Methods =====*/

bool DX12Handle::ResizeBuffer(unsigned int windowWidth, unsigned int windowHeight)
{
	HRESULT hr;

	int bufferCount = _backbuffers.size();

	for (int i = 0; i < bufferCount; i++)
	{
		uint64_t fenceValueForSignal = ++_fenceValue[i];
		_queue->Signal(_fences[i], fenceValueForSignal);
		if (_fences[i]->GetCompletedValue() < _fenceValue[i])
		{
			_fences[i]->SetEventOnCompletion(fenceValueForSignal, _fenceEvent);
			WaitForSingleObject(_fenceEvent, INFINITE);
		}
	}

	for (int i = 0; i < bufferCount; i++)
	{
		if (_backbuffers[i])
			_backbuffers[i]->Release();
	}

	hr = _swapchain->ResizeBuffers(bufferCount, windowWidth, windowHeight, DXGI_FORMAT_R8G8B8A8_UNORM, NULL);
	if (FAILED(hr))
	{
		printf("Failing resizing DX12 swap chain buffer: %s\n", std::system_category().message(hr).c_str());
		return false;
	}

	return MakeBackBuffer();
}


bool DX12Handle::MakeBackBuffer()
{
	D3D12_CPU_DESCRIPTOR_HANDLE backBufferCPUHandle(_backBufferDescHeap->GetCPUDescriptorHandleForHeapStart());
	HRESULT hr;

	int bufferCount = _backbuffers.size();

	/* Create a render target view for each back buffer */
	for (int i = 0; i < bufferCount; i++)
	{
		/* first we get the n'th buffer in the swap chain and store it in the n'th
		 * position of our ID3D12Resource array */
		hr = _swapchain->GetBuffer(i, IID_PPV_ARGS(&_backbuffers[i]));
		if (FAILED(hr))
		{
			printf("Failing getting DX12 swap chain buffer %d: %s\n", i, std::system_category().message(hr).c_str());
			return false;
		}

		/* the we "create" a render target view which binds the swap chain buffer(ID3D12Resource[n]) to the rtv handle */
		_device->CreateRenderTargetView(_backbuffers[i], nullptr, backBufferCPUHandle);

		backBufferCPUHandle.ptr += _backbufferDescOffset;
	}

	return true;
}


/*===== Runtime Methods =====*/
bool DX12Handle::WaitForPrevFrame()
{
	HRESULT hr;

	/* swap between back buffer index so we draw on the correct buffer */
	_currFrameIndex = _swapchain->GetCurrentBackBufferIndex();

	/* if the current fence value is still less than "fenceValue", then we know the GPU has not finished executing
	 * the command queue since it has not reached the "commandQueue->Signal(fence, fenceValue)" command */
	if (_fences[_currFrameIndex]->GetCompletedValue() < _fenceValue[_currFrameIndex])
	{
		/* set an event that triggers when fence Values are the same */
		hr = _fences[_currFrameIndex]->SetEventOnCompletion(_fenceValue[_currFrameIndex], _fenceEvent);
		if (FAILED(hr))
		{
			printf("Failing creating DX12 Fence Event Complete for frame nb %u: %s\n", _currFrameIndex, std::system_category().message(hr).c_str());
			return false;
		}

		/* waiting for the command queue to finish executing (executing the event created above) */
		WaitForSingleObject(_fenceEvent, INFINITE);
	}

	/* increment fenceValue for next frame */
	_fenceValue[_currFrameIndex]++;

	return true;
}

bool DX12Handle::StartDrawing()
{
	HRESULT hr;

	hr = _cmdAllocators[_currFrameIndex]->Reset();
	if (FAILED(hr))
	{
		printf("Failing reset cmd alloc: %s\n", std::system_category().message(hr).c_str());
		return false;
	}


	_barrier.Transition.pResource	= _backbuffers[_currFrameIndex];
	_barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	_barrier.Transition.StateAfter	= D3D12_RESOURCE_STATE_RENDER_TARGET;
	hr = _cmdLists[_currFrameIndex]->Reset(_cmdAllocators[_currFrameIndex], NULL);

	if (FAILED(hr))
	{
		printf("Failing reset cmdList: %s\n", std::system_category().message(hr).c_str());
		return false;
	}

	_cmdLists[_currFrameIndex]->ResourceBarrier(1, &_barrier);

	_cmdLists[_currFrameIndex]->OMSetRenderTargets(1, &_backbufferCPUHandles[_currFrameIndex], FALSE, nullptr);

	_context.currCmdList			= _cmdLists[_currFrameIndex];
	_context.currBackBufferHandle	= _backbufferCPUHandles[_currFrameIndex];

	return true;
}

bool DX12Handle::EndDrawing()
{
	HRESULT hr;

	_barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	_barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	_cmdLists[_currFrameIndex]->ResourceBarrier(1, &_barrier);

	hr = _cmdLists[_currFrameIndex]->Close();
	if (FAILED(hr))
	{
		printf("Failing close cmd list: %s\n", std::system_category().message(hr).c_str());
		return false;
	}

	return true;
}

bool DX12Handle::Render()
{
	HRESULT hr;

	hr = _queue->Signal(_fences[_currFrameIndex], _fenceValue[_currFrameIndex]);
	if (FAILED(hr))
	{
		printf("Failing signal: %s\n", std::system_category().message(hr).c_str());
		return false;
	}

	_swapchain->Present(0, 0);

	return true;
}