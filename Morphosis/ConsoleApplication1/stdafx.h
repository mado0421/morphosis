// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <WinSock2.h>
#include <iostream>
#include <ctime>
#pragma comment(lib,"ws2_32")

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.

#define MAX_PLAYER 4

// 소켓 함수 오류 출력 후 종료
inline void err_quit(const char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// 소켓 함수 오류 출력
inline void err_display(const char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

inline int recvn(SOCKET s, char *buf, int len, int flags)
{
	int received;
	char *ptr = buf;
	int left = len;

	while (left > 0)
	{
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0)
			break;
		left -= received;
		ptr += received;
	}

	return (len - left);
}


namespace GameMsg {
	enum {
		LevelIdx = 0,
		PlayerInfo,
		PlayerLeave,
		Check,
		OK,
		StartGame,

		count
	};
}

namespace RoomState {
	enum {
		Ready=0,
		Playing
	};
}

struct PlayerInfo {
	SOCKET socket = NULL;
	int playerIdx;
	int modelType;
	int techniqueSet;
	int weapon;
};


/********************************************
일단 플레이어 소켓을 받아와서 playerInfo에 넣는 함수 있어야 함.
몇 명인지 확인하는 함수 있어야 하고.
********************************************/
struct Room {
	PlayerInfo	m_playerInfo[MAX_PLAYER];
	int			m_numPlayer = 0;
	int			m_roomState = RoomState::Ready;

	Room() {
		for (int i = 0; i < MAX_PLAYER; ++i) m_playerInfo[i].playerIdx = i;
	}

	bool isFull() { return m_numPlayer >= MAX_PLAYER; }
	bool isRoomStateReady() { return RoomState::Ready == m_roomState; }
	bool AllPlayerReady() {
		char msg;
		int ret;
		for (int i = 0; i < m_numPlayer; ++i) {
			/* playerIdx */
			msg = i;	send(m_playerInfo[i].socket, (char*)&msg, sizeof(char), 0);

			/* check */
			msg = GameMsg::Check;	send(m_playerInfo[i].socket, (char*)&msg, sizeof(char), 0);

			ret = recvn(m_playerInfo[i].socket, (char*)&msg, sizeof(char), 0);
			if (SOCKET_ERROR == ret) return false;
		}
		return true;
	}

	void SendPlayerInfo(int destIdx, int srcIdx) {
		char msg;

		msg = m_playerInfo[srcIdx].playerIdx;	send( m_playerInfo[destIdx].socket, (char*)&msg, sizeof(char), 0);
		msg = m_playerInfo[srcIdx].modelType;	send( m_playerInfo[destIdx].socket, (char*)&msg, sizeof(char), 0);
		msg = m_playerInfo[srcIdx].techniqueSet;send( m_playerInfo[destIdx].socket, (char*)&msg, sizeof(char), 0);
		msg = m_playerInfo[srcIdx].weapon;		send( m_playerInfo[destIdx].socket, (char*)&msg, sizeof(char), 0);
	}

	void AssignPlayer(SOCKET& socket) {
		char msg;

		m_playerInfo[m_numPlayer].socket = socket;

		/* 새로 접속한 애의 playerInfo */
		recvn(m_playerInfo[m_numPlayer].socket, (char*)&msg, sizeof(char), 0);
		m_playerInfo[m_numPlayer].modelType = msg;
		recvn(m_playerInfo[m_numPlayer].socket, (char*)&msg, sizeof(char), 0);
		m_playerInfo[m_numPlayer].techniqueSet = msg;
		recvn(m_playerInfo[m_numPlayer].socket, (char*)&msg, sizeof(char), 0);
		m_playerInfo[m_numPlayer].weapon = msg;

		/* 새로 접속한 애한테 플레이어 idx 보내주기 */
		msg = m_numPlayer;
		send(m_playerInfo[m_numPlayer].socket, (char*)&msg, sizeof(char), 0);

		m_playerInfo[m_numPlayer].playerIdx = m_numPlayer;

		std::cout
			<< "================================================\n"
			<< "ModelType : " << m_playerInfo[m_numPlayer].playerIdx << "\n"
			<< "ModelType : " << m_playerInfo[m_numPlayer].modelType << "\n"
			<< "Technique : " << m_playerInfo[m_numPlayer].techniqueSet << "\n"
			<< "Weapon	  : " << m_playerInfo[m_numPlayer].weapon << "\n"
			<< "================================================\n";

		/* 이전 플레이어 애들한테 새로 접속한 애 정보 보내주기 */
		for (int i = 0; i < m_numPlayer; ++i) {
			SendPlayerInfo(i, m_numPlayer);
		}

		/* 새로 접속한 애한테 이전 플레이어들 정보 보내주기 */
		for (int i = 0; i < m_numPlayer; ++i) {
			SendPlayerInfo(m_numPlayer, i);
		}
		m_numPlayer++;

		if (isFull()) StartGame();
	}

	void StartGame() {
		char msg = GameMsg::StartGame;
		m_roomState = RoomState::Playing;

		for (int i = 0; i < m_numPlayer; ++i) {
			send(m_playerInfo[i].socket, (char*)&msg, sizeof(char), 0);
		}
	}

};