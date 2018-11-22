#include "stdafx.h"
#include "CServerFramework.h"


CServerFramework::CServerFramework()
{
}


CServerFramework::~CServerFramework()
{
}

void CServerFramework::initialize()
{
	//방, 플레이어 등 초기화 필요 있음

	//iocp 핸들 생성
	gh_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	std::wcout.imbue(std::locale("korean"));

	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	id = 0;
	start = std::chrono::system_clock::now();
}

void CServerFramework::createWorkerThread()
{
	for (int i = 0; i < PROCESSOR_NUM; ++i)
	{
		w_threads.push_back(thread{ this->worker_thread });
	}
}

void CServerFramework::worker_thread()
{
	while (true)
	{
		unsigned long io_size;
		unsigned long long iocp_key; 
		WSAOVERLAPPED *over;
		BOOL ret = GetQueuedCompletionStatus(gh_iocp, &io_size, &iocp_key, &over, INFINITE);
		CPlayer* p = (CPlayer*)iocp_key;
		if (FALSE == ret) {
			cout << "Error in GQCS\n";
			disconnectPlayer(p);
			continue;
		}
		if (0 == io_size) {
			disconnectPlayer(p);
			continue;
		}
		EXOVER *p_over = reinterpret_cast<EXOVER *>(over);
		if (EVT_RECV == p_over->event_type) {
			int work_size = io_size;
			char *wptr = p_over->m_iobuf;
			while (0 < work_size) {
				int p_size;
				//접근 방식을 만들어 줘야 함 여기부터 다시하자. 액셉트부터
				if (0 != p->m_packet_size)
					p_size = p->m_packet_size;
				else {
					p_size = wptr[0];
					p->m_packet_size = p_size;
				}
				int need_size = p_size - p->m_prev_packet_size;
				if (need_size <= work_size) {
					memcpy(p->m_packet + p->m_prev_packet_size, wptr, need_size);
					ProcessPacket(p, p->m_packet);
					p->m_prev_packet_size = 0;
					p->m_packet_size = 0;
					work_size -= need_size;
					wptr += need_size;
				}
				else
				{
					memcpy(p->m_packet + p->m_prev_packet_size, wptr, work_size);
					p->m_prev_packet_size += work_size;
					work_size = -work_size;
					wptr += work_size;
				}
			}
			startRecv(p);
		}
		else if (EVT_SEND == p_over->event_type)
		{
			delete p_over;
		}
		else {
			cout << "Unknown Event Type detected in worker thread!!\n";
		}
	}
}

void CServerFramework::createAcceptThread()
{
	a_thread = thread(this->accept_thread);
}

void CServerFramework::accept_thread()
{
	SOCKET s = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	SOCKADDR_IN bind_addr;
	ZeroMemory(&bind_addr, sizeof(SOCKADDR_IN));
	bind_addr.sin_family = AF_INET;
	bind_addr.sin_port = htons(MY_SERVER_PORT);
	bind_addr.sin_addr.s_addr = INADDR_ANY;	// 0.0.0.0  아무대서나 오는 것을 다 받겠다.

	::bind(s, reinterpret_cast<sockaddr *>(&bind_addr), sizeof(bind_addr));
	listen(s, 1000);

	while (true)
	{
		SOCKADDR_IN c_addr;
		ZeroMemory(&c_addr, sizeof(SOCKADDR_IN));
		c_addr.sin_family = AF_INET;
		c_addr.sin_port = htons(MY_SERVER_PORT);
		c_addr.sin_addr.s_addr = INADDR_ANY;	// 0.0.0.0  아무대서나 오는 것을 다 받겠다.
		int addr_size = sizeof(sockaddr);

		SOCKET cs = WSAAccept(s, reinterpret_cast<sockaddr *>(&c_addr), &addr_size, NULL, NULL);
		if (INVALID_SOCKET == cs) {
			ErrorDisplay("In Accept Thread:WSAAccept()");
			continue;
		}
		//		cout << "New Client Connected!\n";
		CPlayer* p = new CPlayer();
		p->key = id++;
		//p초기화 내용
		p->m_s = cs;
		p->m_packet_size = 0;
		p->m_prev_packet_size = 0;
		//CreateIoCompletionPort(reinterpret_cast<HANDLE>(cs), gh_iocp, id, 0);
		CreateIoCompletionPort(reinterpret_cast<HANDLE>(cs), gh_iocp, (ULONG_PTR)p, 0);
		p->m_isconnected = true;

		m_pPlayers.push_back(p);
		
		//자신의 접속 다른 플레이어에게 알리는 부분 필요


		startRecv(p);
	}
}

void CServerFramework::createUpdateThread()
{
	a_thread = thread(this->accept_thread);
}

void CServerFramework::update_thread()
{
	while (1)
	{
		//시간측정 필요
		std::chrono::duration<float> sec = std::chrono::system_clock::now() - start;
		for (auto& a : m_pPlayers)
		{
			a->update(sec.count());
		}
		//충돌체크등은 여기서 해서 전송할 필요가 있을 경우 전송(방?)
	}
}

void CServerFramework::run()
{
	createAcceptThread();
	createWorkerThread();
	createUpdateThread();
	for (auto& th : w_threads)
	{
		th.join();
	}
	a_thread.join();
	u_thread.join();
}

void CServerFramework::startRecv(CPlayer * p)
{
	unsigned long r_flag = 0;
	ZeroMemory(&p->m_rxover.m_over, sizeof(WSAOVERLAPPED));
	int ret = WSARecv(p->m_s, &p->m_rxover.m_wsabuf, 1,
		NULL, &r_flag, &p->m_rxover.m_over, NULL);
	if (0 != ret) {
		int err_no = WSAGetLastError();
		if (WSA_IO_PENDING != err_no) error_display("Recv Error", err_no);
	}
}

void CServerFramework::sendPacket(CPlayer * p, void * ptr)
{
	unsigned char *packet = reinterpret_cast<unsigned char *>(ptr);
	EXOVER *s_over = new EXOVER;
	s_over->event_type = EVT_SEND;
	memcpy(s_over->m_iobuf, packet, packet[0]);
	s_over->m_wsabuf.buf = s_over->m_iobuf;
	s_over->m_wsabuf.len = packet[0];
	ZeroMemory(&s_over->m_over, sizeof(WSAOVERLAPPED));
	int res = WSASend(p->m_s, &s_over->m_wsabuf, 1, NULL, 0,
		&s_over->m_over, NULL);
	if (0 != res) {
		int err_no = WSAGetLastError();
		if (WSA_IO_PENDING != err_no) error_display("Send Error! ", err_no);
	}
}
