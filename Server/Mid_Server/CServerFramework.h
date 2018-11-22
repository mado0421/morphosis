#pragma once
#include "CPlayer.h"
class CServerFramework
{
public:
	vector <CPlayer*> m_pPlayers; 
	HANDLE gh_iocp;
	vector <thread> w_threads;
	thread a_thread;
	thread u_thread;
	static int id;

	std::chrono::system_clock::time_point start;
public:
	CServerFramework();
	~CServerFramework();
	
	void initialize();
	
	void createWorkerThread();
	void worker_thread();

	void createAcceptThread();
	void accept_thread();

	void createUpdateThread();
	void update_thread();

	void run();

	void disconnectPlayer(CPlayer* p);
	void ProcessPacket(CPlayer* p, char* ch);
	
	void startRecv(CPlayer* p);
	void sendPacket(CPlayer* p, void* ptr);
};

