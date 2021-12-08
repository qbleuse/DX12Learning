/* system include */
#include <system_error>
#include <cstdio>

/* d3d */
#include <d3dcompiler.h>
#include "DX12Helper.hpp"
#include "DX12Handle.hpp"
#include "GPM/Vector3.hpp"

#include "Demo/DemoTriangle.hpp"

struct DemoTriangleVertex
{
	GPM::vec3 pos;
	GPM::vec3 color;
};

DemoTriangle::~DemoTriangle()
{
	if (_rootSignature)
		_rootSignature->Release();
	if (_pso)
		_pso->Release();
	if (_vBuffer)
		_vBuffer->Release();
	if (_vBufferUploadHeap)
		_vBufferUploadHeap->Release();
}

DemoTriangle::DemoTriangle(const DemoInputs& inputs_, const DX12Handle& dx12Handle_)
{
	HRESULT hr;

	D3D12_SHADER_BYTECODE vertex;
	D3D12_SHADER_BYTECODE pixel;

	if (!MakeShader(vertex, pixel) || !MakePipeline(dx12Handle_,vertex, pixel))
		return;

	ID3D12GraphicsCommandList4* copyList;
	hr = dx12Handle_._device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, dx12Handle_._cmdAllocators[dx12Handle_._context.currFrameIndex], NULL, IID_PPV_ARGS(&copyList));
	if (FAILED(hr))
	{
		printf("Failing creating DX12 command List of demo %s: %s\n", Name(), std::system_category().message(hr).c_str());
		return;
	}

	DemoTriangleVertex triangle[] = {
		{ { 0.0f, 0.5f, 0.5f },   { 1.0f, 0.0f, 0.0f }},
		{ { 0.5f, -0.5f, 0.5f },  { 0.0f, 1.0f, 0.0f }},
		{ { -0.5f, -0.5f, 0.5f }, { 1.0f, 0.0f, 1.0f }},
	};

	int vBufferSize = sizeof(triangle);

	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;

	D3D12_RESOURCE_DESC resDesc = {};

	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.SampleDesc.Count = 1;
	resDesc.Width            = vBufferSize;
	resDesc.Height           = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels        = 1;
	resDesc.Layout			 = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	hr = dx12Handle_._device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&_vBufferUploadHeap));
	_vBufferUploadHeap->SetName(L"DemoTriangleUploadVBuffer");

	if (FAILED(hr))
	{
		printf("Failing creating vertex upload heap of demo %s: %s\n", Name(), std::system_category().message(hr).c_str());
		return;
	}

	heapProp.Type = D3D12_HEAP_TYPE_DEFAULT;

	hr = dx12Handle_._device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&_vBuffer));
	_vBuffer->SetName(L"DemoTriangleUploadVBuffer");

	if (FAILED(hr))
	{
		printf("Failing creating vertex resource of demo %s: %s\n", Name(), std::system_category().message(hr).c_str());
		return;
	}

	D3D12_SUBRESOURCE_DATA vertexData = {};
	vertexData.pData        = (BYTE*)(triangle); // pointer to our vertex array
	vertexData.RowPitch     = vBufferSize; // size of all our triangle vertex data
	vertexData.SlicePitch   = vBufferSize; // also the size of our triangle vertex data

	UpdateSubresources(copyList, _vBuffer, _vBufferUploadHeap, 0, 0, 1, &vertexData);

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                    = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource    = _vBuffer;
	barrier.Transition.StateBefore  = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter   = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	barrier.Transition.Subresource  = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	copyList->ResourceBarrier(1, &barrier);
	copyList->Close();

	ID3D12CommandList* ppCommandLists[] = { copyList };
	dx12Handle_._queue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	// Create synchronization objects and wait until assets have been uploaded to the GPU.
	{
		ID3D12Fence* uploadFence;
		hr = dx12Handle_._device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&uploadFence));
		if (FAILED(hr))
		{
			printf("Failing creating DX12 Fence of demo %s: %s\n",Name(), std::system_category().message(hr).c_str());
			return;
		}

		// Wait for the command list to execute; we are reusing the same command 
		// list in our main loop but for now, we just want to wait for setup to 
		// complete before continuing.

		// Signal and increment the fence value.
		dx12Handle_._queue->Signal(uploadFence, 1);
		if (FAILED(hr))
		{
			printf("Failing signal of demo %s: %s\n", Name(), std::system_category().message(hr).c_str());
			return;
		}


		uploadFence->SetEventOnCompletion(1, dx12Handle_._fenceEvent);
		WaitForSingleObject(dx12Handle_._fenceEvent, INFINITE);
	}

	_vBufferView.BufferLocation = _vBuffer->GetGPUVirtualAddress();
	_vBufferView.StrideInBytes  = sizeof(DemoTriangleVertex);
	_vBufferView.SizeInBytes    = vBufferSize;

	viewport.MaxDepth = 1.0f;
}

