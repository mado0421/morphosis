#pragma once

// Define ����
#define TEST_


/*********************************************************************
2019-06-16
����̽� ���� ���
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
��Ʈ�Ķ���� �ε����� ������ �۷ι� ����
*********************************************************************/
extern int g_RootParameterCamera;
extern int g_RootParameterObject;
extern int g_RootParameterTexture;
extern int g_RootParameterAnimation;
//extern int g_RootParameterObject = 1;

/*********************************************************************
2019-06-16
�ؽ�ó ���� ���
*********************************************************************/
#define RESOURCE_TEXTURE2D			0x01
#define RESOURCE_TEXTURE2D_ARRAY	0x02	//[]
#define RESOURCE_TEXTURE2DARRAY		0x03
#define RESOURCE_TEXTURE_CUBE		0x04
#define RESOURCE_BUFFER				0x05


/*********************************************************************
2019-06-16
���� ����� ���� ���
*********************************************************************/
#define ASSETPATH "Assets/"
#define LASSETPATH L"Assets/"


/*********************************************************************
2019-06-16
������Ʈ ���� ���
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
ī�޶� ���� ���
*********************************************************************/
#define CAM_Y_DISTANCE 0
