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



/********************************************
일단 플레이어 소켓을 받아와서 playerInfo에 넣는 함수 있어야 함.
몇 명인지 확인하는 함수 있어야 하고.
********************************************/
struct Room {
public:
	PlayerInfo					m_playerInfo[MAX_PLAYER];
	std::vector<ProjectileInfo>	m_bulletList;
	//std::vector <std::mutex>	m_bulletList_lock;
	std::mutex					m_playerInfoMutex;
	std::mutex					m_bulletListMutex;
	int							m_numPlayer;
	int							m_roomState;
	std::vector<std::thread>	m_recvThreads;

	Room() { 
		Initialize();
	}
	Room(const Room& other) {
		Initialize();
	}

	~Room() {
		for (auto &th : m_recvThreads) th.join();
	}

	void Initialize() {
		/* playerIdx Init */
		m_numPlayer = 0;
		m_roomState = RoomState::Ready;
		for (int i = 0; i < MAX_PLAYER; ++i) m_playerInfo[i].playerIdx = i;

		/* 아니 멤버함수 내에서 되잖아 */
		for (int i = 0; i < MAX_PLAYER; ++i) m_recvThreads.push_back(std::thread([&]() { RecvThread(i); }));
		//m_worker.push_back(std::thread(UpdateThread));
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
			msg = GameMsg::Ok;	send(m_playerInfo[i].socket, (char*)&msg, sizeof(char), 0);

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

	void SendPlayerInfo(PlayerInfo& dest, PlayerInfo& src) {
		char msg;

		msg = src.playerIdx;	send(dest.socket, (char*)&msg, sizeof(char), 0);
		msg = src.modelType;	send(dest.socket, (char*)&msg, sizeof(char), 0);
		msg = src.techniqueSet;	send(dest.socket, (char*)&msg, sizeof(char), 0);
		msg = src.weapon;		send(dest.socket, (char*)&msg, sizeof(char), 0);
	}

	void AssignPlayer(SOCKET& socket) {
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

	void StartGame() {
		char msg = GameMsg::RoomPrepareToStart;
		m_roomState = RoomState::Playing;

		for (int i = 0; i < m_numPlayer; ++i) {
			send(m_playerInfo[i].socket, (char*)&msg, sizeof(char), 0);
		}
	}

	void Update(double elapsedTime) {
		//Sleep(1);
		//if (RoomState::Ready == m_roomState) return;
	}

	void TestCout() {
		for (int i = 0; i < MAX_PLAYER; ++i) {

			std::cout << "Player " << i << "\n";
			if (m_playerInfo[i].move[MoveDir::Forward]) std::cout << "Move Forward ";
			if (m_playerInfo[i].move[MoveDir::Backward])std::cout << "Move Backward ";
			if (m_playerInfo[i].move[MoveDir::Left])	std::cout << "Move Left ";
			if (m_playerInfo[i].move[MoveDir::Right])	std::cout << "Move Right ";
			if (m_playerInfo[i].move[MoveDir::Jump])	std::cout << "Jump ";

			std::cout << "\n" << "LookDir " << m_playerInfo[i].lookVector.x << ", " << m_playerInfo[i].lookVector.y << ", " << m_playerInfo[i].lookVector.z;

			for(int i = 0; i < MoveDir::count ; ++i) m_playerInfo[i].move[i] = false;
		}
	}

	void SendPacket(S2CPacket& packet) {
		for (int i = 0; i < MAX_PLAYER; ++i)
			send(m_playerInfo[i].socket, (char*)&packet, sizeof(S2CPacket), 0);
	}

	void RecvPacket(int playerIdx) {
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
			m_playerInfoMutex.lock();

			m_playerInfo[playerIdx].lookVector.x = dir[0];
			m_playerInfo[playerIdx].lookVector.y = dir[1];
			m_playerInfo[playerIdx].lookVector.z = dir[2];

			m_playerInfoMutex.unlock();
		}

		switch (type)
		{
		case KeyInput::MoveForward:	m_playerInfoMutex.lock(); m_playerInfo[playerIdx].move[MoveDir::Forward] = true;	m_playerInfoMutex.unlock(); break;
		case KeyInput::MoveBackward:m_playerInfoMutex.lock(); m_playerInfo[playerIdx].move[MoveDir::Backward] = true;	m_playerInfoMutex.unlock(); break;
		case KeyInput::MoveLeft:	m_playerInfoMutex.lock(); m_playerInfo[playerIdx].move[MoveDir::Left] = true;		m_playerInfoMutex.unlock(); break;
		case KeyInput::MoveRight:	m_playerInfoMutex.lock(); m_playerInfo[playerIdx].move[MoveDir::Right] = true;		m_playerInfoMutex.unlock(); break;
		case KeyInput::Jump:		m_playerInfoMutex.lock(); m_playerInfo[playerIdx].move[MoveDir::Jump] = true;		m_playerInfoMutex.unlock(); break;
		case KeyInput::Attack0:		m_bulletListMutex.lock(); m_bulletList.emplace_back(playerIdx, m_playerInfo[playerIdx].position, m_playerInfo[playerIdx].lookVector); m_bulletListMutex.unlock(); break;
		case KeyInput::Attack1:		m_bulletListMutex.lock(); m_bulletList.emplace_back(playerIdx, m_playerInfo[playerIdx].position, m_playerInfo[playerIdx].lookVector); m_bulletListMutex.unlock(); break;
		case KeyInput::Attack2:		m_bulletListMutex.lock(); m_bulletList.emplace_back(playerIdx, m_playerInfo[playerIdx].position, m_playerInfo[playerIdx].lookVector); m_bulletListMutex.unlock(); break;
		case KeyInput::Attack3:		m_bulletListMutex.lock(); m_bulletList.emplace_back(playerIdx, m_playerInfo[playerIdx].position, m_playerInfo[playerIdx].lookVector); m_bulletListMutex.unlock(); break;
		//case KeyInput::Reload:		m_playerInfo[playerIdx]. break;
		default:
			break;
		}


	}

	void EachFrame() {
		static std::chrono::system_clock::time_point prevTime = std::chrono::system_clock::now();
		static std::chrono::system_clock::time_point currTime;
		static std::chrono::duration<double> elapsedTime;
		//while (true) {
		currTime = std::chrono::system_clock::now();
		elapsedTime = currTime - prevTime;
		prevTime = currTime;
		/* 뭔가 처리할 준비를 함 */
		Update(elapsedTime.count());

		TestCout();

		//}
	}

	void RecvThread(int playerIdx) {
		while (true) {
			RecvPacket(playerIdx);
		}
	}
};

struct ServerFramework {
	/*******************************************************************
	처음 플레이어가 들어오면 m_enterRoomPlayers로 소켓을 보냄.
	그 플레이어가 "저 매칭할래요!" 하면 AssignPlayer해서 Room으로 넣음.
	*******************************************************************/
	std::vector<SOCKET>			m_enterRoomPlayers;
	std::vector<Room>			m_rooms;
	std::vector<std::thread>	m_updateThread;
	bool						m_isShutdown;

