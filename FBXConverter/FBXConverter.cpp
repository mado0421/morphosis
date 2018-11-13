#define TEST

#include "pch.h"		// default header
#include <iostream>		// default header
#include <fstream>		// for read/write file
#include <Windows.h>	// for use UINT
#include <DirectXMath.h>// for use types

using namespace std; 
using namespace DirectX;

//struct int4 {
//	int x, y, z, w;
//};
//struct float2 {
//	float u, v;
//};
//struct float3 {
//	float x, y, z;
//};
//struct float4 {
//	float x, y, z, w;
//};
struct Vertex {
	XMFLOAT2	uv;
	XMFLOAT3	pos;
	XMFLOAT3	normal;
	XMFLOAT4	weights;
	XMINT4		boneIdx;
};

// 먼저 Geometry를 채운다.
// 그 다음 그 안의 정보를 이용해서 Vertex를 만들고
// 그걸 바이너리 파일로 저장해서 한 번에 읽어서 올릴 수 있게 만들기
struct Geometry {
	UINT		nVertices;
	XMFLOAT3	*pVertices;
	UINT		nPolygonVertexIndex;
	UINT		*pPolygonVertexIndex;
	UINT		nNormals;
	XMFLOAT3	*pNormals;
	UINT		nUV;
	XMFLOAT2	*pUV;
	UINT		nUVIndex;
	UINT		*pUVIndex;
};


struct LimbNode {
	__int64 ID;
	XMFLOAT3 LclTranslation;
	XMFLOAT3 LclRotation;
};

struct Cluster {
	__int64 ID;
	UINT nIndexes;
	UINT *pIndexes;
	UINT nWeights;
	float *pWeights;
	XMFLOAT4X4 Transform;
	XMFLOAT4X4 TransformLink;
};

struct AnimationCurveNode {
	__int64 SubDeformerID;
	__int64 ID;
	XMFLOAT3 value;

	AnimationCurve *x, *y, *z;
};

struct SubDeformer {
	Cluster *p;
};

struct AnimationCurve {
	__int64 ID;
	UINT nKeyTime;
	__int64 *pKeyTime;
	UINT nKeyValueFloat;
	float *pKeyValueFloat;
};


struct Bone {
	__int64 ID;
	Bone * parent;
	LimbNode * limbnodeData;
	AnimationCurve *x, *y, *z;
	Cluster *cluster;
};

namespace TYPE {
	enum {
		NONE = -1,
		MODEL,
		ANIMCURVENODE,
		ANIMCURVE,
		DEFORMER,
		SUBDEFORMER,
			
		count
	};
}

namespace SUBTYPE {
	enum {
		NONE = -1,
		DX,
		DY,
		DZ,

		count
	};
}

struct Connection {
	__int64 ID;
	int type;
	void * data;
};

struct ConnectionPair {
	__int64 leftID, rightID;
	int subtype;
};

char * FileRead(char * pDest, const char * fileName);
void ProcessFile(char * pFile);

int main()
{
	char fileDefaultPath[_MAX_PATH];
	strcpy(fileDefaultPath, "Input\\");

	while (true) {
		char * fileBlock = nullptr;
		char filePath[_MAX_PATH];
		char fileName[_MAX_PATH];

#ifdef TEST
		strcpy(filePath, "Input\\test_z-up.FBX");
#else
		//cout << "변환하려는 파일의 이름을 적어주세요\n";
		//cin >> fileName;

		//strcpy(filePath, fileDefaultPath);
		//strcat(filePath, fileName);
#endif
		strcpy(filePath, "Input\\test_z-up.FBX");


		fileBlock = FileRead(fileBlock, filePath);
		if (nullptr == fileBlock) {
			cout << "다시 입력해주세요\n";
			continue;
		}

		//process file

		ProcessFile(fileBlock);

		//process end

		delete[] fileBlock;
	}
}

