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

struct TextureSet {
	int albedoIdx = 0;
	int normalIdx = 1;
	//int AOIdx			= 1;
	//int heightIdx		= 3;
	//int roughnessIdx	= 4;
	//int emissiveIdx		= 5;
};

class CModel
{
public:
	void Render(ID3D12GraphicsCommandList *pd3dCommandList);

	void SetName(const char* str)		{ name = str;		}

	void SetMeshIdx(int idx)		{ meshIdx = idx; }
	void SetAlbedoIdx(int idx)		{ textureDesc.albedoIdx		= idx; }
	void SetNormalIdx(int idx)		{ textureDesc.normalIdx		= idx; }
	//void SetAOIdx(int idx)			{ textureDesc.AOIdx			= idx; }
	//void SetHeightIdx(int idx)		{ textureDesc.heightIdx		= idx; }
	//void SetRoughnessIdx(int idx)	{ textureDesc.roughnessIdx	= idx; }

public:
	string			name		= "";

private:
	int			meshIdx = -1;
	TextureSet	textureDesc;
};
