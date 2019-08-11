#include "stdafx.h"
#include "Importer.h"
#include "Mesh.h"
#include "Texture.h"

Importer::Importer(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
	: m_pd3dDevice(pd3dDevice)
	, m_pd3dCommandList(pd3dCommandList)
{
}

void Importer::ImportMesh(const char * fileName, const char * meshName, MeshType type)
{
	string meshDataName = ASSET_PATH;
	meshDataName += fileName;
	meshDataName += ".dat";

	ifstream in;

	in.open(meshDataName, std::ios::in | std::ios::binary);
	char Name[32];
	in.read((char*)&Name, sizeof(Name));
	string ModelName = Name;

	int nMesh;
	in.read((char*)&nMesh, sizeof(int));
	vector<MeshData> vecMesh;
	vecMesh.resize(nMesh);
	for (int i = 0; i < nMesh; ++i) {
		char name[32];
		in.read((char*)name, sizeof(name));
		vecMesh[i].meshName = name;

		int nCtrlPoint;
		in.read((char*)&nCtrlPoint, sizeof(int));
		vecMesh[i].vecCtrlPoint.resize(nCtrlPoint);
		for (int j = 0; j < nCtrlPoint; ++j) {
			CtrlPointData temp;
			in.read((char*)&temp, sizeof(CtrlPointData));
			vecMesh[i].vecCtrlPoint[j] = temp;
		}

		int nVertex;
		in.read((char*)&nVertex, sizeof(int));
		vecMesh[i].vecVertex.resize(nVertex);
		for (int j = 0; j < nVertex; ++j) {
			VertexData temp;
			in.read((char*)&temp, sizeof(VertexData));
			vecMesh[i].vecVertex[j] = temp;
		}
	}

	if (MeshType::MODEL == type) {
		for (int i = 0; i < vecMesh.size(); ++i) {
			string partName = ModelName;
			partName += "_";
			partName += vecMesh[i].meshName;

			ModelMesh* mesh		= new ModelMesh(m_pd3dDevice, m_pd3dCommandList, vecMesh[i]);
			mesh->m_MeshName	= partName;
			g_vecMesh.push_back(mesh);
		}
	}
	else if (MeshType::ANIMATEDMODEL == type) {
		for (int i = 0; i < vecMesh.size(); ++i) {
			string partName = ModelName;
			partName += "_";
			partName += vecMesh[i].meshName;

			AnimatedModelMesh* mesh = new AnimatedModelMesh(m_pd3dDevice, m_pd3dCommandList, vecMesh[i]);
			mesh->m_MeshName = partName;
			g_vecMesh.push_back(mesh);
		}
	}
}

void Importer::ImportTexture(const wchar_t * fileName, const char * textureName)
{
	wstring filePath = LASSET_PATH;
	filePath += fileName;
	filePath += L".dds";

	Texture* texture = new Texture(RESOURCE_TEXTURE2D);
	texture->LoadTextureFromFile(m_pd3dDevice, m_pd3dCommandList, filePath.c_str());
	texture->m_textureName = textureName;

	g_vecTexture.push_back(texture);
}
