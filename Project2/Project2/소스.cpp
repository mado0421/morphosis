#include <DirectXMath.h>
#include <d3d12.h>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>

#include <fbxsdk.h>
#include <Common.h>

#include <vector>

const char * SAMPLE_FILENAME = "JointHierarchy.fbx";


void Foo(FbxNode* node) {
	std::cout << node->GetName() << "\n";
}

void RecFollowChildNode(FbxNode* node, void(*Goo)(FbxNode*)) {
	int childCount = node->GetChildCount();

	Goo(node);

	for (int i = 0; i < childCount; ++i) {
		RecFollowChildNode(node->GetChild(i), Goo);
	}
}

int main(int argc, char** argv)
{
	FbxManager* lSdkManager = NULL;
	FbxScene* lScene = NULL;
	bool lResult;

	InitializeSdkObjects(lSdkManager, lScene);
	FbxString lFilePath("");
	if (lFilePath.IsEmpty()) lFilePath = SAMPLE_FILENAME;
	FBXSDK_printf("\n\nFile: %s\n\n", lFilePath.Buffer());
	lResult = LoadScene(lSdkManager, lScene, lFilePath.Buffer());

	RecFollowChildNode(lScene->GetRootNode(), Foo);









	DestroySdkObjects(lSdkManager, lResult);
	return 0;
}
