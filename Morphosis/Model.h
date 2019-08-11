#pragma once
#include "stdafx.h"

class CMesh;
class CTexture;

class CModel
{
public:
	CModel() {
		cout << "CModel »ý¼º - " << sizeof(CModel) << "\n";

	}
	~CModel();
public:
	void Render(ID3D12GraphicsCommandList *pd3dCommandList, bool isDebug = false);

	void SetMesh(CMesh* mesh)			{ m_Mesh	= mesh;		}
	void SetTexture(CTexture* texture)	{ m_Texture = texture;	}
	void SetName(const char* name)		{ m_strName = name;		}
public:
	string			m_strName		= "";

private:
	CMesh*			m_Mesh			= NULL;
	CTexture*		m_Texture		= NULL;
};

