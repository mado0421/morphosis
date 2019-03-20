#pragma once

// 라이브러리
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

//테스트를 위한 콘솔창
#ifdef UNICODE

#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console") 

#else

#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console") 

#endif

// Define 정의
#define TEST_

#define WIN32_LEAN_AND_MEAN
#define EPSILON						1.0e-10f
#ifdef TEST_
#define FRAME_BUFFER_WIDTH			(int)(1920   / 2.0)
#define FRAME_BUFFER_HEIGHT			(int)(1080   / 2.0)
#else
#define FRAME_BUFFER_WIDTH			1920
#define FRAME_BUFFER_HEIGHT			1080
#endif
// 헤더
#include "targetver.h"

#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <vector>
#include <list>
#include <fstream>

#include <d3d12.h>
#include "d3dx12.h"
#include <dxgi1_6.h>
#include <DirectXMath.h>	//<-XMVECTOR, XMMATRIX 등등
#include <D3Dcompiler.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

#define ASSETPATH "Assets//"

using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace std;
//using Microsoft::WRL::ComPtr;

extern UINT	gnCbvSrvDescriptorIncrementSize;

struct Int4 {
	int x, y, z, w;

	Int4() :x(-1), y(-1), z(-1), w(-1) {}
	int& operator[](int x) {
		switch (x)
		{
		case 0: return this->x;
		case 1: return this->y;
		case 2: return this->z;
		case 3: return this->w;
		}
	}
};
struct Float2 { float x, y; };
struct Float3 { float x, y, z; };
struct Float4 {
	float x, y, z, w;

	Float4() :x(0), y(0), z(0), w(0) {}
	Float4(float i) :x(i), y(i), z(i), w(i) {}

	float& operator[](int x) {
		switch (x)
		{
		case 0: return this->x;
		case 1: return this->y;
		case 2: return this->z;
		case 3: return this->w;
		}
	}
};

struct TmpVertex {
	Float3	pos;
	Int4	boneIdx;
	Float4	weight;

	TmpVertex() {}
	TmpVertex(Float3 p) :pos(p) {}
};

struct CBone {
public:
	XMMATRIX GetLocalMatrix() {
		XMMATRIX rotM = XMMatrixRotationRollPitchYaw(XMConvertToRadians(Rot.x), XMConvertToRadians(Rot.y), XMConvertToRadians(Rot.z));
		XMMATRIX trsM = XMMatrixTranslation(Pos.x, Pos.y, Pos.z);
		return XMMatrixMultiply(trsM, rotM);
	}
	void Initialize(XMFLOAT3 LclTrans, XMFLOAT3 LclRotate, CBone*parent) {

		//부모 주소 줌
		this->parent = parent;

		//toParent 구함
		MakeToParentMatrix(LclTrans, LclRotate);

		//toDresspose 구함
		MakeToDressposeMatrix();

		//toDressposeInverse 구함
		MakeToDressposeInverseMatrix();
	}
	void MakeToParentMatrix() {
		if (!parent) { XMStoreFloat4x4(&toParent, XMMatrixIdentity()); return; }
		XMFLOAT3 toParentTrans = XMFLOAT3(parent->Pos.x - Pos.x, parent->Pos.y - Pos.y, parent->Pos.z - Pos.z);
		XMFLOAT3 toParentRotate = XMFLOAT3(parent->Rot.x - Rot.x, parent->Rot.y - Rot.y, parent->Rot.z - Rot.z);

		XMStoreFloat4x4(&toParent, XMMatrixRotationRollPitchYaw(XMConvertToRadians(toParentRotate.x), XMConvertToRadians(toParentRotate.y), XMConvertToRadians(toParentRotate.z)));
		toParent._41 = toParentTrans.x;
		toParent._42 = toParentTrans.y;
		toParent._43 = toParentTrans.z;
	}
	void MakeToParentMatrix(XMFLOAT3 LclTrans, XMFLOAT3 LclRotate) {
		XMStoreFloat4x4(&toParent, XMMatrixRotationRollPitchYaw(
			XMConvertToRadians(LclRotate.x),
			XMConvertToRadians(LclRotate.y),
			XMConvertToRadians(LclRotate.z)
		));
		toParent._41 = LclTrans.x;
		toParent._42 = LclTrans.y;
		toParent._43 = LclTrans.z;
	}
	void MakeToDressposeMatrix() {
		if (!parent) XMStoreFloat4x4(&toDresspose, XMMatrixMultiply(XMLoadFloat4x4(&toParent), XMMatrixIdentity()));
		else XMStoreFloat4x4(&toDresspose, XMMatrixMultiply(XMLoadFloat4x4(&toParent), XMLoadFloat4x4(&parent->toDresspose)));
	}
	void MakeToDressposeInverseMatrix() {
		XMVECTOR det;
		det = XMMatrixDeterminant(XMLoadFloat4x4(&toDresspose));
		XMStoreFloat4x4(&toDressposeInverse, XMMatrixInverse(&det, XMLoadFloat4x4(&toDresspose)));
	}
	void MakeToWorldMatrix() {
		XMMATRIX ToWorld;
		if (!parent) ToWorld = XMMatrixMultiply(XMLoadFloat4x4(&Local), XMMatrixMultiply(XMLoadFloat4x4(&toParent), XMMatrixIdentity()));
		else ToWorld = XMMatrixMultiply(XMLoadFloat4x4(&Local), XMMatrixMultiply(XMLoadFloat4x4(&toParent), XMLoadFloat4x4(&parent->toWorld)));
		XMStoreFloat4x4(&toWorld, ToWorld);
	}
public:
	CBone() = default;
	CBone(const CBone& other) {
		parent = other.parent;
		toParent = other.toParent;
		toDresspose = other.toDresspose;
		toDressposeInverse = other.toDressposeInverse;
	}
public:
	CBone*		parent = NULL;
	int			parentIdx;
	XMFLOAT3	Pos;
	XMFLOAT3	Rot;
	XMFLOAT4X4	toParent;
	XMFLOAT4X4	toDresspose;
	XMFLOAT4X4	toDressposeInverse;
	XMFLOAT4X4	toWorld;
	XMFLOAT4X4	Local;
};

