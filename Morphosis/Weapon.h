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

	//잔탄이 없을 경우
	bool isZeroAmmo() { return (curAmmo <= 0); }
	bool isFullAmmo() { return (curAmmo == defaultInfo.maxAmmo); }
	bool isReloadable() { return (!isFullAmmo() && !isReloading); }
	bool isReloadEnd() { return (isReloading && !isCooldownTime()); }
	//재장전 중이거나 발사한 지 얼마 안 되었을 경우
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
	발사 시에는 발사하고 다음 발사까지의 시간
	재장전 시에는 재장전하고 완료할 때까지의 시간
	*/
	UINT curAmmo;
	float curCooldownTime;
	bool isReloading;
};

