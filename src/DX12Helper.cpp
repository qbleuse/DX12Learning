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

/* texture/model loading */
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_loader/tiny_gltf.h"
#include "DDSTextureLoader12.h"

/* math */
using namespace GPM;

#undef max

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

DX12Helper::DefaultResourceUploader::~DefaultResourceUploader()
{
	if (copyList)
		copyList->Release();

	for (int i = 0; i < uploadBuffers.size(); i++)
		uploadBuffers[i]->Release();
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

bool DX12Helper::CreateDefaultBuffer(D3D12_SUBRESOURCE_DATA* bufferData_, DefaultResource& resourceData_, DefaultResourceUploader& uploader_)
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

	uploader_.uploadBuffers.resize(uploader_.uploadBuffers.size() + 1);
	hr = uploader_.device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&uploader_.uploadBuffers.back()));

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

	UpdateSubresources(uploader_.copyList, *resourceData_.buffer, uploader_.uploadBuffers.back(), 0, 0, 1, bufferData_);

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

	UINT offset = uploader_.device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	resourceData_.bufferCPUHandle = (*uploader_.descHeap)->GetCPUDescriptorHandleForHeapStart();
	resourceData_.bufferCPUHandle.ptr += offset * uploader_.cbNb;

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

DX12Helper::TextureResource::~TextureResource()
{
	if (texData.pData)
		stbi_image_free((void*)texData.pData);
}


void DX12Helper::LoadTexture(const std::string& filePath_, D3D12_SUBRESOURCE_DATA& texData_, D3D12_RESOURCE_DESC& texDesc_)
{
	int width = 0;
	int height = 0;
	int channels = 0;

	stbi_set_flip_vertically_on_load(1);

	BYTE* tex = stbi_load(filePath_.c_str(), &width, &height, &channels, 0);

	DXGI_FORMAT dxgiFormat = DXGI_FORMAT_UNKNOWN;

	switch (channels)
	{
		case 1: { dxgiFormat = DXGI_FORMAT_R8_UNORM; break; }
		case 2: { dxgiFormat = DXGI_FORMAT_R8G8_UNORM; break; }
			/* there is no 3-bytes dxgi type in d3d12 (gpu does not support anymore), expanding to 4 */
		case 3:
		{
			int arraySize = width * height * 4;
			BYTE* old = tex;
			tex = (BYTE*)malloc(arraySize);
			BYTE max = std::numeric_limits<BYTE>().max();

			for (int i = 0, j = 0; i < arraySize; i += 4, j += 3)
			{
				tex[i]		= old[j];
				tex[i + 1]	= old[j + 1];
				tex[i + 2]	= old[j + 2];
				tex[i + 3]	= max;
			}

			stbi_image_free(old);
			channels = 4;
		}
		default: { dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM; break; }
	};

	texData_.pData		= tex;
	texData_.RowPitch	= width * channels * sizeof(BYTE);
	texData_.SlicePitch = texData_.RowPitch * height;

	texDesc_ = {};
	texDesc_.Dimension			= D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texDesc_.Alignment			= 0;		// may be 0, 4KB, 64KB, or 4MB. 0 will let runtime decide between 64KB and 4MB (4MB for multi-sampled textures)
	texDesc_.Width				= width;	// width of the texture
	texDesc_.Height				= height;	// height of the texture
	texDesc_.DepthOrArraySize	= 1;		// if 3d image, depth of 3d image. Otherwise an array of 1D or 2D textures (we only have one image, so we set 1)
	texDesc_.MipLevels			= 1;		// Number of mipmaps. We are not generating mipmaps for this texture, so we have only one level
	texDesc_.Format				= dxgiFormat; // This is the dxgi format of the image (format of the pixels)
	texDesc_.SampleDesc.Count	= 1;		// This is the number of samples per pixel, we just want 1 sample
	texDesc_.SampleDesc.Quality = 0;		// The quality level of the samples. Higher is better quality, but worse performance
	texDesc_.Layout				= D3D12_TEXTURE_LAYOUT_UNKNOWN; // The arrangement of the pixels. Setting to unknown lets the driver choose the most efficient one
	texDesc_.Flags				= D3D12_RESOURCE_FLAG_NONE; // no flags
}

