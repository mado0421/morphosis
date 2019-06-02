#include <iostream>

using namespace std;

class Parent {
public:
	virtual void Display() {
		cout << "Parent's Function\n";
	}
};

class Child : public Parent {
public:
	virtual void Display() {
		cout << "Child's Function\n";
		Animate();
	}
	void Animate() {
		cout << "and also, 애 니 조 아\n";
	}
};

int main()
{
	Parent* p1 = new Parent();
	Parent* p2 = new Child();

	p1->Display();
	p2->Display();

	Child* p3 = static_cast<Child*>(p2);
	p3->Display();

	Parent** pp1 = new Parent*[3];
	Child* p4 = new Child();
	pp1[0] = p4;

	pp1[0]->Display();

	return 0;
}
