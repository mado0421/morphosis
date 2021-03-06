#include "stdafx.h"
#include "Importer.h"
//#include "Object.h"
//#include "AnimationController.h"
#include "Mesh.h"
#include "Texture.h"
#include "Model.h"

//std::vector<CAnimationController*>	g_vecAnimController;

//void CImporter::ImportModel(const char * fileName, const char * textureName, ModelType type, const char * modelName, float scale)
//{
//	if (ModelType::FloatingUI == type) {
//		if (_heapchk() != _HEAPOK)
//			DebugBreak();
//		CModel* model = new CModel();
//		CUIMesh* tempMesh = new CUIMesh(m_pd3dDevice, m_pd3dCommandList);
//		model->SetMesh(tempMesh);
//		model->SetTexture(GetTextureByName(textureName));
//		model->SetName(modelName);
//
//		g_vecModel.push_back(model);
//
//		//delete tempMesh;
//		if (_heapchk() != _HEAPOK)
//			DebugBreak();
//		return;
//	}
//	if (_heapchk() != _HEAPOK)
//		DebugBreak();
//	string modelDataName = ASSETPATH;
//	modelDataName += fileName;
//	modelDataName += "_mesh.dat";
//	ImportModelData modelData;
//	modelData.ImportFile(modelDataName.c_str());
//	if (_heapchk() != _HEAPOK)
//		DebugBreak();
//	if (ModelType::DefaultModel == type) {
//		for (int i = 0; i < modelData.GetNumOfMesh(); ++i) {
//			string partName = modelName;
//			partName += "_";
//			partName += modelData.GetMeshData(i).m_MeshName;
//
//			CModel* model = new CModel();
//			CModelMesh* tempMesh = new CModelMesh(m_pd3dDevice, m_pd3dCommandList, modelData.GetMeshData(i), scale);
//			model->SetMesh(tempMesh);
//			model->SetTexture(GetTextureByName(textureName));
//			model->SetName(partName.c_str());
//
//			g_vecModel.push_back(model);
//		}
//	}
//	if (ModelType::AnimatedModel == type) {
//		for (int i = 0; i < modelData.GetNumOfMesh(); ++i) {
//			string partName = modelName;
//			partName += "_";
//			partName += modelData.GetMeshData(i).m_MeshName;
//
//			CModel* model = new CModel();
//			CAnimatedModelMesh* tempMesh = new CAnimatedModelMesh(m_pd3dDevice, m_pd3dCommandList, modelData.GetMeshData(i));
//			model->SetMesh(tempMesh);
//			model->SetTexture(GetTextureByName(textureName));
//			model->SetName(partName.c_str());
//
//			g_vecModel.push_back(model);
//
//		}
//	}
//
//
//}
//void CImporter::ImportAnimClip(const char * fileName, const char * animCtrlName, bool IsLoop, const char * animClipName)
//{
//	string filePath = ASSETPATH;
//	filePath += fileName;
//	filePath += "_anim.dat";
//	AnimationClip* animData = new AnimationClip();
//	animData->ImportFile(filePath.c_str());
//	animData->SetIsLoop(IsLoop);
//
//	if ("" != animClipName) animData->m_AnimName = animClipName;
//
//	GetAnimCtrlByName(animCtrlName)->AddAnimData(animData);
//}
//
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
//
//}

void CImporter::ImportModel(const char * fileName, const char * modelName, float scale)
{
	cout << "CImporter::ImportModel() 은 수정중입니다.\n";


	//string modelDataName = ASSETPATH;
	//modelDataName += fileName;
	//modelDataName += "_mesh.dat";
	//ImportModelData modelData;
	//modelData.ImportFile(modelDataName.c_str());
	//for (int i = 0; i < modelData.GetNumOfMesh(); ++i) {
	//	string partName = modelName;
	//	partName += "_";
	//	partName += modelData.GetMeshData(i).m_MeshName;

	//	CModel* model = new CModel();
	//	CModelMesh* tempMesh = new CModelMesh(m_pd3dDevice, m_pd3dCommandList, modelData.GetMeshData(i), scale);
	//	model->SetMesh(tempMesh);
	//	model->SetName(partName.c_str());

	//	g_vecModel.push_back(model);
	//}

}

void CImporter::ImportTexture(const wchar_t * fileName, const char * textureName, TEXTURETYPE type)
{
	wstring filePath = LASSETPATH;
	filePath += fileName;
	filePath += L".dds";

	CTexture* texture = new CTexture(RESOURCE_TEXTURE2D);
	texture->LoadTextureFromFile(m_pd3dDevice, m_pd3dCommandList, filePath.c_str());
	texture->name = textureName;
	texture->SetType(type);

	g_vecTexture.push_back(texture);
}

//void CImporter::ImportAnimController(const char * animControllerName)
//{
//	CAnimationController* animController = new CAnimationController();
//	animController->SetName(animControllerName);
//
//	g_vecAnimController.push_back(animController);
//}
//
