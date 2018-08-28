#pragma once
#include "Shader.h"

class CDiffusedShader : public CShader
{
public:
	CDiffusedShader();
	~CDiffusedShader();

public:
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);
	virtual void CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual D3D12_RASTERIZER_DESC CDiffusedShader::CreateRasterizerState();
};

