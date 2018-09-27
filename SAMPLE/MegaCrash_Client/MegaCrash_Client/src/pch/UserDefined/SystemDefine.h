#pragma once

/* ========================================================== //
// ===================== Option setting ===================== //
// =================================== (0 : ��Ȱ�� | 1 : Ȱ��) */
#define USE_LIMIT_MAX_FPS			1	// �ִ� FPS ���� ����
#define USE_DEBUG_CONSOLE			0	// ����� �ܼ� ��� ����
#define USE_SERVER_CONNECT			1	// Ŭ�� �� ���� ���� ����
#define USE_FEATURE_LEVEL_12		0	// Feature level 12 ���
#define USE_LOADING_CACHE_SHADER	1	// hlsl ĳ�� ���� ���

// ���� ����ó�� ������� ����
constexpr UINT				SERVERPORT			{ 9000 };
constexpr UINT				BUF_SIZE			{ 4096 };
constexpr UINT				MAX_PACKET_SIZE		{ 255 };
constexpr UINT				STR_SIZE			{ 64 };

// Ŭ���̾�Ʈ ũ�� ����
//constexpr UINT				CLIENT_WIDTH		{ 1280 }; // 1920
//constexpr UINT				CLIENT_HEIGHT		{ 720 }; // 1080
constexpr UINT				CLIENT_WIDTH		{ 1600 }; // 1920
constexpr UINT				CLIENT_HEIGHT		{ 900 }; // 1080

// Ŭ���̾�Ʈ Ÿ��Ʋ ����
constexpr UINT				TITLE_MAX_LENGTH	{ 64 };
constexpr wchar_t * const	TITLESTRING			{ L"MegaCrash" };

// ����ȭ�鿡���� Ŭ���̾�Ʈ ������ǥ(left, top)
static POINT				CLIENT_LEFT_TOP_POS;

// �ִ� FPS ����
#if USE_LIMIT_MAX_FPS
constexpr float MAX_FPS{ 1.0f / 60.f };
#else
constexpr float MAX_FPS{ 0.0f };
#endif

// ���� ������Ʈ FPS ����
constexpr float		MAX_UPDATE_FPS			{ 1.0f / 15.f };

// Ŭ���̾�Ʈ ��Ŷ���� ms ����
constexpr long long SEND_MOVE_PACKET_MS		{ 20 };
constexpr long long SEND_ATK_PACKET_MS		{ 150 };

// ����� �ܼ�â ũ�� ����
constexpr size_t DEBUG_CONSOLE_WIDTH		{ 60 };
constexpr size_t DEBUG_CONSOLE_HEIGHT		{ 35 };

/* �����Ϸ� ����ȭ ����, �ѱ� (Release ��� ����׿�)
#pragma optimize("gpsy",off)
{ 
	// �ڵ��
}
#pragma optimize("gpsy",on)
*/