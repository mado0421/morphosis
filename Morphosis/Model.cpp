#include "stdafx.h"
#include "Model.h"


void CModel::AddMesh(CMesh* mesh)
{
	if (meshes) {
		/*
		기존에 있던 애를 다른 곳에 옮기고 거기에 추가하는 식으로 해야할 것

		CMesh를 다른 곳에 생성하고 그 주소를 가져와서 관리하는 이중 포인터 방식.
		그럼 그 포인터 주소만 가져가도 되는 것이 아닌가?
		*/
		CMesh **temp = new CMesh*[++numOfMeshes];
		for (int i = 0; i < numOfMeshes - 1; ++i) temp[i] = meshes[i];
		temp[numOfMeshes - 1] = mesh;
		meshes = temp;
	}
	else {
		numOfMeshes = 1;
		meshes = new CMesh*[numOfMeshes];
		meshes[0] = mesh;
	}
//	meshArr.push_back((*mesh));
}

void CModel::SetTexture(CTexture* pTexture)
{
	texture = pTexture;
}

void CModel::Render(ID3D12GraphicsCommandList *pd3dCommandList)
{
	if (texture) texture->UpdateShaderVariables(pd3dCommandList);
	for (int i = 0; i < numOfMeshes; ++i) meshes[i]->Render(pd3dCommandList);
//	for (auto iter = meshArr.begin(); iter != meshArr.end(); ++iter) iter->Render(pd3dCommandList);
}

void CModel::UpdateShaderVar(ID3D12GraphicsCommandList * pd3dCommandList)
{
	texture->UpdateShaderVariables(pd3dCommandList);
}

CModel::CModel()
{
}


CModel::~CModel()
{
}
