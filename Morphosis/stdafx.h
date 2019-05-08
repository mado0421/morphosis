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
#include <set>
#include <fstream>
#include <algorithm>

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

const XMFLOAT2 vector2Epsilon{ XMFLOAT2(FLT_EPSILON, FLT_EPSILON) };
const XMFLOAT3 vector3Epsilon{ XMFLOAT3(FLT_EPSILON, FLT_EPSILON, FLT_EPSILON) };

inline void quat_2_euler_d3d(const XMFLOAT4& q, float& yaw, float& pitch, float& roll)
{
	float sqw = q.w*q.w;
	float sqx = q.x*q.x;
	float sqy = q.y*q.y;
	float sqz = q.z*q.z;
	pitch = asinf(2.0f * (q.w*q.x - q.y*q.z)); // rotation about x-axis
	yaw = atan2f(2.0f * (q.x*q.z + q.w*q.y), (-sqx - sqy + sqz + sqw)); // rotation about y-axis
	roll = atan2f(2.0f * (q.x*q.y + q.w*q.z), (-sqx + sqy - sqz + sqw)); // rotation about z-axis
}

namespace Vector2
{
	inline bool CompareVector2WithEpsilon(const XMFLOAT2& lhs, const XMFLOAT2& rhs)
	{
		return XMVector3NearEqual(XMLoadFloat2(&lhs), XMLoadFloat2(&rhs), XMLoadFloat2(&vector2Epsilon));
	}
	inline XMFLOAT2 Add(const XMFLOAT2& xmf2Vector1, const XMFLOAT2& xmf2Vector2)
	{
		XMFLOAT2 xmf2Result;
		XMStoreFloat2(&xmf2Result, XMLoadFloat2(&xmf2Vector1) + XMLoadFloat2(&xmf2Vector2));
		return(xmf2Result);
	}
	inline XMFLOAT2 Add(const XMFLOAT2& xmf2Vector1, const XMFLOAT2& xmf2Vector2, float fScalar)
	{
		XMFLOAT2 xmf2Result;
		XMStoreFloat2(&xmf2Result, XMLoadFloat2(&xmf2Vector1) + (XMLoadFloat2(&xmf2Vector2) * fScalar));
		return(xmf2Result);
	}
	inline XMFLOAT2 Subtract(const XMFLOAT2& xmf2Vector1, const XMFLOAT2& xmf2Vector2)
	{
		XMFLOAT2 xmf2Result;
		XMStoreFloat2(&xmf2Result, XMLoadFloat2(&xmf2Vector1) - XMLoadFloat2(&xmf2Vector2));
		return(xmf2Result);
	}
	inline XMFLOAT2 Lerp(const XMFLOAT2& v0, const XMFLOAT2& v1, float t)
	{
		return Add(v0, Subtract(v1, v0), t);
	}
}

namespace Vector4
{
	inline XMFLOAT4 QuatIdentity()
	{
		XMFLOAT4 quat;
		XMStoreFloat4(&quat, XMQuaternionIdentity());
		return quat;
	}
	inline XMFLOAT4 QuatSlerp(const XMFLOAT4& q0, const XMFLOAT4& q1, float t)
	{
		XMFLOAT4 quat;
		XMStoreFloat4(&quat, XMQuaternionSlerp(XMLoadFloat4(&q0), XMLoadFloat4(&q1), t));
		return quat;
	}
	inline XMFLOAT4 QuatFromMtx(const XMFLOAT4X4& mtx)
	{
		XMFLOAT4 quat;
		XMStoreFloat4(&quat, XMQuaternionRotationMatrix(XMLoadFloat4x4(&mtx)));
		return quat;
	}
	inline XMFLOAT4 QuatFromAngle(const XMFLOAT3& v) {
		XMFLOAT4 quat;
		XMStoreFloat4(&quat, XMQuaternionRotationRollPitchYaw(XMConvertToRadians(v.y), XMConvertToRadians(v.z), XMConvertToRadians(v.x)));
		return quat;
	}
	inline XMFLOAT4 Add(const XMFLOAT4& xmf4Vector1, const XMFLOAT4& xmf4Vector2)
	{
		XMFLOAT4 xmf4Result;
		XMStoreFloat4(&xmf4Result, XMLoadFloat4(&xmf4Vector1) + XMLoadFloat4(&xmf4Vector2));
		return(xmf4Result);
	}
	inline XMFLOAT4 Multiply(const XMFLOAT4& xmf4Vector1, const XMFLOAT4& xmf4Vector2)
	{
		XMFLOAT4 xmf4Result;
		XMStoreFloat4(&xmf4Result, XMLoadFloat4(&xmf4Vector1) * XMLoadFloat4(&xmf4Vector2));
		return(xmf4Result);
	}
	inline XMFLOAT4 Multiply(float fScalar, const XMFLOAT4& xmf4Vector)
	{
		XMFLOAT4 xmf4Result;
		XMStoreFloat4(&xmf4Result, fScalar * XMLoadFloat4(&xmf4Vector));
		return(xmf4Result);
	}
	inline XMVECTOR QuatFromXYZAngle(const XMFLOAT3& angle) {
		return XMQuaternionRotationRollPitchYaw(
			XMConvertToRadians(angle.x),
			XMConvertToRadians(angle.y),
			XMConvertToRadians(angle.z));
	}
}

