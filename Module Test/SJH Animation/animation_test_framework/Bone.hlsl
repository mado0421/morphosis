
struct KeyFrame
{
	float timePos;
	matrix m;
};



StructuredBuffer<KeyFrame>
struct Bone
{
	matrix toParent;
	matrix offset;
	////	ALL
	//int KeyFrameIdx;

	//	Partial
	KeyFrame prev;
	KeyFrame next;
};

StructuredBuffer<Bone> boneBuffer

cbuffer Time	: register(b15)
{
	float timePos;
};