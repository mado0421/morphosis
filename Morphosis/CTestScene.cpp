#include "stdafx.h"
#include "CTestScene.h"

ID3D12RootSignature * CTestScene::CreateRootSignature(ID3D12Device * pd3dDevice)
{
	ID3D12RootSignature *pRootSignature = NULL;
	
	/**************************************************************************
	������ ���̺��� �������� �����ؾ� �ϴ� �������� ����� ��.
	���� ��� ������Ʈ���� ������, ���̴����� �� �ؽ�ó ���� ��.

	���⼭ �򰥸� �� ����.

	Root Parameter�� �� ���� ������ ������ �� ����.
	������ ���̺�(Root Descriptor Table), ��Ʈ ������(Root Descriptor), ��Ʈ ���(Root Constant)

	������ ���̺��� �����ڵ��� �迭�� �ִ� ���� ������ ���ڷ� �Ѱ��شٰ� �����ϸ� ����.
	�����ڸ� ���� ������ �����͸� �ؼ��Ϸ��� ���� ���ڸ� �а�, ���ڿ� �����ִ� �����ڵ��� �迭 �ּҷ� �� �ڿ�
	�ű⼭ �ش��ϴ� �����ڸ� ã��, �� �����ڷ� ������ �����͸� �ؼ��ϸ� ��.
	�߰��� ã�ư��� ������ ������ ������ ��Ʈ �Ķ���� ������ �ִ��� ���� ������ �̷� ����� ��� ��.

	��Ʈ �����ڴ� �����ڸ� ���ڷ� �Ѱ��ִ� ��.
	�����ڸ� ���� ������ �����͸� �ؼ��Ϸ��� ���ڷ� ���� �����ڸ� ���� �ؼ��ϸ� �ȴ�.
	�Ʒ��� ���� �� ����.

	��Ʈ ����� �׳� ���� ���ڷ� �Ѱ��ִ� ��.
	�װ� ���� �ȴ�. ��¥ �����ϰ� �ӵ��� �������� ������ �������� �ּҰ��� �ƴ� ���� ������ ä��������
	����� �� �ִ� ������ ��û���� �پ��� ������ ���� �� ���� ����.

	������ ������ ���̺� �� ���� ��Ʈ ������ �� ���� ����� �����̰�,
	������ ���̺��� ����Ϸ��� D3D12_ROOT_DESCRIPTOR_TABLE ����ü�� ���� �ȿ� ���� ä���� ��� ��.
	�״ϱ� ���� �� �� ���� ����� ������ �Ʒ� ������ ��.

	���� ������ ���̺�δ� ������Ʈ�� �ؽ�ó �� �� �����ؾ� �ϴϱ� �� �� ���������.
	**************************************************************************/
	D3D12_DESCRIPTOR_RANGE pDescriptorRange[3];

	// ù ��° ������ ���̺�(Descriptor Range)�� ������� ������(Constant Buffer Descriptor)�� ��.
	// Descriptor == View == ������ == �����͸� ��� �ؼ��� ������ ���ص� ����ü
	pDescriptorRange[0].RangeType							= D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	// Descriptor�� �� ��.
	pDescriptorRange[0].NumDescriptors						= 1;
	// HLSL���� �� ������ �����ϰ� �Ұ��� ���ϴ� ��ȣ
	// �̷��� �ϸ� ��� CBV Ÿ���̴ϱ� b(uffer)1(==BaseShaderRegister)�� ������ �� �ְ� �ȴ�.
	pDescriptorRange[0].BaseShaderRegister					= RootParameterIdx::OBJECT;
	// ���� BaseShaderRegister�� ���� �� ������ �����ϱ� ���� �߰����� ���ȣ.
	// ������ �� �ٸ� ���� �����ϱ� ���� �� ��ȣ �� ������ ��.
	pDescriptorRange[0].RegisterSpace						= 0;
	// �̰� ��Ȯ�� �� �ϴ°��� �𸣰���. �Ƹ� NumDescriptor ���� 1�� �ƴ϶� 2 �̻��� �� ���°� �ƴұ�??
	pDescriptorRange[0].OffsetInDescriptorsFromTableStart	= 0;

	// ���������� �ؽ�ó�� ���� ���̴� ���ҽ� ������(Shader Resource Descriptor)�� ���������.
	pDescriptorRange[1].RangeType							= D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pDescriptorRange[1].NumDescriptors						= 1;
	pDescriptorRange[1].BaseShaderRegister					= RootParameterIdx::TEXTURE;
	pDescriptorRange[1].RegisterSpace						= 0;
	pDescriptorRange[1].OffsetInDescriptorsFromTableStart	= 0;

	// �̰� UI�� ���ؼ� ����� ��.
	pDescriptorRange[2].RangeType							= D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	pDescriptorRange[2].NumDescriptors						= 1;
	pDescriptorRange[2].BaseShaderRegister					= RootParameterIdx::UI;
	pDescriptorRange[2].RegisterSpace						= 0;
	pDescriptorRange[2].OffsetInDescriptorsFromTableStart	= 0;

	/**************************************************************************
	������ ���̺��� ���� ���� �߰����� ������ ���� �ۼ������ϱ� ������ ������ Root Parameter�� ä���� �� �ð�.
	
	�̸� ���������� ������ ������ �� �غ���.
	���� �ǵ����̸� ����� ������ �ְ� �Ͱ�(�ϵ� ����޾Ҿ����ϱ�, ���� �ٷ� å�� ���Ͻ��̴� �κ��� ���ּ���!),
	����׸� ���ؼ� �浹�ڽ��� ������ ����ϰ� �Ͱ�(�浹�˻��� �� ����),
	�ִϸ��̼� �� ����� �� ������ �ڽ��� �ϰ� �Ͱ�(�ٵ� �Ƹ� ��.... �� �Ÿ� Ű �Է� �޾Ƽ� �޽�/���̾�������/�� �ٲ� �� �ְ�,

	�ִϸ��̼� ��ĵ� �Ѱܾ� �ϰ�, �ϴ� �����鼭 ��������.
	**************************************************************************/

	D3D12_ROOT_PARAMETER pRootParameter[RootParameterIdx::count];

	// ī�޶� ������ �Ѱ��ֱ� ���� 
	pRootParameter[RootParameterIdx::CAMERA].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pRootParameter[RootParameterIdx::CAMERA].Descriptor.ShaderRegister = RootParameterIdx::CAMERA; //b0
	pRootParameter[RootParameterIdx::CAMERA].Descriptor.RegisterSpace = 0;
	pRootParameter[RootParameterIdx::CAMERA].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	// ������Ʈ���� ��ġ(ĳ����, ����ü, ����, ���� ��)
	pRootParameter[RootParameterIdx::OBJECT].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pRootParameter[RootParameterIdx::OBJECT].DescriptorTable.NumDescriptorRanges = 1;
	pRootParameter[RootParameterIdx::OBJECT].DescriptorTable.pDescriptorRanges = &pDescriptorRange[0];
	pRootParameter[RootParameterIdx::OBJECT].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	// �ؽ�ó
	pRootParameter[RootParameterIdx::TEXTURE].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pRootParameter[RootParameterIdx::TEXTURE].DescriptorTable.NumDescriptorRanges = 1;
	pRootParameter[RootParameterIdx::TEXTURE].DescriptorTable.pDescriptorRanges = &pDescriptorRange[1];
	pRootParameter[RootParameterIdx::TEXTURE].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	// UI
	pRootParameter[RootParameterIdx::UI].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pRootParameter[RootParameterIdx::UI].DescriptorTable.NumDescriptorRanges = 1;
	pRootParameter[RootParameterIdx::UI].DescriptorTable.pDescriptorRanges = &pDescriptorRange[2];
	pRootParameter[RootParameterIdx::UI].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	// �ִϸ��̼� ��ĵ�
	pRootParameter[RootParameterIdx::ANIMATION].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pRootParameter[RootParameterIdx::ANIMATION].Descriptor.ShaderRegister = RootParameterIdx::ANIMATION;
	pRootParameter[RootParameterIdx::ANIMATION].Descriptor.RegisterSpace = 0;
	pRootParameter[RootParameterIdx::ANIMATION].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	D3D12_STATIC_SAMPLER_DESC d3dSamplerDesc;
	::ZeroMemory(&d3dSamplerDesc, sizeof(D3D12_STATIC_SAMPLER_DESC));
	d3dSamplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dSamplerDesc.MipLODBias = 0;
	d3dSamplerDesc.MaxAnisotropy = 1;
	d3dSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	d3dSamplerDesc.MinLOD = 0;
	d3dSamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	d3dSamplerDesc.ShaderRegister = 0;
	d3dSamplerDesc.RegisterSpace = 0;
	d3dSamplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = 1;
	d3dRootSignatureDesc.pStaticSamplers = &d3dSamplerDesc;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob *pd3dSignatureBlob = NULL;
	ID3DBlob *pd3dErrorBlob = NULL;
	HRESULT isSuccess = D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &pd3dSignatureBlob, &pd3dErrorBlob);
	isSuccess = pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void **)&pd3dGraphicsRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}

