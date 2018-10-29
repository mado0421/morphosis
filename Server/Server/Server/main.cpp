#include"header.h"
#include"Client.h"
#include"Room.h"
#include"PacketList.h"
//--------------------------------------------------------------//
void Initialize();	//	{wsa, iocp, clients }
void Destroy();

void WorkerThread();
void AcceptThread();

void ProcessPacket(int clientKey,char* packet);
void SendPacket(int clientKey, void* packet);



void Test();
void error_display(const char *msg, int err_no);

//--------------------------------------------------------------//
HANDLE g_iocp;
Client g_clients[MAX_CLIENT];
RoomManager g_rooms;

//--------------------------------------------------------------//

int main()
{
	Initialize();

	Test();

	//	thread make
	thread accept_thread{ AcceptThread };

	vector<thread> work_thread;
	for (int i = 0; i < 4; ++i)work_thread.push_back(thread{ WorkerThread });

	
	//	thread join
	for (auto& w : work_thread)w.join();
	accept_thread.join();


	Destroy();
	return 0;
}

void Initialize()
{
	cout << "Initialize\n";

	//	client init
	for (auto& c : g_clients){
		c.Init();
	}

	//	room init
	g_rooms.Init();

	//	wsa
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);
	g_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);

}

void Destroy()
{
	WSACleanup();
}

void WorkerThread()
{
	while (true)
	{
		unsigned long dwIOSize;
		unsigned long long key;
		WSAOVERLAPPED *pOver;

		bool is_success = GetQueuedCompletionStatus(g_iocp,
			&dwIOSize, &key, &pOver, INFINITE);
		//cout << "GQCS from client [" << key << "] with size [" << dwIOSize << "]\n";

		//	에러 처리
		if (is_success == 0) {
			cout << "Error in GQCS key [" << key << "]\n";
			continue;
		}

		//	접속 종료 처리
		if (dwIOSize == 0) {
			continue;
		}

		//	send/recv 처리
		EXOver *o = reinterpret_cast<EXOver*>(pOver);
		if (o->event_type == EVT_RECV) {
			//	재조립
			int r_size = dwIOSize;
			char* ptr = o->IOCPbuf;
			while (0 < r_size)
			{
				if (g_clients[key].packet_size == 0){
					g_clients[key].packet_size = ptr[0];
				}

				int remain = g_clients[key].packet_size - g_clients[key].prev_size;
				if (remain <= r_size) {	// Complete packet
					memcpy(g_clients[key].prev_packet + g_clients[key].prev_size,
						ptr, remain);
					ProcessPacket(key, g_clients[key].prev_packet);

					r_size -= remain;
					ptr += remain;
					g_clients[key].packet_size = 0;
					g_clients[key].prev_size = 0;
				}
				else
				{	//	Incomplete packet
					memcpy(g_clients[key].prev_packet + g_clients[key].prev_size,
						ptr,
						r_size);
					g_clients[key].prev_size += r_size;
					r_size = 0;
				}
			}
			unsigned long flag = 0;
			ZeroMemory(&o->wsaover, sizeof(WSAOVERLAPPED));
			WSARecv(g_clients[key].socket, &o->wsabuf, 1, NULL,
				&flag, &o->wsaover, NULL);
		}
		else if (o->event_type == EVT_SEND) {
			delete o;
			// -q
		}
	}
}

