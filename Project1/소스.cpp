#include <iostream>
#include <vector>
#include <algorithm>
#include <set>

#include <fbxsdk.h>

#include "Common.h"

using namespace std;

const char * SAMPLE_FILENAME = "test_0429_015_Character.fbx";

void Display4DVector(const char* pHeader, FbxVector4 pValue, const char* pSuffix = "")
{
	FbxString lString;
	FbxString lFloatValue1 = (float)pValue[0];
	FbxString lFloatValue2 = (float)pValue[1];
	FbxString lFloatValue3 = (float)pValue[2];
	FbxString lFloatValue4 = (float)pValue[3];

	lFloatValue1 = pValue[0] <= -HUGE_VAL ? "-INFINITY" : lFloatValue1.Buffer();
	lFloatValue1 = pValue[0] >= HUGE_VAL ? "INFINITY" : lFloatValue1.Buffer();
	lFloatValue2 = pValue[1] <= -HUGE_VAL ? "-INFINITY" : lFloatValue2.Buffer();
	lFloatValue2 = pValue[1] >= HUGE_VAL ? "INFINITY" : lFloatValue2.Buffer();
	lFloatValue3 = pValue[2] <= -HUGE_VAL ? "-INFINITY" : lFloatValue3.Buffer();
	lFloatValue3 = pValue[2] >= HUGE_VAL ? "INFINITY" : lFloatValue3.Buffer();
	lFloatValue4 = pValue[3] <= -HUGE_VAL ? "-INFINITY" : lFloatValue4.Buffer();
	lFloatValue4 = pValue[3] >= HUGE_VAL ? "INFINITY" : lFloatValue4.Buffer();

	lString = pHeader;
	lString += lFloatValue1;
	lString += ", ";
	lString += lFloatValue2;
	lString += ", ";
	lString += lFloatValue3;
	lString += ", ";
	lString += lFloatValue4;
	lString += pSuffix;
	lString += "\n";
	FBXSDK_printf(lString);
}

void DisplayMatrix(const FbxAMatrix& mat, const char* pSuffix = "") {
	for (int i = 0; i < 4; ++i)
	{
		FbxString lHeader(pSuffix);
		FbxString lIndex(i);
		lHeader += "_Row";
		lHeader += lIndex;
		lHeader += ": ";

		Display4DVector(lHeader, mat.GetRow(i));
	}
	FBXSDK_printf("\n");
}

void Display3DVector(const char* pHeader, FbxVector4 pValue, const char* pSuffix  = "" )
{
	FbxString lString;
	FbxString lFloatValue1 = (float)pValue[0];
	FbxString lFloatValue2 = (float)pValue[1];
	FbxString lFloatValue3 = (float)pValue[2];

	lFloatValue1 = pValue[0] <= -HUGE_VAL ? "-INFINITY" : lFloatValue1.Buffer();
	lFloatValue1 = pValue[0] >= HUGE_VAL ? "INFINITY" : lFloatValue1.Buffer();
	lFloatValue2 = pValue[1] <= -HUGE_VAL ? "-INFINITY" : lFloatValue2.Buffer();
	lFloatValue2 = pValue[1] >= HUGE_VAL ? "INFINITY" : lFloatValue2.Buffer();
	lFloatValue3 = pValue[2] <= -HUGE_VAL ? "-INFINITY" : lFloatValue3.Buffer();
	lFloatValue3 = pValue[2] >= HUGE_VAL ? "INFINITY" : lFloatValue3.Buffer();

	lString = pHeader;
	lString += lFloatValue1;
	lString += ", ";
	lString += lFloatValue2;
	lString += ", ";
	lString += lFloatValue3;
	lString += pSuffix;
	lString += "\n";
	FBXSDK_printf(lString);
}

bool IsSkeletonNode(FbxNode* node) {
	int nodeAttributeCount = node->GetNodeAttributeCount();
	for (int i = 0; i < nodeAttributeCount; ++i) {
		FbxNodeAttribute* nodeAttribute = node->GetNodeAttributeByIndex(i);
		if (FbxNodeAttribute::eSkeleton == nodeAttribute->GetAttributeType())
			return true;
	}
	return false;
}

void DisplayNode(FbxNode* node) {
	if (IsSkeletonNode(node)) {

		std::cout << node->GetName() << "\n";
		FbxAMatrix mtx;
		mtx = node->EvaluateLocalTransform();
		DisplayMatrix(mtx, "LocalTransform");

		mtx = node->EvaluateGlobalTransform();
		DisplayMatrix(mtx, "GlobalTransform");

		FbxVector4 vec;
		vec = node->LclRotation;
		Display3DVector("LclRotation: ", vec);

		vec = node->LclTranslation;
		Display3DVector("LclTranslation: ", vec);
	}
}

struct Bone {
	FbxString	m_Name;
	Bone*		m_Parent = NULL;
	FbxNode*	m_Node = NULL;
	FbxAMatrix	m_GlobalTransform;
	FbxAMatrix	m_ToRootTransform;
	FbxAMatrix	m_ToParentTransform;
};

vector<Bone> g_BoneList;

void MakeBoneDataTest(FbxNode* node) {
	if (IsSkeletonNode(node)) {
		//std::cout << node->GetName() << "\n";

		Bone tmp;
		tmp.m_Name = node->GetName();
		tmp.m_Node = node;

		FbxAMatrix mtx;
		mtx = node->EvaluateGlobalTransform();
		tmp.m_GlobalTransform = mtx;
		mtx.SetIdentity();


		FbxTime t;
		t.SetSecondDouble(0.6666);
		mtx = node->EvaluateLocalTransform(t);
		mtx = mtx.Inverse();
		tmp.m_ToParentTransform = mtx;

		g_BoneList.push_back(tmp);
	}
}