struct CKey {
	float keyTime;

	CBone ** boneList;
	int nBones;

	vector<CBone> bones;
};

struct TmpMesh {
	std::string				name;
	std::vector<TmpVertex>	controlPoints;
	std::vector<int>		polygonVertexIndex;

	TmpMesh() {}
	TmpMesh(const char* name) { this->name = name; }
};


namespace GS {
	namespace RootParameter {
		enum {
			CAMERA = 0,
			OBJECT,
			TEXTURE,
			ANIM,
			MATERIAL,
			LIGHT,

			count
		};
	}

	namespace RootDescriptor {
		enum {
			OBJECT = 0,
			TEXTURE,

			count
		};
	}

	namespace PSO {
		enum {
			DEFAULT = 0,
			ANIM,

			count
		};
	}
}

//GPU에서 알아야 하는 정보들
namespace RootParameter {
	enum {
		CAMERA = 0,
		OBJECT,
		MATERIAL,
		LIGHT,
		TEXTURE,
		VERTEX,
		UV,
		UI,
		ANIM,

		count
	};
}

//GPU에서 알아야 하는 정보들
namespace RootDescriptor {
	enum {
		OBJECT = 0,
		TEXTURE,
		UI,

		count
	};
}

namespace Scenes {
	enum {
		TITLE = 0,
		ENTERROOM,
		MATCHING,
		PLAY,
		RESULT,
		TEST,

		count	
	};
}

namespace KEY {
	enum {
		A = 65,
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		I,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,
		S,
		T,
		U,
		V,
		W,
		X,
		Y,
		Z,

		_0 = 48,
		_1,
		_2,
		_3,
		_4,
		_5,
		_6,
		_7,
		_8,
		_9
	};
}

extern ID3D12Resource *CreateBufferResource(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pData, UINT nBytes, D3D12_HEAP_TYPE d3dHeapType = D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATES d3dResourceStates = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, ID3D12Resource **ppd3dUploadBuffer = NULL);
extern ID3D12Resource *CreateTextureResourceFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, const wchar_t *pszFileName, ID3D12Resource **ppd3dUploadBuffer, D3D12_RESOURCE_STATES d3dResourceStates = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

