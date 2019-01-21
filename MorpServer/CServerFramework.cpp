#include "pch.h"
#include "CServerFramework.h"


int CServerFramework::RecvPacketFromClient(int playerIdx)
{
	int retval;
	C2SPacket recvPac;
	ZeroMemory(&recvPac, sizeof(C2SPacket));

	retval = recvn(m_room.m_playerList[playerIdx].m_socket, (char*)&recvPac, sizeof(C2SPacket), 0);
	if (SOCKET_ERROR == retval) return SOCKET_ERROR;

	switch (recvPac.type)
	{
	case PacketType::MoveForward:
		m_room.m_playerList[playerIdx].Move(PacketType::MoveForward); break;
	case PacketType::MoveBackward:
		m_room.m_playerList[playerIdx].Move(PacketType::MoveBackward); break;
	case PacketType::MoveLeft:
		m_room.m_playerList[playerIdx].Move(PacketType::MoveLeft); break;
	case PacketType::MoveRight:
		m_room.m_playerList[playerIdx].Move(PacketType::MoveRight); break;
	case PacketType::RotateXAxis:
		break;
	case PacketType::RotateYAxis:
		break;
	case PacketType::Fire:
		break;
	case PacketType::Jump:
		break;
	default: return SOCKET_ERROR; break;
	}

}

void CServerFramework::SendPacketToClient()
{
}

bool CServerFramework::PlayerDeliverToRoom(SOCKET & socket)
{
	if (m_room.m_numPlayer >= MAX_PLAYER) return false;

	m_room.PlayerArrive(socket);
}

void CServerFramework::Initialize()
{
}

void CServerFramework::StartRound()
{
	HANDLE hTmp;

	for (int i = 0; i < MAX_PLAYER; ++i) {
		m_hCommuEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
	}
	m_hGameEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	for (int i = 0; i < MAX_PLAYER; ++i) {
		hTmp = CreateThread(NULL, 0, CommunicationPlayer, (LPVOID)i, 0, NULL);
		CloseHandle(hTmp);
	}
	hTmp = CreateThread(NULL, 0, GameThread, (LPVOID)i, 0, NULL);
	CloseHandle(hTmp);
}

void CServerFramework::Update()
{
	
}

CServerFramework::CServerFramework()
{
}


CServerFramework::~CServerFramework()
{
}
