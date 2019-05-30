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
#include <fstream>
#include <DirectXMath.h>

using namespace DirectX;

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
bool IsMeshNode(FbxNode* node) {
	int nodeAttributeCount = node->GetNodeAttributeCount();
	for (int i = 0; i < nodeAttributeCount; ++i) {
		FbxNodeAttribute* nodeAttribute = node->GetNodeAttributeByIndex(i);
		if (FbxNodeAttribute::eMesh == nodeAttribute->GetAttributeType())
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

/***************************************************************************************
Export
***************************************************************************************/

XMFLOAT4X4 MakeXMFloat4x4FromFbxAMatrix(const FbxAMatrix& fbxMtx) {
	XMFLOAT4X4 xmf4x4Mtx;

	for (int i = 0; i < 4; ++i)
	{
		FbxVector4 vec = fbxMtx.GetRow(i);

		xmf4x4Mtx.m[i][0] = (float)vec[0];
		xmf4x4Mtx.m[i][1] = (float)vec[1];
		xmf4x4Mtx.m[i][2] = (float)vec[2];
		xmf4x4Mtx.m[i][3] = (float)vec[3];
	}
	
	return xmf4x4Mtx;
}

void ExportFile(const char* fileName, const char* animName) {
	std::ofstream out;

	out.open(fileName, std::ios::out | std::ios::binary);
	char AnimName[32];
	strcpy_s(AnimName, sizeof(AnimName), animName);
	out.write((char*)&AnimName, sizeof(AnimName));

	int NumKeyTime = g_KeyTime.size();
	out.write((char*)&NumKeyTime, sizeof(int));

	for (auto p = g_KeyTime.begin(); p != g_KeyTime.end(); ++p) {
		out.write((char*)&(*p), sizeof(double));
	}

	int NumBone = g_BoneList.size();
	out.write((char*)&NumBone, sizeof(int));

	for (int i = 0; i < g_BoneList.size(); ++i) {
		char name[32];
		strcpy_s(name, sizeof(name), g_BoneList[i].m_Name.Buffer());
		out.write((char*)&name, sizeof(name));

		XMFLOAT4X4 xmf4x4Temp = MakeXMFloat4x4FromFbxAMatrix(g_BoneList[i].m_GlobalTransform);
		out.write((char*)&xmf4x4Temp, sizeof(XMFLOAT4X4));
	}

	for (int i = 0; i < g_BoneList.size(); ++i) {
		for (int j = 0; j < NumKeyTime; ++j) {
			XMFLOAT4X4 xmf4x4Temp = MakeXMFloat4x4FromFbxAMatrix(g_BoneList[i].m_ToRootTransforms[j]);
			out.write((char*)&xmf4x4Temp, sizeof(XMFLOAT4X4));
		}
	}

	out.close();

}

/***************************************************************************************
Mesh
***************************************************************************************/

struct Mesh {
	std::vector<CtrlPoint>	m_CtrlPointList;
	std::vector<Vertex>		m_VertexList;
};

std::vector<Mesh> g_MeshList;

struct CtrlPoint {
	XMFLOAT3	xmf3Position;
	XMINT4		xmi4BoneIdx;
	XMFLOAT4	xmf4BoneWeight;
};

struct Vertex {
	int			ctrlPointIdx;
	XMFLOAT3	xmf3Normal;
	XMFLOAT3	xmf3BiNormal;
	XMFLOAT3	xmf3Tangent;
};

//mesh->GetControlPoints();
//mesh->GetControlPointsCount();
//mesh->GetPolygonVertex();
//mesh->GetPolygonCount();
//mesh->GetPolygonSize();
//mesh->GetPolygonVertexCount();
//mesh->GetPolygonVertexIndex();


void ReadNormal(FbxMesh* mesh, int CtrlPointIdx, int VertexCounter, XMFLOAT3& out) {
	FbxGeometryElementNormal* vertexNormal = mesh->GetElementNormal(0);
	switch (vertexNormal->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch (vertexNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{

			out.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(CtrlPointIdx).mData[0]);
			out.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(CtrlPointIdx).mData[1]);
			out.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(CtrlPointIdx).mData[2]);
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{

			int index = vertexNormal->GetIndexArray().GetAt(CtrlPointIdx);
			out.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
			out.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
			out.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
		}
		break;
		default:
			break;
		}
		break;
	case FbxGeometryElement::eByPolygonVertex:
		switch (vertexNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{

			out.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(VertexCounter).mData[0]);
			out.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(VertexCounter).mData[1]);
			out.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(VertexCounter).mData[2]);
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{

			int index = vertexNormal->GetIndexArray().GetAt(VertexCounter);
			out.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
			out.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
			out.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
		}
		break;
		default:
			break;
		}
		break;


	default: break;
	}
}
void ReadBinormal(FbxMesh* mesh, int CtrlPointIdx, int VertexCounter, XMFLOAT3& out) {
	FbxGeometryElementBinormal* vertexBinormal = mesh->GetElementBinormal(0);
	switch (vertexBinormal->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch (vertexBinormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{

			out.x = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(CtrlPointIdx).mData[0]);
			out.y = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(CtrlPointIdx).mData[1]);
			out.z = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(CtrlPointIdx).mData[2]);
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{

			int index = vertexBinormal->GetIndexArray().GetAt(CtrlPointIdx);
			out.x = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[0]);
			out.y = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[1]);
			out.z = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[2]);
		}
		break;
		default:
			break;
		}
		break;
	case FbxGeometryElement::eByPolygonVertex:
		switch (vertexBinormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{

			out.x = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(VertexCounter).mData[0]);
			out.y = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(VertexCounter).mData[1]);
			out.z = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(VertexCounter).mData[2]);
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{

			int index = vertexBinormal->GetIndexArray().GetAt(VertexCounter);
			out.x = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[0]);
			out.y = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[1]);
			out.z = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[2]);
		}
		break;
		default:
			break;
		}
		break;


	default: break;
	}
}
void ReadTangent(FbxMesh* mesh, int CtrlPointIdx, int VertexCounter, XMFLOAT3& out) {
	FbxGeometryElementTangent* vertexTangent = mesh->GetElementTangent(0);
	switch (vertexTangent->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch (vertexTangent->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{

			out.x = static_cast<float>(vertexTangent->GetDirectArray().GetAt(CtrlPointIdx).mData[0]);
			out.y = static_cast<float>(vertexTangent->GetDirectArray().GetAt(CtrlPointIdx).mData[1]);
			out.z = static_cast<float>(vertexTangent->GetDirectArray().GetAt(CtrlPointIdx).mData[2]);
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{

			int index = vertexTangent->GetIndexArray().GetAt(CtrlPointIdx);
			out.x = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[0]);
			out.y = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[1]);
			out.z = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[2]);
		}
		break;
		default:
			break;
		}
		break;
	case FbxGeometryElement::eByPolygonVertex:
		switch (vertexTangent->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{

			out.x = static_cast<float>(vertexTangent->GetDirectArray().GetAt(VertexCounter).mData[0]);
			out.y = static_cast<float>(vertexTangent->GetDirectArray().GetAt(VertexCounter).mData[1]);
			out.z = static_cast<float>(vertexTangent->GetDirectArray().GetAt(VertexCounter).mData[2]);
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{

			int index = vertexTangent->GetIndexArray().GetAt(VertexCounter);
			out.x = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[0]);
			out.y = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[1]);
			out.z = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[2]);
		}
		break;
		default:
			break;
		}
		break;


	default: break;
	}
}