// math
inline bool IsZero(float fValue) { return((fabsf(fValue) < EPSILON)); }
inline bool IsEqual(float fA, float fB) { return(::IsZero(fA - fB)); }
inline float InverseSqrt(float fValue) { return 1.0f / sqrtf(fValue); }
inline void Swap(float *pfS, float *pfT) { float fTemp = *pfS; *pfS = *pfT; *pfT = fTemp; }

namespace Vector3
{
	inline bool IsZero(XMFLOAT3& xmf3Vector)
	{
		if (::IsZero(xmf3Vector.x) && ::IsZero(xmf3Vector.y) && ::IsZero(xmf3Vector.z)) return(true);
		return(false);
	}

	inline XMFLOAT3 XMVectorToFloat3(XMVECTOR& xmvVector)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, xmvVector);
		return(xmf3Result);
	}

	inline XMFLOAT3 ScalarProduct(XMFLOAT3& xmf3Vector, float fScalar, bool bNormalize = true)
	{
		XMFLOAT3 xmf3Result;
		if (bNormalize)
			XMStoreFloat3(&xmf3Result, XMVector3Normalize(XMLoadFloat3(&xmf3Vector)) * fScalar);
		else
			XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector) * fScalar);
		return(xmf3Result);
	}

	inline XMFLOAT3 Add(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) + XMLoadFloat3(&xmf3Vector2));
		return(xmf3Result);
	}

	inline XMFLOAT3 Add(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2, float fScalar)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) + (XMLoadFloat3(&xmf3Vector2) * fScalar));
		return(xmf3Result);
	}

	inline XMFLOAT3 Subtract(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) - XMLoadFloat3(&xmf3Vector2));
		return(xmf3Result);
	}

	inline float DotProduct(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3Dot(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2)));
		return(xmf3Result.x);
	}

	inline XMFLOAT3 CrossProduct(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2, bool bNormalize = true)
	{
		XMFLOAT3 xmf3Result;
		if (bNormalize)
			XMStoreFloat3(&xmf3Result, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2))));
		else
			XMStoreFloat3(&xmf3Result, XMVector3Cross(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2)));
		return(xmf3Result);
	}

	inline XMFLOAT3 Normalize(XMFLOAT3& xmf3Vector)
	{
		XMFLOAT3 m_xmf3Normal;
		XMStoreFloat3(&m_xmf3Normal, XMVector3Normalize(XMLoadFloat3(&xmf3Vector)));
		return(m_xmf3Normal);
	}

	inline float Length(XMFLOAT3& xmf3Vector)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3Length(XMLoadFloat3(&xmf3Vector)));
		return(xmf3Result.x);
	}

	inline float AngleV(XMVECTOR& xmvVector1, XMVECTOR& xmvVector2)
	{
		XMVECTOR xmvAngle = XMVector3AngleBetweenVectors(xmvVector1, xmvVector2);
		return(XMConvertToDegrees(acosf(XMVectorGetX(xmvAngle))));
	}

	inline float Angle(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2)
	{
		XMVECTOR a = XMLoadFloat3(&xmf3Vector1);
		XMVECTOR b = XMLoadFloat3(&xmf3Vector2);
		return(AngleV(a, b));
	}

	inline XMFLOAT3 TransformNormal(XMFLOAT3& xmf3Vector, XMMATRIX& xmmtxTransform)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3TransformNormal(XMLoadFloat3(&xmf3Vector), xmmtxTransform));
		return(xmf3Result);
	}

	inline XMFLOAT3 TransformCoord(XMFLOAT3& xmf3Vector, XMMATRIX& xmmtxTransform)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3TransformCoord(XMLoadFloat3(&xmf3Vector), xmmtxTransform));
		return(xmf3Result);
	}

	inline XMFLOAT3 Rotate(XMFLOAT3& xmf3Vector, float pitch, float yaw, float roll) {
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3Rotate(XMLoadFloat3(&xmf3Vector), XMQuaternionRotationRollPitchYaw(pitch, yaw, roll)));
		return (xmf3Result);
	}

	//inline XMFLOAT3 TransformCoord(XMFLOAT3& xmf3Vector, XMFLOAT4X4& xmmtx4x4Matrix)
	//{
	//	return(TransformCoord(xmf3Vector, XMLoadFloat4x4(&xmmtx4x4Matrix)));
	//}
}

