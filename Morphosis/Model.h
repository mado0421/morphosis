#pragma once
#include "stdafx.h"

/*********************************************************************
2019-06-11
CModel은 다음과 같이 구조를 변경한다.
 - CMesh를 하나만 가진다.
 - CTexture를 하나만 가진다.
 - CCollisionBox를 가진다.

 - 생성자에서 CMesh와 CTexture를 받는다.
 - 또는, 생성자에서 파일 이름을 받고 그걸로 CMesh와 CTexture를 만든다.
 - 생성자에서 받은 CMesh를 바탕으로 CCollisionBox를 만든다.
 - 사용할 Texture를 Set하는 함수가 필요하다.
 - 렌더링 할 함수가 필요하다.
 - 디버그용 충돌박스를 렌더링 할 함수가 필요하다.
 - 또는, 렌더링을 할 때 bool 변수를 주고 true면 디버그용 충돌박스를 렌더링한다.
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
