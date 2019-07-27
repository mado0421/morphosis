#pragma once
#include "stdafx.h"

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
class CAnimationController;

class CModel
{
public:
	void Render(ID3D12GraphicsCommandList *pd3dCommandList, bool isDebug = false);

	void SetMesh(CMesh* mesh)			{ m_Mesh	= mesh;		}
	void SetTexture(CTexture* texture)	{ m_Texture = texture;	}
	void SetName(const char* name)		{ m_strName = name;		}

	void CreateConstantBufferResource(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	void UpdateConstantBuffer(ID3D12GraphicsCommandList *pd3dCommandList);
public:
	string			m_strName		= "";

private:
	CMesh*			m_Mesh			= NULL;
	CTexture*		m_Texture		= NULL;
};
