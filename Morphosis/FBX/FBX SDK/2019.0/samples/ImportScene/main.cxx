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
#include <list>
#include <vector>

// Local function prototypes.
void DisplayContent(FbxScene* pScene);
void DisplayContent(FbxNode* pNode);
void DisplayTarget(FbxNode* pNode);
void DisplayTransformPropagation(FbxNode* pNode);
void DisplayGeometricTransform(FbxNode* pNode);
void DisplayMetaData(FbxScene* pScene);


namespace K {
	enum {
		T = 0,
		R,

		X = 0,
		Y,
		Z
	};
}

struct Int4 { int x, y, z, w; };
struct Float2 { float x, y; };
struct Float3 {	float x, y, z; };
struct Float4 { float x, y, z, w; };

struct Bone {
	std::string name;
	Float3		LclTranslation;
	Float3		LclRotation;
	Bone*		parent = NULL;

	Bone& operator=(const Bone& other) {
		this->name				= other.name;
		this->LclTranslation	= other.LclTranslation;
		this->LclRotation		= other.LclRotation;
		this->parent			= other.parent;
		return *this;
	}

	bool operator==(const Bone& other) {
		if (this->name != other.name) return false;
		//if (this->LclTranslation != other.LclTranslation) return false;
		return true;
	}
};

struct KeyframeData {
	/* 어떤 bone의 어떤 Component가 언제 어떤 값인지 저장해야 함. */
	Bone	b;
	Float3	t;
	Float3	r;

	KeyframeData(Bone& bone, int Component, int Axis, float value) {
		b = bone;
		AddValue(Component, Axis, value);
	}

	void AddValue(int Component, int Axis, float value) {
		switch (Component) {
		case K::T:
			switch (Axis) {
			case K::X: t.x = value; break;
			case K::Y: t.y = value; break;
			case K::Z: t.z = value; break;

			default: break;
			}
			break;
		case K::R:
			switch (Axis) {
			case K::X: r.x = value; break;
			case K::Y: r.y = value; break;
			case K::Z: r.z = value; break;

			default: break;
			}
			break;
		default: break;
		}
	}
};

struct Key {
	/* 시간별로 저장하자. */
	float time;
	std::list<KeyframeData> data;

	//bool operator<(const Key& other) const {
	//	return (this->time < other.time);
	//}

	Key(float time, Bone& bone, int Component, int Axis, float value) {
		this->time = time;
		Add(bone, Component, Axis, value);
	}

	void Add(Bone& bone, int Component, int Axis, float value) {
		for (auto p = data.begin(); p != data.end(); ++p) {
			if (p->b == bone) {
				p->AddValue(Component, Axis, value);
				return;
			}
		}
		/* 여기까지 왔다 -> 같은 bone이 없었다 -> 그러니 추가해주자. */
		data.emplace_back(bone, Component, Axis, value);
	}
};

struct AnimationData {
	std::vector<Key> KeySet;


	void Add(float time, Bone& bone, int Component, int Axis, float value) {
		for (auto p = KeySet.begin(); p != KeySet.end(); ++p) {
			if (p->time == time) {
				/**********************************************************
				여기 지금 stl 컨테이너에 직접 값을 수정하려고 해서 문제가 생기는거 같음
				저걸 아 어캐 해 하여간 여기까지 했고 스터디 들으러 가자.
				**********************************************************/
				p->Add(bone, Component, Axis, value);
				//for (auto d = p->data.begin(); d != p->data.end(); ++d) {
				//	if (d->b == bone) {
				//		d->AddValue(Component, Axis, value);
				//		return;
				//	}
				//}
				return;
			}
		}
		KeySet.emplace_back(time, bone, Component, Axis, value);
	}
	void Print() {
		for (Key k : KeySet) {
			std::cout << "[" << k.time << "]\n";
			for (KeyframeData d : k.data) {
				std::cout << d.b.name.c_str() << "\t - t ("
					<< d.t.x << ", " << d.t.y << ", " << d.t.z << ") ,r ("
					<< d.r.x << ", " << d.r.y << ", " << d.r.z << ")\n";
			}
			std::cout << "\n";
		}
	}
};

