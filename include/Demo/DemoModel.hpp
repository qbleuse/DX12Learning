#pragma once

#include "Camera.hpp"
#include "Demo.hpp"
#include <array>

class DX12Handle;
struct ID3D12RootSignature;

namespace DX12Helper
{
	struct ConstantResource;
	struct Model;
}

class DemoModel final : public Demo
{

public:

	~DemoModel() final;
	DemoModel(const DemoInputs& inputs_, const DX12Handle& d3dhandle_);

	void UpdateAndRender(const DemoInputs& inputs_) final;

	inline const char* Name() const final { return typeid(*this).name(); }

private:
	Camera mainCamera = {};

	ID3D12RootSignature*	_rootSignature	= nullptr;
	ID3D12PipelineState*	_pso			= nullptr;



	std::array<ID3D12DescriptorHeap*, FRAME_BUFFER_COUNT>			_descHeaps;
	std::array<DX12Helper::ConstantResource, FRAME_BUFFER_COUNT>	_constantBuffers;

	std::vector<ID3D12DescriptorHeap*>	_texDescriptorHeap;
	std::vector<ID3D12Resource*>		_textureResources;
	std::vector<ID3D12Resource*>		_vBuffers;
	
	std::vector<DX12Helper::Model>		_models;

	D3D12_VIEWPORT viewport		= {};
	D3D12_RECT     scissorRect	= {};

	bool MakeModel(const DX12Handle& dx12Handle_);
	bool MakeShader(D3D12_SHADER_BYTECODE& vertex, D3D12_SHADER_BYTECODE& pixel);
	bool MakePipeline(const DX12Handle& dx12Handle_, D3D12_SHADER_BYTECODE& vertex, D3D12_SHADER_BYTECODE& pixel);

	void InspectTransform(DX12Helper::Model& model);
	void Update(const DemoInputs& inputs_);
	void Render(const DemoInputs& inputs_);

	void DrawModel(ID3D12GraphicsCommandList4* cmdList, const DX12Helper::Model& model);
};