void CTestScene::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, void * pContext)
{
	m_pFramework				= (CFramework*)pContext;
	m_pd3dDevice				= pd3dDevice;
	m_pd3dCommandList			= pd3dCommandList;

	m_pCamera					= new CFollowCamera();
	m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	m_pd3dGraphicsRootSignature = CreateRootSignature(pd3dDevice);


}

void CTestScene::Render(ID3D12GraphicsCommandList * pd3dCommandList)
{
	//pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	//pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);
	//pd3dCommandList->SetGraphicsRootConstantBufferView(3, interpolatedMatrixResource->GetGPUVirtualAddress());



}

void CPipelineState::CreatePipelineStateDesc(ID3D12Device * pd3dDevice, ID3D12RootSignature * pd3dGraphicsRootSignature)
{
	ID3DBlob *pd3dVertexShaderBlob = NULL, *pd3dPixelShaderBlob = NULL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
	::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	d3dPipelineStateDesc.pRootSignature			= pd3dGraphicsRootSignature;

	d3dPipelineStateDesc.VS						= CreateVertexShader(&pd3dVertexShaderBlob);
	d3dPipelineStateDesc.PS						= CreatePixelShader(&pd3dPixelShaderBlob);

	d3dPipelineStateDesc.RasterizerState		= CreateRasterizerState();
	d3dPipelineStateDesc.BlendState				= CreateBlendState();
	d3dPipelineStateDesc.DepthStencilState		= CreateDepthStencilState();
	d3dPipelineStateDesc.InputLayout			= CreateInputLayout();

	d3dPipelineStateDesc.SampleMask				= UINT_MAX;
	d3dPipelineStateDesc.PrimitiveTopologyType	= D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	d3dPipelineStateDesc.NumRenderTargets		= 1;
	d3dPipelineStateDesc.RTVFormats[0]			= DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineStateDesc.DSVFormat				= DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineStateDesc.SampleDesc.Count		= 1;
	d3dPipelineStateDesc.Flags					= D3D12_PIPELINE_STATE_FLAG_NONE;

	HRESULT hResult = pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void **)&m_pd3dPipelineState);

	if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
	if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();

	if (d3dPipelineStateDesc.InputLayout.pInputElementDescs) 
		delete[] d3dPipelineStateDesc.InputLayout.pInputElementDescs;

}

