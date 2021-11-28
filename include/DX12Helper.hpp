#pragma once

#include <d3d12.h>
#include <string>

#ifdef SHADER_DEBUG
#define SHADER_FLAG (D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION)
#else
#define SHADER_FLAG 0
#endif

namespace DX12Helper
{
	
	bool CompileVertex(const std::string& shaderSource_, ID3DBlob** VS_, D3D12_SHADER_BYTECODE& shader_)
	{
        ID3DBlob* VSErr = nullptr;

        if (FAILED(D3DCompile(shaderSource_.c_str(), shaderSource_.length(), nullptr, nullptr, nullptr, "vert", "vs_5_0", SHADER_FLAG, 0, VS_, &VSErr)))
        {
            printf("Failed To Compile Vertex Shader %s\n", (const char*)(VSErr->GetBufferPointer()));
            VSErr->Release();
            return false;
        }

        shader_.pShaderBytecode = (*VS_)->GetBufferPointer();
        shader_.BytecodeLength = (*VS_)->GetBufferSize();

        return true;
	}

    bool CompilePixel(const std::string& shaderSource_, ID3DBlob** PS_, D3D12_SHADER_BYTECODE& shader_)
    {
        ID3DBlob* PSErr = nullptr;

        if (FAILED(D3DCompile(shaderSource_.c_str(), shaderSource_.length(), nullptr, nullptr, nullptr, "frag", "ps_5_0", SHADER_FLAG, 0, PS_, &PSErr)))
        {
            printf("Failed To Compile Vertex Shader %s\n", (const char*)(PSErr->GetBufferPointer()));
            PSErr->Release();
            return false;
        }

        shader_.pShaderBytecode = (*PS_)->GetBufferPointer();
        shader_.BytecodeLength = (*PS_)->GetBufferSize();

        return true;
    }
}