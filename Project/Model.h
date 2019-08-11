#pragma once

class Model {
public:
	void Render(ID3D12GraphicsCommandList *pd3dCommandList);

public:
	string	m_ModelName;

	int		m_MeshIdx;
	int		m_TextureIdx;
};