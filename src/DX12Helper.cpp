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

/* texture loading */
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

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

DX12Helper::DefaultResource::~DefaultResource()
{
	if (uploadBuffer)
		uploadBuffer->Release();
}

DX12Helper::DefaultResourceUploader::~DefaultResourceUploader()
{
	if (copyList)
		copyList->Release();
}

bool DX12Helper::MakeUploader(DefaultResourceUploader& uploader_, const DX12Handle& dx12Handle_)
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

bool DX12Helper::CreateDefaultBuffer(D3D12_SUBRESOURCE_DATA* bufferData_, DefaultResource& resourceData_, const DefaultResourceUploader& uploader_)
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
		printf("Failing creating default buffer upload heap: %s\n", std::system_category().message(hr).c_str());
		return false;
	}

	heapProp.Type = D3D12_HEAP_TYPE_DEFAULT;

	hr = uploader_.device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(resourceData_.buffer));

	if (FAILED(hr))
	{
		printf("Failing creating default buffer resource : %s\n", std::system_category().message(hr).c_str());
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

bool DX12Helper::UploadResources(const DefaultResourceUploader& uploader_)
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

DX12Helper::ConstantResource::~ConstantResource()
{
	if (buffer)
		buffer->Release();
}

bool DX12Helper::CreateCBufferHeap(UINT cbvNb, ConstantResourceUploader& uploader_)
{
	HRESULT hr;

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = cbvNb;
	heapDesc.Type			= D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	// This flag indicates that this descriptor heap can be bound to the pipeline and that descriptors contained in it can be referenced by a root table.
	heapDesc.Flags			= D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	hr = uploader_.device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(uploader_.descHeap));

	if (FAILED(hr))
	{
		printf("Failing creating constant buffer descriptor heap: %s\n", std::system_category().message(hr).c_str());
		return false;
	}

	return true;
}

bool DX12Helper::CreateCBuffer(const UINT bufferSize, ConstantResource& resourceData_, ConstantResourceUploader& uploader_)
{
	HRESULT hr;

	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;

	D3D12_RESOURCE_DESC resDesc = {};

	resDesc.Dimension			= D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.SampleDesc.Count	= 1;
	resDesc.Width				= (bufferSize + (D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1)) & ~(D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1);
	resDesc.Height				= 1;
	resDesc.DepthOrArraySize	= 1;
	resDesc.MipLevels			= 1;
	resDesc.Layout				= D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	hr = uploader_.device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&resourceData_.buffer));

	if (FAILED(hr))
	{
		printf("Failing creating constant buffer upload heap: %s\n", std::system_category().message(hr).c_str());
		return false;
	}

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.SizeInBytes = resDesc.Width;

	resourceData_.bufferCPUHandle = (*uploader_.descHeap)->GetCPUDescriptorHandleForHeapStart();
	resourceData_.bufferCPUHandle.ptr += uploader_.descSize * uploader_.cbNb;

	resourceData_.bufferGPUHandle = (*uploader_.descHeap)->GetGPUDescriptorHandleForHeapStart();
	resourceData_.bufferGPUHandle.ptr += uploader_.descSize * uploader_.cbNb;

	uploader_.cbNb++;

	// Describe and create the shadow constant buffer view (CBV) and 
	// cache the GPU descriptor handle.
	cbvDesc.BufferLocation = resourceData_.buffer->GetGPUVirtualAddress();
	uploader_.device->CreateConstantBufferView(&cbvDesc, resourceData_.bufferCPUHandle);

	CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
	hr = resourceData_.buffer->Map(0, &readRange, &resourceData_.mapHandle);
	if (FAILED(hr))
	{
		printf("Failing mapping constant buffer: %s\n", std::system_category().message(hr).c_str());
		return false;
	}

	return true;
}

void DX12Helper::UploadCBuffer(void* bufferData, UINT bufferSize, ConstantResource& resourceData_)
{
	memcpy(resourceData_.mapHandle, bufferData, bufferSize);
}

/*===== TEXTURE =====*/

bool LoadTexture(const std::string& filePath_, BYTE** texdata_, D3D12_RESOURCE_DESC& texDesc_)
{
	int width = 0;
	int height = 0;
	int channels = 0;

	stbi_set_flip_vertically_on_load(1);

	(*texData_) = stbi_load(filePath_.c_str(), &width, &height, &channels, 0);


	int byteSize = width * height * channels * sizeof(BYTE);

	texDesc_ = {};
	texDesc_.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texDesc_.Alignment = 0; // may be 0, 4KB, 64KB, or 4MB. 0 will let runtime decide between 64KB and 4MB (4MB for multi-sampled textures)
	texDesc_.Width = width; // width of the texture
	texDesc_.Height = height; // height of the texture
	texDesc_.DepthOrArraySize = 1; // if 3d image, depth of 3d image. Otherwise an array of 1D or 2D textures (we only have one image, so we set 1)
	texDesc_.MipLevels = 1; // Number of mipmaps. We are not generating mipmaps for this texture, so we have only one level
	texDesc_.Format = dxgiFormat; // This is the dxgi format of the image (format of the pixels)
	texDesc_.SampleDesc.Count = 1; // This is the number of samples per pixel, we just want 1 sample
	texDesc_.SampleDesc.Quality = 0; // The quality level of the samples. Higher is better quality, but worse performance
	texDesc_.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN; // The arrangement of the pixels. Setting to unknown lets the driver choose the most efficient one
	texDesc_.Flags = D3D12_RESOURCE_FLAG_NONE; // no flags
}