bool DemoTriangle::MakeShader(D3D12_SHADER_BYTECODE& vertex, D3D12_SHADER_BYTECODE& pixel)
{
	ID3DBlob* tmp;
	std::string source = (const char*)R"(#line 45
	struct VOut
	{
		float4 position : SV_POSITION;
		float3 color : COLOR; 
	};

	
	VOut vert(float3 position : POSITION, float3 color : COLOR)
	{
		VOut output;
	
		output.position = float4(position,1.0);
		output.color    = color;
	
		return output;
	}

	float4 frag(float4 position : SV_POSITION, float3 color : COLOR) : SV_TARGET
	{
		return float4(color,1.0f);
	}
	)";

	return (DX12Helper::CompileVertex(source, &tmp, vertex) && DX12Helper::CompilePixel(source, &tmp, pixel));
}

bool DemoTriangle::MakePipeline(const DX12Handle& dx12Handle_, D3D12_SHADER_BYTECODE& vertex, D3D12_SHADER_BYTECODE& pixel)
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
		{ "POSITION",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(DemoTriangleVertex, pos), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR",      0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(DemoTriangleVertex, color), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
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

	psoDesc.InputLayout             = inputLayoutDesc; // the structure describing our input layout
	psoDesc.pRootSignature          = _rootSignature; // the root signature that describes the input data this pso needs
	psoDesc.VS                      = vertex; // structure describing where to find the vertex shader bytecode and how large it is
	psoDesc.PS                      = pixel; // same as VS but for pixel shader
	psoDesc.PrimitiveTopologyType   = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; // type of topology we are drawing
	psoDesc.RTVFormats[0]           = DXGI_FORMAT_R8G8B8A8_UNORM; // format of the render target
	psoDesc.SampleDesc              = dx12Handle_._sampleDesc; // must be the same sample description as the swapchain and depth/stencil buffer
	psoDesc.SampleMask              = 0xffffffff; // sample mask has to do with multi-sampling. 0xffffffff means point sampling is done
	psoDesc.RasterizerState         = rasterDesc; // a default rasterizer state.
	psoDesc.BlendState              = blenDesc; // a default blent state.
	psoDesc.NumRenderTargets        = 1; // we are only binding one render target

	// create the pso
	hr = dx12Handle_._device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&_pso));
	if (FAILED(hr))
	{
		printf("Failing creating PSO of %s: %s\n", Name(), std::system_category().message(hr).c_str());
		return false;
	}

	return true;
}

void DemoTriangle::UpdateAndRender(const DemoInputs& inputs_)
{
	viewport.Width      = inputs_.renderContext.width;
	viewport.Height     = inputs_.renderContext.height;
	scissorRect.right   = inputs_.renderContext.width;
	scissorRect.bottom  = inputs_.renderContext.height;

	// Clear the render target by using the ClearRenderTargetView command
	const float clearColor[] = { 0.0f, 0.5f, 0.4f, 1.0f };
	inputs_.renderContext.currCmdList->ClearRenderTargetView(inputs_.renderContext.currBackBufferHandle, clearColor, 0, nullptr);

	inputs_.renderContext.currCmdList->SetPipelineState(_pso);

	// draw triangle
	inputs_.renderContext.currCmdList->SetGraphicsRootSignature(_rootSignature); // set the root signature
	inputs_.renderContext.currCmdList->RSSetViewports(1, &viewport); // set the viewports
	inputs_.renderContext.currCmdList->RSSetScissorRects(1, &scissorRect); // set the scissor rects

	inputs_.renderContext.currCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // set the primitive topology
	inputs_.renderContext.currCmdList->IASetVertexBuffers(0, 1, &_vBufferView); // set the vertex buffer (using the vertex buffer view)

	inputs_.renderContext.currCmdList->DrawInstanced(3, 1, 0, 0); // finally draw 3 vertices (draw the triangle)
}