/****************************************************************************************

   Copyright (C) 2015 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/

#include <fbxsdk.h>
#include <iostream>

#include "../Common/Common.h"

const char * SAMPLE_FILENAME = "test_0429_015_Character.fbx";

void GetAnimationDataRec(FbxAnimLayer* layer, FbxNode* node) {
	int lModelCount;
	FbxAnimCurve *curve = NULL;

	std::cout << node->GetName() << "\n";
	curve = node->LclRotation.GetCurve(layer);

	for (int i = 0; i < curve->KeyGetCount(); ++i) {
		FbxTime time = curve->KeyGetTime(i);
		std::cout << time.GetSecondDouble() << "\n";
	}
	for (lModelCount = 0; lModelCount < node->GetChildCount(); lModelCount++)
	{
		GetAnimationDataRec(layer, node->GetChild(lModelCount));
	}
}

void MakeAnimationData(FbxScene* scene) {
	for (int i = 0; i < scene->GetSrcObjectCount<FbxAnimStack>(); i++)
	{
		FbxAnimStack* lAnimStack = scene->GetSrcObject<FbxAnimStack>(i);
		int l;
		int nbAnimLayers = lAnimStack->GetMemberCount<FbxAnimLayer>();
		for (l = 0; l < nbAnimLayers; l++)
		{
			FbxAnimLayer* lAnimLayer = lAnimStack->GetMember<FbxAnimLayer>(l);
			GetAnimationDataRec(lAnimLayer, scene->GetRootNode());
		}
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

	MakeAnimationData(lScene);

    DestroySdkObjects(lSdkManager, lResult);
    return 0;
}

