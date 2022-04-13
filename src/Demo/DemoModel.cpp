/* system include */
#include <system_error>
#include <cstdio>

/* d3d */
#include <d3dcompiler.h>
#include "DX12Helper.hpp"
#include "DX12Handle.hpp"

/* math*/
#include "GPM/Vector3.hpp"
#include "GPM/Transform.hpp"

/* imgui */
#include "imgui.h"
#include "tiny_loader/tiny_gltf.h"

#include "Demo/DemoModel.hpp"

struct DemoModelVertex
{
	GPM::vec3 pos;
	GPM::vec2 uv;
	GPM::vec3 normal;
};

struct DemoModelConstantBuffer
{
	GPM::mat4 perspective;
	GPM::mat4 view;
	GPM::mat4 model;
};

DemoModel::~DemoModel()
{
	if (_rootSignature)
		_rootSignature->Release();
	if (_pso)
		_pso->Release();

	for (int i = 0; i < _vBuffers.size(); i++)
	{
		if (_vBuffers[i])
			_vBuffers[i]->Release();
	}

	for (int i = 0; i < _texDescriptorHeap.size(); i++)
	{
		if (_texDescriptorHeap[i])
			_texDescriptorHeap[i]->Release();
	}

	for (int i = 0; i < _textureResources.size(); i++)
	{
		if (_textureResources[i])
			_textureResources[i]->Release();
	}

	for (int i = 0; i < _descHeaps.size(); i++)
	{
		if (_descHeaps[i])
			_descHeaps[i]->Release();
	}
}

DemoModel::DemoModel(const DemoInputs& inputs_, const DX12Handle& dx12Handle_)
{
	mainCamera.position.z = 1.0f;

	/* make resources */
	if (!MakeModel(dx12Handle_))
		return;

	/* make shader and pipeline */
	D3D12_SHADER_BYTECODE vertex;
	D3D12_SHADER_BYTECODE pixel;
	if (!MakeShader(vertex, pixel) || !MakePipeline(dx12Handle_, vertex, pixel))
		return;

	/* making constant buffer */
	for (int i = 0; i < _descHeaps.size(); i++)
	{
		DX12Helper::ConstantResourceUploader cbUploader;
		cbUploader.device	= dx12Handle_._device;
		cbUploader.descHeap = &_descHeaps[i];

		if (!DX12Helper::CreateCBufferHeap(1, cbUploader))
			break;


		if (!DX12Helper::CreateCBuffer(sizeof(DemoModelConstantBuffer), _constantBuffers[i], cbUploader))
			break;
	}

	viewport.MaxDepth = 1.0f;
}

bool DemoModel::MakeShader(D3D12_SHADER_BYTECODE& vertex, D3D12_SHADER_BYTECODE& pixel)
{
	ID3DBlob* tmp;
	std::string source = (const char*)R"(#line 81
	struct VOut
	{
		float4 position : SV_POSITION;
		float2 uv		: UV;
		float3 normal	: NORMAL;
	};

	cbuffer ConstantBuffer : register(b0)
	{
		float4x4 perspective;
		float4x4 view;
		float4x4 model;
	};	

	VOut vert(float3 position : POSITION, float2 uv : UV, float3 normal : NORMAL)
	{
		VOut output;
	
		float4 outPos	= mul(float4(position,1.0),model);
		outPos			= mul(outPos,view);
		output.position = mul(outPos,perspective);
		output.uv		= uv;
		output.normal	= normal;
	
		return output;
	}

	Texture2D albedo		: register(t0);
	Texture2D normalMap		: register(t1);
	Texture2D metalRough	: register(t2);
	SamplerState wrap		: register(s0);

	float4 frag(float4 position : SV_POSITION, float2 uv : UV, float3 normal : NORMAL) : SV_TARGET
	{
		return albedo.Sample(wrap,uv);
	}
	)";

	return (DX12Helper::CompileVertex(source, &tmp, vertex) && DX12Helper::CompilePixel(source, &tmp, pixel));
}

/*===== Model =====*/

bool DemoModel::MakeModel(const DX12Handle& dx12Handle_)
{
	/* create resource uploader */
	DX12Helper::DefaultResourceUploader uploader;
	if (!DX12Helper::MakeUploader(uploader, dx12Handle_))
		return false;
	

	DX12Helper::ModelResource modelResources = {};
	modelResources.descHeaps		= &_texDescriptorHeap;
	modelResources.textures			= &_textureResources;
	modelResources.vertexBuffers	= &_vBuffers;

	if (!DX12Helper::UploadModel("media/AntiqueCamera/AntiqueCamera.gltf", modelResources, uploader))
		return false;

	_models = std::move(modelResources.models);

	return true;
}

