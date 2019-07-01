#include "stdafx.h"
#include "Importer.h"
#include "Object/Object.h"
#include "Animation/AnimationController.h"
#include "Mesh/Mesh.h"

void CImporter::ImportModel(const char * fileName, CTexture * texture, CObject * obj)
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
	���⼭ ������ �а�, �޽� ������ŭ ���� ����, �޽��� �ְ�, �ؽ�ó�� �ְ�,
	�װ� ������Ʈ�� �߰������ ��.

	���� �����ͷ� ������ ��. ������Ʈ�� �� �����͸� ���ͷ� �����ϴ°� �ƴ϶�
	�� ��ü�� ���ͷ� �����ϴ� ���̴ϱ� �װ� ���ο� �ΰ� �۾��ؾ� ��.
	*********************************************************************/
	for (int i = 0; i < modelData.GetNumOfMesh(); ++i) {
		CModel* model = new CModel();
		CAnimatedMesh* tempMesh = new CAnimatedMesh(m_pd3dDevice, m_pd3dCommandList, modelData.GetMeshData(i));
		model->SetMesh(tempMesh);
		model->SetTexture(texture);
		obj->AddModel(model);
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

	in.getline(line, 256); levelDataDesc.BoundaryBoxPosition.x	= atof(line)* g_LevelSize;
	in.getline(line, 256); levelDataDesc.BoundaryBoxPosition.y	= atof(line)* g_LevelSize;
	in.getline(line, 256); levelDataDesc.BoundaryBoxPosition.z	= atof(line)* g_LevelSize;
	in.getline(line, 256); levelDataDesc.BoundaryBoxScale.x		= atof(line)* g_LevelSize * 0.5;
	in.getline(line, 256); levelDataDesc.BoundaryBoxScale.y		= atof(line)* g_LevelSize * 0.5;
	in.getline(line, 256); levelDataDesc.BoundaryBoxScale.z		= atof(line)* g_LevelSize * 0.5;
	in.getline(line, 256); levelDataDesc.BoundaryBoxRotation.x	= atof(line);
	in.getline(line, 256); levelDataDesc.BoundaryBoxRotation.y	= atof(line);
	in.getline(line, 256); levelDataDesc.BoundaryBoxRotation.z	= atof(line);
	in.getline(line, 256); levelDataDesc.BoundaryBoxRotation.w	= atof(line);

	for (int i = 0; i < levelDataDesc.nCollisionMaps; ++i) {
		XMFLOAT3 temp3;
		XMFLOAT4 temp4 = XMFLOAT4(0,0,0,1);

		in.getline(line, 256); temp3.x = atof(line)* g_LevelSize;
		in.getline(line, 256); temp3.y = atof(line)* g_LevelSize;
		in.getline(line, 256); temp3.z = atof(line)* g_LevelSize;

		levelDataDesc.CollisionPosition.push_back(temp3);

		in.getline(line, 256); temp3.x = atof(line)* g_LevelSize* 0.5;
		in.getline(line, 256); temp3.y = atof(line)* g_LevelSize* 0.5;
		in.getline(line, 256); temp3.z = atof(line)* g_LevelSize* 0.5;

		levelDataDesc.CollisionScale.push_back(temp3);

		in.getline(line, 256); temp4.x = atof(line);
		in.getline(line, 256); temp4.y = atof(line);
		in.getline(line, 256); temp4.z = atof(line);
		in.getline(line, 256); temp4.w = atof(line);
		//in.getline(line, 256);
		//in.getline(line, 256);
		//in.getline(line, 256);
		//in.getline(line, 256);

		levelDataDesc.CollisionRotation.push_back(temp4);
	}

	for (int i = 0; i < levelDataDesc.nTeam1SpawnPoint; ++i) {
		XMFLOAT3 temp3;
		XMFLOAT4 temp4;

		in.getline(line, 256); temp3.x = atof(line)* g_LevelSize;
		in.getline(line, 256); temp3.y = atof(line)* g_LevelSize;
		in.getline(line, 256); temp3.z = atof(line)* g_LevelSize;

		levelDataDesc.Team1SpawnPointPosition.push_back(temp3);

		in.getline(line, 256); temp4.x = atof(line);
		in.getline(line, 256); temp4.y = atof(line);
		in.getline(line, 256); temp4.z = atof(line);
		in.getline(line, 256); temp4.w = atof(line);

		levelDataDesc.Team1SpawnPointRotation.push_back(temp4);
	}

	for (int i = 0; i < levelDataDesc.nTeam2SpawnPoint; ++i) {
		XMFLOAT3 temp3;
		XMFLOAT4 temp4;

		in.getline(line, 256); temp3.x = atof(line)* g_LevelSize;
		in.getline(line, 256); temp3.y = atof(line)* g_LevelSize;
		in.getline(line, 256); temp3.z = atof(line)* g_LevelSize;

		levelDataDesc.Team2SpawnPointPosition.push_back(temp3);

		in.getline(line, 256); temp4.x = atof(line);
		in.getline(line, 256); temp4.y = atof(line);
		in.getline(line, 256); temp4.z = atof(line);
		in.getline(line, 256); temp4.w = atof(line);

		levelDataDesc.Team2SpawnPointRotation.push_back(temp4);
	}

	in.getline(line, 256); levelDataDesc.CapturePointPosition.x = atof(line)* g_LevelSize;
	in.getline(line, 256); levelDataDesc.CapturePointPosition.y = atof(line)* g_LevelSize;
	in.getline(line, 256); levelDataDesc.CapturePointPosition.z = atof(line)* g_LevelSize;
	in.getline(line, 256); levelDataDesc.CapturePointScale.x	= atof(line)* g_LevelSize* 0.5;
	in.getline(line, 256); levelDataDesc.CapturePointScale.y	= atof(line)* g_LevelSize* 0.5;
	in.getline(line, 256); levelDataDesc.CapturePointScale.z	= atof(line)* g_LevelSize* 0.5;
	in.getline(line, 256); levelDataDesc.CapturePointRotation.x = atof(line);
	in.getline(line, 256); levelDataDesc.CapturePointRotation.y = atof(line);
	in.getline(line, 256); levelDataDesc.CapturePointRotation.z = atof(line);
	in.getline(line, 256); levelDataDesc.CapturePointRotation.w = atof(line);


	in.close();




}