bool DX12Helper::CreateTexture(const std::string& filePath_, TextureResource& resourceData_, DefaultResourceUploader& uploader_)
{
	/* load texture Data */
	D3D12_RESOURCE_DESC texDesc = {};
	LoadTexture(filePath_, resourceData_.texData, texDesc);

	if (!CreateRawTexture(texDesc, resourceData_, uploader_))
		return false;

	/* make the shader resource view from buffer and texDesc to make it available to use */
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping			= D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format							= texDesc.Format;
	srvDesc.ViewDimension					= D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels				= 1;

	uploader_.device->CreateShaderResourceView(*resourceData_.buffer, &srvDesc, resourceData_.srvHandle);

	return true;
}

bool DX12Helper::CreateRawTexture(const D3D12_RESOURCE_DESC& texDesc_, TextureResource& resourceData_, DefaultResourceUploader& uploader_)
{
	HRESULT hr;

	/* create upload heap to send texture info to default */
	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;

	D3D12_RESOURCE_DESC uploadDesc = {};

	uploadDesc.Dimension		= D3D12_RESOURCE_DIMENSION_BUFFER;
	uploadDesc.SampleDesc.Count = 1;
	uploader_.device->GetCopyableFootprints(&texDesc_, 0, 1, 0, nullptr, nullptr, nullptr, &uploadDesc.Width);
	uploadDesc.Height			= 1;
	uploadDesc.DepthOrArraySize = 1;
	uploadDesc.MipLevels		= 1;
	uploadDesc.Layout			= D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	uploader_.uploadBuffers.resize(uploader_.uploadBuffers.size() + 1);
	hr = uploader_.device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &uploadDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&uploader_.uploadBuffers.back()));

	if (FAILED(hr))
	{
		printf("Failing creating default buffer upload heap: %s\n", std::system_category().message(hr).c_str());
		return false;
	}

	/* create default heap, technically the last place where the texture will be sent */
	heapProp.Type = D3D12_HEAP_TYPE_DEFAULT;

	hr = uploader_.device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &texDesc_, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(resourceData_.buffer));

	if (FAILED(hr))
	{
		printf("Failing creating default buffer resource : %s\n", std::system_category().message(hr).c_str());
		return false;
	}

	/* uploading and barrier for making the application wait for the ressource to be uploaded on gpu */
	UpdateSubresources(uploader_.copyList, *resourceData_.buffer, uploader_.uploadBuffers.back(), 0, 0, 1, &resourceData_.texData);

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type					= D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource	= *resourceData_.buffer;
	barrier.Transition.StateBefore	= D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter	= D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.Subresource	= D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	uploader_.copyList->ResourceBarrier(1, &barrier);

	return true;
}