void AcceptThread()
{
	auto sock_listen = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);

	SOCKADDR_IN bind_addr;
	ZeroMemory(&bind_addr, sizeof(SOCKADDR_IN));
	bind_addr.sin_family = AF_INET;
	bind_addr.sin_port = htons(SERVER_PORT);
	bind_addr.sin_addr.s_addr = INADDR_ANY;

	//	__stdcall bind, std::bind -> ::bind
	::bind(sock_listen, reinterpret_cast<SOCKADDR*>(&bind_addr), sizeof(SOCKADDR_IN));
	listen(sock_listen, 1000);
	cout << "listen\n";

	while (true)
	{
		SOCKADDR_IN client_addr;
		ZeroMemory(&client_addr, sizeof(SOCKADDR_IN));
		client_addr.sin_family = AF_INET;
		client_addr.sin_port = htons(SERVER_PORT);
		client_addr.sin_addr.s_addr = INADDR_ANY;
		int client_addr_len = sizeof(SOCKADDR_IN);

		auto new_socket = WSAAccept(sock_listen,
			reinterpret_cast<SOCKADDR*>(&client_addr), &client_addr_len, NULL, NULL);
		cout << "accept new client\n";
		int new_key = -1;

		for (int i = 0; i < MAX_CLIENT; ++i)
		{
			if (g_clients[i].in_connected == false)
			{
				new_key = i;
				break;
			}
		}

		if (new_key == -1) {
			cout << "full of user\n";
			continue;
		}

		// client init
		g_clients[new_key].socket = new_socket;
		g_clients[new_key].exover.event_type = EVT_RECV;
		g_clients[new_key].exover.wsabuf.buf = g_clients[new_key].exover.IOCPbuf;
		g_clients[new_key].exover.wsabuf.len = sizeof(g_clients[new_key].exover.IOCPbuf);
		ZeroMemory(&g_clients[new_key].exover.wsaover, sizeof(WSAOVERLAPPED));

		CreateIoCompletionPort(reinterpret_cast<HANDLE>(new_socket), g_iocp, new_key, 0);
		g_clients[new_key].in_connected = true;

		DWORD flag = 0;
		DWORD lpNumberOfBytesRecvd = 0;
		int ret = WSARecv(new_socket, &g_clients[new_key].exover.wsabuf, 1,
			NULL, &flag, &g_clients[new_key].exover.wsaover, NULL);
		if (ret != 0) {
			int err_no = WSAGetLastError();
			if (WSA_IO_PENDING != err_no) {
				error_display("err Recv in AcceptThread", err_no);
				cout << err_no << "\n";
			}
		}
		SC_Identify_Packet* id_packet = new SC_Identify_Packet;

		id_packet->type = SC_IDENTIFY;
		id_packet->size = sizeof(SC_Identify_Packet);
		id_packet->key = new_key;
		SendPacket(new_key, id_packet);
	}
}

void ProcessPacket(int cl, char * packet)
{
	Packet *p = reinterpret_cast<Packet*>(packet);
	TT_Packet_Echo * rp = reinterpret_cast<TT_Packet_Echo*>(packet);

	TT_Packet_Echo sp;
	sp.size = sizeof(sp);
	sp.type = TT_ECHO;
	std::tm* now;
	switch (p->type)
	{
	case CS_MatchingING:
		g_rooms.Matching(cl);
		break;
	case TT_ECHO:
		now = std::localtime(&rp->time);
		cout << now->tm_mon + 1 << " / " << now->tm_mday << " / " << now->tm_hour << ":" << now->tm_min << endl;
		sp.time = std::time(0);
		SendPacket(cl, &sp);
		break;

	case CS_MOVE:
	{
		//cout << "CSMOVE\n";
		CS_Move_Packet* t = reinterpret_cast<CS_Move_Packet*>(packet);

		SC_Move_Packet* mp = new SC_Move_Packet;
		mp->size = sizeof(SC_Move_Packet);
		mp->type = SC_MOVE;
		mp->key = cl;
		mp->x = t->x;
		mp->y = t->y;
		mp->z = t->z;
		for (int i = 0; i < MAX_CLIENT; ++i)
		{
			if (g_clients[i].in_connected == false)break;
			SendPacket(i, mp);
		}
		break;
	}
	default:
		break;
	}
	
}

void SendPacket(int clientKey, void * packet)
{
	EXOver *o = new EXOver;
	BYTE *p = reinterpret_cast<BYTE*>(packet);
	memcpy(o->IOCPbuf, packet, p[0]);
	o->event_type = EVT_SEND;
	o->wsabuf.buf = o->IOCPbuf;
	o->wsabuf.len = p[0];
	ZeroMemory(&o->wsaover, sizeof(WSAOVERLAPPED));

	int ret = WSASend(g_clients[clientKey].socket, &o->wsabuf, 1, NULL, 0, &o->wsaover, NULL);
	if (0 != ret) {
		int err_no = WSAGetLastError();
		if (WSA_IO_PENDING != err_no)
			error_display("Error in SendPacket:", err_no);
	}

	//cout << "SendPacket to Client [" << clientKey << "] Type [" << (int)p[1] << "] size [" << (int)p[0] << "]\n";
}

void Test()
{
	cout << "sizeof WTrans: " << sizeof(WTrans) << "\n";
	cout << "sizeof WTransPacket: " << sizeof(SC_WTrans_Packet) << "\n";
}

void error_display(const char *msg, int err_no)
{
	WCHAR *lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	cout << msg;
	wcout << L"에러 " << lpMsgBuf << endl;
	LocalFree(lpMsgBuf);
}