// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#include "Network.h"
#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <ctime>
#include <queue>
#include <vector>
#include <mutex>


#pragma comment(lib,"ws2_32")

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.

extern PlayerInfo					m_playerInfo[MAX_PLAYER];
extern std::vector<ProjectileInfo>	m_bulletList;
extern std::vector <std::mutex>		m_bulletList_lock;
extern S2CPacket					m_s2cPacketQueue[32];
extern C2SPacket					m_c2sPackets[MAX_PLAYER];
extern int							m_numPlayer;
extern int							m_roomState;

/********************************************
일단 플레이어 소켓을 받아와서 playerInfo에 넣는 함수 있어야 함.
몇 명인지 확인하는 함수 있어야 하고.
********************************************/
struct Room {
public:

	std::vector<std::thread> m_worker;

	Room() {

		/* playerIdx Init */
		for (int i = 0; i < MAX_PLAYER; ++i) m_playerInfo[i].playerIdx = i;

		/* Thread Handle Init */
		//m_hGameThread = CreateEvent(NULL, FALSE, FALSE, NULL);
		//for (int i = 0; i < MAX_PLAYER; ++i) m_hSendEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		//for (int i = 0; i < MAX_PLAYER; ++i) m_hRecvEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);

		for (int i = 0; i < MAX_PLAYER; ++i) m_worker.push_back(std::thread { RecvThread, i });
		m_worker.push_back(std::thread(UpdateThread));


	}
	~Room() {
		for (auto &th : m_worker) th.join();
	}

	static bool isFull() { return m_numPlayer >= MAX_PLAYER; }
	static bool isRoomStateReady() { return RoomState::Ready == m_roomState; }
	static bool AllPlayerReady() {
		char msg;
		int ret;
		for (int i = 0; i < m_numPlayer; ++i) {
			/* playerIdx */
			msg = i;	send(m_playerInfo[i].socket, (char*)&msg, sizeof(char), 0);

			/* check */
			msg = GameMsg::Ok;	send(m_playerInfo[i].socket, (char*)&msg, sizeof(char), 0);

			ret = recvn(m_playerInfo[i].socket, (char*)&msg, sizeof(char), 0);
			if (SOCKET_ERROR == ret) return false;
		}
		return true;
	}

	static void SendPlayerInfo(int destIdx, int srcIdx) {
		char msg;

		msg = m_playerInfo[srcIdx].playerIdx;	send( m_playerInfo[destIdx].socket, (char*)&msg, sizeof(char), 0);
		msg = m_playerInfo[srcIdx].modelType;	send( m_playerInfo[destIdx].socket, (char*)&msg, sizeof(char), 0);
		msg = m_playerInfo[srcIdx].techniqueSet;send( m_playerInfo[destIdx].socket, (char*)&msg, sizeof(char), 0);
		msg = m_playerInfo[srcIdx].weapon;		send( m_playerInfo[destIdx].socket, (char*)&msg, sizeof(char), 0);
	}

	static void SendPlayerInfo(PlayerInfo& dest, PlayerInfo& src) {
		char msg;

		msg = src.playerIdx;	send(dest.socket, (char*)&msg, sizeof(char), 0);
		msg = src.modelType;	send(dest.socket, (char*)&msg, sizeof(char), 0);
		msg = src.techniqueSet;	send(dest.socket, (char*)&msg, sizeof(char), 0);
		msg = src.weapon;		send(dest.socket, (char*)&msg, sizeof(char), 0);
	}

