// 시작을 위한 팁: 
//   1. [솔루션 탐색기] 창을 사용하여 파일을 추가/관리합니다.
//   2. [팀 탐색기] 창을 사용하여 소스 제어에 연결합니다.
//   3. [출력] 창을 사용하여 빌드 출력 및 기타 메시지를 확인합니다.
//   4. [오류 목록] 창을 사용하여 오류를 봅니다.
//   5. [프로젝트] > [새 항목 추가]로 이동하여 새 코드 파일을 만들거나, [프로젝트] > [기존 항목 추가]로 이동하여 기존 코드 파일을 프로젝트에 추가합니다.
//   6. 나중에 이 프로젝트를 다시 열려면 [파일] > [열기] > [프로젝트]로 이동하고 .sln 파일을 선택합니다.

#ifndef PCH_H
#define PCH_H
#include <WinSock2.h>
#include <iostream>
#include <vector>
#include <thread>
#include <queue>

#include "..\Morphosis\ConsoleApplication1\Network.h"
// TODO: 여기에 미리 컴파일하려는 헤더 추가

#endif //PCH_H

#pragma comment(lib, "ws2_32")

inline void SendPlayerInfo(PlayerInfo& pIf) {
	char msg;
	msg = pIf.modelType;	send(pIf.socket, (char*)&msg, sizeof(char), 0);
	msg = pIf.techniqueSet;	send(pIf.socket, (char*)&msg, sizeof(char), 0);
	msg = pIf.weapon;		send(pIf.socket, (char*)&msg, sizeof(char), 0);
}

inline void RecvPlayerInfo(SOCKET& socket, PlayerInfo * p) {
	char msg;
	int idx;

	recvn(socket, (char*)&msg, sizeof(char), 0);
	idx = msg;
	p[idx].playerIdx = idx;
	recvn(socket, (char*)&msg, sizeof(char), 0);	p[idx].modelType	= msg;
	recvn(socket, (char*)&msg, sizeof(char), 0);	p[idx].techniqueSet = msg;
	recvn(socket, (char*)&msg, sizeof(char), 0);	p[idx].weapon		= msg;

	std::cout
		<< "================================================\n"
		<< "ModelType : " << p[idx].playerIdx			<< "\n"
		<< "ModelType : " << p[idx].modelType			<< "\n"
		<< "Technique : " << p[idx].techniqueSet		<< "\n"
		<< "Weapon	  : " << p[idx].weapon				<< "\n"
		<< "================================================\n";
}

inline void RecvPacket() {
	C2SPacket recvPacket;
	ZeroMemory(&recvPacket, sizeof(C2SPacket));

	//recvn(m_playerInfo[playerIdx].socket, (char*)&recvPacket, sizeof(C2SPacket), 0);
	//switch ((int)recvPacket.type)
	//{
	//case C2SType::Move:
	//	break;
	//case C2SType::Jump:
	//	break;
	//default:
	//	std::cout << "?????\n";
	//	break;
	//}

}

inline void RecvThread() {
	while (true) {
		RecvPacket();
	}
}

struct TestFramework {
	std::queue<char>			m_inputToSendQueue;
	std::vector<std::thread>	m_worker;
	bool						m_isGameEnd;

	PlayerInfo					m_playerInfoArr[MAX_PLAYER];
	SOCKET						m_mySocket;
	int							m_myIdx;

	TestFramework() {}
	~TestFramework() {
		for (auto &th : m_worker) th.join();
	}

	void Initialize(SOCKET& soc) {
		/* 소켓 및 플레이어 인포 설정부 */
		m_mySocket = soc;

		PlayerInfo	tempPIf;
		char		msg;
		tempPIf.socket			= m_mySocket;
		tempPIf.modelType		= rand() % 8;
		tempPIf.techniqueSet	= rand() % 8;
		tempPIf.weapon			= rand() % 8;
		SendPlayerInfo(tempPIf);
		recvn(tempPIf.socket, (char*)&msg, sizeof(char), 0);
		m_myIdx = tempPIf.playerIdx = msg;

		m_playerInfoArr[m_myIdx] = tempPIf;
		for (int i = 0; i < m_myIdx; ++i) RecvPlayerInfo(m_mySocket, m_playerInfoArr);

		for (int i = 0; i < MAX_PLAYER - m_myIdx; ++i) {
			RecvPlayerInfo(m_mySocket, m_playerInfoArr);
		}

		/* 플레이어 정보를 다 받고 나서 스레드 생성해서 받아야 함. */
		m_worker.push_back(std::thread(RecvThread));
	}

	void ProcessKeyInput() {
		/***********************************************************************
		테스트니까 입력은 rand()로 돌리자! 중복키 처리도 해야 하니까 여러 개도 될 수 있는 사양으로 만들기
		***********************************************************************/

		/* 일단 전후좌우 이동 판정 */
		if (0 == rand() % 10)	m_inputToSendQueue.push(KeyInput::MoveForward);
		if (0 == rand() % 10)	m_inputToSendQueue.push(KeyInput::MoveBackward);
		if (0 == rand() % 10)	m_inputToSendQueue.push(KeyInput::MoveLeft);
		if (0 == rand() % 10)	m_inputToSendQueue.push(KeyInput::MoveRight);

		/* 점프 */
		if (0 == rand() % 10)	m_inputToSendQueue.push(KeyInput::Jump);

		/* 시야 회전 */
		if (0 == rand() % 2)	m_inputToSendQueue.push(KeyInput::Rotate);

		/* 일반 공격 */
		if (0 == rand() % 2)	m_inputToSendQueue.push(KeyInput::Attack0);

		/* 스킬 공격 */
		if (0 == rand() % 10)	m_inputToSendQueue.push(KeyInput::Attack1);
		if (0 == rand() % 10)	m_inputToSendQueue.push(KeyInput::Attack2);
		if (0 == rand() % 10)	m_inputToSendQueue.push(KeyInput::Attack3);

		/* 재장전 */
		if (0 == rand() % 30)	m_inputToSendQueue.push(KeyInput::Reload);
	}

	void SendPacket() {
		byte	type;
		float	dir[3];

		while (!m_inputToSendQueue.empty()) {
			ZeroMemory(&type, sizeof(byte));
			type = m_inputToSendQueue.front();
			send(m_mySocket, (char*)&type, sizeof(byte), 0);
			if (KeyInput::Rotate == type) {
				ZeroMemory(&dir, sizeof(float) * 3);
				dir[0] = rand() % 2000 * 0.001 - 1.0;
				dir[1] = rand() % 2000 * 0.001 - 1.0;
				dir[2] = rand() % 2000 * 0.001 - 1.0;
				send(m_mySocket, (char*)&dir, sizeof(float) * 3, 0);
			}
		}
	}

	void Render() {

	}

	void Frame() {
		//Update();
		while (true) {
			Render();
			ProcessKeyInput();
			SendPacket();
			if (m_isGameEnd) break;
		}
	}
};

extern TestFramework g_framework;