bool DemoModel::MakePipeline(const DX12Handle& dx12Handle_, D3D12_SHADER_BYTECODE& vertex, D3D12_SHADER_BYTECODE& pixel)
{
	HRESULT hr;
	ID3DBlob* error;
	ID3DBlob* tmp;

	/* texture descriptor */
	D3D12_DESCRIPTOR_RANGE  descriptorTableRanges[1]			= {};
	descriptorTableRanges[0].RangeType							= D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorTableRanges[0].NumDescriptors						= 3;
	descriptorTableRanges[0].OffsetInDescriptorsFromTableStart	= D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	/* create a descriptor table */
	D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable;
	descriptorTable.NumDescriptorRanges = _countof(descriptorTableRanges);
	descriptorTable.pDescriptorRanges	= &descriptorTableRanges[0];

	/* create a root descriptor, which explains where to find the data for this root parameter */
	D3D12_ROOT_DESCRIPTOR rootCBVDescriptor = {};

	/* create a root parameter and fill it out, cbv in 0, srv in 1 */
	D3D12_ROOT_PARAMETER  rootParameters[2] = {};
	rootParameters[0].ParameterType		= D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].Descriptor		= rootCBVDescriptor;
	rootParameters[0].ShaderVisibility	= D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[1].ParameterType		= D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[1].DescriptorTable	= descriptorTable;
	rootParameters[1].ShaderVisibility	= D3D12_SHADER_VISIBILITY_PIXEL;

	/* create a static sampler */
	D3D12_STATIC_SAMPLER_DESC sampler	= {};
	sampler.AddressU					= D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.AddressV					= D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.AddressW					= D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.ComparisonFunc				= D3D12_COMPARISON_FUNC_NEVER;
	sampler.MaxLOD						= D3D12_FLOAT32_MAX;
	sampler.ShaderVisibility			= D3D12_SHADER_VISIBILITY_PIXEL;

	/* then making the root */
	D3D12_ROOT_SIGNATURE_DESC rootDesc	= {};
	rootDesc.NumParameters				= _countof(rootParameters);
	rootDesc.pParameters				= rootParameters;
	rootDesc.NumStaticSamplers			= 1;
	rootDesc.pStaticSamplers			= &sampler;
	rootDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | // we can deny shader stages here for better performance
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

	hr = D3D12SerializeRootSignature(&rootDesc, D3D_ROOT_SIGNATURE_VERSION_1, &tmp, &error);

	if (FAILED(hr))
	{
		printf("Failing serializing root signature of demo %s: %s, %s\n", Name(), std::system_category().message(hr).c_str(), (char*)error->GetBufferPointer());
		error->Release();
		return false;
	}


	hr = dx12Handle_._device->CreateRootSignature(0, tmp->GetBufferPointer(), tmp->GetBufferSize(), IID_PPV_ARGS(&_rootSignature));
	if (FAILED(hr))
	{
		printf("Failing creating root signature of demo %s: %s\n", Name(), std::system_category().message(hr).c_str());
		printf("%s\n", std::system_category().message(dx12Handle_._device->GetDeviceRemovedReason()).c_str());
		return false;
	}

	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(DemoModelVertex, pos), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "UV",			0, DXGI_FORMAT_R32G32_FLOAT,	0, offsetof(DemoModelVertex, uv), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,	0, offsetof(DemoModelVertex, normal), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	// fill out an input layout description structure
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};

	// we can get the number of elements in an array by "sizeof(array) / sizeof(arrayElementType)"
	inputLayoutDesc.NumElements			= sizeof(inputLayout) / sizeof(D3D12_INPUT_ELEMENT_DESC);
	inputLayoutDesc.pInputElementDescs	= inputLayout;

	D3D12_RASTERIZER_DESC rasterDesc	= {};
	rasterDesc.FillMode					= D3D12_FILL_MODE_SOLID;
	rasterDesc.CullMode					= D3D12_CULL_MODE_NONE;
	rasterDesc.DepthClipEnable			= TRUE;

	D3D12_BLEND_DESC blenDesc						= {  };
	blenDesc.RenderTarget[0].RenderTargetWriteMask	= D3D12_COLOR_WRITE_ENABLE_ALL;

	D3D12_DEPTH_STENCIL_DESC depthStencilDesc	= {};
	depthStencilDesc.DepthEnable				= TRUE; // enable depth testing
	depthStencilDesc.DepthWriteMask				= D3D12_DEPTH_WRITE_MASK_ALL; // can write depth data to all of the depth/stencil buffer
	depthStencilDesc.DepthFunc					= D3D12_COMPARISON_FUNC_LESS; // pixel fragment passes depth test if destination pixel's depth is less than pixel fragment's


	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {}; // a structure to define a pso

	psoDesc.InputLayout				= inputLayoutDesc;							// the structure describing our input layout
	psoDesc.pRootSignature			= _rootSignature;							// the root signature that describes the input data this pso needs
	psoDesc.VS						= vertex;									// structure describing where to find the vertex shader bytecode and how large it is
	psoDesc.PS						= pixel;									// same as VS but for pixel shader
	psoDesc.PrimitiveTopologyType	= D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;	// type of topology we are drawing
	psoDesc.RTVFormats[0]			= DXGI_FORMAT_R8G8B8A8_UNORM;				// format of the render target
	psoDesc.SampleDesc				= dx12Handle_._sampleDesc;					// must be the same sample description as the swapchain and depth/stencil buffer
	psoDesc.SampleMask				= 0xffffffff;								// sample mask has to do with multi-sampling. 0xffffffff means point sampling is done
	psoDesc.RasterizerState			= rasterDesc;								// a default rasterizer state.
	psoDesc.BlendState				= blenDesc;									// a default blent state.
	psoDesc.DepthStencilState		= depthStencilDesc;							// a default depth stencil state
	psoDesc.NumRenderTargets		= 1;										// we are only binding one render target
	psoDesc.DSVFormat				= DXGI_FORMAT_D32_FLOAT;

	// create the pso
	hr = dx12Handle_._device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&_pso));
	if (FAILED(hr))
	{
		printf("Failing creating PSO of %s: %s\n", Name(), std::system_category().message(hr).c_str());
		printf("%s\n", std::system_category().message(dx12Handle_._device->GetDeviceRemovedReason()).c_str());
		return false;
	}

	return true;
}


