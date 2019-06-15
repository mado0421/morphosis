#pragma once

// Define Á¤ÀÇ
#define TEST_

#ifdef TEST_
#define FRAME_BUFFER_WIDTH			(int)(1920   / 2.0)
#define FRAME_BUFFER_HEIGHT			(int)(1080   / 2.0)
#else
#define FRAME_BUFFER_WIDTH			1920
#define FRAME_BUFFER_HEIGHT			1080
#endif

#define RESOURCE_TEXTURE2D			0x01
#define RESOURCE_TEXTURE2D_ARRAY	0x02	//[]
#define RESOURCE_TEXTURE2DARRAY		0x03
#define RESOURCE_TEXTURE_CUBE		0x04
#define RESOURCE_BUFFER				0x05

#define ASSETPATH "Assets/"
#define LASSETPATH L"Assets/"

extern unsigned int	gnCbvSrvDescriptorIncrementSize;
constexpr int		g_NumAnimationBone	= 64;
constexpr auto		G					= (0.05 * 9.8);
constexpr auto		TIMER_ATT			= 0.05;
constexpr auto		TIMER_RESPANW		= 5;