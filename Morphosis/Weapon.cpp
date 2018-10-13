#include "stdafx.h"
#include "Weapon.h"


void CWeapon::Fire()
{
	if (isFireable()) {
		curCooldownTime = defaultInfo.fireRate;
		curAmmo--;
		//탄 생성하기
		//발사하는 사운드
	}
	else {
		//걸리는 사운드
	}
}

void CWeapon::Reload()
{
	if (isReloadable()) {
		curCooldownTime = defaultInfo.reloadTime;
		//재장전 하는 중이야
		isReloading = true;
		//재장전 애니메이션 재생
		//끝나면 잔탄 수를 max로 올려야 하지 않을까?
		//Update()에서 잔탄 수를 올려주는 식으로 하자
	}
}

void CWeapon::Update(float time)
{
	if (isCooldownTime()) curCooldownTime -= time;
	if (isReloadEnd()) isReloading = false;
}

void CWeapon::Init(const WeaponDescription & info)
{
	defaultInfo = info;
}

CWeapon::CWeapon()
{
}


CWeapon::~CWeapon()
{
}
