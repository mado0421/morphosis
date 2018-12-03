// ConsoleApplication1.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//
#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <iomanip>
#include <DirectXMath.h>	//<-XMVECTOR, XMMATRIX 등등
#include <fstream>
#include <vector>
#include <list>

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

struct T{
	UINT n1;
	float * p1;

	UINT n2;
	float * p2;
};


struct T2 {
	__int64 left, right;

	T2(__int64 l, __int64 r) : left(l), right(r) {}
};

struct T3 {
	__int64 ID;
	list<__int64> childList;

	T3(__int64 ID, __int64 child) : ID(ID) {
		childList.push_back(child);
	}
	void AddChild(__int64 val) {
		childList.push_back(val);
	}
};

struct Node {
	__int64 ID;
	Node	*child		= nullptr;
	Node	*sibling	= nullptr;
};

Node* FindID(__int64 id, Node* p) {
	if (id == p->ID) return p;

	if (p->child) {
		Node* temp = FindID(id, p->child);
		if (temp) return temp;
		return FindID(id, p->sibling);
	}
	return nullptr;
}

int main()
{
	/*T a;
	streampos size;
	a.n1 = 100;
	a.p1 = new float[a.n1];
	for (int i = 0; i < a.n1; ++i) a.p1[i] = i;
	a.n2 = 200;
	a.p2 = new float[a.n2];
	for (int i = 0; i < a.n2; ++i) a.p2[i] = a.n2 - i;
	ofstream out("test.dat", ios::binary);
	out.write((char*)&a.n1, sizeof(UINT));
	out.write((char*)&a.p1, sizeof(float) * a.n1);
	out.write((char*)&a.n2, sizeof(UINT));
	out.write((char*)&a.p2, sizeof(float) * a.n2);
	out.close();
	ifstream in("test.dat", ios::binary);
	size = in.tellg();
	UINT n1, n2;
	float *p1, *p2;
	in.read((char*)&n1, sizeof(UINT));
	p1 = new float[n1];
	in.read((char*)&p1, sizeof(float)*n1);
	in.read((char*)&n2, sizeof(UINT));
	p2 = new float[n2];
	in.read((char*)&p2, sizeof(float)*n2);
	in.close();
	T* b;
	b = new T();
	b->n1 = n1;
	b->p1 = new float[b->n1];
	memcpy(b->p1, p1, sizeof(float)*b->n1);
	b->n2 = n2;
	b->p2 = new float[b->n2];
	memcpy(b->p2, p2, sizeof(float)*b->n2);
	for (int i = 0; i < b->n1; ++i) cout << b->p1[i] << "\n";
	for (int i = 0; i < b->n2; ++i) cout << b->p2[i] << "\n";
	delete p1;
	delete p2;
    return 0;*/
	
	vector<T2> vT2;
	__int64 left, right;
	char * str = nullptr;
	str = new char[
		strlen("Connections:  {\
		; Model::Box001, Model::RootNode\
		C: \"OO\", 3428172848, 0\
		; Model::Box002, Model::RootNode\
		C: \"OO\", 3428170656, 0\
		; Model::Box003, Model::RootNode\
		C: \"OO\", 3428164080, 0\
		; Model::Bone001, Model::RootNode\
		C: \"OO\", 3428177232, 0\
		; AnimLayer::BaseLayer, AnimStack::Take 001\
		C: \"OO\", 1756864896, 3438785136\
		; AnimCurveNode::R, AnimLayer::BaseLayer\
		C: \"OO\", 3438774944, 1756864896\
		; Geometry::, Model::Box001\
		C: \"OO\", 2467101040, 3428172848\
		; Geometry::, Model::Box002\
		C: \"OO\", 2467096144, 3428170656\
		; Geometry::, Model::Box003\
		C: \"OO\", 2918101392, 3428164080\
		; NodeAttribute::, Model::Bone001\
		C: \"OO\", 1265861296, 3428177232\
		; AnimCurveNode::R, Model::Bone001\
		C: \"OP\", 3438774944, 3428177232, \"Lcl Rotation\"\
		; Deformer::, Geometry::\
		C: \"OO\", 1265864560, 2467101040\
		; Deformer::, Geometry::\
		C: \"OO\", 1265863408, 2467096144\
		; Deformer::, Geometry::\
		C: \"OO\", 1265859568, 2918101392\
		; AnimCurve::, AnimCurveNode::R\
		C: \"OP\", 3430266208, 3438774944, \"d|X\"\
		; AnimCurve::, AnimCurveNode::R\
		C: \"OP\", 3430267648, 3438774944, \"d|Y\"\
		; AnimCurve::, AnimCurveNode::R\
		C: \"OP\", 3430267968, 3438774944, \"d|Z\"\
		; SubDeformer::, Deformer::\
		C: \"OO\", 2478761280, 1265864560\
		; Model::Bone001, SubDeformer::\
		C: \"OO\", 3428177232, 2478761280\
		; SubDeformer::, Deformer::\
		C: \"OO\", 2478754512, 1265863408\
		; Model::Bone001, SubDeformer::\
		C: \"OO\", 3428177232, 2478754512\
		; SubDeformer::, Deformer::\
		C: \"OO\", 2478756768, 1265859568\
		; Model::Bone001, SubDeformer::\
		C: \"OO\", 3428177232, 2478756768\
}\
") ];	
	strcpy(str, "Connections:  {\
		; Model::Box001, Model::RootNode\
		C: \"OO\", 3428172848, 0\
		; Model::Box002, Model::RootNode\
		C: \"OO\", 3428170656, 0\
		; Model::Box003, Model::RootNode\
		C: \"OO\", 3428164080, 0\
		; Model::Bone001, Model::RootNode\
		C: \"OO\", 3428177232, 0\
		; AnimLayer::BaseLayer, AnimStack::Take 001\
		C: \"OO\", 1756864896, 3438785136\
		; AnimCurveNode::R, AnimLayer::BaseLayer\
		C: \"OO\", 3438774944, 1756864896\
		; Geometry::, Model::Box001\
		C: \"OO\", 2467101040, 3428172848\
		; Geometry::, Model::Box002\
		C: \"OO\", 2467096144, 3428170656\
		; Geometry::, Model::Box003\
		C: \"OO\", 2918101392, 3428164080\
		; NodeAttribute::, Model::Bone001\
		C: \"OO\", 1265861296, 3428177232\
		; AnimCurveNode::R, Model::Bone001\
		C: \"OP\", 3438774944, 3428177232, \"Lcl Rotation\"\
		; Deformer::, Geometry::\
		C: \"OO\", 1265864560, 2467101040\
		; Deformer::, Geometry::\
		C: \"OO\", 1265863408, 2467096144\
		; Deformer::, Geometry::\
		C: \"OO\", 1265859568, 2918101392\
		; AnimCurve::, AnimCurveNode::R\
		C: \"OP\", 3430266208, 3438774944, \"d|X\"\
		; AnimCurve::, AnimCurveNode::R\
		C: \"OP\", 3430267648, 3438774944, \"d|Y\"\
		; AnimCurve::, AnimCurveNode::R\
		C: \"OP\", 3430267968, 3438774944, \"d|Z\"\
		; SubDeformer::, Deformer::\
		C: \"OO\", 2478761280, 1265864560\
		; Model::Bone001, SubDeformer::\
		C: \"OO\", 3428177232, 2478761280\
		; SubDeformer::, Deformer::\
		C: \"OO\", 2478754512, 1265863408\
		; Model::Bone001, SubDeformer::\
		C: \"OO\", 3428177232, 2478754512\
		; SubDeformer::, Deformer::\
		C: \"OO\", 2478756768, 1265859568\
		; Model::Bone001, SubDeformer::\
		C: \"OO\", 3428177232, 2478756768\
}\
");
	str = strstr(str, "Connections: ");
	str = strtok(str, "}");
	str = strstr(str, ";");
	while (true) {
		str = strstr(str, "C: ");
		if (nullptr == str) break;
		str = strstr(str, ", ");
		str += strlen(", ");
		left = _strtoi64(str, &str, 10); str++;
		right = _strtoi64(str, &str, 10); str++;
		vT2.emplace_back(left, right);
	}
	cout << "Total: " << vT2.size() << "\n\n";
	for (auto p = vT2.begin(); p != vT2.end(); ++p) cout << p->left << ", " << p->right << "\n";

	//Node * head;

	//for (auto p = vT2.begin(); p != vT2.end(); ++p) {
	//	Node * t = FindID(p->right, head);
	//	if (nullptr == t) {
	//		Node *temp1, *temp2;
	//		temp1 = new Node();
	//		temp1->ID = p->right;
	//		head->child = temp1;

	//		temp2 = new Node();
	//		temp2->ID = p->left;
	//		temp1->child = temp2;
	//	}
	//	else {
	//		Node *temp1;
	//		temp1 = new Node();
	//		temp1->ID = p->left;
	//		t->child = temp1;
	//	}
	//	
	//}


	list<T3> l;
	for (auto p = vT2.begin(); p != vT2.end(); ++p) {
		__int64 targetID = p->right;
		__int64 left = p->left;
		for (auto p2 = l.begin(); p2 != l.end(); ++p2) {
			if (targetID == p2->ID) {
				p2->AddChild(left);
				goto NEXT;
			}
		}

		l.emplace_back(targetID, left);
	NEXT:
		;
	}

	cout << "\n\n";


	for (auto p = l.begin(); p != l.end(); ++p) {
		cout << p->ID << "'s Child:\n";
		for (auto p2 = p->childList.begin(); p2 != p->childList.end(); ++p2) cout << *p2 << " ";
		cout << "\n\n";
	}


	for (auto p = l.begin(); p != l.end(); ++p) {
		for (auto p2 = p->childList.begin(); p2 != p->childList.end(); ++p2)
		{
			
		}
	}



	/*
	Connections:  {
	
	;Model::Box001, Model::RootNode
	C: "OO",3428172848,0
	
	;Model::Box002, Model::RootNode
	C: "OO",3428170656,0
	
	;Model::Box003, Model::RootNode
	C: "OO",3428164080,0
	
	;Model::Bone001, Model::RootNode
	C: "OO",3428177232,0
	
	;AnimLayer::BaseLayer, AnimStack::Take 001
	C: "OO",1756864896,3438785136
	
	;AnimCurveNode::R, AnimLayer::BaseLayer
	C: "OO",3438774944,1756864896
	
	;Geometry::, Model::Box001
	C: "OO",2467101040,3428172848
	
	;Geometry::, Model::Box002
	C: "OO",2467096144,3428170656
	
	;Geometry::, Model::Box003
	C: "OO",2918101392,3428164080
	
	;NodeAttribute::, Model::Bone001
	C: "OO",1265861296,3428177232
	
	;AnimCurveNode::R, Model::Bone001
	C: "OP",3438774944,3428177232, "Lcl Rotation"
	
	;Deformer::, Geometry::
	C: "OO",1265864560,2467101040
	
	;Deformer::, Geometry::
	C: "OO",1265863408,2467096144
	
	;Deformer::, Geometry::
	C: "OO",1265859568,2918101392
	
	;AnimCurve::, AnimCurveNode::R
	C: "OP",3430266208,3438774944, "d|X"
	
	;AnimCurve::, AnimCurveNode::R
	C: "OP",3430267648,3438774944, "d|Y"
	
	;AnimCurve::, AnimCurveNode::R
	C: "OP",3430267968,3438774944, "d|Z"
	
	;SubDeformer::, Deformer::
	C: "OO",2478761280,1265864560
	
	;Model::Bone001, SubDeformer::
	C: "OO",3428177232,2478761280
	
	;SubDeformer::, Deformer::
	C: "OO",2478754512,1265863408
	
	;Model::Bone001, SubDeformer::
	C: "OO",3428177232,2478754512
	
	;SubDeformer::, Deformer::
	C: "OO",2478756768,1265859568
	
	;Model::Bone001, SubDeformer::
	C: "OO",3428177232,2478756768
}

	*/

}