	static void AssignPlayer(SOCKET& socket) {
		char msg;

		/*
		여기서 필요한 것은 잠시동안 소켓을 담아둘 tempSocket과
		플레이어 인포의 내용을 바탕으로 send()를 하는 현재 SendPlayerInfo()도 고쳐야 함.
		socket대 socket 의사소통을 할 수 있는 함수를 만드는게 좋을 듯.

		생각해보니까 플레이어인포를 임시로 만들어두는 것도 좋을 듯
		만들어두고 옮기면 될 듯
		*/

		PlayerInfo tempInfo;
		tempInfo.socket = socket;

		/* 새로 접속한 애의 playerInfo */
		recvn(tempInfo.socket, (char*)&msg, sizeof(char), 0);
		tempInfo.modelType = msg;
		recvn(tempInfo.socket, (char*)&msg, sizeof(char), 0);
		tempInfo.techniqueSet = msg;
		recvn(tempInfo.socket, (char*)&msg, sizeof(char), 0);
		tempInfo.weapon = msg;

		/* 새로 접속한 애한테 플레이어 idx 보내주기 */
		msg = m_numPlayer;
		send(tempInfo.socket, (char*)&msg, sizeof(char), 0);

		tempInfo.playerIdx = m_numPlayer;

		std::cout
			<< "================================================\n"
			<< "ModelType : " << tempInfo.playerIdx			<< "\n"
			<< "ModelType : " << tempInfo.modelType			<< "\n"
			<< "Technique : " << tempInfo.techniqueSet		<< "\n"
			<< "Weapon	  : " << tempInfo.weapon			<< "\n"
			<< "================================================\n";

		/* 이전 플레이어 애들한테 새로 접속한 애 정보 보내주기 */
		for (int i = 0; i < m_numPlayer; ++i) {
			SendPlayerInfo(m_playerInfo[i], tempInfo);
		}

		/* 새로 접속한 애한테 이전 플레이어들 정보 보내주기 */
		for (int i = 0; i < m_numPlayer; ++i) {
			SendPlayerInfo(tempInfo, m_playerInfo[i]);
		}
		
		m_playerInfo[m_numPlayer++] = tempInfo;


		if (isFull()) StartGame();
	}

	static void StartGame() {
		char msg = GameMsg::RoomPrepareToStart;
		m_roomState = RoomState::Playing;

		for (int i = 0; i < m_numPlayer; ++i) {
			send(m_playerInfo[i].socket, (char*)&msg, sizeof(char), 0);
		}
	}

	static void Update(double elapsedTime) {
		Sleep(1);
		if (RoomState::Ready == m_roomState) return;

	}

	static void SendPacket(S2CPacket& packet) {
		for (int i = 0; i < MAX_PLAYER; ++i)
			send(m_playerInfo[i].socket, (char*)&packet, sizeof(S2CPacket), 0);
	}

	static void RecvPacket(int playerIdx) {
		//C2SPacket recvPacket;
		//ZeroMemory(&recvPacket, sizeof(C2SPacket));
		
		/* 없으면 하지 말아야지;; */
		if (NULL != m_playerInfo[playerIdx].socket) return;

		byte	type;
		float	dir[3];
		ZeroMemory(&type, sizeof(byte));
		recvn(m_playerInfo[playerIdx].socket, (char*)&type, sizeof(byte), 0);
		if (KeyInput::Rotate == type) {
			ZeroMemory(&dir, sizeof(float) * 3);
			recvn(m_playerInfo[playerIdx].socket, (char*)&dir, sizeof(float) * 3, 0);
		}

		//recvn(m_playerInfo[playerIdx].socket, (char*)&recvPacket, sizeof(C2SPacket), 0);

		//switch ((int)recvPacket.type)
		//{
		//case C2SType::Move:
		//	m_playerInfo[playerIdx].Velocity.x += (float)recvPacket.value[0];
		//	m_playerInfo[playerIdx].Velocity.y += (float)recvPacket.value[4];
		//	m_playerInfo[playerIdx].Velocity.z += (float)recvPacket.value[8];
		//	break;
		//case C2SType::Attack:
		//	m_bulletList_lock[playerIdx].lock();
		//	m_bulletList.emplace_back(playerIdx, m_playerInfo[playerIdx].position, m_playerInfo[playerIdx].lookVector);
		//	m_bulletList_lock[playerIdx].unlock();
		//	break;
		//case C2SType::Jump:
		//	m_playerInfo[playerIdx].Velocity.y += (float)recvPacket.value[0];
		//	break;
		//default:
		//	std::cout << "?????\n";
		//	break;
		//}

	}

	static void UpdateThread() {
		std::chrono::system_clock::time_point prevTime = std::chrono::system_clock::now();
		std::chrono::system_clock::time_point currTime;
		std::chrono::duration<double> elapsedTime;
		while (true) {
			/* 다른 커뮤니케이션 스레드에서  */

			//WaitForMultipleObjects(MAX_PLAYER, m_hRecvEvent, TRUE, INFINITE);
			currTime = std::chrono::system_clock::now();
			elapsedTime = currTime - prevTime;
			prevTime = currTime;
			/* 뭔가 처리할 준비를 함 */
			Update(elapsedTime.count());

			//std::cout << "돌아는 감\n";
			/* 패킷을 보냄 */
			//S2CPacket sendpacket;
			//ZeroMemory(&sendpacket, sizeof(S2CPacket));
			//for (int i = 0; i < 32; ++i) SendPacket(m_s2cPacketQueue[i]);

		}
	}

	static void RecvThread(int playerIdx) {
		while (true) {
			RecvPacket(playerIdx);
		}
	}
};




