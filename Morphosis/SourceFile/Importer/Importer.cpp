#include "stdafx.h"
#include "Importer.h"
#include "Object/Object.h"
#include "Animation/AnimationController.h"
#include "Mesh/Mesh.h"

//void CImporter::ImportFile(const char * fileName, CTexture * pTexture, ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, CAnimationPlayerObject & obj)
//{
//	string filePath("Assets/");
//	filePath += fileName;
//
//	string animDataName = filePath;
//	animDataName += "_anim.dat";
//	AnimationClip animData;
//	animData.ImportFile(animDataName.c_str());
//
//	string modelDataName = filePath;
//	modelDataName += "_mesh.dat";
//	ImportModelData modelData;
//	modelData.ImportFile(modelDataName.c_str());
//
//	CModel* model = new CModel();
//	for (int i = 0; i < modelData.GetNumOfMesh(); ++i) {
//		CAnimatedMesh* tempMesh = new CAnimatedMesh(pd3dDevice, pd3dCommandList, modelData.GetMeshData(i));
//		model->AddMesh(tempMesh);
//	}
//	model->SetTexture(pTexture);
//	obj.SetModel(model);
//
//	obj.anim = new CAnimationController();
//	obj.anim->Init(animData);
//
//}
//void CImporter::ImportFile(const char * fileName, CAnimationController* AnimData)
//{
//	string filePath("Assets/");
//	filePath += fileName;
//
//	string animDataName = filePath;
//	animDataName += "_anim.dat";
//	AnimationClip animData;
//	animData.ImportFile(animDataName.c_str());
//
//	//AnimData = new CAnimationController();
//	AnimData->Init(animData);
//}
//void CImporter::ImportFile(const char * fileName, AnimationClip* AnimData)
//{
//	string filePath("Assets/");
//	filePath += fileName;
//
//	string animDataName = filePath;
//	animDataName += "_anim.dat";
//	AnimData->ImportFile(animDataName.c_str());
//}

//AnimationClip & CImporter::ImportAnimClip(const char * fileName)
//{
//	string filePath("Assets/");
//	filePath += fileName;
//
//	string animDataName = filePath;
//	animDataName += "_anim.dat";
//	AnimationClip animData;
//	animData.ImportFile(animDataName.c_str());
//
//	return animData;
//}
//
//CModel & CImporter::ImportModel(const char * fileName)
//{
//
//	// TODO: ���⿡ ��ȯ ������ �����մϴ�.
//}

//void CImporter::ImportModel(const char * fileName, CObject * obj)
//{
//	string modelDataName = fileName;
//	modelDataName += "_mesh.dat";
//	ImportModelData modelData;
//	modelData.ImportFile(modelDataName.c_str());
//
//	CModel* model = new CModel();
//	for (int i = 0; i < modelData.GetNumOfMesh(); ++i) {
//		CAnimatedMesh* tempMesh = new CAnimatedMesh(m_pd3dDevice, m_pd3dCommandList, modelData.GetMeshData(i));
//		model->SetMesh(tempMesh);
//	}
//	model->SetTexture(pTexture);
//	obj.SetModel(model);
//
//	obj.anim = new CAnimationController();
//	obj.anim->Init(animData);
//}

void CImporter::ImportModel(const char * fileName, CTexture * texture, CObject * obj)
{
	string modelDataName = fileName;
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