char * FileRead(char * pDest, const char * fileName) {
	streampos size;

	ifstream file(fileName, ios::in | ios::ate);
	if (!file.is_open()) return nullptr;

	size = file.tellg();
	pDest = new char[size];
	file.seekg(0, ios::beg);
	file.read(pDest, size);
	file.close();

	return pDest;
}

void ProcessFile(char * pFile) {
	//; Object properties 나올 때까지 찾기
	// strtok, strchr, strstr
	int nModel;
	int nNodeAttribute;
	int nAnimationCurve;
	int nAnimationCurveNode;
	int nDeformer;
	int idx;

	Geometry			geo;
	LimbNode			pLimbNode[1024];
	Cluster				pCluster[1024];
	AnimationCurveNode	pAnimationCurveNode[1024];
	AnimationCurve		pAnimationCurve[1024];
	char * pch;

	Connection DB[1024];
	ConnectionPair CPDB[1024];
	for (int i = 0; i < 1024; ++i) DB[i].type = TYPE::NONE;
	int DBIdx = 0;
	int CPDBIdx = 0;

	Bone pBone[1024];
	int boneIdx = 0;



	pch = strstr(pFile, "; Object definitions");
	pch = strstr(pch, "ObjectType: \"Model\"");
	pch = strstr(pch, "Count: ");
	pch += strlen("Count: ");
	nModel = strtol(pch, &pch, 10);

	pch = strstr(pch, "ObjectType: \"NodeAttribute\"");
	pch = strstr(pch, "Count: ");
	pch += strlen("Count: ");
	nNodeAttribute = strtol(pch, &pch, 10);

	pch = strstr(pch, "ObjectType: \"AnimationCurve\"");
	pch = strstr(pch, "Count: ");
	pch += strlen("Count: ");
	nAnimationCurve = strtol(pch, &pch, 10);

	pch = strstr(pch, "ObjectType: \"AnimationCurveNode\"");
	pch = strstr(pch, "Count: ");
	pch += strlen("Count: ");
	nAnimationCurveNode = strtol(pch, &pch, 10);

	pch = strstr(pch, "ObjectType: \"Deformer\"");
	pch = strstr(pch, "Count: ");
	pch += strlen("Count: ");
	nDeformer = strtol(pch, &pch, 10);

	pch = strstr(pch, "; Object properties");

	//==========================================================================
	// Vertices
	pch = strstr(pch, "Vertices: *");
	pch += strlen("Vertices: *");

	geo.nVertices = strtol(pch, &pch, 10) / 3;
	geo.pVertices = new XMFLOAT3[geo.nVertices];

	pch = strstr(pch, "a: ");
	pch += strlen("a: ");

	for (int i = 0; i < geo.nVertices; ++i) {
		geo.pVertices[i].x = strtof(pch, &pch); pch++;
		geo.pVertices[i].y = strtof(pch, &pch); pch++;
		geo.pVertices[i].z = strtof(pch, &pch); pch++;
	}

	//==========================================================================
	// PolygonVertexIndex
	pch = strstr(pch, "PolygonVertexIndex: *");
	pch += strlen("PolygonVertexIndex: *");

	geo.nPolygonVertexIndex = strtol(pch, &pch, 10);
	geo.pPolygonVertexIndex = new UINT[geo.nPolygonVertexIndex];

	pch = strstr(pch, "a: ");
	pch += strlen("a: ");

	for (int i = 0; i < geo.nPolygonVertexIndex; ++i) {
		geo.pPolygonVertexIndex[i] = strtol(pch, &pch, 10); pch++;
	}

	//==========================================================================
	// Normal
	pch = strstr(pch, "Normals: *");
	pch += strlen("Normals: *");

	geo.nNormals = strtol(pch, &pch, 10)/3;
	geo.pNormals = new XMFLOAT3[geo.nNormals];

	pch = strstr(pch, "a: ");
	pch += strlen("a: ");

	for (int i = 0; i < geo.nNormals; ++i) {
		geo.pNormals[i].x = strtof(pch, &pch); pch++;
		geo.pNormals[i].y = strtof(pch, &pch); pch++;
		geo.pNormals[i].z = strtof(pch, &pch); pch++;
	}

	//==========================================================================
	// UV
	pch = strstr(pch, "UV: *");
	pch += strlen("UV: *");

	geo.nUV = strtol(pch, &pch, 10)/2;
	geo.pUV = new XMFLOAT2[geo.nUV];

	pch = strstr(pch, "a: ");
	pch += strlen("a: ");

	for (int i = 0; i < geo.nUV; ++i) {
		geo.pUV[i].x = strtof(pch, &pch); pch++;
		geo.pUV[i].y = 1 - strtof(pch, &pch); pch++;	// uv 의 v는 위아래를 뒤집기 위해 1 - val로 함
	}

	//==========================================================================
	// PolygonVertexIndex
	pch = strstr(pch, "UVIndex: *");
	pch += strlen("UVIndex: *");

	geo.nUVIndex = strtol(pch, &pch, 10);
	geo.pUVIndex = new UINT[geo.nUVIndex];

	pch = strstr(pch, "a: ");
	pch += strlen("a: ");

	for (int i = 0; i < geo.nUVIndex; ++i) {
		geo.pUVIndex[i] = strtol(pch, &pch, 10); pch++;
	}

	cout << "Make Geometry structure complete.\n\n";

	//==========================================================================
	// for Animation Data
	// Model
	idx = 0;
	for (int i = 0; i < nModel; ++i) {
		__int64 ID;
		char forCheck[100];
		LimbNode temp;

		pch = strstr(pch, "Model: ");
		pch += strlen("Model: ");
		ID = _strtoi64(pch, &pch, 10);
		strncat(forCheck, pch, 100);
		if (NULL == strstr(forCheck, "LimbNode")) continue;

		temp.ID = ID;

		//P: "Lcl Translation", "Lcl Translation", "", "A",
		pch = strstr(pch,	"P: \"Lcl Translation\", \"Lcl Translation\", \"\", \"A\",");
		pch += strlen(		"P: \"Lcl Translation\", \"Lcl Translation\", \"\", \"A\",");
		temp.LclTranslation.x = strtof(pch, &pch); pch++;
		temp.LclTranslation.y = strtof(pch, &pch); pch++;
		temp.LclTranslation.z = strtof(pch, &pch); pch++;

		//P: "Lcl Rotation", "Lcl Rotation", "", "A+",
		pch = strstr(pch,	"P: \"Lcl Rotation\", \"Lcl Rotation\", \"\", \"A+\",");
		pch += strlen(		"P: \"Lcl Rotation\", \"Lcl Rotation\", \"\", \"A+\",");
		temp.LclRotation.x = strtof(pch, &pch); pch++;
		temp.LclRotation.y = strtof(pch, &pch); pch++;
		temp.LclRotation.z = strtof(pch, &pch); pch++;

		pLimbNode[idx++] = temp;
	}
	nModel = idx;
	for (int i = 0; i < nModel; ++i) {
		pBone[boneIdx].ID = pLimbNode[i].ID;
		pBone[boneIdx++].limbnodeData = &pLimbNode[i];

		DB[DBIdx].ID = pLimbNode[i].ID;
		DB[DBIdx].type = TYPE::MODEL;
		DB[DBIdx++].data = &pLimbNode[i];
	}

	//==========================================================================
	// Cluster
	idx = 0;
	for (int i = 0; i < nDeformer; ++i) {
		__int64 ID;
		char forCheck[100];
		Cluster temp;
		float tempM[16];

		//Deformer: 
		pch = strstr(pch, "Deformer: ");
		pch += strlen("Deformer: ");
		ID = _strtoi64(pch, &pch, 10);
		strncat(forCheck, pch, 100);
		if (NULL == strstr(forCheck, "Cluster")) continue;

		temp.ID = ID;

		//Indexes: *
		pch = strstr(pch, "Indexes: *");
		pch += strlen("Indexes: *");

		temp.nIndexes = strtol(pch, &pch, 10);
		temp.pIndexes = new UINT[temp.nIndexes];

		for (int j = 0; j < temp.nIndexes; ++j) {
			temp.pIndexes[j] = strtol(pch, &pch, 10); pch++;
		}

		//Weights: *
		pch = strstr(pch, "Weights: *");
		pch += strlen("Weights: *");

		temp.nWeights = strtol(pch, &pch, 10);
		temp.pWeights = new float[temp.nIndexes];

		for (int j = 0; j < temp.nIndexes; ++j) {
			temp.pWeights[j] = strtof(pch, &pch); pch++;
		}


		//Transform: *
		pch = strstr(pch, "Transform: *");
		pch = strstr(pch, "a:");
		pch += strlen("a:");

		for (int j = 0; j < 16; ++j) {
			tempM[j] = strtof(pch, &pch); pch++;
		}

		temp.Transform = {
			tempM[0],tempM[1],tempM[2],tempM[3],
			tempM[4],tempM[5],tempM[6],tempM[7],
			tempM[8],tempM[9],tempM[10],tempM[11],
			tempM[12],tempM[13],tempM[14],tempM[15]
		};

		//TransformLink: *
		pch = strstr(pch, "TransformLink: *");
		pch = strstr(pch, "a:");
		pch += strlen("a:");

		for (int j = 0; j < 16; ++j) {
			tempM[j] = strtof(pch, &pch); pch++;
		}

		temp.TransformLink = {
			tempM[0],tempM[1],tempM[2],tempM[3],
			tempM[4],tempM[5],tempM[6],tempM[7],
			tempM[8],tempM[9],tempM[10],tempM[11],
			tempM[12],tempM[13],tempM[14],tempM[15]
		};

		pCluster[idx++] = temp;
	}
	nDeformer = idx;
	for (int i = 0; i < nDeformer; ++i) {
		DB[DBIdx].ID = pCluster[i].ID;
		DB[DBIdx].type = TYPE::DEFORMER;
		DB[DBIdx++].data = &pCluster[i];
	}

	//==========================================================================
	// AnimationCurveNode
	idx = 0;
	for (int i = 0; i < nAnimationCurveNode; ++i) {
		__int64 ID;
		AnimationCurveNode temp;

		pch = strstr(pch, "AnimationCurveNode: ");
		pch += strlen("AnimationCurveNode: ");
		ID = _strtoi64(pch, &pch, 10);

		temp.ID = ID;

		pch = strstr(pch,	"P: \"d|X\", \"Number\", \"\", \"A\",");
		pch += strlen(		"P: \"d|X\", \"Number\", \"\", \"A\",");
		temp.value.x = strtof(pch, &pch); pch++;

		pch = strstr(pch,	"P: \"d|Y\", \"Number\", \"\", \"A\",");
		pch += strlen(		"P: \"d|Y\", \"Number\", \"\", \"A\",");
		temp.value.y = strtof(pch, &pch); pch++;

		pch = strstr(pch,	"P: \"d|Z\", \"Number\", \"\", \"A\",");
		pch += strlen(		"P: \"d|Z\", \"Number\", \"\", \"A\",");
		temp.value.z = strtof(pch, &pch); pch++;

		pAnimationCurveNode[idx++] = temp;
	}
	nAnimationCurveNode = idx;
	for (int i = 0; i < nAnimationCurveNode; ++i) {
		DB[DBIdx].ID = pAnimationCurveNode[i].ID;
		DB[DBIdx].type = TYPE::ANIMCURVENODE;
		DB[DBIdx++].data = &pAnimationCurveNode[i];
	}

	//==========================================================================
	// AnimationCurve
	idx = 0;
	for (int i = 0; i < nAnimationCurve; ++i) {
		__int64 ID;
		AnimationCurve temp;

		//AnimationCurve: 
		pch = strstr(pch, "AnimationCurve: ");
		pch += strlen("AnimationCurve: ");
		ID = _strtoi64(pch, &pch, 10);

		temp.ID = ID;

		pch = strstr(pch, "KeyTime: *");
		pch += strlen("KeyTime: *");
		temp.nKeyTime = strtol(pch, &pch, 10); pch++;
		temp.pKeyTime = new __int64[temp.nKeyTime];

		pch = strstr(pch, "a: ");
		pch += strlen("a: ");

		for (int j = 0; j < temp.nKeyTime; ++j) temp.pKeyTime[j] = _strtoi64(pch, &pch, 10); pch++;

		pch = strstr(pch, "KeyValueFloat: *");
		pch += strlen("KeyValueFloat: *");
		temp.nKeyValueFloat = strtol(pch, &pch, 10); pch++;
		temp.pKeyValueFloat = new float[temp.nKeyValueFloat];

		pch = strstr(pch, "a: ");
		pch += strlen("a: ");

		for (int j = 0; j < temp.nKeyValueFloat; ++j) temp.pKeyValueFloat[j] = strtof(pch, &pch); pch++;

		pAnimationCurve[idx++] = temp;
	}
	nAnimationCurve = idx;
	for (int i = 0; i < nAnimationCurve; ++i) {
		DB[DBIdx].ID = pAnimationCurve[i].ID;
		DB[DBIdx].type = TYPE::ANIMCURVE;
		DB[DBIdx++].data = &pAnimationCurve[i];
	}

	cout << "Make Animation structure complete.\n\n";

	//==========================================================================
	// ; Object connections
	pch = strstr(pch, "; Object connections");

	while (true) {
		char forCheck[9];

		pch = strstr(pch, "C: ");
		if (NULL == pch) break;
		pch += strlen("\"OO\", \",");
		CPDB[CPDBIdx].leftID = _strtoi64(pch, &pch, 10); pch++;
		CPDB[CPDBIdx].rightID = _strtoi64(pch, &pch, 10); pch++;
		for (int i = 0; i < DBIdx; ++i) {
			if (DB[i].ID == CPDB[CPDBIdx].rightID) {
				if (DB[i].type == TYPE::DEFORMER) {
					DB[DBIdx].ID = CPDB[CPDBIdx].leftID;
					DB[DBIdx++].type = TYPE::SUBDEFORMER;
				}

			}
		}



		//strncat(forCheck, pch, 8);
		//if (0 == strcmp(forCheck, " \"d | X\"")) CPDB[CPDBIdx].subtype = SUBTYPE::DX;
		//if (0 == strcmp(forCheck, " \"d | Y\"")) CPDB[CPDBIdx].subtype = SUBTYPE::DY;
		//if (0 == strcmp(forCheck, " \"d | Z\"")) CPDB[CPDBIdx].subtype = SUBTYPE::DZ;
		CPDBIdx++;
	}

	cout << "Make CPDB complete.\n\n";



	while (true) {

		for (int i = 0; i < CPDBIdx; ++i) {
			for (int j = 0; j < DBIdx; ++j) {

				if (CPDB[i].leftID == DB[j].ID) {
					for (int k = j + 1; k < DBIdx; ++k) {
						if (CPDB[i].rightID == DB[k].ID) {

							if (DB[j].type == TYPE::MODEL &&
								DB[j].type == TYPE::MODEL) {
								for (int l = 0; l < boneIdx; ++l) {
									if (pBone[l].ID == CPDB[i].leftID) {
										pBone[l].parent;
									}
								}
							}
								
						}
					}


				}
			}
		}



	}




}