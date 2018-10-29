#include"Client.h"

void Client::Init()
{
	in_connected = false;
	exover.event_type = EVT_RECV;
	exover.wsabuf.buf = exover.IOCPbuf;
	exover.wsabuf.len = sizeof(exover.IOCPbuf);
	packet_size = 0;
	prev_size = 0;

	//	spon 정해야함
	w_matrix.Identity();
}