bool DX12Helper::CreateDDSTexture(const std::string& filePath_, TextureResource& resourceData_, DefaultResourceUploader& uploader_)
{
	HRESULT hr;
	std::unique_ptr<uint8_t[]> ddsData;
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	bool isCube = false;

	std::wstring wString = std::wstring(filePath_.begin(), filePath_.end());

	hr = DirectX::LoadDDSTextureFromFile(uploader_.device, wString.c_str(), resourceData_.buffer, ddsData, subresources, 0ULL, nullptr, &isCube);

	if (FAILED(hr))
	{
		printf("Failing creating dds texture resource : %s\n", std::system_category().message(hr).c_str());
		return false;
	}

	D3D12_RESOURCE_DESC resourceDesc = (*resourceData_.buffer)->GetDesc();

	/* create upload heap to send texture info to default */
	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;

	D3D12_RESOURCE_DESC uploadDesc = {};

	uploadDesc.Dimension		= D3D12_RESOURCE_DIMENSION_BUFFER;
	uploadDesc.SampleDesc.Count = 1;
	uploader_.device->GetCopyableFootprints(&resourceDesc, 0, subresources.size(), 0, nullptr, nullptr, nullptr, &uploadDesc.Width);
	uploadDesc.Height			= 1;
	uploadDesc.DepthOrArraySize = 1;
	uploadDesc.MipLevels		= 1;
	uploadDesc.Layout			= D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	uploader_.uploadBuffers.resize(uploader_.uploadBuffers.size() + 1);
	hr = uploader_.device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &uploadDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&uploader_.uploadBuffers.back()));

	if (FAILED(hr))
	{
		printf("Failing creating dds buffer upload heap: %s\n", std::system_category().message(hr).c_str());
		return false;
	}

	/* uploading and barrier for making the application wait for the ressource to be uploaded on gpu */
	UpdateSubresources(uploader_.copyList, *resourceData_.buffer, uploader_.uploadBuffers.back(), 0, 0, subresources.size(), subresources.data());

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type					= D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource	= *resourceData_.buffer;
	barrier.Transition.StateBefore	= D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter	= D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.Subresource	= D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	uploader_.copyList->ResourceBarrier(1, &barrier);

	/* make the shader resource view from buffer and texDesc to make it available to use */
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format					= resourceDesc.Format;


	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_UNKNOWN;
	switch (resourceDesc.Dimension)
	{
	case (D3D12_RESOURCE_DIMENSION_BUFFER):
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.NumElements			= resourceDesc.MipLevels;
		srvDesc.Buffer.StructureByteStride	= resourceDesc.Height;
			break;
	case (D3D12_RESOURCE_DIMENSION_TEXTURE1D):
		srvDesc.Texture1D.MipLevels = resourceDesc.MipLevels;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
		if (resourceDesc.DepthOrArraySize > 1)
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
			break;
	case (D3D12_RESOURCE_DIMENSION_TEXTURE2D):
		srvDesc.Texture2D.MipLevels = resourceDesc.MipLevels;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		if (resourceDesc.DepthOrArraySize > 1)
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
			break;
	case (D3D12_RESOURCE_DIMENSION_TEXTURE3D):
		srvDesc.Texture3D.MipLevels = resourceDesc.MipLevels;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
		break;
	default:
		break;
	}

	if (isCube)
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
 
	uploader_.device->CreateShaderResourceView(*resourceData_.buffer, &srvDesc, resourceData_.srvHandle);

	return true;
}

/*===== MODEL  =====*/

bool DX12Helper::UploadModel(const std::string& filePath, ModelResource& modelResource, DefaultResourceUploader& uploader_)
{
	tinygltf::Model		model;
	tinygltf::TinyGLTF	loader;
	std::string			err;
	std::string			warn;

	if (!loader.LoadASCIIFromFile(&model, &err, &warn, filePath.c_str()))
	{
		printf("Error Loading model: %s", err.c_str());
		return false;
	}

	if (!warn.empty())
		printf("Warning Loading model: %s", warn.c_str());

	if (!UploadMeshBuffer(model, modelResource, uploader_) || !UploadTextureBuffer(model, modelResource, uploader_))
		return false;

	/* upload all created resources */
	if (!UploadResources(uploader_))
		return false;

	if (!UploadMesh(model,modelResource,uploader_) || !UploadTexture(model, modelResource, uploader_))
		return false;

	return true;
}

