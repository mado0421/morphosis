#pragma once

class CPlayer
{
public:
	SOCKET m_s;
	bool m_isconnected;
	EXOVER m_rxover;
	int m_packet_size;  // ���� �����ϰ� �ִ� ��Ŷ�� ũ��
	int	m_prev_packet_size; // ������ recv���� �ϼ����� �ʾƼ� ������ ���� ��Ŷ�� �պκ��� ũ��
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

