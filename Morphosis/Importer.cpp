#include "stdafx.h"
#include "Importer.h"
#include "Object.h"
#include "AnimationController.h"
#include "Mesh.h"
#include "Texture.h"

//void CImporter::ImportModel(const char * fileName, CTexture * texture, CObject * obj)
//{
//	TCHAR programpath[_MAX_PATH];
//	GetCurrentDirectory(_MAX_PATH, programpath);
//
//	string modelDataName = ASSETPATH;
//	modelDataName += fileName;
//	modelDataName += "_mesh.dat";
//	ImportModelData modelData;
//	modelData.ImportFile(modelDataName.c_str());
//
//	/*********************************************************************
//	2019-06-13
//	여기서 파일을 읽고, 메쉬 개수만큼 모델을 생성, 메쉬를 넣고, 텍스처를 넣고,
//	그걸 오브젝트에 추가해줘야 함.
//
//	모델은 포인터로 전달이 됨. 오브젝트는 모델 포인터를 벡터로 관리하는게 아니라
//	모델 객체를 벡터로 관리하는 중이니까 그걸 염두에 두고 작업해야 함.
//	*********************************************************************/
//	for (int i = 0; i < modelData.GetNumOfMesh(); ++i) {
//		CModel* model = new CModel();
//		CAnimatedMesh* tempMesh = new CAnimatedMesh(m_pd3dDevice, m_pd3dCommandList, modelData.GetMeshData(i));
//		model->SetMesh(tempMesh);
//		model->SetTexture(texture);
//		obj->AddModel(model);
//	}
//}

std::vector<CTexture*>				g_vecTexture;
std::vector<CModel*>				g_vecModel;
std::vector<CAnimationController*>	g_vecAnimController;

CTexture * CImporter::GetTextureByName(const char * name)
{
	for (int i = 0; i < g_vecTexture.size(); ++i) 
		if (g_vecTexture[i]->m_strName == name) return g_vecTexture[i];
		
	return nullptr;
}

void CImporter::ImportModel(const char * fileName, CTexture * texture, CObject * obj, ImportType type)
{
	TCHAR programpath[_MAX_PATH];
	GetCurrentDirectory(_MAX_PATH, programpath);

	string modelDataName = ASSETPATH;
	modelDataName += fileName;
	modelDataName += "_mesh.dat";
	ImportModelData modelData;
	modelData.ImportFile(modelDataName.c_str());

	/*********************************************************************
	2019-06-13
	여기서 파일을 읽고, 메쉬 개수만큼 모델을 생성, 메쉬를 넣고, 텍스처를 넣고,
	그걸 오브젝트에 추가해줘야 함.

	모델은 포인터로 전달이 됨. 오브젝트는 모델 포인터를 벡터로 관리하는게 아니라
	모델 객체를 벡터로 관리하는 중이니까 그걸 염두에 두고 작업해야 함.
	*********************************************************************/
	if (ImportType::DefaultMesh == type) {
		for (int i = 0; i < modelData.GetNumOfMesh(); ++i) {
			CModel* model = new CModel();
			
			CIlluminatedTexturedMesh* tempMesh = new CIlluminatedTexturedMesh(m_pd3dDevice, m_pd3dCommandList, modelData.GetMeshData(i));
			model->SetMesh(tempMesh);
			model->SetTexture(texture);
			obj->AddModel(model);
		}
	}
	else if (ImportType::AnimatedMesh == type) {
		for (int i = 0; i < modelData.GetNumOfMesh(); ++i) {
			CModel* model = new CModel();
			CAnimatedMesh* tempMesh = new CAnimatedMesh(m_pd3dDevice, m_pd3dCommandList, modelData.GetMeshData(i));
			model->SetMesh(tempMesh);
			model->SetTexture(texture);
			obj->AddModel(model);
		}
	}
	else {
		cout << "ImportModel(const char*, CTexture*, CObject*, ImportType) 제대로 하자\n";
	}

}

