#include <iostream>
#include <DirectXMath.h>

#include <iomanip>

using namespace DirectX;
using namespace std;

///class Parent {
///public:
///	virtual void Display() {
///		cout << "Parent's Function\n";
///	}
///};
///class Child : public Parent {
///public:
///	virtual void Display() {
///		cout << "Child's Function\n";
///		Animate();
///	}
///	void Animate() {
///		cout << "and also, 애 니 조 아\n";
///	}
///};

void Print(const XMFLOAT4X4& m) {
	std::cout << std::fixed << std::setprecision(5) <<
		std::setw(9) << m._11 << " " << std::setw(9) << m._12 << " " << std::setw(9) << m._13 << " " << std::setw(9) << m._14 << "\n" <<
		std::setw(9) << m._21 << " " << std::setw(9) << m._22 << " " << std::setw(9) << m._23 << " " << std::setw(9) << m._24 << "\n" <<
		std::setw(9) << m._31 << " " << std::setw(9) << m._32 << " " << std::setw(9) << m._33 << " " << std::setw(9) << m._34 << "\n" <<
		std::setw(9) << m._41 << " " << std::setw(9) << m._42 << " " << std::setw(9) << m._43 << " " << std::setw(9) << m._44 << "\n\n";
}
void Print(const XMMATRIX& m) {
	XMFLOAT4X4 tmp;
	XMStoreFloat4x4(&tmp, m);
	Print(tmp);
}

int main()
{
	///Parent* p1 = new Parent();
	///Parent* p2 = new Child();
	///p1->Display();
	///p2->Display();
	///Child* p3 = static_cast<Child*>(p2);
	///p3->Display();
	///Parent** pp1 = new Parent*[3];
	///Child* p4 = new Child();
	///pp1[0] = p4;
	///pp1[0]->Display();

	Print(XMMatrixRotationRollPitchYaw(XMConvertToRadians(0), XMConvertToRadians(180), XMConvertToRadians(0)));


	return 0;
}