void MakeMesh(FbxNode* node) {
	if (IsMeshNode(node)) {
		FbxMesh* mesh = node->GetMesh();
		Mesh tmpMesh;
		unsigned int numVertex = mesh->GetPolygonCount();

		// CtrlPoints
		int			nCtrlPoints = mesh->GetControlPointsCount();
		FbxVector4* pCtrlPoints = mesh->GetControlPoints();
		tmpMesh.m_CtrlPointList.resize(nCtrlPoints);
		for (int i = 0; i < nCtrlPoints; ++i) {
			tmpMesh.m_CtrlPointList[i].xmf3Position.x = pCtrlPoints[i].mData[0];
			tmpMesh.m_CtrlPointList[i].xmf3Position.y = pCtrlPoints[i].mData[1];
			tmpMesh.m_CtrlPointList[i].xmf3Position.z = pCtrlPoints[i].mData[2];
		}

		// PolygonVertex
		int			vertexCounter = 0;
		int			nPolygon = mesh->GetPolygonCount();
		for (int i = 0; i < nPolygon; ++i) {
			int		nPolygonSize = mesh->GetPolygonSize(i);
			for (int j = 0; j < nPolygonSize; ++j) {
				int CtrlPointIdx = mesh->GetPolygonVertex(i, j);
				Vertex tmpVertex;
				XMFLOAT3 xmf3Tmp;
				tmpVertex.ctrlPointIdx = CtrlPointIdx;
				// 이걸로 Vertex는 CtrlPointIdx를 갖게 됨.
				// 아직 Normal, BiNormal, Tangent 남음

				ReadNormal(mesh, CtrlPointIdx, vertexCounter, xmf3Tmp);
				tmpVertex.xmf3Normal = xmf3Tmp;

				ReadBinormal(mesh, CtrlPointIdx, vertexCounter, xmf3Tmp);
				tmpVertex.xmf3BiNormal = xmf3Tmp;

				ReadTangent(mesh, CtrlPointIdx, vertexCounter, xmf3Tmp);
				tmpVertex.xmf3Tangent = xmf3Tmp;

				tmpMesh.m_VertexList.push_back(tmpVertex);
				vertexCounter++;
			}
		}



	}
}