void CImporter::ImportModel(const char * fileName, int textureIdx, CObject * obj, ImportType type)
{
	TCHAR programpath[_MAX_PATH];
	GetCurrentDirectory(_MAX_PATH, programpath);

	string modelDataName = ASSETPATH;
	modelDataName += fileName;
	modelDataName += "_mesh.dat";
	ImportModelData modelData;
	modelData.ImportFile(modelDataName.c_str());

	/*********************************************************************
	2019-06-13
	여기서 파일을 읽고, 메쉬 개수만큼 모델을 생성, 메쉬를 넣고, 텍스처를 넣고,
	그걸 오브젝트에 추가해줘야 함.

	모델은 포인터로 전달이 됨. 오브젝트는 모델 포인터를 벡터로 관리하는게 아니라
	모델 객체를 벡터로 관리하는 중이니까 그걸 염두에 두고 작업해야 함.
	*********************************************************************/
	if (ImportType::DefaultMesh == type) {
		for (int i = 0; i < modelData.GetNumOfMesh(); ++i) {
			CModel* model = new CModel();

			CIlluminatedTexturedMesh* tempMesh = new CIlluminatedTexturedMesh(m_pd3dDevice, m_pd3dCommandList, modelData.GetMeshData(i));
			model->SetMesh(tempMesh);
			model->SetTexture(g_vecTexture[textureIdx]);

			g_vecModel.push_back(model);

			obj->AddModel(model);
		}
	}
	else if (ImportType::AnimatedMesh == type) {
		for (int i = 0; i < modelData.GetNumOfMesh(); ++i) {
			CModel* model = new CModel();
			CAnimatedMesh* tempMesh = new CAnimatedMesh(m_pd3dDevice, m_pd3dCommandList, modelData.GetMeshData(i));
			model->SetMesh(tempMesh);
			model->SetTexture(g_vecTexture[textureIdx]);

			g_vecModel.push_back(model);

			obj->AddModel(model);
		}
	}
	else {
		cout << "ImportModel(const char*, CTexture*, CObject*, ImportType) 제대로 하자\n";
	}
}

void CImporter::ImportAnimClip(const char * fileName, CObject * obj)
{
	string filePath("Assets/");
	filePath += fileName;

	string animDataName = filePath;
	animDataName += "_anim.dat";
	AnimationClip* animData = new AnimationClip();
	animData->ImportFile(animDataName.c_str());

	obj->AddAnimClip(animData);
}

