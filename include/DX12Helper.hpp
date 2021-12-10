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

	struct ResourceHelper
	{
		ID3D12Resource**			buffer			= nullptr;
		ID3D12Resource*				uploadBuffer	= nullptr;

		~ResourceHelper();
	};

	struct ResourceUploader
	{
		ID3D12Device*				device		= nullptr;
		ID3D12GraphicsCommandList4* copyList	= nullptr;
		ID3D12CommandQueue*			queue		= nullptr;
		HANDLE const*				fenceEvent	= nullptr;

		~ResourceUploader();
	};

	bool MakeUploader(ResourceUploader& uploader_, const DX12Handle& dx12Handle_);
	bool CreateBuffer(D3D12_SUBRESOURCE_DATA* bufferData_, ResourceHelper& resourceData_, const ResourceUploader& uploader_);
	bool UploadResources(const ResourceUploader& uploader_);
}