bool DX12Helper::UploadMesh(const tinygltf::Model& gltfModel, ModelResource& modelResource, DefaultResourceUploader& uploader_)
{
	const tinygltf::Scene& dftScene = gltfModel.scenes[gltfModel.defaultScene];
	for (int node = 0; node < dftScene.nodes.size(); node++)
	{
		const tinygltf::Node& currNode	= gltfModel.nodes[node];
		const tinygltf::Mesh& mesh		= gltfModel.meshes[currNode.mesh];
		Model currModel;
		currModel.name					= currNode.name;

		if (!currNode.scale.empty())
		{
			currModel.trs.scaleX(static_cast<f32>(currNode.scale[0]));
			currModel.trs.scaleY(static_cast<f32>(currNode.scale[1]));
			currModel.trs.scaleZ(static_cast<f32>(currNode.scale[2]));
		}

		if (!currNode.rotation.empty())
		{
			currModel.trs.rotateX(static_cast<f32>(currNode.rotation[0]));
			currModel.trs.rotateY(static_cast<f32>(currNode.rotation[1]));
			currModel.trs.rotateZ(static_cast<f32>(currNode.rotation[2]));
		}

		if (!currNode.translation.empty())
		{
			currModel.trs.translateX(static_cast<f32>(currNode.translation[0]));
			currModel.trs.translateY(static_cast<f32>(currNode.translation[1]));
			currModel.trs.translateZ(static_cast<f32>(currNode.translation[2]));
		}


		for (int primitive = 0; primitive < mesh.primitives.size(); primitive++)
		{
			const tinygltf::Primitive& currPrimitives = mesh.primitives[primitive];
			currModel.vBufferViews.resize(3);
			currModel.vertexBuffers.resize(3);

			for (const std::pair<const std::string, int>& attribute : currPrimitives.attributes)
			{
				const tinygltf::Accessor&	access		= gltfModel.accessors[attribute.second];
				const tinygltf::BufferView& bufferView	= gltfModel.bufferViews[access.bufferView];

				UINT size	= static_cast<UINT>(bufferView.byteLength - access.byteOffset);
				UINT stride	= access.ByteStride(bufferView);
				D3D12_GPU_VIRTUAL_ADDRESS loc = (*modelResource.vertexBuffers)[bufferView.buffer]->GetGPUVirtualAddress() + bufferView.byteOffset + access.byteOffset;

				int ind = -1;
				if (attribute.first == "NORMAL")
					ind = 2;
				else if (attribute.first == "POSITION")
					ind = 0;
				else if (attribute.first == "TEXCOORD_0")
					ind = 1;
				else
					break;

				currModel.vBufferViews[ind].BufferLocation	= loc;
				currModel.vBufferViews[ind].SizeInBytes		= size;
				currModel.vBufferViews[ind].StrideInBytes	= stride;

				currModel.vertexBuffers[ind] = (*modelResource.vertexBuffers)[bufferView.buffer];
			}

			/* count should not change for a single primitive (info per vertex )*/
			currModel.count = gltfModel.accessors[currPrimitives.attributes.begin()->second].count;

			if (currPrimitives.indices >= 0)
			{
				const tinygltf::Accessor& access		= gltfModel.accessors[currPrimitives.indices];
				currModel.count							= access.count;
				const tinygltf::BufferView& bufferView	= gltfModel.bufferViews[access.bufferView];

				UINT size	= static_cast<UINT>(bufferView.byteLength - access.byteOffset);
				D3D12_GPU_VIRTUAL_ADDRESS loc = (*modelResource.vertexBuffers)[bufferView.buffer]->GetGPUVirtualAddress() + bufferView.byteOffset + access.byteOffset;

				currModel.iBufferView.BufferLocation	= loc;
				currModel.iBufferView.SizeInBytes		= size;

				switch (access.componentType)
				{
					case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
						currModel.iBufferView.Format = DXGI_FORMAT_R8_UINT;
						break;
					case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
						currModel.iBufferView.Format = DXGI_FORMAT_R16_UINT;
						break;
					case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
						currModel.iBufferView.Format = DXGI_FORMAT_R32_UINT;
						break;
				}

				currModel.indexBuffer = (*modelResource.vertexBuffers)[bufferView.buffer];
			}

			/* we want copy constructor */
			modelResource.models.push_back(currModel);
		}
	}

	return true;
}

