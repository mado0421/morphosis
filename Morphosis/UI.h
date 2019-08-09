#pragma once
#include "Object.h"

class CUI : public CObject {
public:
	CUI();
public:
	virtual void SetRootParameter(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL, bool isDebug = false);

	void	Initialize(XMFLOAT2 size);
	void	SetScale(XMFLOAT2 scale);

	const XMFLOAT2 GetSize();

	bool	IsMouseEnter();
	bool	IsMouseOver();
	bool	IsMouseLeave();
	bool	IsClicked();

private:
	XMFLOAT2	m_xmf2Size;
	XMFLOAT2	m_xmf2Scale;
	MouseState	m_MouseState;
};