struct Vertex {
	Float3	pos;
	Int4	boneIdx;
	Float4	weight;
};

struct GeometryData {
	std::vector<Vertex> controlPoints;
	std::vector<Float2> UVs;
	std::vector<Float3> normals;
	std::vector<int>	posIdx;
	std::vector<int>	uvIdx;
};

static bool gVerbose = true;
AnimationData animData;

void GetCurve(FbxAnimCurve* lAnimCurve, FbxNode* pNode, int com, int axi) {
	int			lKeyCount = lAnimCurve->KeyGetCount();
	FbxTime		lKeyTime;
	float		lKeyValue;
	int			lCount;

	for (lCount = 0; lCount < lKeyCount; lCount++)
	{
		lKeyValue = static_cast<float>(lAnimCurve->KeyGetValue(lCount));
		lKeyTime = lAnimCurve->KeyGetTime(lCount);
		lKeyTime.GetSecondDouble();

		/*****************************************************************
		여기서 잠시 pNode로 Bone을 만드는 작업이 있을 것.
		근데 이건 이 함수에서 할 필요는 없으니까 이후에 옮겨주자.
		*****************************************************************/
		FbxVector4 lTmpVector;
		Bone tmp;

		tmp.name = pNode->GetName();

		lTmpVector = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
		tmp.LclTranslation.x = lTmpVector[0];
		tmp.LclTranslation.y = lTmpVector[1];
		tmp.LclTranslation.z = lTmpVector[2];
		lTmpVector = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
		tmp.LclRotation.x = lTmpVector[0];
		tmp.LclRotation.y = lTmpVector[1];
		tmp.LclRotation.z = lTmpVector[2];

		//tmp.parent = pNode->GetParent();

		animData.Add(lKeyTime.GetSecondDouble(), tmp, com, axi, lKeyValue);

	}
}

void GetComponent(FbxAnimLayer* pAnimLayer, FbxNode* pNode) {
	FbxAnimCurve* lAnimCurve = NULL;
	lAnimCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
	if (lAnimCurve) GetCurve(lAnimCurve, pNode, K::T, K::X);

	lAnimCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
	if (lAnimCurve) GetCurve(lAnimCurve, pNode, K::T, K::Y);

	lAnimCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
	if (lAnimCurve) GetCurve(lAnimCurve, pNode, K::T, K::Z);

	lAnimCurve = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
	if (lAnimCurve) GetCurve(lAnimCurve, pNode, K::R, K::X);

	lAnimCurve = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
	if (lAnimCurve) GetCurve(lAnimCurve, pNode, K::R, K::Y);

	lAnimCurve = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
	if (lAnimCurve) GetCurve(lAnimCurve, pNode, K::R, K::Z);
}

void GetAnimationDataRec(FbxAnimLayer* pAnimLayer, FbxNode* pNode) {
	int lModelCount;

	GetComponent(pAnimLayer, pNode);

	for (lModelCount = 0; lModelCount < pNode->GetChildCount(); lModelCount++)
	{
		GetAnimationDataRec(pAnimLayer, pNode->GetChild(lModelCount));
	}
}


