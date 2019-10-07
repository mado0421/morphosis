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

	void SetName(const char* name)		{ m_strName = name;		}

	void SetMeshIdx(int idx)		{ meshIdx = idx; }
	void SetAlbedoIdx(int idx)		{ textureDesc.albedoIdx		= idx; }
	void SetNormalIdx(int idx)		{ textureDesc.normalIdx		= idx; }
	//void SetAOIdx(int idx)			{ textureDesc.AOIdx			= idx; }
	//void SetHeightIdx(int idx)		{ textureDesc.heightIdx		= idx; }
	//void SetRoughnessIdx(int idx)	{ textureDesc.roughnessIdx	= idx; }

public:
	string			m_strName		= "";

private:
	int			meshIdx = -1;
	TextureSet	textureDesc;
};
