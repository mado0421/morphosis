#pragma once
#include "Mesh.h"
#include "Texture.h"
#include "Material.h"

class CModel
{
public:
	CModel();
	~CModel();

private:
	CMesh * mesh = NULL;
	CTexture * texture = NULL;

};