bool DX12Helper::UploadMeshBuffer(const tinygltf::Model& gltfModel, ModelResource& modelResource, DefaultResourceUploader& uploader_)
{
	modelResource.vertexBuffers->resize(gltfModel.buffers.size());
	for (int i = 0; i < gltfModel.buffers.size(); i++)
	{
		const tinygltf::Buffer& buffer = gltfModel.buffers[i];

		DefaultResource dftResource = {};
		dftResource.buffer = modelResource.vertexBuffers->data() + i;

		D3D12_SUBRESOURCE_DATA data = {};
		data.pData		= buffer.data.data();
		data.RowPitch	= buffer.data.size();
		data.SlicePitch = buffer.data.size();

		CreateDefaultBuffer(&data, dftResource, uploader_);
	}

	return true;
}

bool DX12Helper::UploadTextureBuffer(const tinygltf::Model& gltfModel, ModelResource& modelResource, DefaultResourceUploader& uploader_)
{
	modelResource.textures->resize(gltfModel.images.size() + 1);

	D3D12_RESOURCE_DESC texDesc = {};
	texDesc.Dimension			= D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texDesc.Alignment			= 0;		// may be 0, 4KB, 64KB, or 4MB. 0 will let runtime decide between 64KB and 4MB (4MB for multi-sampled textures)
	texDesc.DepthOrArraySize	= 1;		// if 3d image, depth of 3d image. Otherwise an array of 1D or 2D textures (we only have one image, so we set 1)
	texDesc.MipLevels			= 1;		// Number of mipmaps. We are not generating mipmaps for this texture, so we have only one level
	texDesc.SampleDesc.Count	= 1;		// This is the number of samples per pixel, we just want 1 sample
	texDesc.SampleDesc.Quality	= 0;		// The quality level of the samples. Higher is better quality, but worse performance
	texDesc.Layout				= D3D12_TEXTURE_LAYOUT_UNKNOWN; // The arrangement of the pixels. Setting to unknown lets the driver choose the most efficient one
	texDesc.Flags				= D3D12_RESOURCE_FLAG_NONE; // no flags

	/* upload black texture to gpu */
	{
	
		unsigned char zero = '\0';
		TextureResource textureResource = {};
		textureResource.buffer = modelResource.textures->data();
	
		/* the actual data to send */
		textureResource.texData.pData = &zero;
		textureResource.texData.RowPitch = sizeof(BYTE);
		textureResource.texData.SlicePitch = textureResource.texData.RowPitch;
	
		texDesc.Width = 1;	// width of the texture
		texDesc.Height = 1;	// height of the texture
		texDesc.Format = DXGI_FORMAT_R8_UNORM;
	
		if (!CreateRawTexture(texDesc, textureResource, uploader_))
			return false;
	
		textureResource.texData.pData = nullptr;
	}

	for (int images = 0; images < gltfModel.images.size(); images++)
	{
		const tinygltf::Image& currImage = gltfModel.images[images];

		TextureResource textureResource = {};

		/* the gpu resource we will fill up in */
		textureResource.buffer			= modelResource.textures->data() + images + 1;

		BYTE* tex		= (BYTE*)currImage.image.data();
		int channels	= currImage.component;
		switch (channels)
		{
			case 1: { texDesc.Format = DXGI_FORMAT_R8_UNORM; break; }
			case 2: { texDesc.Format = DXGI_FORMAT_R8G8_UNORM; break; }
				  /* there is no 3-bytes dxgi type in d3d12 (gpu does not support anymore), expanding to 4 */
			case 3:
			{
				int arraySize = currImage.width * currImage.height * 4;
				BYTE* old = tex;
				tex = (BYTE*)malloc(arraySize);
				BYTE max = std::numeric_limits<BYTE>().max();

				for (int i = 0, j = 0; i < arraySize; i += 4, j += 3)
				{
					tex[i] = old[j];
					tex[i + 1] = old[j + 1];
					tex[i + 2] = old[j + 2];
					tex[i + 3] = max;
				}

				stbi_image_free(old);
				channels = 4;
			}
			default: { texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; break; }
		};

		/* the actual data to send */
		textureResource.texData.pData		= tex;
		textureResource.texData.RowPitch	= currImage.width * channels * sizeof(BYTE);
		textureResource.texData.SlicePitch	= textureResource.texData.RowPitch * currImage.height;

		texDesc.Width	= currImage.width;	// width of the texture
		texDesc.Height	= currImage.height;	// height of the texture

		if (!CreateRawTexture(texDesc, textureResource, uploader_))
			return false;

		textureResource.texData.pData = nullptr;
	}
	return true;
}

