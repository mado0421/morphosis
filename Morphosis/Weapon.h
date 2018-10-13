#pragma once

struct WeaponDescription {
	UINT maxAmmo;
	float reloadTime;
	float fireRate;
	float maxDamage;
	float minDamage;
	float incrementCriticalDamage;
	float incidenceCriticalAtt;
	float maxAccuracy;
	float range;
	float stability;
};

class CWeapon
{
public:
	void Fire();
	void Reload();
	void Update(float time);
	void Init(const WeaponDescription& Info);

	//��ź�� ���� ���
	bool isZeroAmmo() { return (curAmmo <= 0); }
	bool isFullAmmo() { return (curAmmo == defaultInfo.maxAmmo); }
	bool isReloadable() { return (!isFullAmmo() && !isReloading); }
	bool isReloadEnd() { return (isReloading && !isCooldownTime()); }
	//������ ���̰ų� �߻��� �� �� �� �Ǿ��� ���
	bool isCooldownTime() { return (curCooldownTime > 0); };
	bool isFireable() {
		if (isCooldownTime()) return false;
		if (isZeroAmmo()) return false;
		return true;
	}

public:
	CWeapon();
	~CWeapon();

private:
	WeaponDescription defaultInfo;

	/*
	�߻� �ÿ��� �߻��ϰ� ���� �߻������ �ð�
	������ �ÿ��� �������ϰ� �Ϸ��� �������� �ð�
	*/
	UINT curAmmo;
	float curCooldownTime;
	bool isReloading;
};