	ServerFramework() { Initialize(); }
	~ServerFramework() {
		for (auto &th : m_updateThread) th.join();
	}
	void Initialize() {
		m_updateThread.push_back(std::thread([&]() { Update(); }));
	}
	void Update() {
		while (true) {
			if (m_isShutdown) break;
			for (auto p = m_rooms.begin(); p != m_rooms.end(); ++p) {
				if (RoomState::Playing == p->m_roomState) p->EachFrame();
			}
		}
	}
	void EnterPlayer(SOCKET& playerSocket) {
		m_enterRoomPlayers.push_back(playerSocket);
	}
	void AssignPlayer(SOCKET& playerSocket) {
		for (auto p = m_enterRoomPlayers.begin(); p != m_enterRoomPlayers.end(); ++p) {
			if (playerSocket == (*p)) {
				Room tempRoom;
				for (auto rp = m_rooms.begin(); rp != m_rooms.end(); ++rp) {
					if (!rp->isFull()) {
						rp->AssignPlayer((*p));
						goto SERVER_ASSIGN_SUCCESS_TO_FIND_EMPTY_ROOM;
					}
				}
				m_rooms.push_back(tempRoom);
				m_rooms.back().AssignPlayer((*p));
			
			SERVER_ASSIGN_SUCCESS_TO_FIND_EMPTY_ROOM:
				break;
			}
		}
	}
};

extern ServerFramework g_serverFramework;