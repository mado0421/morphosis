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
#include <algorithm>
#include <DirectXMath.h>

using namespace DirectX;

static bool gVerbose = true;

struct Bone {
	FbxString	m_Name;
	FbxNode*	m_Node		= NULL;
	Bone*		m_Parent	= NULL;
	bool		m_bIsBindPose = false;

	FbxMatrix	m_GlobalTransform;

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

void MakeBinePoseMatrix(FbxScene* scene) {
	int      i, j, k, lPoseCount;
	FbxString  lName;

	lPoseCount = scene->GetPoseCount();

	for (i = 0; i < lPoseCount; i++)
	{
		FbxPose* lPose = scene->GetPose(i);
		if (!lPose->IsBindPose()) continue;
		for (j = 0; j < lPose->GetCount(); j++)
		{
			lName = lPose->GetNodeName(j).GetCurrentName();
			for (auto p = g_BoneList.begin(); p != g_BoneList.end(); ++p) {
				if (p->m_Name == lName) {
					p->m_GlobalTransform = lPose->GetMatrix(j);
					p->m_bIsBindPose = true;
				}
			}
		}
	}
}
void DeleteNotBindBone() {
	for (auto p = g_BoneList.begin(); p != g_BoneList.end();) {
		if (!p->m_bIsBindPose) {
			p = g_BoneList.erase(p);
		}
		else ++p;
	}
}


void MakeBoneDataTest(FbxNode* node) {
	if (IsSkeletonNode(node)) {
		Bone tmp;
		tmp.m_Name = node->GetName();
		tmp.m_Node = node;

		//FbxAMatrix mtx;
		//mtx = node->EvaluateGlobalTransform();
		//tmp.m_GlobalTransform = mtx;
		//mtx.SetIdentity();

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

		//DisplayMatrix(b->m_GlobalTransform, "Offset");

		std::cout << "\n";

		for (unsigned int i = 0; i < g_KeyTime.size(); ++i) {
			std::cout << "#" << i << "\n";
			DisplayMatrix(b->m_ToParentTransforms[i], "ToParent");
			DisplayMatrix(b->m_ToRootTransforms[i], "ToRoot");
		}
	}
}

/***************************************************************************************
Mesh
***************************************************************************************/

struct CtrlPoint {
	XMFLOAT3	xmf3Position;
	XMINT4		xmi4BoneIdx;
	XMFLOAT4	xmf4BoneWeight;

	CtrlPoint() 
		:xmi4BoneIdx(-1, -1, -1, -1)
		,xmf4BoneWeight(0, 0, 0, 0)
	{}
};
struct Vertex {
	int			ctrlPointIdx;
	XMFLOAT3	xmf3Normal;
	XMFLOAT3	xmf3BiNormal;
	XMFLOAT3	xmf3Tangent;
	XMFLOAT2	xmf2UV;
};
struct Mesh {
	FbxString				m_Name;
	std::vector<CtrlPoint>	m_CtrlPointList;
	std::vector<Vertex>		m_VertexList;

