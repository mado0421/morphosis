#include "stdafx.h"
#include "Player.h"
#include "AI.h"


void CMoveBehavior::Update(CPlayer * obj)
{

	XMFLOAT3 nextPos = XMFLOAT3(0,0,0);

	if (m_state == AIMoveState::GOTOCLOSESTNODE) {
		float min = FLT_MAX;
		float temp;

		for (int i = 0; i < g_vecAINode.size(); ++i) {
			temp = GetDistance(obj->GetPosition(), g_vecAINode[i]->m_xmf3Position);
			if (temp < min) {
				min = temp;
				m_targetNode = g_vecAINode[i];
			}
		}

		if (NULL == m_targetNode) return;
		nextPos = m_targetNode->m_xmf3Position;

		if (min < 10.0f) m_state = AIMoveState::GOTONEXTNODE;
	}
	else {
		nextPos = m_targetNode->next->m_xmf3Position;

		if (GetDistance(obj->GetPosition(), m_targetNode->next->m_xmf3Position) < 10.0f) {
			m_targetNode = m_targetNode->next;
			if (NULL == m_targetNode->next) {
				obj->m_AIBrain->ChangeBehavior(new CDefendBehavior());
			}
		}

	}
	nextPos.y = obj->GetPosition().y;

	obj->SetLookAt(nextPos);
	obj->MoveForwardTrigOn();

}

void CDefendBehavior::Update(CPlayer * obj)
{
}

void CAttackhavior::Update(CPlayer * obj)
{



}