void GetMeshData(FbxNode* pNode) {
	//int meshCount;
	//FbxMesh* pMesh = pNode->GetMesh();

	//if (NULL != pMesh) {
	//	int nControlPoints = pMesh->GetControlPointsCount();
	//	int nElementNormals = pMesh->GetElementNormalCount();
	//	FbxVector4* pControlPoints = pMesh->GetControlPoints();

	//	std::cout << pNode->GetName() << "\n"
	//		<< nControlPoints << "개\n\n";
	//	//pNode->GetGeometry()->GetL


	//	for (int i = 0; i < nControlPoints; ++i) {
	//		FbxVector4 fbxVector4 = pControlPoints[i];
	//		for (int j = 0; j < nElementNormals; ++j) {
	//			FbxGeometryElementNormal* pNormals = pMesh->GetElementNormal(j);
	//			if (pNormals->GetMappingMode() == FbxGeometryElement::eByControlPoint) {
	//				if (pNormals->GetReferenceMode() == FbxGeometryElement::eDirect) {
	//					fbxVector4 = pNormals->GetDirectArray().GetAt(i);

	//					/* ... */

	//					std::cout << "("
	//						<< fbxVector4.mData[0] << ", "
	//						<< fbxVector4.mData[1] << ", "
	//						<< fbxVector4.mData[2] << ", "
	//						<< fbxVector4.mData[3] << ")\n";

	//				}
	//			}
	//		}
	//	}

	//	int nPolygons = pMesh->GetPolygonCount();
	//	for (int i = 0, nIdx = 0; i < nPolygons; ++i) {
	//		FbxVector4 fbxVector4;
	//		int nPolygonSize = pMesh->GetPolygonSize(i);
	//		for (int j = 0; j < nPolygonSize; ++j) {
	//			int nControlPointIdx = pMesh->GetPolygonVertex(i, j);
	//			fbxVector4 = pControlPoints[nControlPointIdx];

	//			/* ... */
	//			//std::cout
	//			//	<< fbxVector4.mData[0] << ", "
	//			//	<< fbxVector4.mData[1] << ", "
	//			//	<< fbxVector4.mData[2] << ", "
	//			//	<< fbxVector4.mData[3] << "\n";
	//		}
	//	}


	//}

	//for (meshCount = 0; meshCount < pNode->GetChildCount(); meshCount++)
	//{
	//	GetMeshData(pNode->GetChild(meshCount));
	//}
}


void GetClusterData(FbxNode* pNode) {
	int meshCount;
	FbxNodeAttribute::EType type;
	FbxGeometry* geo;
	FbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();
	if (NULL != lNodeAttribute) {
		type = lNodeAttribute->GetAttributeType();
		if (FbxNodeAttribute::eMesh == type) {
			geo = pNode->GetGeometry();
			
			int nSkinDeformers = geo->GetDeformerCount(FbxDeformer::eSkin);
			for (int i = 0; i < nSkinDeformers; ++i) {
				FbxSkin* skinDeformer = (FbxSkin*)(geo->GetDeformer(i, FbxDeformer::eSkin));
				int nClusters = skinDeformer->GetClusterCount();
				for (int j = 0; j < nClusters; ++j) {
					FbxCluster* cluster = skinDeformer->GetCluster(j);
					int nIdx = cluster->GetControlPointIndicesCount();
					int* pIdx = cluster->GetControlPointIndices();
					double* pWeights = cluster->GetControlPointWeights();

					

					/* ... */
					std::cout << pNode->GetName() << " - " << cluster->GetLink()->GetName() << ", " << nIdx << "\n";

				}
			}
		}
	}
	for (meshCount = 0; meshCount < pNode->GetChildCount(); meshCount++)
	{
		GetClusterData(pNode->GetChild(meshCount));
	}
}