	void AddClusterData(int cpIdx, int boneIdx, double weight) {
		if (-1 == m_CtrlPointList[cpIdx].xmi4BoneIdx.x) {
			m_CtrlPointList[cpIdx].xmi4BoneIdx.x = boneIdx;
			m_CtrlPointList[cpIdx].xmf4BoneWeight.x = weight;
			return;
		}
		if (-1 == m_CtrlPointList[cpIdx].xmi4BoneIdx.y) {
			m_CtrlPointList[cpIdx].xmi4BoneIdx.y = boneIdx;
			m_CtrlPointList[cpIdx].xmf4BoneWeight.y = weight;
			return;
		}
		if (-1 == m_CtrlPointList[cpIdx].xmi4BoneIdx.z) {
			m_CtrlPointList[cpIdx].xmi4BoneIdx.z = boneIdx;
			m_CtrlPointList[cpIdx].xmf4BoneWeight.z = weight;
			return;
		}
		if (-1 == m_CtrlPointList[cpIdx].xmi4BoneIdx.w) {
			m_CtrlPointList[cpIdx].xmi4BoneIdx.w = boneIdx;
			m_CtrlPointList[cpIdx].xmf4BoneWeight.w = weight;
			return;
		}
	}
};

std::vector<Mesh> g_MeshList;

void DisplayCtrlPoint(const Mesh& mesh) {
	//for (auto p = mesh.m_CtrlPointList.begin(); p != mesh.m_CtrlPointList.end(); ++p) {
	//	std::cout <<
	//		"Pos: (" << p->xmf3Position.x << ", " << p->xmf3Position.y << ", " << p->xmf3Position.z << ")\n" <<
	//		"Idx: (" << p->xmi4BoneIdx.x << ", " << p->xmi4BoneIdx.y << ", " << p->xmi4BoneIdx.z << ", " << p->xmi4BoneIdx.w << ")\n" <<
	//		"Wei: (" << p->xmf4BoneWeight.x << ", " << p->xmf4BoneWeight.y << ", " << p->xmf4BoneWeight.z << ", " << p->xmf4BoneWeight.w << ")\n";
	//}
	for (int i = 0; i < /*mesh.m_VertexList.size()*/ 20; ++i) {
		std::cout <<
			"Pos: (" << mesh.m_CtrlPointList[i].xmf3Position.x << ", " <<	mesh.m_CtrlPointList[i].xmf3Position.y << ", " << mesh.m_CtrlPointList[i].xmf3Position.z << ")\n" <<
			"Idx: (" << mesh.m_CtrlPointList[i].xmi4BoneIdx.x << ", " << mesh.m_CtrlPointList[i].xmi4BoneIdx.y << ", " << mesh.m_CtrlPointList[i].xmi4BoneIdx.z << ", " <<			mesh.m_CtrlPointList[i].xmi4BoneIdx.w << ")\n" <<
			"Wei: (" << mesh.m_CtrlPointList[i].xmf4BoneWeight.x << ", " << mesh.m_CtrlPointList[i].xmf4BoneWeight.y << ", " << mesh.m_CtrlPointList[i].xmf4BoneWeight.z << ", " << mesh.m_CtrlPointList[i].xmf4BoneWeight.w << ")\n";
	}																																												
}

void DisplayVertex(const Mesh& mesh) {
	//for (auto p = mesh.m_VertexList.begin(); p != mesh.m_VertexList.end(); ++p) {
	//	std::cout <<
	//		"Idx: - " << p->ctrlPointIdx << "\n"
	//		"Nor: (" << p->xmf3Normal.x << ", " << p->xmf3Normal.y << ", " << p->xmf3Normal.z << ")\n" <<
	//		"Bin: (" << p->xmf3BiNormal.x << ", " << p->xmf3BiNormal.y << ", " << p->xmf3BiNormal.z << ")\n" <<
	//		"Tan: (" << p->xmf3Tangent.x << ", " << p->xmf3Tangent.y << ", " << p->xmf3Tangent.z << ")\n" <<
	//		"UV : (" << p->xmf2UV.x << ", " << p->xmf2UV.y << ")\n";
	//}
	for (int i = 0; i < /*mesh.m_VertexList.size()*/ 20; ++i) {
		std::cout <<
			"Idx: - " <<mesh.m_VertexList[i].ctrlPointIdx << "\n"
			"Nor: (" << mesh.m_VertexList[i].xmf3Normal.x << ", "	<< mesh.m_VertexList[i].xmf3Normal.y << ", "	<< mesh.m_VertexList[i].xmf3Normal.z << ")\n" <<
			"Bin: (" << mesh.m_VertexList[i].xmf3BiNormal.x << ", " << mesh.m_VertexList[i].xmf3BiNormal.y << ", "	<< mesh.m_VertexList[i].xmf3BiNormal.z << ")\n" <<
			"Tan: (" << mesh.m_VertexList[i].xmf3Tangent.x << ", "	<< mesh.m_VertexList[i].xmf3Tangent.y << ", "	<< mesh.m_VertexList[i].xmf3Tangent.z << ")\n" <<
			"UV : (" << mesh.m_VertexList[i].xmf2UV.x << ", "		<< mesh.m_VertexList[i].xmf2UV.y << ")\n";
	}
}

void DisplayMesh() {
	for (auto p = g_MeshList.begin(); p != g_MeshList.end(); ++p) {
		std::cout << "\n" << p->m_Name << "\n\n";
		DisplayCtrlPoint(*p);
		//DisplayVertex(*p);
	}
}


int GetBoneIdxByName(FbxString name) {
	for (auto p = g_BoneList.begin(); p != g_BoneList.end(); ++p) {
		if (name == p->m_Name) 
			return std::distance(g_BoneList.begin(), p);
	}
	return 0;
}

void BoneIdxDefaultToZero() {
	for (auto p = g_MeshList.begin(); p != g_MeshList.end(); ++p) {
		for (auto c = p->m_CtrlPointList.begin(); c != p->m_CtrlPointList.end(); ++c) {
			if (-1 == c->xmi4BoneIdx.x) c->xmi4BoneIdx.x = 0;
			if (-1 == c->xmi4BoneIdx.y) c->xmi4BoneIdx.y = 0;
			if (-1 == c->xmi4BoneIdx.z) c->xmi4BoneIdx.z = 0;
			if (-1 == c->xmi4BoneIdx.w) c->xmi4BoneIdx.w = 0;
		}
	}
}
void MeshSwapYZ() {
	for (auto p = g_MeshList.begin(); p != g_MeshList.end(); ++p) {
		for (auto c = p->m_CtrlPointList.begin(); c != p->m_CtrlPointList.end(); ++c) {
			std::swap(c->xmf3Position.y, c->xmf3Position.z);
		}
	}
}

void ReadNormal(FbxMesh* mesh, int CtrlPointIdx, int VertexCounter, XMFLOAT3& out) {
	for (int i = 0; i < mesh->GetElementNormalCount(); ++i) {
		FbxGeometryElementNormal* vertexNormal = mesh->GetElementNormal(i);
		if (vertexNormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex) {
			switch (vertexNormal->GetReferenceMode()) {
			case FbxGeometryElement::eDirect:
				out.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(VertexCounter).mData[0]);
				out.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(VertexCounter).mData[1]);
				out.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(VertexCounter).mData[2]);
				break;
			case FbxGeometryElement::eIndexToDirect: {
				int index = vertexNormal->GetIndexArray().GetAt(VertexCounter);
				out.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
				out.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
				out.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
				}
				break;
			default:
				break;
			}
		}
	}
}
void ReadBinormal(FbxMesh* mesh, int CtrlPointIdx, int VertexCounter, XMFLOAT3& out) {
	for (int i = 0; i < mesh->GetElementBinormalCount(); ++i) {
		FbxGeometryElementBinormal* vertexBinormal = mesh->GetElementBinormal(i);
		if (vertexBinormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex) {
			switch (vertexBinormal->GetReferenceMode()) {
			case FbxGeometryElement::eDirect:
				out.x = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(VertexCounter).mData[0]);
				out.y = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(VertexCounter).mData[1]);
				out.z = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(VertexCounter).mData[2]);
				break;
			case FbxGeometryElement::eIndexToDirect: {
				int index = vertexBinormal->GetIndexArray().GetAt(VertexCounter);
				out.x = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[0]);
				out.y = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[1]);
				out.z = static_cast<float>(vertexBinormal->GetDirectArray().GetAt(index).mData[2]);
			}
				break;
			default:
				break;
			}
		}
	}
}
void ReadTangent(FbxMesh* mesh, int CtrlPointIdx, int VertexCounter, XMFLOAT3& out) {
	for (int i = 0; i < mesh->GetElementTangentCount(); ++i) {
		FbxGeometryElementTangent* vertexTangent = mesh->GetElementTangent(i);
		if (vertexTangent->GetMappingMode() == FbxGeometryElement::eByPolygonVertex) {
			switch (vertexTangent->GetReferenceMode()) {
			case FbxGeometryElement::eDirect:
				out.x = static_cast<float>(vertexTangent->GetDirectArray().GetAt(VertexCounter).mData[0]);
				out.y = static_cast<float>(vertexTangent->GetDirectArray().GetAt(VertexCounter).mData[1]);
				out.z = static_cast<float>(vertexTangent->GetDirectArray().GetAt(VertexCounter).mData[2]);
				break;
			case FbxGeometryElement::eIndexToDirect: {
				int index = vertexTangent->GetIndexArray().GetAt(VertexCounter);
				out.x = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[0]);
				out.y = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[1]);
				out.z = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[2]);
			}
				break;
			default:
				break;
			}
		}
	}
}
void ReadUV(FbxMesh* mesh, int CtrlPointIdx, int VertexCounter, XMFLOAT2& out) {
	FbxGeometryElementUV* vertexUV = mesh->GetElementUV(0);
	switch (vertexUV->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch (vertexUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{

			out.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(CtrlPointIdx).mData[0]);
			out.y = 1 - static_cast<float>(vertexUV->GetDirectArray().GetAt(CtrlPointIdx).mData[1]);
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{

			int index = vertexUV->GetIndexArray().GetAt(CtrlPointIdx);
			out.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[0]);
			out.y = 1 - static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[1]);
		}
		break;
		default:
			break;
		}
		break;
	case FbxGeometryElement::eByPolygonVertex:
		switch (vertexUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{

			out.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(VertexCounter).mData[0]);
			out.y = 1 - static_cast<float>(vertexUV->GetDirectArray().GetAt(VertexCounter).mData[1]);
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{

			int index = vertexUV->GetIndexArray().GetAt(VertexCounter);
			out.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[0]);
			out.y = 1 - static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[1]);
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
		tmpMesh.m_Name = node->GetName();
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
				tmpVertex.ctrlPointIdx = CtrlPointIdx;
				// 이걸로 Vertex는 CtrlPointIdx를 갖게 됨.
				// 아직 Normal, BiNormal, Tangent 남음
				ReadNormal(mesh, CtrlPointIdx, vertexCounter, tmpVertex.xmf3Normal);
				ReadBinormal(mesh, CtrlPointIdx, vertexCounter, tmpVertex.xmf3BiNormal);
				ReadTangent(mesh, CtrlPointIdx, vertexCounter, tmpVertex.xmf3Tangent);
				//// 이제 UV 해줘야 함.
				ReadUV(mesh, CtrlPointIdx, vertexCounter, tmpVertex.xmf2UV);
				tmpMesh.m_VertexList.push_back(tmpVertex);
				vertexCounter++;
			}
		}

		FbxGeometry* geo = node->GetGeometry();

		int nDeformer = geo->GetDeformerCount(FbxDeformer::eSkin);
		for (int i = 0; i < nDeformer; ++i) {
			FbxSkin* skinDeformer = static_cast<FbxSkin*>(geo->GetDeformer(i, FbxDeformer::eSkin));
			int nCluster = skinDeformer->GetClusterCount();

			for (int j = 0; j < nCluster; ++j) {
				FbxCluster* cluster = skinDeformer->GetCluster(j);
				int			nIdx = cluster->GetControlPointIndicesCount();
				int*		pIdx = cluster->GetControlPointIndices();
				double*		pWeight = cluster->GetControlPointWeights();

				int			boneIdx = GetBoneIdxByName(cluster->GetLink()->GetName());

				for (int k = 0; k < nIdx; ++k) {
					tmpMesh.AddClusterData(pIdx[k], boneIdx, pWeight[k]);
				}
			}
		}

		g_MeshList.push_back(tmpMesh);

	}
}

