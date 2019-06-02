#pragma once

namespace PSO {
	enum {
		//CHAR = 0,
		//OBJECT,
		//UI,
		//FLEXIBLE_UI,
		//FLOATING_UI,
		//FLOATING_FLEXIBLE_UI,
		//LEVEL,
		//EFFECT,
		//PARTICLE,
		TEXTURE = 0,
		ILLUMINATEDTEXTURE,
		MODEL,
		UI,
		DEBUG,

		count
	};
}

class CPSO
{
public:
	CPSO();
	~CPSO();

public:
	virtual void Initialize(ID3D12Device * pd3dDevice, ID3D12RootSignature * pd3dGraphicsRootSignature);

	// PSO를 만드는 부분
	virtual void						CreatePipelineStateDesc(ID3D12Device * pd3dDevice, ID3D12RootSignature * pd3dGraphicsRootSignature);
	virtual D3D12_INPUT_LAYOUT_DESC		CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC		CreateRasterizerState();
	virtual D3D12_BLEND_DESC			CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC	CreateDepthStencilState();

	// PSO를 만들 때 어떤 Shader.hlsl을 사용할 지 정하는 부분들
	virtual D3D12_SHADER_BYTECODE		CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE		CreatePixelShader(ID3DBlob **ppd3dShaderBlob);
	D3D12_SHADER_BYTECODE				CompileShaderFromFile(const WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob **ppd3dShaderBlob);

	ID3D12PipelineState					*GetPipelineState();

public:
	ID3D12PipelineState * m_pd3dPipelineState = NULL;
};

class CTexturedPSO : public CPSO
{
public:
	virtual D3D12_INPUT_LAYOUT_DESC		CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE		CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE		CreatePixelShader(ID3DBlob **ppd3dShaderBlob);
};

class CTexturedIlluminatedPSO : public CPSO
{
public:
	virtual D3D12_INPUT_LAYOUT_DESC		CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE		CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE		CreatePixelShader(ID3DBlob **ppd3dShaderBlob);
};

class CModelPSO : public CPSO
{
public:
	virtual D3D12_INPUT_LAYOUT_DESC		CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE		CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE		CreatePixelShader(ID3DBlob **ppd3dShaderBlob);

};

class CDebugPSO : public CPSO
{
public:
	virtual D3D12_INPUT_LAYOUT_DESC		CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC		CreateRasterizerState();

	virtual D3D12_SHADER_BYTECODE		CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE		CreatePixelShader(ID3DBlob **ppd3dShaderBlob);
};

class CUIPSO : public CPSO
{
public:
	virtual D3D12_INPUT_LAYOUT_DESC		CreateInputLayout();
	virtual D3D12_BLEND_DESC			CreateBlendState();
	virtual D3D12_RASTERIZER_DESC		CreateRasterizerState();

	virtual D3D12_SHADER_BYTECODE		CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE		CreatePixelShader(ID3DBlob **ppd3dShaderBlob);
};

class CAnimatedPSO : public CPSO
{
public:
	virtual D3D12_INPUT_LAYOUT_DESC		CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC		CreateRasterizerState();

	virtual D3D12_SHADER_BYTECODE		CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE		CreatePixelShader(ID3DBlob **ppd3dShaderBlob);
};

class CDefaultPSO : public CPSO
{
public:
	virtual D3D12_INPUT_LAYOUT_DESC		CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE		CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE		CreatePixelShader(ID3DBlob **ppd3dShaderBlob);
};