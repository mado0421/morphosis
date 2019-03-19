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
#include <fstream>
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
	int			idx;
	Float3		LclTranslation;
	Float3		LclRotation;
	Bone*		parent = NULL;
	FbxNode*	node = NULL;

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

	Bone() {

	}
	Bone(const char* name, int idx, Float3 t, Float3 r, Bone* p) {
		this->name = name;
		this->idx = idx;
		LclTranslation = t;
		LclRotation = r;
		parent = p;
	}
	Bone(const char* name, int idx, Float3 t, Float3 r, FbxNode* p) {
		this->name = name;
		this->idx = idx;
		LclTranslation = t;
		LclRotation = r;
		node = p;
	}
	Bone(const char* name, int idx, FbxNode* p) {
		this->name = name;
		this->idx = idx;
		node = p;
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

struct AnimationInfo {
	char name[32];
	float totalTime;
	bool isLoopAnimation;
	char nextName[32];
};

struct AnimationData {
	std::vector<Key> Keys;
	AnimationInfo info;

	void SetInfo(const char* name, const char* nextName, bool isLoop) {
		float maxTime = 0.0f;

		strcpy_s(info.name, name);
		strcpy_s(info.nextName, nextName);
		info.isLoopAnimation = isLoop;
		for (auto p = Keys.cbegin(); p != Keys.cend(); ++p) if (maxTime < p->time) maxTime = p->time;
		info.totalTime = maxTime;
	}
	void Add(float time, Bone& bone, int Component, int Axis, float value) {
		for (auto p = Keys.begin(); p != Keys.end(); ++p) {
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
		Keys.emplace_back(time, bone, Component, Axis, value);
	}
	void Print() {
		for (Key k : Keys) {
			std::cout << "[" << k.time << "]\n";
			for (KeyframeData d : k.data) {
				std::cout << d.b.name.c_str() << "\t - t ("
					<< d.t.x << ", " << d.t.y << ", " << d.t.z << ") ,r ("
					<< d.r.x << ", " << d.r.y << ", " << d.r.z << ")\n";
			}
			std::cout << "\n";
		}
	}
	void Export(const char* fileName) {
		std::ofstream out;
		out.open(fileName, std::ios::out | std::ios::binary);
		/* 먼저 헤더 정보를 넣어준다. */
		out.write((char*)&info.name,			sizeof(char) * 32);
		out.write((char*)&info.nextName,		sizeof(char) * 32);
		out.write((char*)&info.totalTime,		sizeof(float));
		out.write((char*)&info.isLoopAnimation, sizeof(bool));

		/* 키가 몇 개 있고, 각 키마다 Bone이 몇 개 있을 것인지 저장해야 함.*/
		/* 일단은 모든 Bone이 xyz 요소값을 가지는 것으로 간주하고 작성하자.*/
		int nKeys = Keys.size();
		out.write((char*)&nKeys, sizeof(int));
		for (int i = 0; i < nKeys; ++i) {
			int nBones = Keys[i].data.size();
			out.write((char*)&nBones, sizeof(int));
		}

		for (int i = 0; i < nKeys; ++i) {
			int nBones		= Keys[i].data.size();
			float keyTime	= Keys[i].time;

			out.write((char*)&keyTime, sizeof(float));
			for (auto p = Keys[i].data.cbegin(); p != Keys[i].data.cend(); ++p) {
				//pass

				//float temp = p->
				//out.write((char*)&keyTime, sizeof(float));
			}
		}


	}
};

struct Vertex {
	Float3	pos;
	Int4	boneIdx;
	Float4	weight;

	Vertex() {}
	Vertex(Float3 p) :pos(p) {}
};

struct GeometryData {
	std::vector<Vertex> controlPoints;
	std::vector<Float2> UVs;
	std::vector<Float3> normals;
	std::vector<int>	posIdx;
	std::vector<int>	uvIdx;
};

struct Mesh {
	std::string name;
	std::vector<Vertex> controlPoints;

	Mesh() {}
	Mesh(const char* name) { this->name = name; }
};

static bool gVerbose = true;
AnimationData animData;


class DataManager {
public:
	void Init(FbxScene* scene) { pScene = scene; }
	void MakeBoneData() {
		for (int i = 0; i < pScene->GetSrcObjectCount<FbxAnimStack>(); ++i) {
			FbxAnimStack* pAnimStack = pScene->GetSrcObject<FbxAnimStack>(i);
			int nAnimLayers = pAnimStack->GetMemberCount<FbxAnimLayer>();
			for (int j = 0; j < nAnimLayers; ++j) {
				FbxAnimLayer* pAnimLayer = pAnimStack->GetMember<FbxAnimLayer>(j);
				GetBoneDataRec(pAnimLayer, pScene->GetRootNode());
			}
		}
		ConnectBoneParent();
		PrintBoneData();
	}
	void MakeMeshData() {
		GetMeshDataRec(pScene->GetRootNode());
		PrintMeshData();
	}
	void AddClusterWeights() {
		GetClusterData(pScene->GetRootNode());
	}
private:
	//void GetBoneDataRec(FbxAnimLayer* pAnimLayer, FbxNode* pNode) {
	//	int childCount;

	//	// pass...
	//	FbxVector4 lTmpVector;
	//	Float3 t, r;

	//	lTmpVector = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	//	t.x = lTmpVector[0];
	//	t.y = lTmpVector[1];
	//	t.z = lTmpVector[2];
	//	lTmpVector = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
	//	r.x = lTmpVector[0];
	//	r.y = lTmpVector[1];
	//	r.z = lTmpVector[2];

	//	bones.emplace_back(pNode->GetName(), bones.size(), t, r, pNode->GetParent());

	//	for (childCount = 0; childCount < pNode->GetChildCount(); ++childCount) {
	//		GetBoneDataRec(pAnimLayer, pNode->GetChild(childCount));
	//	}
	//}
	//void ConnectBoneParent() {
	//	for (auto p = bones.begin(); p != bones.end(); ++p) {
	//		for (auto pp = bones.begin(); pp != bones.end(); ++pp) {
	//			if (NULL == p->node) break;
	//			//std::cout << p->node->GetName() << "\n";
	//			if (p->node->GetName() == pp->name) {
	//				p->parent = &(*pp);
	//				break;
	//			}
	//		}
	//	}
	//}
	//void PrintBoneData() {
	//	for (auto p = bones.begin(); p != bones.end(); ++p) {
	//		if (NULL == p->parent) {
	//			std::cout
	//				<< p->name.c_str() << " - "
	//				<< p->idx << " - T("
	//				<< p->LclTranslation.x << ", "
	//				<< p->LclTranslation.y << ", "
	//				<< p->LclTranslation.z << "), R( "
	//				<< p->LclRotation.x << ", "
	//				<< p->LclRotation.y << ", "
	//				<< p->LclRotation.z << ")\n";
	//			continue;
	//		}
	//		std::cout
	//			<< p->name.c_str() << " - "
	//			<< p->idx << " - T("
	//			<< p->LclTranslation.x << ", "
	//			<< p->LclTranslation.y << ", "
	//			<< p->LclTranslation.z << "), R( "
	//			<< p->LclRotation.x << ", "
	//			<< p->LclRotation.y << ", "
	//			<< p->LclRotation.z << "), Parent - "
	//			<< p->parent->name.c_str() << "\n";

	//	}
	//}
	void GetBoneDataRec(FbxAnimLayer* pAnimLayer, FbxNode* pNode) {
		int childCount;
		FbxVector4 lTmpVector;

		bones.emplace_back(pNode->GetName(), bones.size(), pNode->GetParent());

		for (childCount = 0; childCount < pNode->GetChildCount(); ++childCount) {
			GetBoneDataRec(pAnimLayer, pNode->GetChild(childCount));
		}
	}
	void ConnectBoneParent() {
		for (auto p = bones.begin(); p != bones.end(); ++p) {
			for (auto pp = bones.begin(); pp != bones.end(); ++pp) {
				if (NULL == p->node) break;
				//std::cout << p->node->GetName() << "\n";
				if (p->node->GetName() == pp->name) {
					p->parent = &(*pp);
					break;
				}
			}
		}
	}
	void PrintBoneData() {
		for (auto p = bones.begin(); p != bones.end(); ++p) {
			if (NULL == p->parent) {
				std::cout
					<< p->name.c_str() << " - "
					<< p->idx << "\n";
				continue;
			}
			std::cout
				<< p->name.c_str() << " - "
				<< p->idx << " - Parent - "
				<< p->parent->name.c_str() << "\n";

		}
	}
	void GetMeshDataRec(FbxNode* pNode) {
		int meshCount;
		FbxMesh* pMesh = pNode->GetMesh();
		if (NULL != pMesh) {
			int nControlPoints = pMesh->GetControlPointsCount();
			int nElementNormals = pMesh->GetElementNormalCount();
			FbxVector4* pControlPoints = pMesh->GetControlPoints();
			int nPolygons = pMesh->GetPolygonCount();

			int meshIdx = meshes.size();
			meshes.emplace_back(pNode->GetName());

			for (int i = 0, nIdx = 0; i < nPolygons; ++i) {
				FbxVector4 fbxVector4;
				int nPolygonSize = pMesh->GetPolygonSize(i);
				for (int j = 0; j < nPolygonSize; ++j) {
					int nControlPointIdx = pMesh->GetPolygonVertex(i, j);
					Float3 p;

					fbxVector4 = pControlPoints[nControlPointIdx];
					p.x = fbxVector4.mData[0];
					p.y = fbxVector4.mData[1];
					p.z = fbxVector4.mData[2];

					meshes[meshIdx].controlPoints.emplace_back(p);
				}
			}
		}
		for (meshCount = 0; meshCount < pNode->GetChildCount(); meshCount++)
		{
			GetMeshDataRec(pNode->GetChild(meshCount));
		}
	}
	void PrintMeshData() {
		for (auto p = meshes.begin(); p != meshes.end(); ++p) {
			std::cout << p->name.c_str() << "\n";
			for (auto cp = p->controlPoints.begin(); cp != p->controlPoints.end(); ++cp) {
				/*std::cout << "("
					<< cp->pos.x << ", "
					<< cp->pos.y << ", "
					<< cp->pos.z << ")\n";*/
			}
		}
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

				std::cout << pNode->GetName() << "\n";
				int nSkinDeformers = geo->GetDeformerCount(FbxDeformer::eSkin);
				for (int i = 0; i < nSkinDeformers; ++i) {
					FbxSkin* skinDeformer = (FbxSkin*)(geo->GetDeformer(i, FbxDeformer::eSkin));
					int nClusters = skinDeformer->GetClusterCount();



					for (int j = 0; j < nClusters; ++j) {
						FbxCluster* cluster = skinDeformer->GetCluster(j);
						int nIdx = cluster->GetControlPointIndicesCount();
						int* pIdx = cluster->GetControlPointIndices();
						double* pWeights = cluster->GetControlPointWeights();

						std::cout /*<< pNode->GetName()*/ << cluster->GetLink()->GetName() << " has " << nIdx << " Infos.\n";
						for (int k = 0; k < nIdx; ++k) {
							std::cout << "\t#" << k << " - " << pIdx[k] << " weight is " << pWeights[k] << "\n";
						}

						/* ... */

					}
				}
			}
		}
		for (meshCount = 0; meshCount < pNode->GetChildCount(); meshCount++)
		{
			GetClusterData(pNode->GetChild(meshCount));
		}
	}
private:
	/******************************************************************
	Bone은 상대적 이동값과 회전값, 부모 Bone, Vector 내의 idx값을 가진다.
	맨 처음 생성 시에는 idx랑 부모만 갖게 하고
	Animation Data를 넣으면서 위치변환과 회전을 줌.
	******************************************************************/
	std::vector<Bone> bones;

	/******************************************************************
	Mesh는 일단 ControlPoint 값만 가진다.
	Mesh가 여러 개 있을 수 있으므로 Mesh를 Vector로 관리하고
	Mesh 내에서 ControlPoint 배열을 만들어 관리한다.
	******************************************************************/
	std::vector<Mesh> meshes;
	
	/******************************************************************
	Animation은 Key값을 가져야 한다.
	Key는 크게 KeyTime, BoneIdx, value 이 세 값을 가진다.
	ex) 1.333s - Head001 - translate(0, 0, 0), rotate(0, 45, 0)
	******************************************************************/
	std::vector<Key> keys;

	/******************************************************************
	FbxScene 정보가 필요함.
	******************************************************************/
	FbxScene* pScene = NULL;
};