namespace Vector3
{
	inline bool IsZero(const XMFLOAT3& xmf3Vector)
	{
		if (::IsZero(xmf3Vector.x) &&
			::IsZero(xmf3Vector.y) &&
			::IsZero(xmf3Vector.z))
			return(true);
		return(false);
	}
	inline XMFLOAT3 ScalarProduct(const XMFLOAT3& xmf3Vector, float fScalar, bool bNormalize = true)
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
	inline XMFLOAT3 Add(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2, float fScalar)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) + (XMLoadFloat3(&xmf3Vector2) * fScalar));
		return(xmf3Result);
	}
	inline XMFLOAT3 Subtract(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) - XMLoadFloat3(&xmf3Vector2));
		return(xmf3Result);
	}
	inline float DotProduct(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3Dot(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2)));
		return(xmf3Result.x);
	}
	inline XMFLOAT3 CrossProduct(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2, bool bNormalize = true)
	{
		XMFLOAT3 xmf3Result;
		if (bNormalize)
			XMStoreFloat3(&xmf3Result, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2))));
		else
			XMStoreFloat3(&xmf3Result, XMVector3Cross(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2)));
		return(xmf3Result);
	}
	inline XMFLOAT3 Normalize(const XMFLOAT3& xmf3Vector)
	{
		XMFLOAT3 m_xmf3Normal;
		XMStoreFloat3(&m_xmf3Normal, XMVector3Normalize(XMLoadFloat3(&xmf3Vector)));
		return(m_xmf3Normal);
	}
	inline XMFLOAT3 Multiply(float fScalar, const XMFLOAT3& xmf3Vector)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, fScalar * XMLoadFloat3(&xmf3Vector));
		return(xmf3Result);
	}
	inline float Length(const XMFLOAT3& xmf3Vector)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3Length(XMLoadFloat3(&xmf3Vector)));
		return(xmf3Result.x);
	}
	inline float Angle(const XMVECTOR& xmvVector1, const XMVECTOR& xmvVector2)
	{
		XMVECTOR xmvAngle = XMVector3AngleBetweenNormals(xmvVector1, xmvVector2);
		return XMConvertToDegrees(XMVectorGetX(xmvAngle));
		//return(XMConvertToDegrees(acosf(XMVectorGetX(xmvAngle))));
	}
	inline float Angle(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2)
	{
		return(Angle(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2)));
	}
	inline XMFLOAT3 TransformNormal(const XMFLOAT3& xmf3Vector, const XMMATRIX& xmmtxTransform)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3TransformNormal(XMLoadFloat3(&xmf3Vector), xmmtxTransform));
		return(xmf3Result);
	}
	inline XMFLOAT3 TransformCoord(const XMFLOAT3& xmf3Vector, const XMMATRIX& xmmtxTransform)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3TransformCoord(XMLoadFloat3(&xmf3Vector), xmmtxTransform));
		return(xmf3Result);
	}
	inline XMFLOAT3 TransformCoord(const XMFLOAT3& xmf3Vector, const XMFLOAT4X4& xmmtx4x4Matrix)
	{
		return(TransformCoord(xmf3Vector, XMLoadFloat4x4(&xmmtx4x4Matrix)));
	}
	inline bool CompareVector3WithEpsilon(const XMFLOAT3& lhs, const XMFLOAT3& rhs)
	{
		return XMVector3NearEqual(XMLoadFloat3(&lhs), XMLoadFloat3(&rhs), XMLoadFloat3(&vector3Epsilon));
	}
	inline XMFLOAT3 Lerp(const XMFLOAT3& v0, const XMFLOAT3& v1, float t)
	{
		return Add(v0, Subtract(v1, v0), t);
	}
	inline XMFLOAT3 PosFromMtx(const XMFLOAT4X4& mtx)
	{
		return XMFLOAT3{ mtx._41, mtx._42, mtx._43 };
	}
	inline XMFLOAT3 AngleFromQuat(const XMFLOAT4& quat) {
		XMFLOAT3 tmp;
		quat_2_euler_d3d(quat, tmp.y, tmp.x, tmp.z);
		tmp.x = XMConvertToDegrees(tmp.x) * -1;
		tmp.y = XMConvertToDegrees(tmp.y) * -1;
		tmp.z = XMConvertToDegrees(tmp.z) * -1;
		return tmp;
	}
	inline XMFLOAT3 AngleFromMtx(const XMFLOAT4X4& mtx) {
		XMFLOAT4 tmp = Vector4::QuatFromMtx(mtx);
		return Vector3::AngleFromQuat(tmp);
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
	inline XMFLOAT4X4 Multiply(const XMFLOAT4X4& xmmtx4x4Matrix1, const XMFLOAT4X4& xmmtx4x4Matrix2)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMLoadFloat4x4(&xmmtx4x4Matrix1) * XMLoadFloat4x4(&xmmtx4x4Matrix2));
		return(xmmtx4x4Result);
	}
	inline XMFLOAT4X4 Multiply(const XMFLOAT4X4& xmmtx4x4Matrix1, const XMMATRIX& xmmtxMatrix2)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMLoadFloat4x4(&xmmtx4x4Matrix1) * xmmtxMatrix2);
		return(xmmtx4x4Result);
	}
	inline XMFLOAT4X4 Multiply(const XMMATRIX& xmmtxMatrix1, const XMFLOAT4X4& xmmtx4x4Matrix2)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, xmmtxMatrix1 * XMLoadFloat4x4(&xmmtx4x4Matrix2));
		return(xmmtx4x4Result);
	}
	inline XMFLOAT4X4 Inverse(const XMFLOAT4X4& xmmtx4x4Matrix)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixInverse(NULL, XMLoadFloat4x4(&xmmtx4x4Matrix)));
		return(xmmtx4x4Result);
	}
	inline XMFLOAT4X4 Transpose(const XMFLOAT4X4& xmmtx4x4Matrix)
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
	inline XMFLOAT4X4 LookAtLH(const XMFLOAT3& xmf3EyePosition, const XMFLOAT3& xmf3LookAtPosition, const XMFLOAT3& xmf3UpDirection)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixLookAtLH(XMLoadFloat3(&xmf3EyePosition), XMLoadFloat3(&xmf3LookAtPosition), XMLoadFloat3(&xmf3UpDirection)));
		return(xmmtx4x4Result);
	}
	inline void ToTransform(XMFLOAT4X4* mtx, const XMFLOAT3& pos, const XMFLOAT4& quat)
	{
		XMStoreFloat4x4(mtx, XMMatrixRotationQuaternion(XMLoadFloat4(&quat)));
		(*mtx)._41 = pos.x;
		(*mtx)._42 = pos.y;
		(*mtx)._43 = pos.z;
	}
	inline void InterpolateMtx(XMFLOAT4X4* pOutputMtx, const XMFLOAT4X4& mtx0, const XMFLOAT4X4& mtx1, float t)
	{
		ToTransform(
			pOutputMtx
			, Vector3::Lerp(Vector3::PosFromMtx(mtx0), Vector3::PosFromMtx(mtx1), t)
			, Vector4::QuatSlerp(Vector4::QuatFromMtx(mtx0), Vector4::QuatFromMtx(mtx1), t));
	}
	inline void InterpolateMtx(XMMATRIX* pOutputMtx, const XMMATRIX& mtx0, const XMMATRIX& mtx1, float t)
	{
		XMFLOAT4X4 xmf4x4OutputMtx;
		XMFLOAT4X4 xmf4x4Mtx0;
		XMFLOAT4X4 xmf4x4Mtx1;
		XMStoreFloat4x4(&xmf4x4Mtx0, mtx0);
		XMStoreFloat4x4(&xmf4x4Mtx1, mtx1);
		InterpolateMtx(&xmf4x4OutputMtx, xmf4x4Mtx0, xmf4x4Mtx1, t);
		memcpy(pOutputMtx, &XMLoadFloat4x4(&xmf4x4OutputMtx), sizeof(XMMATRIX));
	}
	inline XMMATRIX MakeFromXYZAngle(const XMFLOAT3& angle) {
		return XMMatrixRotationRollPitchYaw(
			XMConvertToRadians(angle.x),
			XMConvertToRadians(angle.y),
			XMConvertToRadians(angle.z));
	}
}

inline XMMATRIX XMMatrixRotationRollPitchYawDegree(float x, float y, float z) {
	return XMMatrixRotationRollPitchYaw(XMConvertToRadians(x), XMConvertToRadians(y), XMConvertToRadians(z));
}
