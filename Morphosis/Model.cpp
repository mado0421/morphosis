#include "stdafx.h"
#include "Model.h"


void CModel::AddMesh(CMesh* mesh)
{
	if (meshes) {
		/*
		������ �ִ� �ָ� �ٸ� ���� �ű�� �ű⿡ �߰��ϴ� ������ �ؾ��� ��

		CMesh�� �ٸ� ���� �����ϰ� �� �ּҸ� �����ͼ� �����ϴ� ���� ������ ���.
		�׷� �� ������ �ּҸ� �������� �Ǵ� ���� �ƴѰ�?
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
