#include <iostream>
#include <fbxsdk.h>

#include "Common.h"

const char * SAMPLE_FILENAME = "test_0429_015_Character.fbx";


void Foo(FbxNode* node) {

	int nodeAttributeCount = node->GetNodeAttributeCount();
	for (int i = 0; i < nodeAttributeCount; ++i) {
		FbxNodeAttribute* nodeAttribute = node->GetNodeAttributeByIndex(i);
		if (FbxNodeAttribute::eSkeleton == nodeAttribute->GetAttributeType()) {
			std::cout << node->GetName() << "\n";
			FbxAMatrix mtx = node->EvaluateLocalTransform();
			
		}
	}
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
	FbxString lFilePath("Assets/");
	lFilePath += SAMPLE_FILENAME;
	FBXSDK_printf("\n\nFile: %s\n\n", lFilePath.Buffer());
	lResult = LoadScene(lSdkManager, lScene, lFilePath.Buffer());

	RecFollowChildNode(lScene->GetRootNode(), Foo);









	DestroySdkObjects(lSdkManager, lResult);
	return 0;
}