int main(int argc, char** argv)
{
    FbxManager* lSdkManager = NULL;
    FbxScene* lScene = NULL;
    bool lResult;

    // Prepare the FBX SDK.
    InitializeSdkObjects(lSdkManager, lScene);
    // Load the scene.

    // The example can take a FBX file as an argument.
	FbxString lFilePath("animation_character_0.FBX");
	for( int i = 1, c = argc; i < c; ++i )
	{
		if( FbxString(argv[i]) == "-test" ) gVerbose = false;
		else if( lFilePath.IsEmpty() ) lFilePath = argv[i];
	}

	if( lFilePath.IsEmpty() )
	{
        lResult = false;
        FBXSDK_printf("\n\nUsage: ImportScene <FBX file name>\n\n");
	}
	else
	{
		FBXSDK_printf("\n\nFile: %s\n\n", lFilePath.Buffer());
		lResult = LoadScene(lSdkManager, lScene, lFilePath.Buffer());
	}

    if(lResult == false)
    {
        FBXSDK_printf("\n\nAn error occurred while loading the scene...");
    }
    else 
    {
        // Display the scene.
        //DisplayMetaData(lScene);
        //FBXSDK_printf("\n\n---------------------\nGlobal Light Settings\n---------------------\n\n");
        //if( gVerbose ) DisplayGlobalLightSettings(&lScene->GetGlobalSettings());
        //FBXSDK_printf("\n\n----------------------\nGlobal Camera Settings\n----------------------\n\n");
        //if( gVerbose ) DisplayGlobalCameraSettings(&lScene->GetGlobalSettings());
        //FBXSDK_printf("\n\n--------------------\nGlobal Time Settings\n--------------------\n\n");
        //if( gVerbose ) DisplayGlobalTimeSettings(&lScene->GetGlobalSettings());
        //FBXSDK_printf("\n\n---------\nHierarchy\n---------\n\n");
        //if( gVerbose ) DisplayHierarchy(lScene);
        //FBXSDK_printf("\n\n------------\nNode Content\n------------\n\n");
        //if( gVerbose ) DisplayContent(lScene);
        //FBXSDK_printf("\n\n----\nPose\n----\n\n");
        //if( gVerbose ) DisplayPose(lScene);
 /*       FBXSDK_printf("\n\n---------\nAnimation\n---------\n\n");
        if( gVerbose ) DisplayAnimation(lScene);*/
        ////now display generic information
        //FBXSDK_printf("\n\n---------\nGeneric Information\n---------\n\n");
        //if( gVerbose ) DisplayGenericInfo(lScene);

		/*****************************************************************
		먼저 Geometry Data를 얻어야 함.

		이거 또 재귀로 해줘야됨
		*****************************************************************/
		//FbxNodeAttribute::EType type;
		//FbxNode* tmp = lScene->GetRootNode();
		//FbxGeometry* geo = tmp->GetGeometry();
		//type = tmp->GetNodeAttribute()->GetAttributeType();
		//if (FbxNodeAttribute::eMesh == type) {
		//	int nSkinDeformers = geo->GetDeformerCount(FbxDeformer::eSkin);
		//	for (int i = 0; i < nSkinDeformers; ++i) {
		//		FbxSkin* skinDeformer = (FbxSkin*)(geo->GetDeformer(i, FbxDeformer::eSkin));
		//		int nClusters = skinDeformer->GetClusterCount();
		//		for (int j = 0; j < nClusters; ++j) {
		//			FbxCluster* cluster = skinDeformer->GetCluster(j);
		//			int nIdx = cluster->GetControlPointIndicesCount();
		//			int* pIdx = cluster->GetControlPointIndices();
		//			double* pWeights = cluster->GetControlPointWeights();
		//		}
		//	}
		//}
		//for (int i = 0; i < tmp->GetChildCount(); ++i) {
		//}

		//GetClusterData(lScene->GetRootNode());

		GetMeshData(lScene->GetRootNode());




		/*****************************************************************
		Animation Data를 얻는 부분
		*****************************************************************/
		//for (int i = 0; i < lScene->GetSrcObjectCount<FbxAnimStack>(); i++)
		//{
		//	FbxAnimStack* lAnimStack = lScene->GetSrcObject<FbxAnimStack>(i);
		//	int l;
		//	int nbAnimLayers = lAnimStack->GetMemberCount<FbxAnimLayer>();
		//	for (l = 0; l < nbAnimLayers; l++)
		//	{
		//		FbxAnimLayer* lAnimLayer = lAnimStack->GetMember<FbxAnimLayer>(l);
		//		GetAnimationDataRec(lAnimLayer, lScene->GetRootNode());
		//	}
		//}
		//animData.Print();


    }

    // Destroy all objects created by the FBX SDK.
    DestroySdkObjects(lSdkManager, lResult);

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

