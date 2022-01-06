#pragma once

#include "Camera.hpp"
#include "Demo.hpp"
#include <array>

class DX12Handle;
struct ID3D12RootSignature;

namespace DX12Helper
{
	struct ConstantResource;
}

class DemoQuad final : public Demo
{

public:

	~DemoQuad() final;
	DemoQuad(const DemoInputs& inputs_, const DX12Handle& d3dhandle_);

	void UpdateAndRender(const DemoInputs& inputs_) final;

	inline const char* Name() const final { return typeid(*this).name(); }

private:
	Camera mainCamera = {};

	ID3D12RootSignature*		_rootSignature	= nullptr;
	ID3D12PipelineState*		_pso			= nullptr;
	ID3D12Resource*				_vBuffer		= nullptr;
	ID3D12Resource*				_iBuffer		= nullptr;

	std::array<ID3D12DescriptorHeap*,			FRAME_BUFFER_COUNT> _descHeaps;
	std::array<DX12Helper::ConstantResource,	FRAME_BUFFER_COUNT>	_constantBuffers;

	ID3D12DescriptorHeap*		_mainDescriptorHeap = nullptr;
	ID3D12Resource*				_textureResource	= nullptr;

	D3D12_VERTEX_BUFFER_VIEW    _vBufferView;
	D3D12_INDEX_BUFFER_VIEW		_iBufferView;

	D3D12_VIEWPORT viewport		= {};
	D3D12_RECT     scissorRect	= {};

	bool MakeGeometry(const DX12Handle& dx12Handle_);
	bool MakeShader(D3D12_SHADER_BYTECODE& vertex, D3D12_SHADER_BYTECODE& pixel);
	bool MakePipeline(const DX12Handle& dx12Handle_, D3D12_SHADER_BYTECODE& vertex, D3D12_SHADER_BYTECODE& pixel);
	bool MakeTexture(const DX12Handle& dx12Handle_);

	void Update(const DemoInputs& inputs_);
	void Render(const DemoInputs& inputs_);
};