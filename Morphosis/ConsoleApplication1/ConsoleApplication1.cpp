// ConsoleApplication1.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//
#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <iomanip>
#include <DirectXMath.h>	//<-XMVECTOR, XMMATRIX 등등

using namespace DirectX;
using namespace std;

void Print(const XMFLOAT4X4 &m) {
	cout.setf(ios::fixed, ios::floatfield);
	cout << setw(10) << m._11 << " " << setw(10) << m._12 << " " << setw(10) << m._13 << " " << setw(10) << m._14 << "\n";
	cout << setw(10) << m._21 << " " << setw(10) << m._22 << " " << setw(10) << m._23 << " " << setw(10) << m._24 << "\n";
	cout << setw(10) << m._31 << " " << setw(10) << m._32 << " " << setw(10) << m._33 << " " << setw(10) << m._34 << "\n";
	cout << setw(10) << m._41 << " " << setw(10) << m._42 << " " << setw(10) << m._43 << " " << setw(10) << m._44 << "\n\n";
}
void Print(const XMMATRIX & m) {
	XMFLOAT4X4 temp;
	XMStoreFloat4x4(&temp, m);
	Print(temp);
}
void Print(const XMFLOAT4 &v) {
	cout.setf(ios::fixed, ios::floatfield);
	cout << setw(10) << v.x << " " << setw(10) << v.y << " " << setw(10) << v.z << " " << setw(10) << v.w << "\n\n";
}
void Print(const XMVECTOR &v) {
	XMFLOAT4 temp;
	XMStoreFloat4(&temp, v);
	Print(temp);
}

int main()
{
	Print(XMMatrixRotationRollPitchYaw(0, XMConvertToRadians(-90), 0));



    return 0;
}

