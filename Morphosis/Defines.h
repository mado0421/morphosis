#pragma once
#define FRAME_BUFFER_WIDTH			1920
#define FRAME_BUFFER_HEIGHT			1080
#define RESOURCE_TEXTURE2D			0x01
#define RESOURCE_TEXTURE2D_ARRAY	0x02	//[]
#define RESOURCE_TEXTURE2DARRAY		0x03
#define RESOURCE_TEXTURE_CUBE		0x04
#define RESOURCE_BUFFER				0x05

#define ASSETPATH "Assets/"
#define LASSETPATH L"Assets/"

#define g_RootParameterCamera 0
#define g_RootParameterTexture 2

extern UINT gnCbvSrvDescriptorIncrementSize;