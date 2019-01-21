#pragma once
#include "CRoom.h"

/***************************************************************************

클래스 작성 규칙

함수를 위에 변수를 아래에
생성자, 소멸자 등은 중간에 넣기

***************************************************************************/

class CServerFramework
{
public:
	static DWORD WINAPI GameThread(LPVOID arg);
	static DWORD WINAPI CommunicationPlayer(LPVOID arg);

	static int RecvPacketFromClient(int playerIdx);
	static void SendPacketToClient();

	static bool PlayerDeliverToRoom(SOCKET& socket);

	/* 이건 생성 처음에 했을 때 */
	static void Initialize();

	/* 이건 플레이어가 전부 준비되었을 때 */
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

