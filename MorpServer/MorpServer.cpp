#include "pch.h"
#include "CServerFramework.h"

#define SERVERIP	"119.195.130.222"
#define	SERVERPORT	9000
#define BUFSIZE		512

CServerFramework g_serverFramework;

int main()
{

	int cnt = 0;
	int retval;

	//	윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	//	socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET)err_quit("socket()");

	//	bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)err_quit("bind()");

	//	listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR)err_quit("listen()");

	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	HANDLE hThread;
	int addrlen;

	g_serverFramework.Initialize();

	while (true)
	{
		while (true)
		{
			//	accept()
			if (cnt == MAX_PLAYER)break;
			addrlen = sizeof(clientaddr);
			client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
			printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
				inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
			if (client_sock == INVALID_SOCKET) {
				err_display("accept()");
				break;
			}

			g_serverFramework.PlayerDeliverToRoom(client_sock);
			if (g_serverFramework.isReadyToStart()) {
				g_serverFramework.StartRound(); 
				break;
			}
		}
		g_serverFramework.StartRound();
	}



	////	accept()
	//if (cnt == MAX_PLAYER)return 0;
	//addrlen = sizeof(clientaddr);
	//client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
	//printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
	//	inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
	//if (client_sock == INVALID_SOCKET) {
	//	err_display("accept()");
	//	return 0;
	//}

	////	데이터 통신에 사용할 변수
	////	서버와 데이터 통신
	//while (1) {
	//	static int test = 0;

	//	retval = recvn(client_sock, (char*)&test, sizeof(int), 0);
	//	if (retval == SOCKET_ERROR) {
	//		err_display("recv()");
	//		break;
	//	}
	//	::printf("[TCP 클라이언트] %d바이트를 보냈습니다.\n", retval);

	//	test++;

	//	//	데이터 받기
	//	retval = send(client_sock, (char*)&test, sizeof(int), 0);
	//	if (retval == SOCKET_ERROR) {
	//		err_display("send()");
	//		break;
	//	}
	//	else if (retval == 0)
	//		break;
	//}

	//	close_socket()
	closesocket(listen_sock);

	//	윈속 종료
	WSACleanup();
	::printf("프로그램이 종료됨.\n");
	return 0;


    std::cout << "Hello World!\n"; 
}

