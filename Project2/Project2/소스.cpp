#include <DirectXMath.h>
#include <d3d12.h>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>

#include <vector>

using namespace DirectX;
using namespace std;

// math
inline bool		IsZero(float fValue) { return((fabsf(fValue) < FLT_EPSILON)); }
inline bool		IsEqual(float fA, float fB) { return(::IsZero(fA - fB)); }
inline float	InverseSqrt(float fValue) { return 1.0f / sqrtf(fValue); }
inline void		Swap(float *pfS, float *pfT) { float fTemp = *pfS; *pfS = *pfT; *pfT = fTemp; }

const XMFLOAT2 vector2Epsilon{ XMFLOAT2(FLT_EPSILON, FLT_EPSILON) };
const XMFLOAT3 vector3Epsilon{ XMFLOAT3(FLT_EPSILON, FLT_EPSILON, FLT_EPSILON) };

void quat_2_euler_ogl(const XMFLOAT4& q, float& yaw, float& pitch, float& roll)
{
	float sqw = q.w*q.w;
	float sqx = q.x*q.x;
	float sqy = q.y*q.y;
	float sqz = q.z*q.z;
	pitch = asinf(2.0f * (q.y*q.z + q.w*q.x)); // rotation about x-axis
	roll = atan2f(2.0f * (q.w*q.y - q.x*q.z), (-sqx - sqy + sqz + sqw)); // rotation about y-axis
	yaw = atan2f(2.0f * (q.w*q.z - q.x*q.y), (-sqx + sqy - sqz + sqw)); // rotation about z-axis
}

void quat_2_euler_d3d(const XMFLOAT4& q, float& yaw, float& pitch, float& roll)
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
		XMStoreFloat4(&quat, XMQuaternionRotationRollPitchYaw(XMConvertToRadians(v.x), XMConvertToRadians(v.y), XMConvertToRadians(v.z)));
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

		tmp.x = XMConvertToDegrees(tmp.x);
		tmp.y = XMConvertToDegrees(tmp.y);
		tmp.z = XMConvertToDegrees(tmp.z);

		return tmp;
	}
	inline XMFLOAT3 AngleFromMtx(const XMFLOAT4X4& mtx) {
		XMFLOAT4 tmp = Vector4::QuatFromMtx(mtx);
		return Vector3::AngleFromQuat(tmp);
	}
	inline XMFLOAT3 AngleFromMtx(const XMMATRIX& m) {
		XMFLOAT4X4 mtx;
		XMStoreFloat4x4(&mtx, m);
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
			XMConvertToRadians(angle.x) * -1,
			XMConvertToRadians(angle.y) * -1,
			XMConvertToRadians(angle.z) * -1);
	}
}

