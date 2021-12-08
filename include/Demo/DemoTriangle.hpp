#pragma once

#include "Camera.hpp"
#include "Demo.hpp"

class DX12Handle;
struct ID3D12RootSignature;

class DemoTriangle final : public Demo
{

public:

	~DemoTriangle() final;
	DemoTriangle(const DemoInputs& inputs_, const DX12Handle& d3dhandle_);

	void UpdateAndRender(const DemoInputs& inputs_) final;

	inline const char* Name() const final { return typeid(*this).name(); }

private:

	ID3D12RootSignature*        _rootSignature      = nullptr;
	ID3D12PipelineState*        _pso                = nullptr;
	ID3D12Resource*             _vBuffer            = nullptr;
	ID3D12Resource*             _vBufferUploadHeap  = nullptr;
	D3D12_VERTEX_BUFFER_VIEW    _vBufferView;

	D3D12_VIEWPORT viewport     = {};
	D3D12_RECT     scissorRect  = {};

	bool MakeShader(D3D12_SHADER_BYTECODE& vertex, D3D12_SHADER_BYTECODE& pixel);
	bool MakePipeline(const DX12Handle& dx12Handle_, D3D12_SHADER_BYTECODE& vertex, D3D12_SHADER_BYTECODE& pixel);
};