namespace Vector4
{
	inline XMFLOAT4 Add(XMFLOAT4& xmf4Vector1, XMFLOAT4& xmf4Vector2)
	{
		XMFLOAT4 xmf4Result;
		XMStoreFloat4(&xmf4Result, XMLoadFloat4(&xmf4Vector1) + XMLoadFloat4(&xmf4Vector2));
		return(xmf4Result);
	}

	inline XMFLOAT4 Multiply(XMFLOAT4& xmf4Vector1, XMFLOAT4& xmf4Vector2)
	{
		XMFLOAT4 xmf4Result;
		XMStoreFloat4(&xmf4Result, XMLoadFloat4(&xmf4Vector1) * XMLoadFloat4(&xmf4Vector2));
		return(xmf4Result);
	}

	inline XMFLOAT4 Multiply(float fScalar, XMFLOAT4& xmf4Vector)
	{
		XMFLOAT4 xmf4Result;
		XMStoreFloat4(&xmf4Result, fScalar * XMLoadFloat4(&xmf4Vector));
		return(xmf4Result);
	}
}

namespace Matrix4x4
{
	inline XMFLOAT4X4 Identity()
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixIdentity());
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4 Multiply(XMFLOAT4X4& xmmtx4x4Matrix1, XMFLOAT4X4& xmmtx4x4Matrix2)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMLoadFloat4x4(&xmmtx4x4Matrix1) * XMLoadFloat4x4(&xmmtx4x4Matrix2));
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4 Multiply(XMFLOAT4X4& xmmtx4x4Matrix1, XMMATRIX& xmmtxMatrix2)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMLoadFloat4x4(&xmmtx4x4Matrix1) * xmmtxMatrix2);
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4 Multiply(XMMATRIX& xmmtxMatrix1, XMFLOAT4X4& xmmtx4x4Matrix2)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, xmmtxMatrix1 * XMLoadFloat4x4(&xmmtx4x4Matrix2));
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4 Inverse(XMFLOAT4X4& xmmtx4x4Matrix)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixInverse(NULL, XMLoadFloat4x4(&xmmtx4x4Matrix)));
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4 Transpose(XMFLOAT4X4& xmmtx4x4Matrix)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixTranspose(XMLoadFloat4x4(&xmmtx4x4Matrix)));
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4 PerspectiveFovLH(float FovAngleY, float AspectRatio, float NearZ, float FarZ)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixPerspectiveFovLH(FovAngleY, AspectRatio, NearZ, FarZ));
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4 OrthographicLH(float ViewWidth, float ViewHeight, float NearZ, float FarZ)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixOrthographicLH(ViewWidth, ViewHeight, NearZ, FarZ));
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4 LookAtLH(XMFLOAT3& xmf3EyePosition, XMFLOAT3& xmf3LookAtPosition, XMFLOAT3& xmf3UpDirection)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixLookAtLH(XMLoadFloat3(&xmf3EyePosition), XMLoadFloat3(&xmf3LookAtPosition), XMLoadFloat3(&xmf3UpDirection)));
		return(xmmtx4x4Result);
	}
}

namespace Triangle
{
	inline bool Intersect(XMFLOAT3& xmf3RayPosition, XMFLOAT3& xmf3RayDirection, XMFLOAT3& v0, XMFLOAT3& v1, XMFLOAT3& v2, float& fHitDistance)
	{
		return(TriangleTests::Intersects(XMLoadFloat3(&xmf3RayPosition), XMLoadFloat3(&xmf3RayDirection), XMLoadFloat3(&v0), XMLoadFloat3(&v1), XMLoadFloat3(&v2), fHitDistance));
	}
}

namespace Plane
{
	inline XMFLOAT4 Normalize(XMFLOAT4& xmf4Plane)
	{
		XMFLOAT4 xmf4Result;
		XMStoreFloat4(&xmf4Result, XMPlaneNormalize(XMLoadFloat4(&xmf4Plane)));
		return(xmf4Result);
	}
}
