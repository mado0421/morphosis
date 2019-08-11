#include "stdafx.h"
//#include "Importer.h"
//#include "Object.h"
//#include "AnimationController.h"
//#include "Mesh.h"
//#include "Texture.h"
//#include "ObjectManager.h"
//
//
//
//void CImporter::ImportMesh(const char * fileName, const char * meshName, MeshType type)
//{
//	string modelDataName = ASSETPATH;
//	modelDataName += fileName;
//	modelDataName += ".dat";
//
//	ModelData modelData;
//	ifstream in;
//
//	in.open(modelDataName, std::ios::in | std::ios::binary);
//	char ModelName[32];
//	in.read((char*)&ModelName, sizeof(ModelName));
//	modelData.m_ModelName = ModelName;
//
//	int nMeshList;
//	in.read((char*)&nMeshList, sizeof(int));
//	modelData.m_nMeshList = nMeshList;
//	modelData.m_MeshList = new MeshData[nMeshList];
//
//	for (int i = 0; i < nMeshList; ++i) {
//		char name[32];
//		in.read((char*)name, sizeof(name));
//		modelData.m_MeshList[i].m_MeshName = name;
//
//
//		in.read((char*)&modelData.m_MeshList[i].m_nCtrlPointList, sizeof(int));
//		modelData.m_MeshList[i].m_CtrlPointList = new CtrlPointData[modelData.m_MeshList[i].m_nCtrlPointList];
//
//		in.read((char*)modelData.m_MeshList[i].m_CtrlPointList, sizeof(CtrlPointData) * modelData.m_MeshList[i].m_nCtrlPointList);
//
//		in.read((char*)&modelData.m_MeshList[i].m_nVertexList, sizeof(int));
//		modelData.m_MeshList[i].m_VertexList = new VertexData[modelData.m_MeshList[i].m_nVertexList];
//
//		in.read((char*)modelData.m_MeshList[i].m_VertexList, sizeof(VertexData) * modelData.m_MeshList[i].m_nVertexList);
//	}
//
//
//	if (MeshType::DefaulModel == type) {
//		for (int i = 0; i < modelData.GetNumOfMesh(); ++i) {
//			string partName = meshName;
//			partName += "_";
//			partName += modelData.GetMeshData(i).m_MeshName;
//
//			CModelMesh* tempMesh = new CModelMesh(m_pd3dDevice, m_pd3dCommandList, modelData.GetMeshData(i));
//			tempMesh->name = partName;
//			g_vecMesh.push_back(tempMesh);
//		}
//	}
//	else {
//		for (int i = 0; i < modelData.GetNumOfMesh(); ++i) {
//			string partName = meshName;
//			partName += "_";
//			partName += modelData.GetMeshData(i).m_MeshName;
//
//			CAnimatedModelMesh* tempMesh = new CAnimatedModelMesh(m_pd3dDevice, m_pd3dCommandList, modelData.GetMeshData(i));
//			tempMesh->name = partName;
//			g_vecMesh.push_back(tempMesh);
//
//		}
//	}
//
//	for (int i = 0; i < modelData.m_nMeshList; ++i) {
//		delete[] modelData.m_MeshList[i].m_CtrlPointList;
//		modelData.m_MeshList[i].m_CtrlPointList = NULL;
//		delete[] modelData.m_MeshList[i].m_VertexList;
//		modelData.m_MeshList[i].m_VertexList = NULL;
//
//	}
//	delete[] modelData.m_MeshList;
//	modelData.m_MeshList = NULL;
//}
//
//void CImporter::ImportAnimClip(const char * fileName, const char * animClipName, bool IsLoop)
//{
//	string filePath = ASSETPATH;
//	filePath += fileName;
//	filePath += ".dat";
//
//	AnimClip* animData = new AnimClip();;
//
//	std::ifstream in;
//
//	in.open(filePath, std::ios::in | std::ios::binary);
//	char AnimName[32];
//	in.read((char*)AnimName, sizeof(AnimName));
//	//animData->m_AnimName = AnimName;
//	animData->m_AnimName = animClipName;
//
//	int nKeyTime;
//	in.read((char*)&nKeyTime, sizeof(int));
//	animData->m_KeyTime = new double[nKeyTime];
//	in.read((char*)animData->m_KeyTime, sizeof(double) * nKeyTime);
//
//	int nBoneList;
//	in.read((char*)&nBoneList, sizeof(int));
//	animData->m_BoneList = new BoneData[nBoneList];
//
//	for (int i = 0; i < nBoneList; ++i) {
//		char name[32];
//		in.read((char*)name, sizeof(name));
//		animData->m_BoneList[i].m_Name = name;
//
//		in.read((char*)&animData->m_BoneList[i].m_GlobalTransform, sizeof(XMFLOAT4X4));
//	}
//
//	for (int i = 0; i < nBoneList; ++i) {
//		animData->m_BoneList[i].m_nKeyframe = nKeyTime;
//		animData->m_BoneList[i].m_pToRootTransforms = new XMFLOAT4X4[nKeyTime];
//		for (int j = 0; j < nKeyTime; ++j) 
//			in.read((char*)&animData->m_BoneList[i].m_pToRootTransforms[j], sizeof(XMFLOAT4X4));
//		
//	}
//	in.close();
//
//	g_vecAnimClip.push_back(animData);
//
//}
//void CImporter::ImportLevel(const char * fileName, LEVELDATA_DESC & levelDataDesc)
//{
//	ifstream in;
//	string filePath("Assets/");
//	filePath += fileName;
//	filePath += ".dat";
//
//	in.open(filePath, std::ifstream::in);
//
//	char line[256];
//
//	in.getline(line, 256);
//	levelDataDesc.levelName = line;
//
//	in.getline(line, 256);
//	levelDataDesc.nCollisionMaps = atoi(line);
//
//	in.getline(line, 256);
//	levelDataDesc.nTeam1SpawnPoint = atoi(line);
//
//	in.getline(line, 256);
//	levelDataDesc.nTeam2SpawnPoint = atoi(line);
//
//	in.getline(line, 256);
//	if (strcmp(line, "true"))	levelDataDesc.isCapturePointExist = true;
//	else						levelDataDesc.isCapturePointExist = false;
//
//	in.getline(line, 256); levelDataDesc.BoundaryBoxPosition.x	= (float)atof(line)* g_fLevelScale;
//	in.getline(line, 256); levelDataDesc.BoundaryBoxPosition.y	= (float)atof(line)* g_fLevelScale;
//	in.getline(line, 256); levelDataDesc.BoundaryBoxPosition.z	= (float)atof(line)* g_fLevelScale;
//	in.getline(line, 256); levelDataDesc.BoundaryBoxScale.x		= (float)atof(line)* g_fLevelScale * 0.5f;
//	in.getline(line, 256); levelDataDesc.BoundaryBoxScale.y		= (float)atof(line)* g_fLevelScale * 0.5f;
//	in.getline(line, 256); levelDataDesc.BoundaryBoxScale.z		= (float)atof(line)* g_fLevelScale * 0.5f;
//	in.getline(line, 256); levelDataDesc.BoundaryBoxRotation.x	= (float)atof(line);
//	in.getline(line, 256); levelDataDesc.BoundaryBoxRotation.y	= (float)atof(line);
//	in.getline(line, 256); levelDataDesc.BoundaryBoxRotation.z	= (float)atof(line);
//	in.getline(line, 256); levelDataDesc.BoundaryBoxRotation.w	= (float)atof(line); 
//
//	for (int i = 0; i < levelDataDesc.nCollisionMaps; ++i) {
//		XMFLOAT3 temp3;
//		XMFLOAT4 temp4 = XMFLOAT4(0,0,0,1);
//
//		in.getline(line, 256); temp3.x = (float)atof(line)* g_fLevelScale;
//		in.getline(line, 256); temp3.y = (float)atof(line)* g_fLevelScale;
//		in.getline(line, 256); temp3.z = (float)atof(line)* g_fLevelScale;
//
//		levelDataDesc.CollisionPosition.push_back(temp3);
//
//		in.getline(line, 256); temp3.x = (float)atof(line)* g_fLevelScale* 0.5f;
//		in.getline(line, 256); temp3.y = (float)atof(line)* g_fLevelScale* 0.5f;
//		in.getline(line, 256); temp3.z = (float)atof(line)* g_fLevelScale* 0.5f;
//
//		levelDataDesc.CollisionScale.push_back(temp3);
//
//		in.getline(line, 256); temp4.x = (float)atof(line);
//		in.getline(line, 256); temp4.y = (float)atof(line);
//		in.getline(line, 256); temp4.z = (float)atof(line);
//		in.getline(line, 256); temp4.w = (float)atof(line);
//		//in.getline(line, 256);
//		//in.getline(line, 256);
//		//in.getline(line, 256);
//		//in.getline(line, 256);
//
//		levelDataDesc.CollisionRotation.push_back(temp4);
//	}
//
//	for (int i = 0; i < levelDataDesc.nTeam1SpawnPoint; ++i) {
//		XMFLOAT3 temp3;
//		XMFLOAT4 temp4;
//
//		in.getline(line, 256); temp3.x = (float)atof(line)* g_fLevelScale;
//		in.getline(line, 256); temp3.y = (float)atof(line)* g_fLevelScale;
//		in.getline(line, 256); temp3.z = (float)atof(line)* g_fLevelScale;
//
//		levelDataDesc.Team1SpawnPointPosition.push_back(temp3);
//
//		in.getline(line, 256); temp4.x = (float)atof(line);
//		in.getline(line, 256); temp4.y = (float)atof(line);
//		in.getline(line, 256); temp4.z = (float)atof(line);
//		in.getline(line, 256); temp4.w = (float)atof(line);
//
//		levelDataDesc.Team1SpawnPointRotation.push_back(temp4);
//	}
//
//	for (int i = 0; i < levelDataDesc.nTeam2SpawnPoint; ++i) {
//		XMFLOAT3 temp3;
//		XMFLOAT4 temp4;
//
//		in.getline(line, 256); temp3.x = (float)atof(line)* g_fLevelScale;
//		in.getline(line, 256); temp3.y = (float)atof(line)* g_fLevelScale;
//		in.getline(line, 256); temp3.z = (float)atof(line)* g_fLevelScale;
//
//		levelDataDesc.Team2SpawnPointPosition.push_back(temp3);
//
//		in.getline(line, 256); temp4.x = (float)atof(line);
//		in.getline(line, 256); temp4.y = (float)atof(line);
//		in.getline(line, 256); temp4.z = (float)atof(line);
//		in.getline(line, 256); temp4.w = (float)atof(line);
//
//		levelDataDesc.Team2SpawnPointRotation.push_back(temp4);
//	}
//
//	in.getline(line, 256); levelDataDesc.CapturePointPosition.x = (float)atof(line)* g_fLevelScale;
//	in.getline(line, 256); levelDataDesc.CapturePointPosition.y = (float)atof(line)* g_fLevelScale;
//	in.getline(line, 256); levelDataDesc.CapturePointPosition.z = (float)atof(line)* g_fLevelScale;
//	in.getline(line, 256); levelDataDesc.CapturePointScale.x	= (float)atof(line)* g_fLevelScale* 0.5f;
//	in.getline(line, 256); levelDataDesc.CapturePointScale.y	= (float)atof(line)* g_fLevelScale* 0.5f;
//	in.getline(line, 256); levelDataDesc.CapturePointScale.z	= (float)atof(line)* g_fLevelScale* 0.5f;
//	in.getline(line, 256); levelDataDesc.CapturePointRotation.x = (float)atof(line);
//	in.getline(line, 256); levelDataDesc.CapturePointRotation.y = (float)atof(line);
//	in.getline(line, 256); levelDataDesc.CapturePointRotation.z = (float)atof(line);
//	in.getline(line, 256); levelDataDesc.CapturePointRotation.w = (float)atof(line);
//
//
//	in.close();
//
//
//
//}
//void CImporter::ImportTexture(const wchar_t * fileName, const char * textureName)
//{
//	wstring filePath = LASSETPATH;
//	filePath += fileName;
//	filePath += L".dds";
//
//	CTexture* texture = new CTexture(RESOURCE_TEXTURE2D);
//	texture->LoadTextureFromFile(m_pd3dDevice, m_pd3dCommandList, filePath.c_str());
//	texture->m_strName = textureName;
//
//	g_vecTexture.push_back(texture);
//}
