/* system include */
#include <system_error>
#include <cstdio>

/* d3d */
#include <d3dcompiler.h>
#include "DX12Helper.hpp"
#include "DX12Handle.hpp"
#include "GPM/Vector3.hpp"

#include "Demo/DemoQuad.hpp"

struct DemoQuadVertex
{
	GPM::vec3 pos;
};

DemoQuad::~DemoQuad()
{
	if (_rootSignature)
		_rootSignature->Release();
	if (_pso)
		_pso->Release();
	if (_vBuffer)
		_vBuffer->Release();
	if (_iBuffer)
		_iBuffer->Release();
}

DemoQuad::DemoQuad(const DemoInputs& inputs_, const DX12Handle& dx12Handle_)
{
	/* make shader and pipeline */
	D3D12_SHADER_BYTECODE vertex;
	D3D12_SHADER_BYTECODE pixel;
	if (!MakeShader(vertex, pixel) || !MakePipeline(dx12Handle_, vertex, pixel))
		return;

	/* create resource uploader */
	DX12Helper::ResourceUploader uploader;
	if (!DX12Helper::MakeUploader(uploader, dx12Handle_))
		return;

	/* create vertex buffer resources */
	DemoQuadVertex triangle[] = 
	{
		{{ -0.5f,  0.5f, 0.0f }}, // top left
		{{  0.5f, -0.5f, 0.0f }}, // bottom right
		{{ -0.5f, -0.5f, 0.0f }}, // bottom left
		{{  0.5f,  0.5f, 0.0f }}  // top right
	};

	int vBufferSize = sizeof(triangle);

	D3D12_SUBRESOURCE_DATA vertexData = {};
	vertexData.pData = (BYTE*)(triangle); // pointer to our vertex array
	vertexData.RowPitch = vBufferSize; // size of all our triangle vertex data
	vertexData.SlicePitch = vBufferSize; // also the size of our triangle vertex data

	/* create resource helper and call for vertex buffer uploaded on GPU*/
	DX12Helper::ResourceHelper vertexHelper;
	vertexHelper.buffer = &_vBuffer;

	if (!DX12Helper::CreateBuffer(&vertexData, vertexHelper, uploader))
		return;

	DWORD indices[] =
	{
		0, 1, 2, // first triangle
		0, 3, 1 // second triangle
	};

	int iBufferSize = sizeof(indices);

	D3D12_SUBRESOURCE_DATA indicesData = {};
	indicesData.pData		= (BYTE*)(indices); // pointer to our vertex array
	indicesData.RowPitch	= iBufferSize; // size of all our triangle vertex data
	indicesData.SlicePitch	= iBufferSize; // also the size of our triangle vertex data

	DX12Helper::ResourceHelper indicesHelper;
	indicesHelper.buffer = &_iBuffer;

	if (!DX12Helper::CreateBuffer(&indicesData, indicesHelper, uploader))
		return;

	/* upload all created resources */
	if (!DX12Helper::UploadResources(uploader))
		return;

	/* create vertex buffer view out of resources */
	_vBufferView.BufferLocation = _vBuffer->GetGPUVirtualAddress();
	_vBufferView.StrideInBytes	= sizeof(DemoQuadVertex);
	_vBufferView.SizeInBytes	= vBufferSize;

	/* create index buffer view out of resources */
	_iBufferView.BufferLocation = _iBuffer->GetGPUVirtualAddress();
	_iBufferView.Format			= DXGI_FORMAT_R32_UINT; // 32-bit unsigned integer (this is what a dword is, double word, a word is 2 bytes)
	_iBufferView.SizeInBytes	= iBufferSize;

	viewport.MaxDepth = 1.0f;
}

bool DemoQuad::MakeShader(D3D12_SHADER_BYTECODE& vertex, D3D12_SHADER_BYTECODE& pixel)
{
	ID3DBlob* tmp;
	std::string source = (const char*)R"(#line 45
	struct VOut
	{
		float4 position : SV_POSITION;
	};

	
	VOut vert(float3 position : POSITION)
	{
		VOut output;
	
		output.position = float4(position,1.0);
	
		return output;
	}

	float4 frag(float4 position : SV_POSITION) : SV_TARGET
	{
		return float4(1.0f,1.0f,1.0f,1.0f);
	}
	)";

	return (DX12Helper::CompileVertex(source, &tmp, vertex) && DX12Helper::CompilePixel(source, &tmp, pixel));
}

