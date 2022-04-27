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

class DemoScene final : public Demo
{

public:

	~DemoScene() final;
	DemoScene(const DemoInputs& inputs_, const DX12Handle& d3dhandle_);

	void UpdateAndRender(const DemoInputs& inputs_) final;

	inline const char* Name() const final { return typeid(*this).name(); }

private:
	Camera mainCamera = {};

	/* model info */
	ID3D12RootSignature* _rootSignature = nullptr;
	ID3D12PipelineState* _pso			= nullptr;

	std::array<ID3D12DescriptorHeap*, FRAME_BUFFER_COUNT>	_descHeaps;
	std::vector<DX12Helper::ConstantResource>				_constantBuffers;

	std::vector<ID3D12DescriptorHeap*>	_texDescriptorHeap;
	std::vector<ID3D12Resource*>		_textureResources;
	std::vector<ID3D12Resource*>		_vBuffers;

	std::vector<DX12Helper::Model>		_models;

	/* model info */
	ID3D12RootSignature* _skyBoxRootSignature	= nullptr;
	ID3D12PipelineState* _skyBoxPso				= nullptr;

	ID3D12DescriptorHeap*	_cubeMapDescHeap	= nullptr;
	ID3D12Resource*			_cubeMapTexture		= nullptr;

	ID3D12Resource*				_boxVertexBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW	_boxVBufferView;

	D3D12_VIEWPORT viewport		= {};
	D3D12_RECT     scissorRect	= {};

	bool MakeModel(const DX12Handle& dx12Handle_);
	bool MakeModelShader(D3D12_SHADER_BYTECODE& vertex, D3D12_SHADER_BYTECODE& pixel);
	bool MakeModelPipeline(const DX12Handle& dx12Handle_, D3D12_SHADER_BYTECODE& vertex, D3D12_SHADER_BYTECODE& pixel);

	bool MakeSkyBox(const DX12Handle& dx12Handle_);
	bool MakeSkyBoxShader(D3D12_SHADER_BYTECODE& vertex, D3D12_SHADER_BYTECODE& pixel);
	bool MakeSkyBoxPipeline(const DX12Handle& dx12Handle_, D3D12_SHADER_BYTECODE& vertex, D3D12_SHADER_BYTECODE& pixel);

	void InspectTransform(DX12Helper::Model& model);
	void Update(const DemoInputs& inputs_);
	void Render(const DemoInputs& inputs_);

	void DrawModel(ID3D12GraphicsCommandList4* cmdList, const DX12Helper::Model& model);
	void DrawSkyBox(ID3D12GraphicsCommandList4* cmdList, int frameIndex);
};