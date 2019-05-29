/****************************************************************************************

   Copyright (C) 2015 Autodesk, Inc.
   All rights reserved.

   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.

****************************************************************************************/

/////////////////////////////////////////////////////////////////////////
//
// This example illustrates how to detect if a scene is password 
// protected, import and browse the scene to access node and animation 
// information. It displays the content of the FBX file which name is 
// passed as program argument. You can try it with the various FBX files 
// output by the export examples.
//
/////////////////////////////////////////////////////////////////////////

#include "../Common/Common.h"
#include "DisplayCommon.h"
#include "DisplayHierarchy.h"
#include "DisplayAnimation.h"
#include "DisplayMarker.h"
#include "DisplaySkeleton.h"
#include "DisplayMesh.h"
#include "DisplayNurb.h"
#include "DisplayPatch.h"
#include "DisplayLodGroup.h"
#include "DisplayCamera.h"
#include "DisplayLight.h"
#include "DisplayGlobalSettings.h"
#include "DisplayPose.h"
#include "DisplayPivotsAndLimits.h"
#include "DisplayUserProperties.h"
#include "DisplayGenericInfo.h"
#include <iostream>
#include <set>
#include <vector>

// Local function prototypes.
void DisplayContent(FbxScene* pScene);
void DisplayContent(FbxNode* pNode);
void DisplayTarget(FbxNode* pNode);
void DisplayTransformPropagation(FbxNode* pNode);
void DisplayGeometricTransform(FbxNode* pNode);
void DisplayMetaData(FbxScene* pScene);

static bool gVerbose = true;

struct Bone {
	FbxString	m_Name;
	FbxNode*	m_Node		= NULL;
	Bone*		m_Parent	= NULL;

	FbxAMatrix	m_GlobalTransform;

	std::vector<FbxAMatrix> m_ToParentTransforms;
	std::vector<FbxAMatrix> m_ToRootTransforms;
};

std::vector<Bone> g_BoneList;
std::set<double> g_KeyTime;

/***************************************************************************************
MakeAnimation
***************************************************************************************/

void AnimationData(FbxScene*);
void AnimationNode(FbxNode*, FbxAnimLayer*);
void AnimationCurve(FbxAnimCurve*);

bool IsSkeletonNode(FbxNode* node) {
	int nodeAttributeCount = node->GetNodeAttributeCount();
	for (int i = 0; i < nodeAttributeCount; ++i) {
		FbxNodeAttribute* nodeAttribute = node->GetNodeAttributeByIndex(i);
		if (FbxNodeAttribute::eSkeleton == nodeAttribute->GetAttributeType())
			return true;
	}
	return false;
}

void AnimationData(FbxScene* scene) {
	for (int i = 0; i < scene->GetSrcObjectCount<FbxAnimStack>(); ++i)
	{
		FbxAnimStack* stack = scene->GetSrcObject<FbxAnimStack>(i);
		for (int j = 0; j < stack->GetMemberCount<FbxAnimLayer>(); ++j) {
			FbxAnimLayer* layer = stack->GetMember<FbxAnimLayer>(j);

			AnimationNode(scene->GetRootNode(), layer);
		}
	}
}

void AnimationNode(FbxNode* node, FbxAnimLayer* layer) {
	FbxAnimCurve* curve;

	if (IsSkeletonNode(node)) {
		std::cout << node->GetName() << "\n";

		curve = node->LclRotation.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_X);
		if (curve) AnimationCurve(curve);

		curve = node->LclRotation.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_Y);
		if (curve) AnimationCurve(curve);

		curve = node->LclRotation.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_Z);
		if (curve) AnimationCurve(curve);

		curve = node->LclTranslation.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_X);
		if (curve) AnimationCurve(curve);

		curve = node->LclTranslation.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_Y);
		if (curve) AnimationCurve(curve);

		curve = node->LclTranslation.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_Z);
		if (curve) AnimationCurve(curve);
	}

	for (int i = 0; i < node->GetChildCount(); ++i)
		AnimationNode(node->GetChild(i), layer);
}

void AnimationCurve(FbxAnimCurve* curve) {
	for (int i = 0; i < curve->KeyGetCount(); ++i) {
		FbxTime keyTime = curve->KeyGetTime(i);
		g_KeyTime.insert(keyTime.GetSecondDouble());
	}
}

/***************************************************************************************
MakeBone
***************************************************************************************/

void MakeBoneDataTest(FbxNode* node) {
	if (IsSkeletonNode(node)) {
		Bone tmp;
		tmp.m_Name = node->GetName();
		tmp.m_Node = node;

		FbxAMatrix mtx;
		mtx = node->EvaluateGlobalTransform();
		tmp.m_GlobalTransform = mtx;
		mtx.SetIdentity();

		//mtx = node->EvaluateLocalTransform();
		//mtx = mtx.Inverse();
		//tmp.m_ToParentTransform = mtx;

		g_BoneList.push_back(tmp);
	}
}

