#pragma once
#include "stdafx.h"
/***********************************************************************
클래스 사양

- 오브젝트와 UI

오브젝트
\ 프리펩
\ 위치

이동하는 오브젝트
\ 이동 방향과 속력

애니메이션이 있는 오브젝트
\ 애니메이션 컨트롤러->이건 전역으로 해도 되겠는데??
\ 애니메이션 타임(float로 관리)
\ 애니메이션 상태(int로 관리)

오브젝트의 종류:
지형, 가구, 총알, 플레이어, 적 NPC, 엑세스 포인트, 트리거, 파티클
***********************************************************************/
/***********************************************************************
프리펩

모델 리스트를 가져야 함(비어있을 수 있음)
충돌체 리스트를 가져야 함(비어있을 수 있음)

모델

메쉬를 가짐
텍스처를 가짐(없을 시, 기본 텍스처 사용)

프리펩 리스트의 0번은 아무 것도 없는 것으로.
***********************************************************************/
/***********************************************************************
애니메이션

애니메이션 클립으로 관리. 어짜피 지금은 방법이 없나?

휴머노이드 애니메이션 컨트롤러(FSM)

- 아이들
- 아이들 특수행동
- 앞으로 걷기/달리기
- 왼쪽으로/오른쪽으로 걷기/달리기
- 뒤로 걷기/달리기
(파판14처럼 해버리면? 캐릭터 방향을 조절하는 식)
- 점프 시작
- 활강 중
- 점프 끝(착지 모션)
- 근접공격
- 사격

휴머노이드가 아닌 애니메이션 컨트롤러가 필요한가?
문? 흔들리는 나무? 풀? 깃발? 

***********************************************************************/

//class Mesh;
//class Texture;
//class Collider;
//
//class Model;
//
//class Prefab {
//	vector<Model>		m_vecModel;
//	vector<Collider>	m_vecCollider;
//};

struct CB_OBJECT_INFO {
	XMFLOAT4X4	xmf4x4World;
};

class Object {
public:
	void Rotate(const XMFLOAT3& angle);
	void Move(const XMFLOAT3& velocity);

public:
	/*******************************************************************
	프레임마다 호출될 함수
	*******************************************************************/
	virtual void Update(float fTime);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);
	void SetRootParameter(ID3D12GraphicsCommandList * pd3dCommandList);	// 렌더링 하기 전에 호출
	virtual void UpdateConstantBuffer(ID3D12GraphicsCommandList * pd3dCommandList); // 렌더링 하기 전에 호출


	/*******************************************************************
	세팅 함수
	*******************************************************************/
	void SetPrefabIdx(int idx) { m_idxPrefab = idx; }

	/*******************************************************************
	초기화 함수
	*******************************************************************/
	Object();
	virtual void CreateConstantBufferResource(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList);
	virtual void CreateConstantBufferView(ID3D12Device * pd3dDevice, D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorStartHandle, int& offset);
	void SetCbvGPUDescriptorHandlePtr(UINT64 nCbvGPUDescriptorHandlePtr);

protected:
	int							m_idxPrefab = 0;
	XMFLOAT4X4					m_xmf4x4World;

	D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dCbvGPUDescriptorHandle;
	ID3D12Resource*				m_pd3dCBResource;
	CB_OBJECT_INFO*				m_pCBMappedObjects;

};

class MovingObject : public Object {
protected:
	XMFLOAT3	m_xmf3LocalVelocity;
};

class AnimatedMovindObject : public MovingObject {
protected:
	///AnimationController g_AnimCtrl; // 이건 전역에서 관리.
	int			m_idxAnimState;
	float		m_fAnimTime;
};

class Terrain : public Object {
public:
	virtual void Update(float fTime);
};

class Player : public AnimatedMovindObject {
public:
	virtual void Update(float fTime);
};

class Bullet : public MovingObject {
public:
	virtual void Update(float fTime);
};
