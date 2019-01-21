// pch.cpp: 미리 컴파일된 헤더에 해당하는 소스 파일입니다. 성공하려면 컴파일이 필요합니다.

#include "pch.h"

// 일반적으로 이 파일을 무시하지만 미리 컴파일된 헤더를 사용하는 경우 유지합니다.

void CPlayer::Update(float elapsedTime)
{
}

void CPlayer::Move(byte direction)
{
	/*
	이동할 방향을 저장해두는 것이 옳다?
	Update에서 움직이자.
	*/
	std::cout << "일단은 이동했음 띄워두기\n";
}

void CPlayer::Fire(int type)
{
	std::cout << type << "번째 공격을 했음~\n";
}

void CPlayer::Jump()
{
	std::cout << "점프했음~\n";
}