void MakeParent(FbxNode* node) {
	if (IsSkeletonNode(node)) {
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

void RecFollowChildNode(FbxNode* node, void(*Foo)(FbxNode*)) {
	int childCount = node->GetChildCount();

	Foo(node);

	for (int i = 0; i < childCount; ++i) {
		RecFollowChildNode(node->GetChild(i), Foo);
	}
}

/***************************************************************************************
g_BoneList
***************************************************************************************/

// g_KeyTime이 이미 만들어져 있어야 함.
void MakeToRootTransform() {
	if (g_KeyTime.empty()) { std::cout << "야! MakeToRootTransform() 하려는데 KeyTime이 비어있다!!\n"; return; }

	int timeCount = 0;

	// 모든 시간에 대하여
	for (auto t = g_KeyTime.begin(); t != g_KeyTime.end(); ++t) {
		// 모든 Bone에 대하여 LocalTransform을 구해줌.
		for (auto b = g_BoneList.begin(); b != g_BoneList.end(); ++b) {
			FbxTime		time;
			FbxAMatrix	mtx;
			time.SetSecondDouble((*t));
			mtx = b->m_Node->EvaluateLocalTransform(time);
			mtx = mtx.Inverse();
			b->m_ToParentTransforms.push_back(mtx);
		}

		// 모든 Bone에 대하여 ToRoot를 구해줌.
		for (auto b = g_BoneList.begin(); b != g_BoneList.end(); ++b) {
			FbxAMatrix mtx;
			if (b->m_Parent) mtx = b->m_ToParentTransforms[timeCount] * b->m_Parent->m_ToRootTransforms[timeCount];
			else mtx = b->m_ToParentTransforms[timeCount];
			b->m_ToRootTransforms.push_back(mtx);
		}

		// 다음 시간으로 가자!
		timeCount++;
	}
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

void DisplayAllBones() {
	std::cout << g_BoneList.size() << "\n";


	for (auto b = g_BoneList.begin(); b != g_BoneList.end(); ++b) {
		std::cout << b->m_Name << "\n";

		DisplayMatrix(b->m_GlobalTransform, "Offset");

		std::cout << "\n";

		for (unsigned int i = 0; i < g_KeyTime.size(); ++i) {
			std::cout << "#" << i << "\n";
			DisplayMatrix(b->m_ToParentTransforms[i], "ToParent");
			DisplayMatrix(b->m_ToRootTransforms[i], "ToRoot");
		}
	}
}





const char * SAMPLE_FILENAME = "test_0429_015_Character.fbx";


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

	AnimationData(lScene);

	RecFollowChildNode(lScene->GetRootNode(), MakeBoneDataTest);
	RecFollowChildNode(lScene->GetRootNode(), MakeParent);

	MakeToRootTransform();

	DisplayAllBones();



    DestroySdkObjects(lSdkManager, lResult);

	//for (auto t = g_KeyTime.begin(); t != g_KeyTime.end(); ++t) {
	//	std::cout << *t << "\n";
	//}

    return 0;
}

void DisplayContent(FbxScene* pScene)
{
    int i;
    FbxNode* lNode = pScene->GetRootNode();

    if(lNode)
    {
        for(i = 0; i < lNode->GetChildCount(); i++)
        {
            DisplayContent(lNode->GetChild(i));
        }
    }
}

void DisplayContent(FbxNode* pNode)
{
    FbxNodeAttribute::EType lAttributeType;
    int i;

    if(pNode->GetNodeAttribute() == NULL)
    {
        FBXSDK_printf("NULL Node Attribute\n\n");
    }
    else
    {
        lAttributeType = (pNode->GetNodeAttribute()->GetAttributeType());

        switch (lAttributeType)
        {
	    default:
	        break;
        case FbxNodeAttribute::eMarker:  
            DisplayMarker(pNode);
            break;

        case FbxNodeAttribute::eSkeleton:  
            DisplaySkeleton(pNode);
            break;

        case FbxNodeAttribute::eMesh:      
            DisplayMesh(pNode);
            break;

        case FbxNodeAttribute::eNurbs:      
            DisplayNurb(pNode);
            break;

        case FbxNodeAttribute::ePatch:     
            DisplayPatch(pNode);
            break;

        case FbxNodeAttribute::eCamera:    
            DisplayCamera(pNode);
            break;

        case FbxNodeAttribute::eLight:     
            DisplayLight(pNode);
            break;

        case FbxNodeAttribute::eLODGroup:
            DisplayLodGroup(pNode);
            break;
        }   
    }

    DisplayUserProperties(pNode);
    DisplayTarget(pNode);
    DisplayPivotsAndLimits(pNode);
    DisplayTransformPropagation(pNode);
    DisplayGeometricTransform(pNode);

    for(i = 0; i < pNode->GetChildCount(); i++)
    {
        DisplayContent(pNode->GetChild(i));
    }
}


void DisplayTarget(FbxNode* pNode)
{
    if(pNode->GetTarget() != NULL)
    {
        DisplayString("    Target Name: ", (char *) pNode->GetTarget()->GetName());
    }
}

void DisplayTransformPropagation(FbxNode* pNode)
{
    FBXSDK_printf("    Transformation Propagation\n");

    // 
    // Rotation Space
    //
    EFbxRotationOrder lRotationOrder;
    pNode->GetRotationOrder(FbxNode::eSourcePivot, lRotationOrder);

    FBXSDK_printf("        Rotation Space: ");

    switch (lRotationOrder)
    {
    case eEulerXYZ: 
        FBXSDK_printf("Euler XYZ\n");
        break;
    case eEulerXZY:
        FBXSDK_printf("Euler XZY\n");
        break;
    case eEulerYZX:
        FBXSDK_printf("Euler YZX\n");
        break;
    case eEulerYXZ:
        FBXSDK_printf("Euler YXZ\n");
        break;
    case eEulerZXY:
        FBXSDK_printf("Euler ZXY\n");
        break;
    case eEulerZYX:
        FBXSDK_printf("Euler ZYX\n");
        break;
    case eSphericXYZ:
        FBXSDK_printf("Spheric XYZ\n");
        break;
    }

    //
    // Use the Rotation space only for the limits
    // (keep using eEulerXYZ for the rest)
    //
    FBXSDK_printf("        Use the Rotation Space for Limit specification only: %s\n",
        pNode->GetUseRotationSpaceForLimitOnly(FbxNode::eSourcePivot) ? "Yes" : "No");


    //
    // Inherit Type
    //
    FbxTransform::EInheritType lInheritType;
    pNode->GetTransformationInheritType(lInheritType);

    FBXSDK_printf("        Transformation Inheritance: ");

    switch (lInheritType)
    {
    case FbxTransform::eInheritRrSs:
        FBXSDK_printf("RrSs\n");
        break;
    case FbxTransform::eInheritRSrs:
        FBXSDK_printf("RSrs\n");
        break;
    case FbxTransform::eInheritRrs:
        FBXSDK_printf("Rrs\n");
        break;
    }
}

void DisplayGeometricTransform(FbxNode* pNode)
{
    FbxVector4 lTmpVector;

    FBXSDK_printf("    Geometric Transformations\n");

    //
    // Translation
    //
    lTmpVector = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
    FBXSDK_printf("        Translation: %f %f %f\n", lTmpVector[0], lTmpVector[1], lTmpVector[2]);

    //
    // Rotation
    //
    lTmpVector = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
    FBXSDK_printf("        Rotation:    %f %f %f\n", lTmpVector[0], lTmpVector[1], lTmpVector[2]);

    //
    // Scaling
    //
    lTmpVector = pNode->GetGeometricScaling(FbxNode::eSourcePivot);
    FBXSDK_printf("        Scaling:     %f %f %f\n", lTmpVector[0], lTmpVector[1], lTmpVector[2]);
}


void DisplayMetaData(FbxScene* pScene)
{
    FbxDocumentInfo* sceneInfo = pScene->GetSceneInfo();
    if (sceneInfo)
    {
        FBXSDK_printf("\n\n--------------------\nMeta-Data\n--------------------\n\n");
        FBXSDK_printf("    Title: %s\n", sceneInfo->mTitle.Buffer());
        FBXSDK_printf("    Subject: %s\n", sceneInfo->mSubject.Buffer());
        FBXSDK_printf("    Author: %s\n", sceneInfo->mAuthor.Buffer());
        FBXSDK_printf("    Keywords: %s\n", sceneInfo->mKeywords.Buffer());
        FBXSDK_printf("    Revision: %s\n", sceneInfo->mRevision.Buffer());
        FBXSDK_printf("    Comment: %s\n", sceneInfo->mComment.Buffer());

        FbxThumbnail* thumbnail = sceneInfo->GetSceneThumbnail();
        if (thumbnail)
        {
            FBXSDK_printf("    Thumbnail:\n");

            switch (thumbnail->GetDataFormat())
            {
            case FbxThumbnail::eRGB_24:
                FBXSDK_printf("        Format: RGB\n");
                break;
            case FbxThumbnail::eRGBA_32:
                FBXSDK_printf("        Format: RGBA\n");
                break;
            }

            switch (thumbnail->GetSize())
            {
	        default:
	            break;
            case FbxThumbnail::eNotSet:
                FBXSDK_printf("        Size: no dimensions specified (%ld bytes)\n", thumbnail->GetSizeInBytes());
                break;
            case FbxThumbnail::e64x64:
                FBXSDK_printf("        Size: 64 x 64 pixels (%ld bytes)\n", thumbnail->GetSizeInBytes());
                break;
            case FbxThumbnail::e128x128:
                FBXSDK_printf("        Size: 128 x 128 pixels (%ld bytes)\n", thumbnail->GetSizeInBytes());
            }
        }
    }
}

