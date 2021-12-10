/* system include */
#include <system_error>
#include <cstdio>

/* shader include */
#include <d3dcompiler.h>

/* activate shader debug */
#if 1
	#define SHADER_DEBUG
#endif

#include "DX12Handle.hpp"
#include "DX12Helper.hpp"

/*===== SHADER =====*/

bool DX12Helper::CompileVertex(const std::string& shaderSource_, ID3DBlob** VS_, D3D12_SHADER_BYTECODE& shader_)
{
    ID3DBlob* VSErr = nullptr;

    if (FAILED(D3DCompile(shaderSource_.c_str(), shaderSource_.length(), nullptr, nullptr, nullptr, "vert", "vs_5_0", SHADER_FLAG, 0, VS_, &VSErr)))
    {
        printf("Failed To Compile Vertex Shader %s\n", (const char*)(VSErr->GetBufferPointer()));
        VSErr->Release();
        return false;
    }

    shader_.pShaderBytecode = (*VS_)->GetBufferPointer();
    shader_.BytecodeLength = (*VS_)->GetBufferSize();

    return true;
}

bool DX12Helper::CompilePixel(const std::string& shaderSource_, ID3DBlob** PS_, D3D12_SHADER_BYTECODE& shader_)
{
    ID3DBlob* PSErr = nullptr;

    if (FAILED(D3DCompile(shaderSource_.c_str(), shaderSource_.length(), nullptr, nullptr, nullptr, "frag", "ps_5_0", SHADER_FLAG, 0, PS_, &PSErr)))
    {
        printf("Failed To Compile Vertex Shader %s\n", (const char*)(PSErr->GetBufferPointer()));
        PSErr->Release();
        return false;
    }

    shader_.pShaderBytecode = (*PS_)->GetBufferPointer();
    shader_.BytecodeLength = (*PS_)->GetBufferSize();

    return true;
}

/*===== RESOURCES =====*/

DX12Helper::ResourceHelper::~ResourceHelper()
{
	if (uploadBuffer)
		uploadBuffer->Release();
}

DX12Helper::ResourceUploader::~ResourceUploader()
{
	if (copyList)
		copyList->Release();
}

bool DX12Helper::MakeUploader(ResourceUploader& uploader_, const DX12Handle& dx12Handle_)
{
	HRESULT hr;

	hr = dx12Handle_._device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, dx12Handle_._cmdAllocators[dx12Handle_._context.currFrameIndex], NULL, IID_PPV_ARGS(&uploader_.copyList));
	if (FAILED(hr))
	{
		printf("Failing creating DX12 upload command List: %s\n", std::system_category().message(hr).c_str());
		return false;
	}

	uploader_.device		= dx12Handle_._device;
	uploader_.queue			= dx12Handle_._queue;
	uploader_.fenceEvent	= &dx12Handle_._fenceEvent;

	return true;
}

bool DX12Helper::CreateBuffer(D3D12_SUBRESOURCE_DATA* bufferData_, ResourceHelper& resourceData_, const ResourceUploader& uploader_)
{
	HRESULT hr;

	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;

	D3D12_RESOURCE_DESC resDesc = {};

	resDesc.Dimension			= D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.SampleDesc.Count	= 1;
	resDesc.Width				= bufferData_->SlicePitch;
	resDesc.Height				= 1;
	resDesc.DepthOrArraySize	= 1;
	resDesc.MipLevels			= 1;
	resDesc.Layout				= D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	hr = uploader_.device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&resourceData_.uploadBuffer));

	if (FAILED(hr))
	{
		printf("Failing creating vertex upload heap: %s\n", std::system_category().message(hr).c_str());
		return false;
	}

	heapProp.Type = D3D12_HEAP_TYPE_DEFAULT;

	hr = uploader_.device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(resourceData_.buffer));

	if (FAILED(hr))
	{
		printf("Failing creating vertex resource : %s\n", std::system_category().message(hr).c_str());
		return false;
	}

	UpdateSubresources(uploader_.copyList, *resourceData_.buffer, resourceData_.uploadBuffer, 0, 0, 1, bufferData_);

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type					= D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource	= *resourceData_.buffer;
	barrier.Transition.StateBefore	= D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter	= D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	barrier.Transition.Subresource	= D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	uploader_.copyList->ResourceBarrier(1, &barrier);

    return true;
}

bool DX12Helper::UploadResources(const ResourceUploader& uploader_)
{
	HRESULT hr;

	uploader_.copyList->Close();

	ID3D12CommandList* ppCommandLists[] = { uploader_.copyList };
	uploader_.queue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	// Create synchronization objects and wait until assets have been uploaded to the GPU.
	{
		ID3D12Fence* uploadFence;
		hr = uploader_.device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&uploadFence));
		if (FAILED(hr))
		{
			printf("Failing creating DX12 Fence: %s\n", std::system_category().message(hr).c_str());
			return false;
		}

		// Wait for the command list to execute; we are reusing the same command 
		// list in our main loop but for now, we just want to wait for setup to 
		// complete before continuing.

		// Signal and increment the fence value.
		uploader_.queue->Signal(uploadFence, 1);
		if (FAILED(hr))
		{
			printf("Failing signal: %s\n", std::system_category().message(hr).c_str());
			return false;
		}


		uploadFence->SetEventOnCompletion(1, *uploader_.fenceEvent);
		WaitForSingleObject(*uploader_.fenceEvent, INFINITE);
		uploadFence->Release();

		return true;
	}
}