void CImporter::ImportLevel(const char * fileName, LEVELDATA_DESC & levelDataDesc)
{
	ifstream in;
	string filePath("Assets/");
	filePath += fileName;
	filePath += ".dat";

	in.open(filePath, std::ifstream::in);

	char line[256];

	in.getline(line, 256);
	levelDataDesc.levelName = line;

	in.getline(line, 256);
	levelDataDesc.nCollisionMaps = atoi(line);

	in.getline(line, 256);
	levelDataDesc.nTeam1SpawnPoint = atoi(line);

	in.getline(line, 256);
	levelDataDesc.nTeam2SpawnPoint = atoi(line);

	in.getline(line, 256);
	if (strcmp(line, "true"))	levelDataDesc.isCapturePointExist = true;
	else						levelDataDesc.isCapturePointExist = false;

	in.getline(line, 256); levelDataDesc.BoundaryBoxPosition.x	= (float)atof(line)* g_fLevelScale;
	in.getline(line, 256); levelDataDesc.BoundaryBoxPosition.y	= (float)atof(line)* g_fLevelScale;
	in.getline(line, 256); levelDataDesc.BoundaryBoxPosition.z	= (float)atof(line)* g_fLevelScale;
	in.getline(line, 256); levelDataDesc.BoundaryBoxScale.x		= (float)atof(line)* g_fLevelScale * 0.5f;
	in.getline(line, 256); levelDataDesc.BoundaryBoxScale.y		= (float)atof(line)* g_fLevelScale * 0.5f;
	in.getline(line, 256); levelDataDesc.BoundaryBoxScale.z		= (float)atof(line)* g_fLevelScale * 0.5f;
	in.getline(line, 256); levelDataDesc.BoundaryBoxRotation.x	= (float)atof(line);
	in.getline(line, 256); levelDataDesc.BoundaryBoxRotation.y	= (float)atof(line);
	in.getline(line, 256); levelDataDesc.BoundaryBoxRotation.z	= (float)atof(line);
	in.getline(line, 256); levelDataDesc.BoundaryBoxRotation.w	= (float)atof(line);

	for (int i = 0; i < levelDataDesc.nCollisionMaps; ++i) {
		XMFLOAT3 temp3;
		XMFLOAT4 temp4 = XMFLOAT4(0,0,0,1);

		in.getline(line, 256); temp3.x = (float)atof(line)* g_fLevelScale;
		in.getline(line, 256); temp3.y = (float)atof(line)* g_fLevelScale;
		in.getline(line, 256); temp3.z = (float)atof(line)* g_fLevelScale;

		levelDataDesc.CollisionPosition.push_back(temp3);

		in.getline(line, 256); temp3.x = (float)atof(line)* g_fLevelScale* 0.5f;
		in.getline(line, 256); temp3.y = (float)atof(line)* g_fLevelScale* 0.5f;
		in.getline(line, 256); temp3.z = (float)atof(line)* g_fLevelScale* 0.5f;

		levelDataDesc.CollisionScale.push_back(temp3);

		in.getline(line, 256); temp4.x = (float)atof(line);
		in.getline(line, 256); temp4.y = (float)atof(line);
		in.getline(line, 256); temp4.z = (float)atof(line);
		in.getline(line, 256); temp4.w = (float)atof(line);
		//in.getline(line, 256);
		//in.getline(line, 256);
		//in.getline(line, 256);
		//in.getline(line, 256);

		levelDataDesc.CollisionRotation.push_back(temp4);
	}

	for (int i = 0; i < levelDataDesc.nTeam1SpawnPoint; ++i) {
		XMFLOAT3 temp3;
		XMFLOAT4 temp4;

		in.getline(line, 256); temp3.x = (float)atof(line)* g_fLevelScale;
		in.getline(line, 256); temp3.y = (float)atof(line)* g_fLevelScale;
		in.getline(line, 256); temp3.z = (float)atof(line)* g_fLevelScale;

		levelDataDesc.Team1SpawnPointPosition.push_back(temp3);

		in.getline(line, 256); temp4.x = (float)atof(line);
		in.getline(line, 256); temp4.y = (float)atof(line);
		in.getline(line, 256); temp4.z = (float)atof(line);
		in.getline(line, 256); temp4.w = (float)atof(line);

		levelDataDesc.Team1SpawnPointRotation.push_back(temp4);
	}

	for (int i = 0; i < levelDataDesc.nTeam2SpawnPoint; ++i) {
		XMFLOAT3 temp3;
		XMFLOAT4 temp4;

		in.getline(line, 256); temp3.x = (float)atof(line)* g_fLevelScale;
		in.getline(line, 256); temp3.y = (float)atof(line)* g_fLevelScale;
		in.getline(line, 256); temp3.z = (float)atof(line)* g_fLevelScale;

		levelDataDesc.Team2SpawnPointPosition.push_back(temp3);

		in.getline(line, 256); temp4.x = (float)atof(line);
		in.getline(line, 256); temp4.y = (float)atof(line);
		in.getline(line, 256); temp4.z = (float)atof(line);
		in.getline(line, 256); temp4.w = (float)atof(line);

		levelDataDesc.Team2SpawnPointRotation.push_back(temp4);
	}

	in.getline(line, 256); levelDataDesc.CapturePointPosition.x = (float)atof(line)* g_fLevelScale;
	in.getline(line, 256); levelDataDesc.CapturePointPosition.y = (float)atof(line)* g_fLevelScale;
	in.getline(line, 256); levelDataDesc.CapturePointPosition.z = (float)atof(line)* g_fLevelScale;
	in.getline(line, 256); levelDataDesc.CapturePointScale.x	= (float)atof(line)* g_fLevelScale* 0.5f;
	in.getline(line, 256); levelDataDesc.CapturePointScale.y	= (float)atof(line)* g_fLevelScale* 0.5f;
	in.getline(line, 256); levelDataDesc.CapturePointScale.z	= (float)atof(line)* g_fLevelScale* 0.5f;
	in.getline(line, 256); levelDataDesc.CapturePointRotation.x = (float)atof(line);
	in.getline(line, 256); levelDataDesc.CapturePointRotation.y = (float)atof(line);
	in.getline(line, 256); levelDataDesc.CapturePointRotation.z = (float)atof(line);
	in.getline(line, 256); levelDataDesc.CapturePointRotation.w = (float)atof(line);


	in.close();




}

extern std::vector<CTexture*>				g_vecTexture;


void CImporter::ImportTexture(const wchar_t * fileName, const char * textureName)
{
	wstring filePath = LASSETPATH;
	filePath += fileName;
	filePath += L".dds";

	CTexture* texture = new CTexture(RESOURCE_TEXTURE2D);
	texture->LoadTextureFromFile(m_pd3dDevice, m_pd3dCommandList, filePath.c_str());
	texture->SetName(textureName);

	g_vecTexture.push_back(texture);
}
