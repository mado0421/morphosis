#include "stdafx.h"
#include "CTestScene.h"

ID3D12RootSignature * CTestScene::CreateRootSignature(ID3D12Device * pd3dDevice)
{
	ID3D12RootSignature *pRootSignature = NULL;
	
	/**************************************************************************
	서술자 테이블은 동적으로 관리해야 하는 정보들을 담당할 것.
	예를 들면 오브젝트들의 정보나, 쉐이더에서 쓸 텍스처 정보 등.

	여기서 헷갈릴 수 있음.

	Root Parameter는 세 가지 종류로 구분할 수 있음.
	서술자 테이블(Root Descriptor Table), 루트 서술자(Root Descriptor), 루트 상수(Root Constant)

	서술자 테이블은 서술자들의 배열이 있는 곳의 정보를 인자로 넘겨준다고 생각하면 편함.
	서술자를 통해 버퍼의 데이터를 해석하려면 먼저 인자를 읽고, 인자에 적혀있는 서술자들의 배열 주소로 간 뒤에
	거기서 해당하는 서술자를 찾고, 그 서술자로 버퍼의 데이터를 해석하면 됨.
	중간에 찾아가는 과정이 길지만 한정된 루트 파라미터 공간을 최대한 많이 쓰려면 이런 방식을 써야 함.

	루트 서술자는 서술자를 인자로 넘겨주는 것.
	서술자를 통해 버퍼의 데이터를 해석하려면 인자로 받은 서술자를 통해 해석하면 된다.
	아래서 쓰게 될 예정.

	루트 상수는 그냥 값을 인자로 넘겨주는 것.
	그거 쓰면 된다. 진짜 간단하고 속도도 빠르지만 한정된 공간에서 주소값이 아닌 실제 값으로 채워버리면
	사용할 수 있는 개수가 엄청나게 줄어들기 때문에 많이 쓸 수는 없다.

	지금은 서술자 테이블 두 개와 루트 서술자 두 개를 사용할 예정이고,
	서술자 테이블을 사용하려면 D3D12_ROOT_DESCRIPTOR_TABLE 구조체를 만들어서 안에 내용 채워서 줘야 함.
	그니까 먼저 그 두 개를 만드는 과정을 아래 보여줄 것.

	지금 서술자 테이블로는 오브젝트랑 텍스처 두 개 관리해야 하니까 두 개 만들어주자.
	**************************************************************************/
	D3D12_DESCRIPTOR_RANGE pDescriptorRange[3];

	// 첫 번째 서술자 테이블(Descriptor Range)은 상수버퍼 서술자(Constant Buffer Descriptor)가 들어감.
	// Descriptor == View == 서술자 == 데이터를 어떻게 해석할 것인지 정해둔 구조체
	pDescriptorRange[0].RangeType							= D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	// Descriptor는 한 개.
	pDescriptorRange[0].NumDescriptors						= 1;
	// HLSL에서 몇 번으로 접근하게 할건지 정하는 번호
	// 이렇게 하면 얘는 CBV 타입이니까 b(uffer)1(==BaseShaderRegister)로 접근할 수 있게 된다.
	pDescriptorRange[0].BaseShaderRegister					= RootParameterIdx::OBJECT;
	// 같은 BaseShaderRegister를 줬을 때 공간을 구분하기 위한 추가적인 방번호.
	// 지금은 다 다른 층을 줬으니까 굳이 방 번호 안 가려도 됨.
	pDescriptorRange[0].RegisterSpace						= 0;
	// 이건 정확히 뭘 하는건지 모르겠음. 아마 NumDescriptor 값이 1이 아니라 2 이상일 때 쓰는게 아닐까??
	pDescriptorRange[0].OffsetInDescriptorsFromTableStart	= 0;

	// 마찬가지로 텍스처를 위한 쉐이더 리소스 서술자(Shader Resource Descriptor)도 만들어주자.
	pDescriptorRange[1].RangeType							= D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pDescriptorRange[1].NumDescriptors						= 1;
	pDescriptorRange[1].BaseShaderRegister					= RootParameterIdx::TEXTURE;
	pDescriptorRange[1].RegisterSpace						= 0;
	pDescriptorRange[1].OffsetInDescriptorsFromTableStart	= 0;

	// 이건 UI를 위해서 만들어 줌.
	pDescriptorRange[2].RangeType							= D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	pDescriptorRange[2].NumDescriptors						= 1;
	pDescriptorRange[2].BaseShaderRegister					= RootParameterIdx::UI;
	pDescriptorRange[2].RegisterSpace						= 0;
	pDescriptorRange[2].OffsetInDescriptorsFromTableStart	= 0;

	/**************************************************************************
	서술자 테이블을 쓰기 위한 추가적인 정보를 전부 작성했으니까 이제는 실제로 Root Parameter를 채워야 할 시간.
	
	미리 무엇무엇을 넣을지 생각을 좀 해보자.
	나는 되도록이면 기즈모 정보도 넣고 싶고(하도 고통받았었으니까, 지금 바로 책의 기하쉐이더 부분을 봐주세요!),
	디버그를 위해서 충돌박스를 빨갛게 출력하고도 싶고(충돌검사할 때 쓰게),
	애니메이션 본 출력할 때 기즈모랑 박스도 하고 싶고(근데 아마 음.... 할 거면 키 입력 받아서 메쉬/와이어프레임/본 바꿀 수 있게,

	애니메이션 행렬도 넘겨야 하고, 일단 적으면서 생각하자.
	**************************************************************************/

	D3D12_ROOT_PARAMETER pRootParameter[RootParameterIdx::count];

	// 카메라 정보를 넘겨주기 위해 
	pRootParameter[RootParameterIdx::CAMERA].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pRootParameter[RootParameterIdx::CAMERA].Descriptor.ShaderRegister = RootParameterIdx::CAMERA; //b0
	pRootParameter[RootParameterIdx::CAMERA].Descriptor.RegisterSpace = 0;
	pRootParameter[RootParameterIdx::CAMERA].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	// 오브젝트들의 위치(캐릭터, 투사체, 무기, 레벨 등)
	pRootParameter[RootParameterIdx::OBJECT].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pRootParameter[RootParameterIdx::OBJECT].DescriptorTable.NumDescriptorRanges = 1;
	pRootParameter[RootParameterIdx::OBJECT].DescriptorTable.pDescriptorRanges = &pDescriptorRange[0];
	pRootParameter[RootParameterIdx::OBJECT].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	// 텍스처
	pRootParameter[RootParameterIdx::TEXTURE].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pRootParameter[RootParameterIdx::TEXTURE].DescriptorTable.NumDescriptorRanges = 1;
	pRootParameter[RootParameterIdx::TEXTURE].DescriptorTable.pDescriptorRanges = &pDescriptorRange[1];
	pRootParameter[RootParameterIdx::TEXTURE].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	// UI
	pRootParameter[RootParameterIdx::UI].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pRootParameter[RootParameterIdx::UI].DescriptorTable.NumDescriptorRanges = 1;
	pRootParameter[RootParameterIdx::UI].DescriptorTable.pDescriptorRanges = &pDescriptorRange[2];
	pRootParameter[RootParameterIdx::UI].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	// 애니메이션 행렬들
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