//struct Node {
//public:
//	XMMATRIX GetTMtx() { return XMLoadFloat4x4(&xmf4x4TMtx); }
//	XMMATRIX GetRMtx() { return XMLoadFloat4x4(&xmf4x4RMtx); }
//	//값을 반대로
//	XMMATRIX GetTInvMtx() { return XMLoadFloat4x4(&xmf4x4TInvMtx); }	
//	//역행렬
//	XMMATRIX GetTMtxInv() { return XMLoadFloat4x4(&xmf4x4TMtxInv); }	
//	//값을 반대로
//	XMMATRIX GetRInvMtx() { return XMLoadFloat4x4(&xmf4x4RInvMtx); }
//	//역행렬
//	XMMATRIX GetRMtxInv() { return XMLoadFloat4x4(&xmf4x4RMtxInv); }
//
//public:
//	Node() = default;
//	Node(const XMFLOAT3& pos, const XMFLOAT3& rot)
//		: xmf3T(pos)
//		, xmf3R(rot)
//	{
//		XMStoreFloat4x4(&xmf4x4RMtx, Matrix4x4::MakeFromXYZAngle(xmf3R));
//		XMStoreFloat4x4(&xmf4x4TMtx, XMMatrixTranslation(xmf3T.x, xmf3T.y, xmf3T.z));
//
//		XMFLOAT3 tmp = xmf3R;
//		tmp.x *= -1;
//		tmp.y *= -1;
//		tmp.z *= -1;
//		XMStoreFloat4x4(&xmf4x4RInvMtx, Matrix4x4::MakeFromXYZAngle(tmp));
//		XMVECTOR det = XMMatrixDeterminant(XMLoadFloat4x4(&xmf4x4RMtx));
//		XMStoreFloat4x4(&xmf4x4RMtxInv, XMMatrixInverse(&det, XMLoadFloat4x4(&xmf4x4RMtx)));
//
//		tmp = xmf3T;
//		tmp.x *= -1;
//		tmp.y *= -1;
//		tmp.z *= -1;
//		XMStoreFloat4x4(&xmf4x4TInvMtx, XMMatrixTranslation(tmp.x, tmp.y, tmp.z));
//		det = XMMatrixDeterminant(XMLoadFloat4x4(&xmf4x4TMtx));
//		XMStoreFloat4x4(&xmf4x4TMtxInv, XMMatrixInverse(&det, XMLoadFloat4x4(&xmf4x4TMtx)));
//	}
//
//private:
//	XMFLOAT3 xmf3T;
//	XMFLOAT3 xmf3R;
//	XMFLOAT4X4 xmf4x4TMtx;
//	XMFLOAT4X4 xmf4x4RMtx;
//	XMFLOAT4X4 xmf4x4RMtxInv;	//역행렬
//	XMFLOAT4X4 xmf4x4RInvMtx;	//값을 반대로
//	XMFLOAT4X4 xmf4x4TMtxInv;	//역행렬
//	XMFLOAT4X4 xmf4x4TInvMtx;	//값을 반대로
//};

void PrintM(const XMFLOAT4X4& m) {
	cout << fixed << setprecision(5) <<
		setw(9) << m._11 << " " << setw(9) << m._12 << " " << setw(9) << m._13 << " " << setw(9) << m._14 << "\n" <<
		setw(9) << m._21 << " " << setw(9) << m._22 << " " << setw(9) << m._23 << " " << setw(9) << m._24 << "\n" <<
		setw(9) << m._31 << " " << setw(9) << m._32 << " " << setw(9) << m._33 << " " << setw(9) << m._34 << "\n" <<
		setw(9) << m._41 << " " << setw(9) << m._42 << " " << setw(9) << m._43 << " " << setw(9) << m._44 << "\n\n";
}
void PrintM(const XMMATRIX& mtx) {
	XMFLOAT4X4 m;
	XMStoreFloat4x4(&m, mtx);
	PrintM(m);
}

void PrintV(const XMFLOAT3& xmf3) {
	cout << fixed << setprecision(5) <<
		"(" << setw(9) << xmf3.x << ", " << setw(9) << xmf3.y << ", " << setw(9) << xmf3.z << ")\n\n";
}
void PrintV(const XMFLOAT4& xmf4) {
	cout << fixed << setprecision(5) <<
		"(" << setw(9) << xmf4.x << ", " << setw(9) << xmf4.y << ", " << setw(9) << xmf4.z << ")\n\n";
}
void PrintV(const XMVECTOR& v) {
	XMFLOAT4 tmp;
	XMStoreFloat4(&tmp, v);
	PrintV(tmp);
}

void Print(const char* str) {
	cout << str << "\n";
}
XMMATRIX XMMatrixRotationRollPitchYawDegree(float x, float y, float z) {
	return XMMatrixRotationRollPitchYaw(XMConvertToRadians(x), XMConvertToRadians(y), XMConvertToRadians(z));
}
XMMATRIX XMMatrixRotationRollPitchYawDegree(const XMFLOAT3& angle) {
	return XMMatrixRotationRollPitchYaw(XMConvertToRadians(angle.x), XMConvertToRadians(angle.y), XMConvertToRadians(angle.z));
}


