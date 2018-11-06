#pragma once

inline float GetTime(__int64 int64time) {
	__int64 i64 = int64time * 0.01;
	double i64d = i64 / 30790772.f;
	return (i64d / 30.0f);
}


struct AnimCurveNode {
	
};

struct AnimCurve {

};

struct Key {
	__int64 bookmark;
	float time;
	XMFLOAT3 translation;
	XMFLOAT3 rotation;
};

struct Bone {
public:
	int FindKeyIdxByBookmark(__int64 v) {
		for (int i = 0; i < numOfKeys; ++i) {
			if (keys[i]->bookmark == v) return i;
		}
		return -1;	//failed
	}

public:
	XMFLOAT3 translation;
	XMFLOAT3 rotation;
	Bone* parent;
	Bone** sibling;
	int numOfSibling;

	XMFLOAT4X4 bindPose;
	XMFLOAT4X4 transform;
	XMFLOAT4X4 transformLink;

	int numOfKeys;
	Key **keys;

	int numOfVertice;
	int *idx;
	float *weights;
};










class CAnimationData
{
public:


public:
	CAnimationData();
	~CAnimationData();

public:
	Bone **bones;
	int numOfBones;
};