bool DX12Helper::UploadTexture(const tinygltf::Model& gltfModel, ModelResource& modelResource, DefaultResourceUploader& uploader_)
{
	HRESULT hr;

	/* create the descriptor heap that will store our srv */
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.Flags	= D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	heapDesc.Type	= D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	/* we will only consider pbr with normal material*/
	heapDesc.NumDescriptors = 3;
	UINT offset = uploader_.device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	/* make the shader resource view to make it available to use */
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	modelResource.descHeaps->resize(gltfModel.materials.size());
	for (int i = 0; i < gltfModel.materials.size(); i++)
	{
		const tinygltf::Material& currMat = gltfModel.materials[i];

		ID3D12DescriptorHeap** currDescHeap = &(*modelResource.descHeaps)[i];
		hr = uploader_.device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(currDescHeap));

		if (FAILED(hr))
		{
			printf("Failing creating main descriptor heap for %s: %s\n", currMat.name.c_str(), std::system_category().message(hr).c_str());
			return false;
		}

		D3D12_CPU_DESCRIPTOR_HANDLE handle = (*currDescHeap)->GetCPUDescriptorHandleForHeapStart();

		/* when mat does not have it makes index -1, as everything is offseted by one in our array,
		 * a -1 texture points to the default black texture. */
		const int currImages[3] = { currMat.pbrMetallicRoughness.baseColorTexture.index + 1,
												currMat.normalTexture.index + 1,
												currMat.pbrMetallicRoughness.metallicRoughnessTexture.index + 1};

		for (int i = 0; i < _countof(currImages); i++)
		{
			if (currImages[i] == 0)
			{
				srvDesc.Format = DXGI_FORMAT_R8_UNORM;
			}
			else
			{
				switch (gltfModel.images[currImages[i] - 1].component)
				{
				case 1: { srvDesc.Format = DXGI_FORMAT_R8_UNORM; break; }
				case 2: { srvDesc.Format = DXGI_FORMAT_R8G8_UNORM; break; }
					  /* there is no 3-bytes dxgi type in d3d12 (gpu does not support anymore), expanding to 4 */
				case 3:
				default: { srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; break; }
				};
			}

			uploader_.device->CreateShaderResourceView((*modelResource.textures)[currImages[i]], &srvDesc, handle);

			handle.ptr += offset;
		}
	}

	const tinygltf::Scene& dftScene = gltfModel.scenes[gltfModel.defaultScene];
	int modelIndex = 0;
	for (int node = 0; node < dftScene.nodes.size(); node++)
	{
		const tinygltf::Node&	currNode	= gltfModel.nodes[node];
		const tinygltf::Mesh&	mesh		= gltfModel.meshes[currNode.mesh];

		for (int primitive = 0; primitive < mesh.primitives.size(); primitive++)
		{
			const tinygltf::Primitive&	currPrimitive	= mesh.primitives[primitive];
			const tinygltf::Material&	currMat			= gltfModel.materials[currPrimitive.material];
			ID3D12DescriptorHeap* currDescHeap			= (*modelResource.descHeaps)[currPrimitive.material];
			Model& currModel							= modelResource.models[modelIndex++];

			/* set desc heap */
			currModel.descHeap = currDescHeap;

			const int currImages[3] = { currMat.pbrMetallicRoughness.baseColorTexture.index + 1, 
													currMat.normalTexture.index + 1,
													currMat.pbrMetallicRoughness.metallicRoughnessTexture.index + 1};

			for (int i = 0; i < _countof(currImages); i++)
			{
				currModel.textures.push_back((*modelResource.textures)[currImages[i]]);
			}
		}
	}
	return true;
}