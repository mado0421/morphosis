#pragma once
#include "Mesh.h"
#include "Texture.h"
#include "Material.h"

class CModel
{
public:
	void AddMesh(CMesh* mesh);
	void SetTexture(CTexture* pTexture);
	void Render(ID3D12GraphicsCommandList *pd3dCommandList);
	void UpdateShaderVar(ID3D12GraphicsCommandList * pd3dCommandList);

public:
	CModel();
	~CModel();

private:
	vector<CMesh>	meshArr;
	CMesh			**meshes = NULL;
	int				numOfMeshes = 0;
	CTexture		*texture = NULL;

};

