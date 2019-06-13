#pragma once
#include "stdafx.h"


//class CModel
//{
//public:
//	void AddMesh(CMesh* mesh);
//	void SetTexture(CTexture* pTexture);
//	void Render(ID3D12GraphicsCommandList *pd3dCommandList);
//	void UpdateShaderVar(ID3D12GraphicsCommandList * pd3dCommandList);
//
//public:
//	CModel();
//	~CModel();
//
//private:
//	vector<CMesh>	meshArr;
//	CMesh			**meshes = NULL;
//	int				numOfMeshes = 0;
//	CTexture		*texture = NULL;
//
//};


/*********************************************************************
2019-06-11
CModel�� ������ ���� ������ �����Ѵ�.
 - CMesh�� �ϳ��� ������.
 - CTexture�� �ϳ��� ������.
 - CCollisionBox�� ������.

 - �����ڿ��� CMesh�� CTexture�� �޴´�.
 - �Ǵ�, �����ڿ��� ���� �̸��� �ް� �װɷ� CMesh�� CTexture�� �����.
 - �����ڿ��� ���� CMesh�� �������� CCollisionBox�� �����.
 - ����� Texture�� Set�ϴ� �Լ��� �ʿ��ϴ�.
 - ������ �� �Լ��� �ʿ��ϴ�.
 - ����׿� �浹�ڽ��� ������ �� �Լ��� �ʿ��ϴ�.
 - �Ǵ�, �������� �� �� bool ������ �ְ� true�� ����׿� �浹�ڽ��� �������Ѵ�.
*********************************************************************/

class CMesh;
class CTexture;

class CCollisionBox {
public:
	void Render(ID3D12GraphicsCommandList *pd3dCommandList);
	void Update(const XMFLOAT4X4& xmf4x4World);

public:
	CCollisionBox() = delete;
	CCollisionBox(const XMFLOAT3& pos, const XMFLOAT3& extent, const XMFLOAT4& quat);

private:
	BoundingOrientedBox m_BoundingBox;
	CMesh*				m_DebugMesh;
};

class CModel
{
public:
	void Render(ID3D12GraphicsCommandList *pd3dCommandList, bool isDebug = false);

	void SetMesh(CMesh* mesh)			{ m_Mesh = mesh; }
	void SetTexture(CTexture* texture)	{ m_Texture = texture; }

private:
	CMesh*			m_Mesh			= NULL;
	CTexture*		m_Texture		= NULL;
	CCollisionBox*	m_CollisionBox	= NULL;


};
