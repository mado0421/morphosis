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
//	// TODO: 여기에 반환 구문을 삽입합니다.
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
	여기서 파일을 읽고, 메쉬 개수만큼 모델을 생성, 메쉬를 넣고, 텍스처를 넣고,
	그걸 오브젝트에 추가해줘야 함.

	모델은 포인터로 전달이 됨. 오브젝트는 모델 포인터를 벡터로 관리하는게 아니라
	모델 객체를 벡터로 관리하는 중이니까 그걸 염두에 두고 작업해야 함.
	*********************************************************************/
	for (int i = 0; i < modelData.GetNumOfMesh(); ++i) {
		CModel* model = new CModel();
		CAnimatedMesh* tempMesh = new CAnimatedMesh(m_pd3dDevice, m_pd3dCommandList, modelData.GetMeshData(i));
		model->SetMesh(tempMesh);
		model->SetTexture(texture);
		obj->AddModel(model);
	}
}
