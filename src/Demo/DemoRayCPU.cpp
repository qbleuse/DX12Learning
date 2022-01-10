/* system include */
#include <system_error>
#include <cstdio>

/* dx12 */
#include "DX12Handle.hpp"
#include "DX12Helper.hpp"

#include "Demo/DemoRayCPU.hpp"


DemoRayCPU::~DemoRayCPU()
{
	free(cpuTexture);

	for (int i = 0; i < _descHeaps.size(); i++)
	{
		if (_descHeaps[i])
			_descHeaps[i]->Release();
	}

	if (_rootSignature)
		_rootSignature->Release();
	if (_pso)
		_pso->Release();
}


DemoRayCPU::UploadTexture::~UploadTexture()
{
	if (gpuTexture)
		gpuTexture->Release();
}

DemoRayCPU::DemoRayCPU(const DemoInputs& inputs, const DX12Handle& dx12Handle_)
{
	viewport.MaxDepth = 1.0f;

	D3D12_SHADER_BYTECODE vertex;
	D3D12_SHADER_BYTECODE pixel;
    mainCamera.position = { 0.f, 0.f, 2.f };
	if (!MakeTexture(inputs, dx12Handle_) 
		|| !MakeShader(vertex,pixel) 
		|| !MakePipeline(dx12Handle_,vertex,pixel))
		return;

}

bool DemoRayCPU::MakeShader(D3D12_SHADER_BYTECODE& vertex, D3D12_SHADER_BYTECODE& pixel)
{
	ID3DBlob* tmp;
	std::string source = (const char*)R"(#line 30
    struct VOut
    {
        float4 position : SV_POSITION;
        float2 uv : UV; 
    };
    
    VOut vert(uint vI :SV_VertexId )
    {
        VOut output;
    
        float2 uv = float2((vI << 1) & 2, vI & 2);
        output.uv = float2(uv.x,uv.y);
        output.position = float4(uv.x * 2 - 1, -uv.y * 2 + 1, 0, 1);
    
        return output;

    }

	Buffer tex : register(t0);
	SamplerState  clamp : register(s0);

	float4 frag(float4 position : SV_POSITION, float2 uv : UV) : SV_TARGET
	{
		return tex.Load(uv.x * 1900.0f  + uv.y * 800.0f);
	}
    )";

	return (DX12Helper::CompileVertex(source, &tmp, vertex) && DX12Helper::CompilePixel(source, &tmp, pixel));
}

bool DemoRayCPU::MakePipeline(const DX12Handle& dx12Handle_, D3D12_SHADER_BYTECODE& vertex, D3D12_SHADER_BYTECODE& pixel)
{
	HRESULT hr;
	ID3DBlob* error;
	ID3DBlob* tmp;

	/* texture descriptor */
	D3D12_DESCRIPTOR_RANGE  descriptorTableRanges[1] = {};
	descriptorTableRanges[0].RangeType							= D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorTableRanges[0].NumDescriptors						= 1;
	descriptorTableRanges[0].OffsetInDescriptorsFromTableStart	= D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	/* create a descriptor table */
	D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable;
	descriptorTable.NumDescriptorRanges = _countof(descriptorTableRanges);
	descriptorTable.pDescriptorRanges = &descriptorTableRanges[0];

	/* create a root descriptor, which explains where to find the data for this root parameter */
	D3D12_ROOT_DESCRIPTOR rootCBVDescriptor = {};

	/* create a root parameter and fill it out, cbv in 0, srv in 1 */
	D3D12_ROOT_PARAMETER  rootParameters[1];
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[0].DescriptorTable = descriptorTable;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	/* create a static sampler */
	D3D12_STATIC_SAMPLER_DESC sampler = {};
	sampler.AddressU			= D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	sampler.AddressV			= D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	sampler.AddressW			= D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	sampler.ComparisonFunc		= D3D12_COMPARISON_FUNC_NEVER;
	sampler.MaxLOD				= D3D12_FLOAT32_MAX;
	sampler.ShaderVisibility	= D3D12_SHADER_VISIBILITY_PIXEL;

	/* then making the root */
	D3D12_ROOT_SIGNATURE_DESC rootDesc = {};
	rootDesc.NumParameters = _countof(rootParameters);
	rootDesc.pParameters = rootParameters;
	rootDesc.NumStaticSamplers = 1;
	rootDesc.pStaticSamplers = &sampler;
	rootDesc.Flags = // we can deny shader stages here for better performance
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
		return false;
	}


	D3D12_RASTERIZER_DESC rasterDesc = {};
	rasterDesc.FillMode = D3D12_FILL_MODE_SOLID;
	rasterDesc.CullMode = D3D12_CULL_MODE_NONE;

	D3D12_BLEND_DESC blenDesc = {  };
	blenDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {}; // a structure to define a pso

	psoDesc.pRootSignature	= _rootSignature;							// the root signature that describes the input data this pso needs
	psoDesc.VS				= vertex;									// structure describing where to find the vertex shader bytecode and how large it is
	psoDesc.PS				= pixel;									// same as VS but for pixel shader
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;	// type of topology we are drawing
	psoDesc.RTVFormats[0]	= DXGI_FORMAT_R8G8B8A8_UNORM;				// format of the render target
	psoDesc.SampleDesc		= dx12Handle_._sampleDesc;					// must be the same sample description as the swapchain and depth/stencil buffer
	psoDesc.SampleMask		= 0xffffffff;								// sample mask has to do with multi-sampling. 0xffffffff means point sampling is done
	psoDesc.RasterizerState = rasterDesc;								// a default rasterizer state.
	psoDesc.BlendState		= blenDesc;									// a default blent state.
	psoDesc.NumRenderTargets = 1;										// we are only binding one render target

	// create the pso
	hr = dx12Handle_._device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&_pso));
	if (FAILED(hr))
	{
		printf("Failing creating PSO of %s: %s\n", Name(), std::system_category().message(hr).c_str());
		return false;
	}

	return true;
}

