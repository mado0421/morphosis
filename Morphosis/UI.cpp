#include "stdafx.h"

#include "UI.h"

CUI::CUI()
{
	cout << "CUI »ý¼º - " << sizeof(CUI) << "\n";


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
	if (!m_ModelList.empty()) {
		if (m_AnimationController) {
			ChangeAnimClip();
			SetAnimatedMatrix(m_AnimationTime);
		}
		UpdateConstantBuffer(pd3dCommandList);
		for (auto p = m_ModelList.begin(); p != m_ModelList.end(); ++p) {

			pd3dCommandList->SetGraphicsRootDescriptorTable(g_RootParameterUI, m_d3dCbvGPUDescriptorHandle);
			(*p)->Render(pd3dCommandList, isDebug);
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
