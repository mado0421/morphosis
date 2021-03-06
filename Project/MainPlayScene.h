#pragma once
#include "Scene.h"

class Object;
class PlayerObject;

class MainPlayScene : public Scene {
public:
	~MainPlayScene();

public:
	virtual void Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Release();

	virtual void Render(ID3D12GraphicsCommandList* m_pd3dCommandList);
	virtual void Update(float fTimeElapsed);
	virtual void ProcessInput(UCHAR * pKeysBuffer);

protected:
	virtual void CreateConstantBufferResorceAndView();

private:
	vector<Object*>			m_vecObject;
	vector<PlayerObject*>	m_vecPlayerObject;
};