D3D12_INPUT_LAYOUT_DESC CPipelineState::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = NULL;
	d3dInputLayoutDesc.NumElements = 0;

	return(d3dInputLayoutDesc);
}

D3D12_RASTERIZER_DESC CPipelineState::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));

	d3dRasterizerDesc.FillMode				= D3D12_FILL_MODE_SOLID;
	d3dRasterizerDesc.CullMode				= D3D12_CULL_MODE_NONE;
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias				= 0;
	d3dRasterizerDesc.DepthBiasClamp		= 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias	= 0.0f;
	d3dRasterizerDesc.DepthClipEnable		= TRUE;
	d3dRasterizerDesc.MultisampleEnable		= FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount		= 0;
	d3dRasterizerDesc.ConservativeRaster	= D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return(d3dRasterizerDesc);
}

D3D12_BLEND_DESC CPipelineState::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable					= FALSE;
	d3dBlendDesc.IndependentBlendEnable					= FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable			= FALSE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable			= FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend				= D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlend				= D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOp				= D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha			= D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha			= D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha			= D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp				= D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask	= D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

D3D12_DEPTH_STENCIL_DESC CPipelineState::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));

	d3dDepthStencilDesc.DepthEnable						= TRUE;
	d3dDepthStencilDesc.DepthWriteMask					= D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc						= D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable					= FALSE;
	d3dDepthStencilDesc.StencilReadMask					= 0x00;
	d3dDepthStencilDesc.StencilWriteMask				= 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp			= D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp	= D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp			= D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc			= D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp			= D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp		= D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp			= D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc			= D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}
