#pragma once

class Scene {
public:
	Scene();
	~Scene();

public:
	virtual void Render(ID3D12GraphicsCommandList* m_pd3dCommandList) = 0;
	virtual void Update(float fTimeElapsed) = 0;
	virtual void ProcessInput(UCHAR * pKeysBuffer) = 0;

protected:
	ID3D12Device					*m_pd3dDevice					= NULL;
	ID3D12GraphicsCommandList		*m_pd3dCommandList				= NULL;
	ID3D12DescriptorHeap			*m_pd3dCbvSrvDescriptorHeap		= NULL;
	ID3D12RootSignature				*m_pd3dGraphicsRootSignature	= NULL;

private:

};



//class LobbyScene : public Scene {
//public:
//	virtual void Render(ID3D12GraphicsCommandList* m_pd3dCommandList);
//	virtual void Update(float fTimeElapsed);
//	virtual void ProcessInput(UCHAR * pKeysBuffer);
//};
//
//
//
//class ResultScene : public Scene {
//public:
//	virtual void Render(ID3D12GraphicsCommandList* m_pd3dCommandList);
//	virtual void Update(float fTimeElapsed);
//	virtual void ProcessInput(UCHAR * pKeysBuffer);
//};