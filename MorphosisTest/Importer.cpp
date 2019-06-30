#include "stdafx.h"
#include "Importer.h"
#include "Object.h"
#include "AnimationController.h"
#include "Mesh.h"

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
