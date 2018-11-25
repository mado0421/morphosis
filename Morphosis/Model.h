#pragma once
#include "Mesh.h"
#include "Texture.h"
#include "Material.h"
#include "AnimationData.h"

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

struct TexMesh {
	CMesh		*mesh	= nullptr;
	CTexture	*tex	= nullptr;

	TexMesh(CMesh *pMesh, CTexture *pTex)
		: mesh(pMesh)
		, tex(pTex)
	{}

};

class Model {
public:	// �Լ���
	void Render(ID3D12GraphicsCommandList *pd3dCommandList);
	void AddTexMesh(CMesh *pMesh, CTexture *pTex);
	void GenerateInterpolatedAnimMatrix(float fTime);

public:	// ������ �Ҹ���

private:// ������
	
	//// mesh���� �ּ�
	//std::vector<CMesh*> m_meshAddresses;

	//// texture���� �ּ�
	//std::vector<CTexture*> m_textureAddresses;

	std::vector<TexMesh>	m_texMeshes;
	Anim					*m_pAnimData;

};