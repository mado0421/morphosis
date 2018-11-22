#pragma once

class CPlayer
{
public:
	SOCKET m_s;
	bool m_isconnected;
	EXOVER m_rxover;
	int m_packet_size;  // 지금 조립하고 있는 패킷의 크기
	int	m_prev_packet_size; // 지난번 recv에서 완성되지 않아서 저장해 놓은 패킷의 앞부분의 크기
	char m_packet[MAX_PACKET_SIZE];

	int key;
	
	CPlayer()
	{
		m_isconnected = false;
		ZeroMemory(&m_rxover.m_over, sizeof(WSAOVERLAPPED));
		m_rxover.m_wsabuf.buf = m_rxover.m_iobuf;
		m_rxover.m_wsabuf.len = sizeof(m_rxover.m_wsabuf.buf);
		m_rxover.event_type = EVT_RECV;
		m_prev_packet_size = 0;
	}
	~CPlayer();

	void update(float t = 0);
};

