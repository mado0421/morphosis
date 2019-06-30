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