DataManager dataManager;





















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

		//std::cout << pNode->GetName() << "'s parent is " << pNode->GetParent()->GetName() << ".\n";

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
	int meshCount;
	FbxMesh* pMesh = pNode->GetMesh();
	if (NULL != pMesh) {
		int nControlPoints = pMesh->GetControlPointsCount();
		int nElementNormals = pMesh->GetElementNormalCount();
		FbxVector4* pControlPoints = pMesh->GetControlPoints();
		std::cout << pNode->GetName() << "\n"
			<< nControlPoints << "개\n\n";
		int nPolygons = pMesh->GetPolygonCount();
		for (int i = 0, nIdx = 0; i < nPolygons; ++i) {
			FbxVector4 fbxVector4;
			int nPolygonSize = pMesh->GetPolygonSize(i);
			for (int j = 0; j < nPolygonSize; ++j) {
				int nControlPointIdx = pMesh->GetPolygonVertex(i, j);
				fbxVector4 = pControlPoints[nControlPointIdx];
				/* ... */
				std::cout
					<< fbxVector4.mData[0] << ", "
					<< fbxVector4.mData[1] << ", "
					<< fbxVector4.mData[2] << ", "
					<< fbxVector4.mData[3] << "\n";
			}
		}
	}
	for (meshCount = 0; meshCount < pNode->GetChildCount(); meshCount++)
	{
		GetMeshData(pNode->GetChild(meshCount));
	}

	//int meshCount;
	//FbxNodeAttribute::EType type;
	//FbxGeometry* geo;
	//FbxMesh* pMesh;
	//FbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();
	//if (NULL != lNodeAttribute) {
	//	type = lNodeAttribute->GetAttributeType();
	//	if (FbxNodeAttribute::eMesh == type) {
	//		geo = pNode->GetGeometry();
	//		pMesh = pNode->GetMesh();
	//		if (NULL != pMesh) {
	//			std::cout
	//				<< pMesh->GetTextureUVIndex(0, 0) << ", "
	//				<< pMesh->GetTextureUVIndex(0, 1) << "\n";
	//		}
	//		int nControlPoints = geo->GetControlPointsCount();
	//		FbxVector4* pControlPoints = geo->GetControlPoints();
	//		std::cout << pNode->GetName() << " - "
	//			<< nControlPoints << "\n";
	//		for (int i = 0; i < nControlPoints; ++i) {
	//			FbxVector4 tmpVec4 = pControlPoints[i];
	//			//std::cout << "("
	//			//	<< tmpVec4.mData[0] << ", "
	//			//	<< tmpVec4.mData[1] << ", "
	//			//	<< tmpVec4.mData[2] << ", "
	//			//	<< tmpVec4.mData[3] << ")\n";
	//		}
	//		int nUV = geo->GetElementUVCount();
	//		//nUV = 
	//		for (int i = 0; i < nUV; ++i) {
	//			FbxGeometryElementUV* pUV = geo->GetElementUV(i);
	//			std::cout << "("
	//				<< pUV->mDirectArray[0][i].mData[0] << ", "
	//				<< pUV->mDirectArray[0][i].mData[1] << ")\n";
	//		}
	//		//pUV->mDirectArray[]
	//	}
	//}
	//for (meshCount = 0; meshCount < pNode->GetChildCount(); meshCount++)
	//{
	//	GetMeshData(pNode->GetChild(meshCount));
	//}
	/*
	void FBXExporter::ReadUV(FbxMesh* inMesh, int inCtrlPointIndex, int inTextureUVIndex, int inUVLayer, XMFLOAT2& outUV)
{
   if(inUVLayer >= 2 || inMesh->GetElementUVCount() <= inUVLayer)
   {
      throw std::exception("Invalid UV Layer Number");
   }
   FbxGeometryElementUV* vertexUV = inMesh->GetElementUV(inUVLayer);

   switch(vertexUV->GetMappingMode())
   {
   case FbxGeometryElement::eByControlPoint:
      switch(vertexUV->GetReferenceMode())
      {
      case FbxGeometryElement::eDirect:
      {
         outUV.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(inCtrlPointIndex).mData[0]);
         outUV.y = static_cast<float>(vertexUV->GetDirectArray().GetAt(inCtrlPointIndex).mData[1]);
      }
      break;

      case FbxGeometryElement::eIndexToDirect:
      {
         int index = vertexUV->GetIndexArray().GetAt(inCtrlPointIndex);
         outUV.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[0]);
         outUV.y = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[1]);
      }
      break;

      default:
         throw std::exception("Invalid Reference");
      }
      break;

   case FbxGeometryElement::eByPolygonVertex:
      switch(vertexUV->GetReferenceMode())
      {
      case FbxGeometryElement::eDirect:
      case FbxGeometryElement::eIndexToDirect:
      {
         outUV.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(inTextureUVIndex).mData[0]);
         outUV.y = static_cast<float>(vertexUV->GetDirectArray().GetAt(inTextureUVIndex).mData[1]);
      }
      break;

      default:
         throw std::exception("Invalid Reference");
      }
      break;
   }
}
	*/
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

					std::cout /*<< pNode->GetName()*/ << cluster->GetLink()->GetName() << " has " << nIdx << " Infos.\n";
					for (int k = 0; k < nIdx; ++k) {
						std::cout << "\t#" << k << " - " << pIdx[k] << " weight is " << pWeights[k] << "\n";
					}

					/* ... */

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

		//GetMeshData(lScene->GetRootNode());
		//GetClusterData(lScene->GetRootNode());

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

		dataManager.Init(lScene);
		//dataManager.MakeBoneData();
		//dataManager.MakeMeshData();
		dataManager.AddClusterWeights();
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

