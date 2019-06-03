#include "stdafx.h"
#include "Importer.h"
#include "Object/Object.h"
#include "Animation/AnimationData.h"

void CImporter::ImportFile(const char * fileName, CTexture * pTexture, ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, CAnimationPlayerObject & obj)
{
	string filePath("Assets/");
	filePath += fileName;

	string animDataName = filePath;
	animDataName += "_anim.dat";
	ImportAnimData animData;
	animData.ImportFile(animDataName.c_str());

	string modelDataName = filePath;
	modelDataName += "_mesh.dat";
	ImportModelData modelData;
	modelData.ImportFile(modelDataName.c_str());

	CModel* model = new CModel();
	for (int i = 0; i < modelData.GetNumOfMesh(); ++i) {
		CAnimMesh* tempMesh = new CAnimMesh(pd3dDevice, pd3dCommandList, modelData.GetMeshData(i));
		model->AddMesh(tempMesh);
	}
	model->SetTexture(pTexture);
	obj.SetModel(model);

	obj.anim = new AnimationData();
	obj.anim->Init(animData);

}

void CImporter::ImportFile(const char * fileName, AnimationData* AnimData)
{
	string filePath("Assets/");
	filePath += fileName;

	string animDataName = filePath;
	animDataName += "_anim.dat";
	ImportAnimData animData;
	animData.ImportFile(animDataName.c_str());

	//AnimData = new AnimationData();
	AnimData->Init(animData);
}

void CImporter::ImportFile(const char * fileName, ImportAnimData* AnimData)
{
	string filePath("Assets/");
	filePath += fileName;

	string animDataName = filePath;
	animDataName += "_anim.dat";
	AnimData->ImportFile(animDataName.c_str());
}
