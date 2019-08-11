#include "stdafx.h"

#include "UI.h"

CUI::CUI()
{


	m_xmf2Size = XMFLOAT2(0, 0);
	m_xmf2Scale = XMFLOAT2(1.0f, 1.0f);
	m_MouseState = MouseState::NONE;
}

void CUI::SetRootParameter(ID3D12GraphicsCommandList * pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(g_RootParameterUI, m_d3dCbvGPUDescriptorHandle);

}

void CUI::Render(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera, bool isDebug)
{
	if (!m_IsAlive) return;
	if (!m_vecModelIdx.empty()) {
		UpdateConstantBuffer(pd3dCommandList);
		for (auto p = m_vecModelIdx.begin(); p != m_vecModelIdx.end(); ++p) {

			pd3dCommandList->SetGraphicsRootDescriptorTable(g_RootParameterUI, m_d3dCbvGPUDescriptorHandle);
			g_vecModel[(*p)]->Render(pd3dCommandList);
		}
	}
}

void CUI::Initialize(XMFLOAT2 size)
{
	m_xmf2Size = size;
}

void CUI::SetScale(XMFLOAT2 scale)
{
	m_xmf2Scale = scale;
}

const XMFLOAT2 CUI::GetSize()
{
	return XMFLOAT2(m_xmf2Size.x * m_xmf2Scale.x, m_xmf2Size.y * m_xmf2Scale.y);
}