void RecMakeMesh(FbxNode* node) {
	MakeMesh(node);
	for (int i = 0; i < node->GetChildCount(); ++i) {
		MakeMesh(node->GetChild(i));
	}
	//MakeClusterData(node);
	//for (int i = 0; i < node->GetChildCount(); ++i) {
	//	MakeClusterData(node->GetChild(i));
	//}
	BoneIdxDefaultToZero();
	MeshSwapYZ();
}

/***************************************************************************************
Export
***************************************************************************************/

XMFLOAT4X4 MakeXMFloat4x4FromFbxMatrix(const FbxMatrix& fbxMtx) {
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

void ExportAnimFile(const char* fileName, const char* animName) {
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

		XMFLOAT4X4 xmf4x4Temp = MakeXMFloat4x4FromFbxMatrix(g_BoneList[i].m_GlobalTransform);
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
void ExportMeshFile(const char* fileName, const char* modelName) {
	std::ofstream out;

	out.open(fileName, std::ios::out | std::ios::binary);
	char ModelName[32];
	strcpy_s(ModelName, sizeof(ModelName), modelName);
	out.write((char*)&ModelName, sizeof(ModelName));

	int NumMesh = g_MeshList.size();
	out.write((char*)&NumMesh, sizeof(int));

	for (auto m = g_MeshList.begin(); m != g_MeshList.end(); ++m) {
		char name[32];
		strcpy_s(name, sizeof(name),m->m_Name.Buffer());
		out.write((char*)&name, sizeof(name));

		int NumCP = m->m_CtrlPointList.size();
		out.write((char*)&NumCP, sizeof(int));
		CtrlPoint* tmpCp = new CtrlPoint[NumCP];
		for (int i = 0; i < NumCP; ++i) {
			tmpCp[i] = m->m_CtrlPointList[i];
		}
		out.write((char*)tmpCp, sizeof(CtrlPoint) * NumCP);

		int NumV = m->m_VertexList.size();
		out.write((char*)&NumV, sizeof(int));
		Vertex* tmpV = new Vertex[NumV];
		for (int i = 0; i < NumV; ++i) {
			tmpV[i] = m->m_VertexList[i];
		}
		out.write((char*)tmpV, sizeof(Vertex) * NumV);
	}
	out.close();
}

const char * SAMPLE_FILENAME = "0615_Box";


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

	/*********************************************************************
	2019-06-15
	애니메이션 추출하려면 이 부분 주석 해제
	*********************************************************************/
	//AnimationData(lScene);
	//RecFollowChildNode(lScene->GetRootNode(), MakeBoneDataTest);
	//MakeBinePoseMatrix(lScene);
	//DeleteNotBindBone();
	//RecFollowChildNode(lScene->GetRootNode(), MakeParent);
	//MakeToRootTransform();
	//FbxString lFileOutput;
	//lFileOutput += lFilePath;
	//lFileOutput += "_anim.dat";
	//ExportAnimFile(lFileOutput, "PlayerRun");


	/*********************************************************************
	2019-06-15
	메쉬 추출하려면 이 부분 주석 해제
	*********************************************************************/
	RecMakeMesh(lScene->GetRootNode());
	FbxString meshFileName;
	meshFileName += lFilePath;
	meshFileName += "_mesh.dat";
	ExportMeshFile(meshFileName, "Prop_Box");

    DestroySdkObjects(lSdkManager, lResult);
	
    return 0;
}