bool DemoRayCPU::MakeTexture(const DemoInputs& inputs, const DX12Handle& dx12Handle_)
{
	HRESULT hr;

	/* create the descriptor heap that will store our srv */
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = 1;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	for (int i = 0; i < _descHeaps.size(); i++)
	{
		hr = dx12Handle_._device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&_descHeaps[i]));
	}

	if (FAILED(hr))
	{
		printf("Failing creating main descriptor heap for %s: %s\n", Name(), std::system_category().message(hr).c_str());
		return false;
	}

	/* describe the texture */
	D3D12_RESOURCE_DESC texDesc = {};
	texDesc.Dimension			= D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texDesc.Alignment			= 0;					// may be 0, 4KB, 64KB, or 4MB. 0 will let runtime decide between 64KB and 4MB (4MB for multi-sampled textures)
	texDesc.Width				= inputs.renderContext.width;	// width of the texture
	texDesc.Height				= inputs.renderContext.height;	// height of the texture
	texDesc.DepthOrArraySize	= 1;					// if 3d image, depth of 3d image. Otherwise an array of 1D or 2D textures (we only have one image, so we set 1)
	texDesc.MipLevels			= 1;					// Number of mipmaps. We are not generating mipmaps for this texture, so we have only one level
	texDesc.Format				= DXGI_FORMAT_R32G32B32A32_FLOAT; // This is the dxgi format of the image (format of the pixels)
	texDesc.SampleDesc.Count	= 1;					// This is the number of samples per pixel, we just want 1 sample
	texDesc.SampleDesc.Quality	= 0;					// The quality level of the samples. Higher is better quality, but worse performance
	texDesc.Layout				= D3D12_TEXTURE_LAYOUT_UNKNOWN; // The arrangement of the pixels. Setting to unknown lets the driver choose the most efficient one
	texDesc.Flags				= D3D12_RESOURCE_FLAG_NONE; // no flags

	width = inputs.renderContext.width;
	height = inputs.renderContext.height;

	/* create upload heap to send texture info to default */
	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_READBACK;

	D3D12_RESOURCE_DESC uploadDesc = {};

	uploadDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	uploadDesc.SampleDesc.Count = 1;
	dx12Handle_._device->GetCopyableFootprints(&texDesc, 0, 1, 0, nullptr, nullptr, nullptr, &uploadDesc.Width);
	uploadDesc.Height = 1;
	uploadDesc.DepthOrArraySize = 1;
	uploadDesc.MipLevels = 1;
	uploadDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	/* make the shader resource view from buffer and texDesc to make it available to use */
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping			= D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format							= texDesc.Format;
	srvDesc.ViewDimension					= D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.NumElements				= 1;

	for (int i = 0; i < gpuTextures.size(); i++)
	{
		hr = dx12Handle_._device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &uploadDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&gpuTextures[i].gpuTexture));

		if (FAILED(hr))
		{
			printf("Failing creating default buffer upload heap: %s\n", std::system_category().message(hr).c_str());
			return false;
		}

		dx12Handle_._device->CreateShaderResourceView(gpuTextures[i].gpuTexture, &srvDesc, _descHeaps[i]->GetCPUDescriptorHandleForHeapStart());

		gpuTextures[i].gpuTexture->Map(0, nullptr, &gpuTextures[i].mapHandle);
	}

	textureBufferSize = texDesc.Width * texDesc.Height * sizeof(GPM::vec4);
	cpuTexture = (GPM::vec4*)malloc(texDesc.Width * texDesc.Height * sizeof(GPM::vec4));

	return true;
}

/*===== RUNTIME =====*/

void DemoRayCPU::UpdateAndRender(const DemoInputs& inputs_)
{

	viewport.Width		= inputs_.renderContext.width;
	viewport.Height		= inputs_.renderContext.height;
	scissorRect.right	= inputs_.renderContext.width;
	scissorRect.bottom	= inputs_.renderContext.height;

    // Clear the render target by using the ClearRenderTargetView command
    const float clearColor[] = { 1.0f, 0.2f, 0.4f, 1.0f };
    //inputs.renderContext.currCmdList->ClearRenderTargetView(inputs.renderContext.currBackBufferHandle, clearColor, 0, nullptr);

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			cpuTexture[i * width + j] = GPM::Vec4(clearColor);
		}
	}

	///* uploading and barrier for making the application wait for the ressource to be uploaded on gpu */
	memcpy(gpuTextures[inputs_.renderContext.currFrameIndex].mapHandle, cpuTexture, textureBufferSize);

	ID3D12GraphicsCommandList4* cmdList = inputs_.renderContext.currCmdList;
	//cmdList->ClearRenderTargetView(inputs_.renderContext.currBackBufferHandle, clearColor, 0, nullptr);


	cmdList->SetGraphicsRootSignature(_rootSignature); // set the root signature
	cmdList->SetPipelineState(_pso);
	cmdList->RSSetViewports(1, &viewport); // set the viewports
	cmdList->RSSetScissorRects(1, &scissorRect); // set the scissor rects

	cmdList->SetDescriptorHeaps(1, &_descHeaps[inputs_.renderContext.currFrameIndex]); // set the descriptor heap
	// set the descriptor table to the descriptor heap
	cmdList->SetGraphicsRootDescriptorTable(0, _descHeaps[inputs_.renderContext.currFrameIndex]->GetGPUDescriptorHandleForHeapStart());

	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // set the primitive topology

	cmdList->DrawInstanced(3, 1, 0, 0); // finally draw 6 indices (draw the quad)
}