#include"header.h"
#include"Client.h"
//--------------------------------------------------------------//
void Initialize();	//	{wsa, iocp, clients }
void Destroy();

void WorkerThread();
void AcceptThread();

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
	for (auto& c : g_clients)
	{
		c.in_connected = false;
		c.exover.event_type = false;
		c.exover.wsabuf.buf = c.exover.IOCPbuf;
		c.exover.wsabuf.len = sizeof(c.exover.IOCPbuf);
		c.packet_size = 0;
		c.prev_size = 0;
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
		g_clients[new_key].exover.event_type = true;
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
