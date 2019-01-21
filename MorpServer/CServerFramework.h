#pragma once
#include "CRoom.h"

/***************************************************************************

Ŭ���� �ۼ� ��Ģ

�Լ��� ���� ������ �Ʒ���
������, �Ҹ��� ���� �߰��� �ֱ�

***************************************************************************/

class CServerFramework
{
public:
	static DWORD WINAPI GameThread(LPVOID arg);
	static DWORD WINAPI CommunicationPlayer(LPVOID arg);

	static int RecvPacketFromClient(int playerIdx);
	static void SendPacketToClient();

	static bool PlayerDeliverToRoom(SOCKET& socket);

	/* �̰� ���� ó���� ���� �� */
	static void Initialize();

	/* �̰� �÷��̾ ���� �غ�Ǿ��� �� */
	static void StartRound();

	static void Update();

	static bool isReadyToStart() { return m_room.isAllPlayerReady(); }
public:
	CServerFramework();
	~CServerFramework();

private:
	static HANDLE	m_hCommuEvent[MAX_PLAYER];
	static HANDLE	m_hGameEvent;

	static CRoom	m_room;
};

