﻿// 시작을 위한 팁: 
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

#include "..\Morphosis\ConsoleApplication1\Network.h"
// TODO: 여기에 미리 컴파일하려는 헤더 추가

#endif //PCH_H

#pragma comment(lib, "ws2_32")

struct PlayerInfo {
	SOCKET socket;
	int playerIdx;
	int modelType;
	int techniqueSet;
	int weapon;
};

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

	switch ((int)recvPacket.type)
	{
	case C2SType::Move:

		break;
	case C2SType::Fire:

		break;
	case C2SType::Jump:

		break;
	default:
		std::cout << "?????\n";
		break;
	}

}

inline void RecvThread() {
	while (true) {
		RecvPacket();
	}
}