class Node {
public:
	Node() = default;
	Node(const XMFLOAT3& xmf3Translation, const XMFLOAT3& xmf3Rotation)
	{
		XMMATRIX t = XMMatrixTranslation(xmf3Translation.x, xmf3Translation.y, xmf3Translation.z);
		XMMATRIX r = XMMatrixTranspose(XMMatrixRotationRollPitchYawDegree(xmf3Rotation.x, xmf3Rotation.y, xmf3Rotation.z));
		XMStoreFloat4x4(&m_xmf4x4ToParent, XMMatrixMultiply(r, t));
		XMStoreFloat4x4(&m_xmf4x4Lcl, XMMatrixIdentity());
	}
	Node(const XMFLOAT3& xmf3Translation, const XMFLOAT3& xmf3Rotation, const XMFLOAT3& xmf3LclRotation)
		:Node(xmf3Translation, xmf3Rotation)
	{
		XMMATRIX r = XMMatrixTranspose(XMMatrixRotationRollPitchYawDegree(xmf3LclRotation.x, xmf3LclRotation.y, xmf3LclRotation.z));
		XMStoreFloat4x4(&m_xmf4x4Lcl, r);
	}

public:
	XMMATRIX GetToParent() { return XMLoadFloat4x4(&m_xmf4x4ToParent); }
	XMMATRIX GetToParentInv() {
		XMVECTOR det = XMMatrixDeterminant(XMLoadFloat4x4(&m_xmf4x4ToParent));
		return XMMatrixInverse(&det, XMLoadFloat4x4(&m_xmf4x4ToParent));
	}
	XMMATRIX GetLcl() { return XMLoadFloat4x4(&m_xmf4x4Lcl); }

private:
	XMFLOAT4X4 m_xmf4x4ToParent;
	XMFLOAT4X4 m_xmf4x4Lcl;
};

void DrawProgressBar(const float t) {
	cout << static_cast<int>(t * 100.0f) << "% [";
	for (int i = 0; i < 100; ++i) {
		if (i < t * 100.0f) cout << "/";
		else				cout << ".";
	}
	cout << "]\n\n";
}

void PrintTotal(const XMFLOAT3& a1, const XMFLOAT3& a2, float t) {

	XMMATRIX m1 = XMMatrixRotationRollPitchYawDegree(a1.x, a1.y, a1.z);
	XMMATRIX m2 = XMMatrixRotationRollPitchYawDegree(a2.x, a2.y, a2.z);

	XMMATRIX m = XMMatrixMultiply(m1, m2);
	XMFLOAT4X4 r; XMStoreFloat4x4(&r, m);
	PrintV(Vector3::AngleFromMtx(r));
	//XMFLOAT4X4 m; XMStoreFloat4x4(&m, XMMatrixRotationRollPitchYawDegree(a1.x, a1.y, a1.z));
	//XMFLOAT3 b = Vector3::AngleFromMtx(m);
	//PrintV(b);

	//XMFLOAT3 c = Vector3::ScalarProduct(a2, -1, false);
	//PrintV(c);

	//b = Vector3::Subtract(b, c);
	//PrintV(b);


	//XMVECTOR d = XMMatrixDeterminant(XMMatrixRotationRollPitchYawDegree(a1.x, a1.y, a1.z));


}

struct Bone {
	int			m_iParentIdx;
	XMFLOAT3	m_xmf3Translation;
	XMFLOAT3	m_xmf3Rotation;
};

struct CrtlPoint {
	XMFLOAT3	m_xmf3Position;
	XMINT4		m_xmi4BoneIdx;
	XMFLOAT4	m_xmf4Weight;
};

struct Mesh {
	vector<CrtlPoint>	m_CrtlPointList;
	vector<int>			m_PolyVertexIdxList;
};

