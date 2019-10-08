#pragma once
class CPrefab
{
public:
	CPrefab();
	~CPrefab();

	void Render(ID3D12GraphicsCommandList *pd3dCommandList);

public:
	vector<int> m_vecModelIdx;
	string name;
};

