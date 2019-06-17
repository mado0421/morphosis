#pragma once

// Define 정의
#define TEST_


/*********************************************************************
2019-06-16
디바이스 관련 상수
*********************************************************************/
#ifdef TEST_
#define FRAME_BUFFER_WIDTH			(int)(1920   / 2.0)
#define FRAME_BUFFER_HEIGHT			(int)(1080   / 2.0)
#else
#define FRAME_BUFFER_WIDTH			1920
#define FRAME_BUFFER_HEIGHT			1080
#endif


/*********************************************************************
2019-06-16
루트파라미터 인덱스로 관리할 글로벌 변수
*********************************************************************/
extern int g_RootParameterCamera;
extern int g_RootParameterObject;
extern int g_RootParameterTexture;
extern int g_RootParameterAnimation;
//extern int g_RootParameterObject = 1;

/*********************************************************************
2019-06-16
텍스처 관련 상수
*********************************************************************/
#define RESOURCE_TEXTURE2D			0x01
#define RESOURCE_TEXTURE2D_ARRAY	0x02	//[]
#define RESOURCE_TEXTURE2DARRAY		0x03
#define RESOURCE_TEXTURE_CUBE		0x04
#define RESOURCE_BUFFER				0x05


/*********************************************************************
2019-06-16
파일 입출력 관련 상수
*********************************************************************/
#define ASSETPATH "Assets/"
#define LASSETPATH L"Assets/"


/*********************************************************************
2019-06-16
오브젝트 관련 상수
*********************************************************************/
extern unsigned int	gnCbvSrvDescriptorIncrementSize;
constexpr float		g_PlayerDefaultSpeed	= 128.0f;
constexpr int		g_NumAnimationBone		= 64;
constexpr int		g_NumProjectilePerPlayer= 30;
constexpr float		g_Gravity				= 9.8f;
constexpr float		TIMER_ATT				= 0.05f;
constexpr float		TIMER_RESPANW			= 5.0f;

/*********************************************************************
2019-06-16
카메라 관련 상수
*********************************************************************/
#define CAM_Y_DISTANCE 0
