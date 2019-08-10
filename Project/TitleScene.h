#pragma once
#include "Scene.h"

class TitleScene : public Scene {
public:
	virtual void Render(ID3D12GraphicsCommandList* m_pd3dCommandList);
	virtual void Update(float fTimeElapsed);
	virtual void ProcessInput(UCHAR * pKeysBuffer);
};