void MakeParent(FbxNode* node) {
	if (IsSkeletonNode(node)) {
		//std::cout << node->GetName() << "\n";
		for (auto p = g_BoneList.begin(); p != g_BoneList.end(); ++p) {
			if (p->m_Name == node->GetName()) {
				FbxString parentName = node->GetParent()->GetName();
				for (auto pp = g_BoneList.begin(); pp != g_BoneList.end(); ++pp) {
					if (pp->m_Name == parentName) {
						p->m_Parent = &(*pp);
					}
				}
			}
		}
	}
}

void MakeToRootTransform() {
	for (auto p = g_BoneList.begin(); p != g_BoneList.end(); ++p) {
		p->m_ToRootTransform.SetIdentity();
		if (p->m_Parent) {
			p->m_ToRootTransform = p->m_ToParentTransform * p->m_Parent->m_ToRootTransform;
		}
		else {
			p->m_ToRootTransform = p->m_ToParentTransform;
		}
	}
}

void DisplayBone(Bone& b) {
	std::cout << b.m_Name << "\n";

	DisplayMatrix(b.m_GlobalTransform, "GlobalTransform");
	DisplayMatrix(b.m_ToParentTransform, "ToParentTransform");
	DisplayMatrix(b.m_ToRootTransform, "ToRootTransform");

	std::cout << "\n";
}

void MultiplyGlobalAndToRoot(Bone& b) {
	FbxAMatrix a;
	a = b.m_GlobalTransform;
	a *= b.m_ToRootTransform;

	std::cout << b.m_Name << "\n";
	DisplayMatrix(a, "Result");
}

void AllBones(void(*Foo)(Bone&)) {
	for (auto p = g_BoneList.begin(); p != g_BoneList.end(); ++p) {
		Foo(*p);
	}
}

void RecFollowChildNode(FbxNode* node, void(*Foo)(FbxNode*)) {
	int childCount = node->GetChildCount();

	Foo(node);

	for (int i = 0; i < childCount; ++i) {
		RecFollowChildNode(node->GetChild(i), Foo);
	}
}

struct Keyframe {
	FbxTime			m_keyTime;
	vector<Bone>	m_BoneList;
	vector<FbxAMatrix> m_LocalTransformMatrixByTime;
};

void DisplayBoneLcl(FbxString name) {
	for (auto p = g_BoneList.begin(); p != g_BoneList.end(); ++p) {
		if (name == p->m_Name) {
			cout << p->m_Name << "\n";
			FbxAMatrix baseMtx = p->m_Node->EvaluateLocalTransform();

			for (int i = 0; i < 40; ++i) {
				FbxString time(i*0.1f);
				time += ": ";
				FbxTime t;
				t.SetSecondDouble(i*0.1f);
				cout << t.GetSecondDouble() << "\n";
				DisplayMatrix(baseMtx * p->m_Node->EvaluateLocalTransform(t).Inverse(), time);
			}
			FbxTime t;
			t.SetSecondDouble(1.3333);
			DisplayMatrix(baseMtx * p->m_Node->EvaluateLocalTransform(t).Inverse(), "1.333333: ");

		}
	}
}

std::set<double> g_KeyTime;



// g_KeyTime이 이미 만들어져 있어야 함.
void MakeToRootTransform1() {
	if (g_KeyTime.empty()) { std::cout << "야! MakeToRootTransform() 하려는데 KeyTime이 비어있다!!\n"; return; }

	int timeCount = 0;

	// 모든 시간에 대하여
	for (auto t = g_KeyTime.begin(); t != g_KeyTime.end(); ++t) {
		// 모든 Bone에 대하여 LocalTransform을 구해줌.
		for (auto b = g_BoneList.begin(); b != g_BoneList.end(); ++b) {
			FbxTime		time;
			FbxAMatrix	mtx;
			time.SetSecondDouble(0.0f);
			mtx = b->m_Node->EvaluateLocalTransform(time);
			//mtx = mtx.Inverse();
			//b->m_ToParentTransforms.push_back(mtx);
		}

		//// 모든 Bone에 대하여 ToRoot를 구해줌.
		//for (auto b = g_BoneList.begin(); b != g_BoneList.end(); ++b) {
		//	FbxAMatrix mtx;
		//	if (b->m_Parent) mtx = b->m_ToParentTransforms[timeCount] * b->m_Parent->m_ToRootTransforms[timeCount];
		//	else mtx = b->m_ToParentTransforms[timeCount];
		//}

		// 다음 시간으로 가자!
		timeCount++;
	}
}









int main(int argc, char** argv)
{
	FbxManager* lSdkManager = NULL;
	FbxScene* lScene = NULL;
	bool lResult;

	InitializeSdkObjects(lSdkManager, lScene);
	FbxString lFilePath("Assets/");
	lFilePath += SAMPLE_FILENAME;
	FBXSDK_printf("\n\nFile: %s\n\n", lFilePath.Buffer());
	lResult = LoadScene(lSdkManager, lScene, lFilePath.Buffer());

	g_KeyTime.insert(1);
	g_KeyTime.insert(2);

	//RecFollowChildNode(lScene->GetRootNode(), DisplayNode);

	RecFollowChildNode(lScene->GetRootNode(), MakeBoneDataTest);
	RecFollowChildNode(lScene->GetRootNode(), MakeParent);
	MakeToRootTransform();
	AllBones(DisplayBone);
	AllBones(MultiplyGlobalAndToRoot);

	//MakeAnimationData(lScene);

	//DisplayBoneLcl("Bip001 R Thigh");
	//DisplayBoneLcl("Bip001 L Thigh");
	MakeToRootTransform1();

	DestroySdkObjects(lSdkManager, lResult);
	return 0;
}
