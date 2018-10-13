#include "stdafx.h"
#include "Weapon.h"


void CWeapon::Fire()
{
	if (isFireable()) {
		curCooldownTime = defaultInfo.fireRate;
		curAmmo--;
		//ź �����ϱ�
		//�߻��ϴ� ����
	}
	else {
		//�ɸ��� ����
	}
}

void CWeapon::Reload()
{
	if (isReloadable()) {
		curCooldownTime = defaultInfo.reloadTime;
		//������ �ϴ� ���̾�
		isReloading = true;
		//������ �ִϸ��̼� ���
		//������ ��ź ���� max�� �÷��� ���� ������?
		//Update()���� ��ź ���� �÷��ִ� ������ ����
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
