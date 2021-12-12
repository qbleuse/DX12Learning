#pragma once

#include <string>
#include <d3dx12.h>


#ifdef SHADER_DEBUG
#define SHADER_FLAG (D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION)
#else
#define SHADER_FLAG 0
#endif

class DX12Handle;

namespace DX12Helper
{
	/* Shader */

	bool CompileVertex(const std::string& shaderSource_, ID3DBlob** VS_, D3D12_SHADER_BYTECODE& shader_);
	bool CompilePixel(const std::string& shaderSource_, ID3DBlob** PS_, D3D12_SHADER_BYTECODE& shader_);

	/* Resources */

	struct DefaultResource
	{
		ID3D12Resource**			buffer			= nullptr;
		ID3D12Resource*				uploadBuffer	= nullptr;

		~DefaultResource();
	};

	struct DefaultResourceUploader
	{
		ID3D12Device*				device		= nullptr;
		ID3D12GraphicsCommandList4* copyList	= nullptr;
		ID3D12CommandQueue*			queue		= nullptr;
		HANDLE const*				fenceEvent	= nullptr;

		~DefaultResourceUploader();
	};

	bool MakeUploader(DefaultResourceUploader& uploader_, const DX12Handle& dx12Handle_);
	bool CreateDefaultBuffer(D3D12_SUBRESOURCE_DATA* bufferData_, DefaultResource& resourceData_, const DefaultResourceUploader& uploader_);
	bool UploadResources(const DefaultResourceUploader& uploader_);

	struct ConstantResourceUploader
	{
		ID3D12Device*			device		= nullptr;
		ID3D12DescriptorHeap**	descHeap	= nullptr;

		const UINT descSize = 0;
		UINT cbNb			= 0;
	};

	struct ConstantResource
	{
		ID3D12Resource* buffer		= nullptr;
		void*			mapHandle	= nullptr;
		
		D3D12_CPU_DESCRIPTOR_HANDLE bufferCPUHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE bufferGPUHandle;

		~ConstantResource();
	};

	bool CreateCBufferHeap(UINT cbvNb, ConstantResourceUploader& uploader_);
	bool CreateCBuffer(const UINT bufferSize, ConstantResource& resourceData_, ConstantResourceUploader& uploader_);
	void UploadCBuffer(void* bufferData, UINT bufferSize, ConstantResource& resourceData_);
}