void GetMeshDataRec(FbxNode* pNode) {
	int meshCount;
	FbxMesh* mesh = pNode->GetMesh();
	if (NULL != mesh) {

		int nControlPoints	= mesh->GetControlPointsCount();
		int nElementNormals = mesh->GetElementNormalCount();

		FbxVector4* pControlPoints = mesh->GetControlPoints();
		FbxVector4 fbxVector4;


		int nPolygons = mesh->GetPolygonCount();
		int meshIdx = meshes.size();
		XMFLOAT3 p;

		meshes.emplace_back(pNode->GetName());

		for (int i = 0; i < nControlPoints; ++i) {
			fbxVector4 = pControlPoints[i];
			p.x = fbxVector4.mData[0];
			p.y = fbxVector4.mData[1];
			p.z = fbxVector4.mData[2];
			meshes[meshIdx].controlPoints.emplace_back(p);
		}

		int nPolygon = pMesh->GetPolygonCount();
		for (int i = 0, nIdx = 0; i < nPolygons; ++i) {
			FbxVector4 fbxVector4;
			int nPolygonSize = pMesh->GetPolygonSize(i);
			for (int j = 0; j < nPolygonSize; ++j) {
				int nControlPointIdx = pMesh->GetPolygonVertex(i, j);
				meshes[meshIdx].polygonVertexIndex.push_back(nControlPointIdx);
			}
		}
	}
	for (meshCount = 0; meshCount < pNode->GetChildCount(); meshCount++)
	{
		GetMeshDataRec(pNode->GetChild(meshCount));
	}
}


























const char * SAMPLE_FILENAME = "test_0429_015_Character";


int main(int argc, char** argv)
{
	FbxManager* lSdkManager = NULL;
	FbxScene* lScene = NULL;
	bool lResult;

	InitializeSdkObjects(lSdkManager, lScene);
	FbxString lFilePath("Assets/");
	lFilePath += SAMPLE_FILENAME;

	FbxString lFileInput;

	lFileInput += lFilePath;
	lFileInput += ".fbx";

	FBXSDK_printf("\n\nFile: %s\n\n", lFileInput.Buffer());
	lResult = LoadScene(lSdkManager, lScene, lFileInput.Buffer());

	AnimationData(lScene);

	RecFollowChildNode(lScene->GetRootNode(), MakeBoneDataTest);
	RecFollowChildNode(lScene->GetRootNode(), MakeParent);

	MakeToRootTransform();
 
	//DisplayAllBones();

	FbxString lFileOutput;

	lFileOutput += lFilePath;
	lFileOutput += "_anim.dat";

	ExportFile(lFileOutput, "PlayerIdle");

	std::cout << g_KeyTime.size() << "\n";

    DestroySdkObjects(lSdkManager, lResult);

	//for (auto t = g_KeyTime.begin(); t != g_KeyTime.end(); ++t) {
	//	std::cout << *t << "\n";
	//}

    return 0;
}