#include "stdafx.h"
#include "Prefab.h"
#include "Model.h"


CPrefab::CPrefab()
{
}


CPrefab::~CPrefab()
{
}

void CPrefab::Render(ID3D12GraphicsCommandList * pd3dCommandList)
{
	for (int i = 0; i < m_vecModelIdx.size(); ++i) g_vecModel[m_vecModelIdx[i]]->Render(pd3dCommandList);
}
