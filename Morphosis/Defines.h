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

#define ASSETPATH "Assets//"

extern unsigned int	gnCbvSrvDescriptorIncrementSize;
constexpr int		g_NumAnimationBone	= 64;
constexpr auto		G					= (0.05 * 9.8);
constexpr auto		TIMER_ATT			= 0.05;
constexpr auto		TIMER_RESPANW		= 5;