/*===== REALTIME =====*/

void DemoModel::UpdateAndRender(const DemoInputs& inputs_)
{
	Update(inputs_);
	Render(inputs_);
}

void DemoModel::Update(const DemoInputs& inputs_)
{
	mainCamera.UpdateFreeFly(inputs_.cameraInputs);

	viewport.Width		= inputs_.renderContext.width;
	viewport.Height		= inputs_.renderContext.height;
	scissorRect.right	= inputs_.renderContext.width;
	scissorRect.bottom	= inputs_.renderContext.height;

	for (int i = 0; i < _models.size(); i++)
	{
		InspectTransform(_models[i]);
	}
}

void DemoModel::InspectTransform(DX12Helper::Model& model)
{
	ImGui::Text(model.name.c_str());

	GPM::Vec3 scale		= model.trs.scaling();
	GPM::Vec3 rotate	= model.trs.eulerAngles();
	GPM::Vec3 pos		= model.trs.translation();

	ImGui::DragFloat3("Scale", (float*)scale.e, 0.01f, 0.0f, 0.0f, "%.3f", 0);
	ImGui::DragFloat3("Rotation", (float*)rotate.e, 0.01f, 0.0f, 0.0f, "%.3f", 0);
	ImGui::DragFloat3("Position", (float*)pos.e, 0.01f, 0.0f, 0.0f, "%.3f", 0);

	model.trs = GPM::Transform::TRS(pos, rotate, scale);
}

void DemoModel::Render(const DemoInputs& inputs_)
{
	ID3D12GraphicsCommandList4* cmdList = inputs_.renderContext.currCmdList;
	// Clear the render target by using the ClearRenderTargetView command
	const float clearColor[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	cmdList->ClearRenderTargetView(inputs_.renderContext.currBackBufferHandle, clearColor, 0, nullptr);
	cmdList->ClearDepthStencilView(inputs_.renderContext.depthBufferHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	cmdList->SetGraphicsRootSignature(_rootSignature); // set the root signature
	cmdList->SetPipelineState(_pso);
	cmdList->RSSetViewports(1, &viewport); // set the viewports
	cmdList->RSSetScissorRects(1, &scissorRect); // set the scissor rects
	cmdList->SetGraphicsRootConstantBufferView(0, _constantBuffers[inputs_.renderContext.currFrameIndex].buffer->GetGPUVirtualAddress());

	/* update CBuffer */
	DemoModelConstantBuffer cBuffer = {};
	cBuffer.perspective = GPM::Transform::perspective(60.0f * TO_RADIANS, viewport.Width / viewport.Height, 0.001f, 1000.0f);
	cBuffer.view		= mainCamera.GetViewMatrix();

	for (int i = 0; i < _models.size(); i++)
	{
		cBuffer.model = _models[i].trs.model;
		DX12Helper::UploadCBuffer((void*)&cBuffer, sizeof(cBuffer), _constantBuffers[inputs_.renderContext.currFrameIndex]);

		DrawModel(cmdList, _models[i]);
	}
}

void DemoModel::DrawModel(ID3D12GraphicsCommandList4* cmdList, const DX12Helper::Model& model)
{
	cmdList->SetDescriptorHeaps(1, &model.descHeap); // set the descriptor heap
	// set the descriptor table to the descriptor heap (parameter 1, as constant buffer root descriptor is parameter index 0)
	cmdList->SetGraphicsRootDescriptorTable(1, model.descHeap->GetGPUDescriptorHandleForHeapStart());

	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // set the primitive topology
	cmdList->IASetVertexBuffers(0, model.vBufferViews.size(), model.vBufferViews.data()); // set the vertex buffer (using the vertex buffer view)

	if (model.indexBuffer)
	{
		cmdList->IASetIndexBuffer(&model.iBufferView); // set the index buffer (using the index buffer view)
		cmdList->DrawIndexedInstanced(model.count, 1, 0, 0, 0); // finally draw 6 indices (draw the quad)
	}
	else
	{
		cmdList->DrawInstanced(model.count, 1, 0, 0); // finally draw 6 indices (draw the quad)
	}
}