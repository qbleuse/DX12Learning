/* system include */
#include <system_error>
#include <cstdio>

/* dx12 */
#include "DX12Handle.hpp"

#include "Demo/DemoRayCPU.hpp"


DemoRayCPU::~DemoRayCPU()
{
	if (gpuTexture)
		gpuTexture->Release();
	free(cpuTexture);
}

DemoRayCPU::DemoRayCPU(const DemoInputs& inputs, const DX12Handle& dx12Handle_)
{
    mainCamera.position = { 0.f, 0.f, 2.f };


}

bool DemoRayCPU::MakeTexture(const DemoInputs& inputs, const DX12Handle& dx12Handle_)
{
	HRESULT hr;

	/* describe the texture */
	D3D12_RESOURCE_DESC texDesc = {};
	texDesc.Dimension			= D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texDesc.Alignment			= 0;					// may be 0, 4KB, 64KB, or 4MB. 0 will let runtime decide between 64KB and 4MB (4MB for multi-sampled textures)
	texDesc.Width				= inputs.windowSize.x;	// width of the texture
	texDesc.Height				= inputs.windowSize.y;	// height of the texture
	texDesc.DepthOrArraySize	= 1;					// if 3d image, depth of 3d image. Otherwise an array of 1D or 2D textures (we only have one image, so we set 1)
	texDesc.MipLevels			= 1;					// Number of mipmaps. We are not generating mipmaps for this texture, so we have only one level
	texDesc.Format				= DXGI_FORMAT_R8G8B8A8_UNORM; // This is the dxgi format of the image (format of the pixels)
	texDesc.SampleDesc.Count	= 1;					// This is the number of samples per pixel, we just want 1 sample
	texDesc.SampleDesc.Quality	= 0;					// The quality level of the samples. Higher is better quality, but worse performance
	texDesc.Layout				= D3D12_TEXTURE_LAYOUT_UNKNOWN; // The arrangement of the pixels. Setting to unknown lets the driver choose the most efficient one
	texDesc.Flags				= D3D12_RESOURCE_FLAG_NONE; // no flags

	/* create upload heap to send texture info to default */
	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;

	D3D12_RESOURCE_DESC uploadDesc = {};

	uploadDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	uploadDesc.SampleDesc.Count = 1;
	dx12Handle_._device->GetCopyableFootprints(&texDesc, 0, 1, 0, nullptr, nullptr, nullptr, &uploadDesc.Width);
	uploadDesc.Height = 1;
	uploadDesc.DepthOrArraySize = 1;
	uploadDesc.MipLevels = 1;
	uploadDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	hr = dx12Handle_._device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &uploadDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&gpuTexture));

	if (FAILED(hr))
	{
		printf("Failing creating default buffer upload heap: %s\n", std::system_category().message(hr).c_str());
		return false;
	}

	

	cpuTexture = (BYTE*)malloc(texDesc.Width * texDesc.Height * 4);

	return true;
}

/*===== RUNTIME =====*/

void DemoRayCPU::UpdateAndRender(const DemoInputs& inputs)
{

    // Clear the render target by using the ClearRenderTargetView command
    //const float clearColor[] = { 1.0f, 0.2f, 0.4f, 1.0f };
    //inputs.renderContext.currCmdList->ClearRenderTargetView(inputs.renderContext.currBackBufferHandle, clearColor, 0, nullptr);

	for (int i = 0; i < inputs.windowSize.y; i++)
	{
		for (int j = 0; i < inputs.windowSize.x; j++)
		{
			cpuTexture[i * (int)inputs.windowSize.x + j] = 125;
		}
	}
	/* uploading and barrier for making the application wait for the ressource to be uploaded on gpu */
	UpdateSubresources(inputs.renderContext.currCmdList, *resourceData_.buffer, resourceData_.uploadBuffer, 0, 0, 1, &resourceData_.texData);

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = *resourceData_.buffer;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	uploader_.copyList->ResourceBarrier(1, &barrier);
}