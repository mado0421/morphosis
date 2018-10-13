#include"header.h"
#include"Client.h"
//--------------------------------------------------------------//
void Initialize();	//	{wsa, iocp, clients }
void Destroy();

void WorkerThread();
void AcceptThread();
//bool PacketProcess(const unsigned char* pBuf);

//--------------------------------------------------------------//
HANDLE g_iocp;
Client g_clients[MAX_CLIENT];

//--------------------------------------------------------------//
int main()
{
	Initialize();

	//	thread make
	vector<thread> work_thread;
	for (int i = 0; i < 4; ++i)work_thread.push_back(thread{ WorkerThread });

	thread accept_thread{ AcceptThread };
	
	//	thread join
	for (auto& w : work_thread)w.join();
	accept_thread.join();


	Destroy();
	return 0;
}

void Initialize()
{
	//	data init
	for (auto& c : g_clients){
		c.Init();
	}

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
		unsigned long key;
		WSAOVERLAPPED *pOver;

		bool is_success = GetQueuedCompletionStatus(g_iocp,
			&dwIOSize, &key, &pOver, INFINITE);
		cout << "GQCS from client [" << key << "] with size [" << dwIOSize << "]\n";

		//	俊矾 贸府
		if (is_success == 0) {
			cout << "Error in GQCS ket [" << key << "]\n";
			continue;
		}

		//	立加 辆丰 贸府
		if (dwIOSize == 0) {
			continue;
		}

		//	send/recv 贸府
		EXOver *o = reinterpret_cast<EXOver*>(pOver);
		if (o->event_type == EVT_RECV) {
			//	犁炼赋
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
					//ProcessPacket  prev_packet

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
	auto sock_listen = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);

	SOCKADDR_IN bind_addr;
	ZeroMemory(&bind_addr, sizeof(SOCKADDR_IN));
	bind_addr.sin_family = AF_INET;
	bind_addr.sin_port = htons(SERVER_PORT);
	bind_addr.sin_addr.s_addr = INADDR_ANY;

	bind(sock_listen, reinterpret_cast<SOCKADDR*>(&bind_addr), sizeof(SOCKADDR_IN));
	listen(sock_listen, 1000);

	while (true)
	{
		SOCKADDR_IN client_addr;
		ZeroMemory(&client_addr, sizeof(SOCKADDR_IN));
		client_addr.sin_family = AF_INET;
		client_addr.sin_port = htons(SERVER_PORT);
		client_addr.sin_addr.s_addr = INADDR_ANY;
		int client_addr_len = sizeof(SOCKADDR_IN);

		auto new_socket = WSAAccept(sock_listen,
			reinterpret_cast<SOCKADDR*>(&client_addr), &client_addr_len,
			NULL, NULL);
		cout << "accept new client\n";
		int new_key = -1;

		for (int i = 0; i < MAX_CLIENT; ++i)
		{
			if (g_clients[i].in_connected = false)
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
		g_clients[new_key].exover.event_type = EVT_RECV;
		g_clients[new_key].exover.wsabuf.buf = g_clients[new_key].exover.IOCPbuf;
		g_clients[new_key].exover.wsabuf.len = sizeof(g_clients[new_key].exover.IOCPbuf);
		ZeroMemory(&g_clients[new_key].exover.wsaover, sizeof(WSAOVERLAPPED));

		CreateIoCompletionPort(reinterpret_cast<HANDLE>(new_socket), g_iocp, new_key, 0);
		g_clients[new_key].in_connected = true;

		unsigned long flag = 0;
		int ret = WSARecv(new_socket, &g_clients[new_key].exover.wsabuf, 1,
			NULL, &flag, &g_clients[new_key].exover.wsaover, NULL);
		if (ret != 0) {
			cout << "Recv in Accept error\n";
		}
	}
}
