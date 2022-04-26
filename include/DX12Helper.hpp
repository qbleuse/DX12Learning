#pragma once

#include <string>
#include <d3dx12.h>


#ifdef SHADER_DEBUG
#define SHADER_FLAG (D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION)
#else
#define SHADER_FLAG 0
#endif

class DX12Handle;

namespace tinygltf
{
	class Model;
}

#include "GPM/Transform.hpp"

namespace DX12Helper
{
	/* Shader */

	bool CompileVertex(const std::string& shaderSource_, ID3DBlob** VS_, D3D12_SHADER_BYTECODE& shader_);
	bool CompilePixel(const std::string& shaderSource_, ID3DBlob** PS_, D3D12_SHADER_BYTECODE& shader_);

	/* Resources */

	struct DefaultResource
	{
		ID3D12Resource**			buffer			= nullptr;
	};

	struct DefaultResourceUploader
	{
		ID3D12Device*				device		= nullptr;
		ID3D12GraphicsCommandList4* copyList	= nullptr;
		ID3D12CommandQueue*			queue		= nullptr;
		HANDLE const*				fenceEvent	= nullptr;

		std::vector<ID3D12Resource*> uploadBuffers;

		~DefaultResourceUploader();
	};

	bool MakeUploader(DefaultResourceUploader& uploader_, const DX12Handle& dx12Handle_);
	bool CreateDefaultBuffer(D3D12_SUBRESOURCE_DATA* bufferData_, DefaultResource& resourceData_, DefaultResourceUploader& uploader_);
	bool UploadResources(const DefaultResourceUploader& uploader_);

	struct ConstantResourceUploader
	{
		ID3D12Device*			device		= nullptr;
		ID3D12DescriptorHeap**	descHeap	= nullptr;

		UINT cbNb			= 0;
	};

	struct ConstantResource
	{
		ID3D12Resource* buffer		= nullptr;
		void*			mapHandle	= nullptr;
		
		D3D12_CPU_DESCRIPTOR_HANDLE bufferCPUHandle;

		~ConstantResource();
	};

	bool CreateCBufferHeap(UINT cbvNb, ConstantResourceUploader& uploader_);
	bool CreateCBuffer(const UINT bufferSize, ConstantResource& resourceData_, ConstantResourceUploader& uploader_);
	void UploadCBuffer(void* bufferData, UINT bufferSize, ConstantResource& resourceData_);

	struct TextureResource
	{
		ID3D12Resource**				buffer			= nullptr;
		D3D12_CPU_DESCRIPTOR_HANDLE		srvHandle;

		D3D12_SUBRESOURCE_DATA texData;

		~TextureResource();
	};


	/* Texture */
	void LoadTexture(const std::string& filePath_, D3D12_SUBRESOURCE_DATA& texData, D3D12_RESOURCE_DESC& texDesc_);
	bool CreateTexture(const std::string& filePath_, TextureResource& resourceData_, DefaultResourceUploader& uploader_);

	/* assumes data is already filled up in resourceData_.texData*/
	bool CreateRawTexture(const D3D12_RESOURCE_DESC& texDesc_, TextureResource& resourceData_, DefaultResourceUploader& uploader_);

	/* DDS Texture */
	bool CreateDDSTexture(const std::string& filePath_, TextureResource& resourceData_, DefaultResourceUploader& uploader_);

	/* Model */

	/* this will be used to represent a model */
	struct Model
	{
		std::string name;

		/* resources */
		ID3D12DescriptorHeap*					descHeap;
		std::vector<ID3D12Resource*>			textures;

		std::vector<ID3D12Resource*>			vertexBuffers;
		std::vector<D3D12_VERTEX_BUFFER_VIEW>	vBufferViews;

		ID3D12Resource*							indexBuffer;
		D3D12_INDEX_BUFFER_VIEW					iBufferView;

		UINT count = 0;

		GPM::Transform trs;
	};

	struct ModelResource
	{
		/* resources */
		std::vector<ID3D12DescriptorHeap*>*	descHeaps		= nullptr;
		std::vector<ID3D12Resource*>*		textures		= nullptr;
		std::vector<ID3D12Resource*>*		vertexBuffers	= nullptr;

		/* the models created from the info */
		std::vector<Model> models;
	};



	/* used to load a gltf2.0 model onto the GPU
	 * /!\ this method will merge all the mesh and pre-transform all the vertices /!\ */
	bool UploadModel(const std::string& filePath, ModelResource& modelResource, DefaultResourceUploader& uploader_);

	bool UploadMesh(const tinygltf::Model& gltfModel, ModelResource& modelResource, DefaultResourceUploader& uploader_);
	bool UploadMeshBuffer(const tinygltf::Model& gltfModel, ModelResource& modelResource, DefaultResourceUploader& uploader_);
	bool UploadTextureBuffer(const tinygltf::Model& gltfModel, ModelResource& modelResource, DefaultResourceUploader& uploader_);
	bool UploadTexture(const tinygltf::Model& gltfModel, ModelResource& modelResource, DefaultResourceUploader& uploader_);
}