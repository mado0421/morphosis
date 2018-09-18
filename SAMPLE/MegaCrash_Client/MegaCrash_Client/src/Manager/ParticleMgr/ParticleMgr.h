#pragma once
#include "Object\Particle\Particle.h"

class CParticleMgr
{
	// ����ũ �˰��� �������� ����Ʈ
	shared_ptr<CAdvancedParticleBillBoards>		m_pSparkBillboard;
	// ����ũ ���� ���� ��½ ����Ʈ
	shared_ptr<CAdvancedParticleBillBoards>		m_pSparkIntenseCenterBillboard;

	// ���̽� �Ͼ� �ѱ� ���� �÷���
	shared_ptr<CAdvancedParticleBillBoards>		m_pWhiteBaseMuzzleBillboard;
	// ����� �ѱ� ���� �÷���
	shared_ptr<CAdvancedParticleBillBoards>		m_pColorMuzzleBillboard;

	// ����
	shared_ptr<CAdvancedParticleBillBoards>		m_pSmokeBillboard;

	// ��� ����ü ����
	shared_ptr<CAdvancedParticleBillBoards>		m_pRubySmokeBillboard;
	// ����� ����ü ����
	shared_ptr<CAdvancedParticleBillBoards>		m_pLegionSmokeBillboard;
	// �ԽǷ� ����ü ����
	shared_ptr<CAdvancedParticleBillBoards>		m_pEpsilonSmokeBillboard;
	// �׶�� ����ü ����
	shared_ptr<CAdvancedParticleBillBoards>		m_pGravisSmokeBillboard;

	shared_ptr<CAdvancedParticleBillBoards>		m_pBulletTrailBlastBillboard;


	// ���� ����
	shared_ptr<CAdvancedParticleBillBoards>		m_pExplodeSmokeBillboard; // Default
	shared_ptr<CAdvancedParticleBillBoards>		m_pExplodeIntenseSmokeBillboard; // Additive
	shared_ptr<CAdvancedParticleBillBoards>		m_pExplodeUpperCloudBillboard; // Additive
	shared_ptr<CAdvancedParticleBillBoards>		m_pExplodeFireBlackSmokeBillboard; // Default
	shared_ptr<CAdvancedParticleBillBoards>		m_pExplodeBackDraftCloudBillboard; // Additive
	shared_ptr<CAdvancedParticleBillBoards>		m_pExplodeSparkBillboard; // Additive
	
	shared_ptr<CAdvancedParticleBillBoards>		m_pHexagonDefaultBillboard; // Default
	shared_ptr<CAdvancedParticleBillBoards>		m_pHexagonAdditiveBillboard; // Default

public:
	CParticleMgr();
	virtual ~CParticleMgr();
	void Render(ID3D12GraphicsCommandList *pd3dCmdLst, CCamera *pCamera);
	void Animate(float fTimeElapsed);
	void ParticlesParameterSettingInitialize();
	void PopSparkParticle(const XMFLOAT3& pos);
	void PopMuzzleFlashParticle(const XMFLOAT3& pos,const XMFLOAT4& color);
	void PopSmokeParticle(const XMFLOAT3& pos);
	void PopBasicExplode(const XMFLOAT3& pos);
	
	void PopRubyBoostTrail(const XMFLOAT3& pos);
	void PopRubyBulletTrail(const XMFLOAT3& pos);
	void PopRubySpecial(const XMFLOAT3& pos);
	void PopRubyBulletExplosion(const XMFLOAT3& pos);
	void PopRubyExplode(const XMFLOAT3& pos);
	void PopRubySpecialExplosion(const XMFLOAT3& pos);

	void PopLegionBoostTrail(const XMFLOAT3& pos);
	void PopLegionBulletTrail(const XMFLOAT3& pos);
	void PopLegionSpecial(const XMFLOAT3& pos);
	void PopLegionBulletExplosion(const XMFLOAT3& pos);
	void PopLegionExplode(const XMFLOAT3& pos);
	void PopLegionSpecialExplosion(const XMFLOAT3& pos, const XMFLOAT3& attackerPos);

	void PopEpsilonBoostTrail(const XMFLOAT3& pos);
	void PopEpsilonBulletTrail(const XMFLOAT3& pos);
	void PopEpsilonSpecial(const XMFLOAT3& pos);
	void PopEpsilonSpecialBulletTrail(const XMFLOAT3& pos);
	void PopEpsilonBulletExplosion(const XMFLOAT3& pos);
	void PopEpsilonExplode(const XMFLOAT3& pos);
	void PopEpsilonSpecialExplosion(const XMFLOAT3& pos);

	void PopGravisBoostTrail(const XMFLOAT3& pos);
	void PopGravisBulletTrail(const XMFLOAT3& pos);
	void PopGravisSpecial(const XMFLOAT3& pos);
	void PopGravisBulletExplosion(const XMFLOAT3& pos);
	void PopGravisExplode(const XMFLOAT3& pos);
	void PopGravisSpecialExplosion(const XMFLOAT3& pos, const XMFLOAT3& attackerPos);


	void Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCmdLst);
};