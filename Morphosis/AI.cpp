#include "stdafx.h"
#include "Object.h"
#include "AI.h"

std::vector<AINode*>					g_vecAINode;

void CMoveBehavior::Update(CPlayer * obj)
{
	if (!g_player0Info.isDied && GetDistance(obj->GetPosition(), g_player0Info.pos) < 150) {
		obj->m_AIBrain->ChangeBehavior(new CAttackhavior());
	}

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

		if (min < 14.0f) m_state = AIMoveState::GOTONEXTNODE;
	}
	else {
		if (NULL == m_targetNode || NULL == m_targetNode->next) {
			obj->m_AIBrain->ChangeBehavior(new CDefendBehavior());
			return;
		}
		nextPos = m_targetNode->next->m_xmf3Position;

		if (GetDistance(obj->GetPosition(), m_targetNode->next->m_xmf3Position) < 14.0f) {
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
	if (!g_player0Info.isDied && GetDistance(obj->GetPosition(), g_player0Info.pos) < 150) {
		obj->m_AIBrain->ChangeBehavior(new CAttackhavior());
	}
}

void CAttackhavior::Update(CPlayer * obj)
{
	if(g_player0Info.isDied) obj->m_AIBrain->ChangeBehavior(new CMoveBehavior());
	if (GetDistance(obj->GetPosition(), g_player0Info.pos) > 150) {
		obj->m_AIBrain->ChangeBehavior(new CMoveBehavior());
	}
	XMFLOAT3 nextPos = XMFLOAT3(0, 0, 0);

	nextPos = g_player0Info.pos;
	nextPos.y = obj->GetPosition().y;

	obj->SetLookAt(nextPos);
	if (GetDistance(obj->GetPosition(), g_player0Info.pos) < 50) {
		Request temp;
		temp.playerIdx = obj->playerId;
		temp.type = RequestType::Shoot;
		g_queueRequest.push(temp);
	}
	else {
		Request temp;
		temp.playerIdx = obj->playerId;
		temp.type = RequestType::MoveForward;
		g_queueRequest.push(temp);
	}
}