bool DemoQuad::MakePipeline(const DX12Handle& dx12Handle_, D3D12_SHADER_BYTECODE& vertex, D3D12_SHADER_BYTECODE& pixel)
{
	HRESULT hr;
	ID3DBlob* error;
	ID3DBlob* tmp;

	D3D12_ROOT_SIGNATURE_DESC rootDesc = {};
	rootDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;


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
		return false;
	}

	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(DemoQuadVertex, pos), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	// fill out an input layout description structure
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};

	// we can get the number of elements in an array by "sizeof(array) / sizeof(arrayElementType)"
	inputLayoutDesc.NumElements = sizeof(inputLayout) / sizeof(D3D12_INPUT_ELEMENT_DESC);
	inputLayoutDesc.pInputElementDescs = inputLayout;

	D3D12_RASTERIZER_DESC rasterDesc = {};
	rasterDesc.FillMode = D3D12_FILL_MODE_SOLID;
	rasterDesc.CullMode = D3D12_CULL_MODE_NONE;

	D3D12_BLEND_DESC blenDesc = {  };

	blenDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;


	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {}; // a structure to define a pso

	psoDesc.InputLayout = inputLayoutDesc; // the structure describing our input layout
	psoDesc.pRootSignature = _rootSignature; // the root signature that describes the input data this pso needs
	psoDesc.VS = vertex; // structure describing where to find the vertex shader bytecode and how large it is
	psoDesc.PS = pixel; // same as VS but for pixel shader
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; // type of topology we are drawing
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // format of the render target
	psoDesc.SampleDesc = dx12Handle_._sampleDesc; // must be the same sample description as the swapchain and depth/stencil buffer
	psoDesc.SampleMask = 0xffffffff; // sample mask has to do with multi-sampling. 0xffffffff means point sampling is done
	psoDesc.RasterizerState = rasterDesc; // a default rasterizer state.
	psoDesc.BlendState = blenDesc; // a default blent state.
	psoDesc.NumRenderTargets = 1; // we are only binding one render target

	// create the pso
	hr = dx12Handle_._device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&_pso));
	if (FAILED(hr))
	{
		printf("Failing creating PSO of %s: %s\n", Name(), std::system_category().message(hr).c_str());
		return false;
	}

	return true;
}

void DemoQuad::UpdateAndRender(const DemoInputs& inputs_)
{
	viewport.Width = inputs_.renderContext.width;
	viewport.Height = inputs_.renderContext.height;
	scissorRect.right = inputs_.renderContext.width;
	scissorRect.bottom = inputs_.renderContext.height;

	// Clear the render target by using the ClearRenderTargetView command
	const float clearColor[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	inputs_.renderContext.currCmdList->ClearRenderTargetView(inputs_.renderContext.currBackBufferHandle, clearColor, 0, nullptr);

	// draw triangle
	inputs_.renderContext.currCmdList->SetGraphicsRootSignature(_rootSignature); // set the root signature
	inputs_.renderContext.currCmdList->SetPipelineState(_pso);
	inputs_.renderContext.currCmdList->RSSetViewports(1, &viewport); // set the viewports
	inputs_.renderContext.currCmdList->RSSetScissorRects(1, &scissorRect); // set the scissor rects

	inputs_.renderContext.currCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // set the primitive topology
	inputs_.renderContext.currCmdList->IASetVertexBuffers(0, 1, &_vBufferView); // set the vertex buffer (using the vertex buffer view)
	inputs_.renderContext.currCmdList->IASetIndexBuffer(&_iBufferView); // set the index buffer (using the index buffer view)

	inputs_.renderContext.currCmdList->DrawIndexedInstanced(6, 1, 0, 0, 0); // finally draw 6 indices (draw the quad)
}