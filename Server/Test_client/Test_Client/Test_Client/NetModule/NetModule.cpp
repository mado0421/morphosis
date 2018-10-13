#include "../stdafx.h"
#include "NetModule.h"


NetModule::NetModule()
{
}


NetModule::~NetModule()
{
}

void NetModule::Init()
{
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);


}

void NetModule::Destroy()
{
}