struct Key {
	vector<int>			m_boneIdxList;
	vector<XMFLOAT3>	m_xmf3TranslationList;
	vector<XMFLOAT3>	m_xmf3RotationList;
	vector<XMFLOAT4X4>	m_xmf4x4GlobalTransform;

	vector<Bone*>		m_pBoneList;
	float				m_fKeyTime;
};


int main() {

	string			fileName = { "test_0429_015_Character(m).dat" };
	vector<Bone>	lBoneList;
	vector<Mesh>	lMeshList;
	vector<Key>		lKeyList;

	ifstream in;
	in.open(fileName, ios::in | ios::binary);

	int nBones;
	in.read((char*)&nBones, sizeof(int));
	lBoneList.resize(nBones);
	for (int i = 0; i < nBones; ++i) {
		in.read((char*)&(lBoneList[i].m_iParentIdx), sizeof(int));
		in.read((char*)&(lBoneList[i].m_xmf3Translation), sizeof(XMFLOAT3));
		in.read((char*)&(lBoneList[i].m_xmf3Rotation), sizeof(XMFLOAT3));
	}

	int nMeshes;
	in.read((char*)&nMeshes, sizeof(int));
	lMeshList.resize(nMeshes);
	for (int i = 0; i < nMeshes; ++i) {
		int nCPs;
		in.read((char*)&nCPs, sizeof(int));
		lMeshList[i].m_CrtlPointList.resize(nCPs);
		for (int j = 0; j < nCPs; ++j) {
			in.read((char*)&(lMeshList[i].m_CrtlPointList[j].m_xmf3Position), sizeof(XMFLOAT3));
			in.read((char*)&(lMeshList[i].m_CrtlPointList[j].m_xmi4BoneIdx), sizeof(XMINT4));
			in.read((char*)&(lMeshList[i].m_CrtlPointList[j].m_xmf4Weight), sizeof(XMFLOAT4));
		}
	}
	for (int i = 0; i < nMeshes; ++i) {
		int nPVI;
		in.read((char*)&nPVI, sizeof(int));
		lMeshList[i].m_PolyVertexIdxList.resize(nPVI);
		for (int j = 0; j < nPVI; ++j) {
			in.read((char*)&(lMeshList[i].m_PolyVertexIdxList[j]), sizeof(int));
		}
	}


	int nKeys;
	in.read((char*)&nKeys, sizeof(int));
	lKeyList.resize(nKeys);

	for (int i = 0; i < nKeys; ++i) {
		int nBones;/* = keys[i].data.size();*/
		in.read((char*)&nBones, sizeof(int));
		lKeyList[i].m_pBoneList.resize(nBones);
		lKeyList[i].m_boneIdxList.resize(nBones);
		lKeyList[i].m_xmf3TranslationList.resize(nBones);
		lKeyList[i].m_xmf3RotationList.resize(nBones);
		lKeyList[i].m_xmf4x4GlobalTransform.resize(nBones);
	}

	for (int i = 0; i < nKeys; ++i) {
		float		keyTime;
		int			boneIdx;
		XMFLOAT3	tmpFloat3;
		XMFLOAT4X4	tmpFloat4x4;

		in.read((char*)&keyTime, sizeof(float));
		lKeyList[i].m_fKeyTime = keyTime;

		for (int j = 0; j < lKeyList[i].m_boneIdxList.size(); ++j) {
			in.read((char*)&boneIdx, sizeof(int));
			lKeyList[i].m_boneIdxList[j] = boneIdx;
			in.read((char*)&lKeyList[i].m_xmf3TranslationList[j], sizeof(XMFLOAT3));
			in.read((char*)&lKeyList[i].m_xmf3RotationList[j], sizeof(XMFLOAT3));
			in.read((char*)&lKeyList[i].m_xmf4x4GlobalTransform[j], sizeof(XMFLOAT4X4));
		}
	}

	in.close();


	return 0;
}