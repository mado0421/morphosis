#include "pch.h"		// default header
#include "헤더.h"
#include <vector>
#define TEST

using namespace std;

struct Deformer {
	__int64 ID;
};

vector<Cluster*> vCluster;
vector<Deformer*> vDeformer;

char * FileRead(char * pDest, const char * fileName) {
	streampos size;
	char filePath[_MAX_PATH];

	strcpy(filePath, fileName);
	strcat(filePath, ".FBX");

	ifstream file(filePath, ios::in | ios::ate);
	if (!file.is_open()) return nullptr;

	size = file.tellg();
	pDest = new char[size];
	file.seekg(0, ios::beg);
	file.read(pDest, size);
	file.close();

	return pDest;
}
void ExportGeometry(char * pFile, const char * fileName);
void ExportCluster(char * pFile, const char * fileName);
void ExportConnection(char * pFile, const char * fileName);

int main()
{
	char inputDefaultPath[_MAX_PATH];
	char outputDefaultPath[_MAX_PATH];
	strcpy(inputDefaultPath, "Input\\");
	strcpy(outputDefaultPath, "Output\\");

	while (true) {
		char * fileBlock = nullptr;
		char inputFilePath[_MAX_PATH];
		char outputFilePath[_MAX_PATH];
		char fileName[_MAX_PATH];

#ifdef TEST
		strcpy(inputFilePath, "Input\\animation_multipleMesh_0");
		strcpy(outputFilePath, "Output\\animation_multipleMesh_0");
#else
		cout << "변환하려는 파일의 이름을 적어주세요\n";
		cin >> fileName;

		strcpy(inputFilePath, inputDefaultPath);
		strcat(inputFilePath, fileName);
		strcpy(outputFilePath, outputDefaultPath);
		strcat(outputFilePath, fileName);
#endif

		fileBlock = FileRead(fileBlock, inputFilePath);

		if (nullptr == fileBlock) {
			cout << "다시 입력해주세요\n";
			continue;
		}


		//ExportGeometry(fileBlock, outputFilePath);

		//fileBlock = FileRead(fileBlock, inputFilePath);
		ExportCluster(fileBlock, outputFilePath);

		delete[] fileBlock;
	}
}

void ExportGeometry(char * pFile, const char * fileName) {
	//; Object properties 나올 때까지 찾기
	// strtok, strchr, strstr
	int nModel;
	int nGeometry;
	int nNodeAttribute;
	int nAnimationCurve;
	int nAnimationCurveNode;
	int nDeformer;

	Geometry			geo[16];
	Model				model[16];
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

	pch = strstr(pch, "ObjectType: \"Geometry\"");
	pch = strstr(pch, "Count: ");
	pch += strlen("Count: ");
	nGeometry = strtol(pch, &pch, 10);

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
	for (int j = 0; j < nGeometry; ++j) {
		pch = strstr(pch, "Geometry: ");
		pch += strlen("Geometry: ");
		geo[j].ID = _strtoi64(pch, &pch, 10);



		//==========================================================================
		// Vertices
		pch = strstr(pch, "Vertices: *");
		pch += strlen("Vertices: *");

		geo[j].nVertices = strtol(pch, &pch, 10) / 3;
		geo[j].pVertices = new XMFLOAT3[geo[j].nVertices];

		pch = strstr(pch, "a: ");
		pch += strlen("a: ");

		for (int i = 0; i < geo[j].nVertices; ++i) {
			geo[j].pVertices[i].x = strtof(pch, &pch); pch++;
			geo[j].pVertices[i].y = strtof(pch, &pch); pch++;
			geo[j].pVertices[i].z = strtof(pch, &pch); pch++;
		}

		//==========================================================================
		// PolygonVertexIndex
		pch = strstr(pch, "PolygonVertexIndex: *");
		pch += strlen("PolygonVertexIndex: *");

		geo[j].nPolygonVertexIndex = strtol(pch, &pch, 10);
		geo[j].pPolygonVertexIndex = new UINT[geo[j].nPolygonVertexIndex];

		pch = strstr(pch, "a: ");
		pch += strlen("a: ");

		for (int i = 0; i < geo[j].nPolygonVertexIndex; ++i) {
			geo[j].pPolygonVertexIndex[i] = strtol(pch, &pch, 10); pch++;
		}

		//==========================================================================
		// Normal
		pch = strstr(pch, "Normals: *");
		pch += strlen("Normals: *");

		geo[j].nNormals = strtol(pch, &pch, 10) / 3;
		geo[j].pNormals = new XMFLOAT3[geo[j].nNormals];

		pch = strstr(pch, "a: ");
		pch += strlen("a: ");

		for (int i = 0; i < geo[j].nNormals; ++i) {
			geo[j].pNormals[i].x = strtof(pch, &pch); pch++;
			geo[j].pNormals[i].y = strtof(pch, &pch); pch++;
			geo[j].pNormals[i].z = strtof(pch, &pch); pch++;
		}

		//==========================================================================
		// UV
		pch = strstr(pch, "UV: *");
		pch += strlen("UV: *");

		geo[j].nUV = strtol(pch, &pch, 10) / 2;
		geo[j].pUV = new XMFLOAT2[geo[j].nUV];

		pch = strstr(pch, "a: ");
		pch += strlen("a: ");

		for (int i = 0; i < geo[j].nUV; ++i) {
			geo[j].pUV[i].x = strtof(pch, &pch); pch++;
			geo[j].pUV[i].y = 1 - strtof(pch, &pch); pch++;	// uv 의 v는 위아래를 뒤집기 위해 1 - val로 함
		}

		//==========================================================================
		// PolygonVertexIndex
		pch = strstr(pch, "UVIndex: *");
		pch += strlen("UVIndex: *");

		geo[j].nUVIndex = strtol(pch, &pch, 10);
		geo[j].pUVIndex = new UINT[geo[j].nUVIndex];

		pch = strstr(pch, "a: ");
		pch += strlen("a: ");

		for (int i = 0; i < geo[j].nUVIndex; ++i) {
			geo[j].pUVIndex[i] = strtol(pch, &pch, 10); pch++;
		}
	}

	for (int j = 0; j < nModel; ++j) {
		static int idx;
		static int Limbidx;
		__int64 ID;
		char filePath[_MAX_PATH];
		char additionalName[_MAX_PATH];
		char * forCheck = nullptr;

		pch = strstr(pch, "Model: ");
		pch += strlen("Model: ");
		ID = _strtoi64(pch, &pch, 10);

		forCheck = strdup(pch);
		strtok(forCheck, "{");
		if (NULL != strstr(forCheck, "Mesh"))
		{
			itoa(idx, additionalName, 10);
			strcpy(filePath, fileName);
			strcat(filePath, "_mesh_(");
			strcat(filePath, additionalName);
			strcat(filePath, ").dat");

			//{ ~ }까지 내용을 자르기 위함
			pch = strstr(pch, "{");

			forCheck = strdup(pch);
			strtok(forCheck, "}");
			if (NULL != strstr(forCheck, "Lcl Translation")) {
				forCheck = strstr(forCheck, "\"Lcl Translation\"");
				forCheck = strstr(forCheck, ","); forCheck++;
				forCheck = strstr(forCheck, ","); forCheck++;
				forCheck = strstr(forCheck, ","); forCheck++;
				forCheck = strstr(forCheck, ","); forCheck++;
				geo[idx].LclTranslation.x = strtof(forCheck, &forCheck); forCheck++;
				geo[idx].LclTranslation.y = strtof(forCheck, &forCheck); forCheck++;
				geo[idx].LclTranslation.z = strtof(forCheck, &forCheck); forCheck++;
			}

			forCheck = strdup(pch);
			strtok(forCheck, "}");
			if (NULL != strstr(forCheck, "Lcl Rotation")) {
				forCheck = strstr(forCheck, "\"Lcl Rotation\"");
				forCheck = strstr(forCheck, ","); forCheck++;
				forCheck = strstr(forCheck, ","); forCheck++;
				forCheck = strstr(forCheck, ","); forCheck++;
				forCheck = strstr(forCheck, ","); forCheck++;
				geo[idx].LclRotation.x = strtof(forCheck, &forCheck); forCheck++;
				geo[idx].LclRotation.y = strtof(forCheck, &forCheck); forCheck++;
				geo[idx].LclRotation.z = strtof(forCheck, &forCheck); forCheck++;
			}

			ofstream out(filePath, ofstream::binary);

			out.write((char*)&geo[idx].ID, sizeof(__int64));
			out.write((char*)&geo[idx].nVertices, sizeof(UINT));
			out.write((char*)&geo[idx].pVertices, sizeof(XMFLOAT3)*geo[idx].nVertices);

			out.write((char*)&geo[idx].nPolygonVertexIndex, sizeof(UINT));
			out.write((char*)&geo[idx].pPolygonVertexIndex, sizeof(UINT)*geo[idx].nPolygonVertexIndex);

			out.write((char*)&geo[idx].nNormals, sizeof(UINT));
			out.write((char*)&geo[idx].pNormals, sizeof(XMFLOAT3)*geo[idx].nNormals);

			out.write((char*)&geo[idx].nUV, sizeof(UINT));
			out.write((char*)&geo[idx].pUV, sizeof(XMFLOAT2)*geo[idx].nUV);

			out.write((char*)&geo[idx].nUVIndex, sizeof(UINT));
			out.write((char*)&geo[idx].pUVIndex, sizeof(UINT)*geo[idx].nUVIndex);

			out.write((char*)&geo[idx].LclTranslation, sizeof(XMFLOAT3));
			out.write((char*)&geo[idx].LclRotation, sizeof(XMFLOAT3));

			idx++;
			out.close();
		}
		else
		{
			forCheck = strdup(pch);
			strtok(forCheck, "{");
			if (NULL != strstr(forCheck, "LimbNode")) {

				LimbNode temp;

				temp.ID = ID;

				//{ ~ }까지 내용을 자르기 위함
				pch = strstr(pch, "{");

				forCheck = strdup(pch);
				strtok(forCheck, "}");
				if (NULL != strstr(forCheck, "Lcl Translation")) {
					forCheck = strstr(forCheck, "\"Lcl Translation\"");
					forCheck = strstr(forCheck, ","); forCheck++;
					forCheck = strstr(forCheck, ","); forCheck++;
					forCheck = strstr(forCheck, ","); forCheck++;
					forCheck = strstr(forCheck, ","); forCheck++;
					temp.LclTranslation.x = strtof(forCheck, &forCheck); forCheck++;
					temp.LclTranslation.y = strtof(forCheck, &forCheck); forCheck++;
					temp.LclTranslation.z = strtof(forCheck, &forCheck); forCheck++;
				}

				forCheck = strdup(pch);
				strtok(forCheck, "}");
				if (NULL != strstr(forCheck, "Lcl Rotation")) {
					forCheck = strstr(forCheck, "\"Lcl Rotation\"");
					forCheck = strstr(forCheck, ","); forCheck++;
					forCheck = strstr(forCheck, ","); forCheck++;
					forCheck = strstr(forCheck, ","); forCheck++;
					forCheck = strstr(forCheck, ","); forCheck++;
					temp.LclRotation.x = strtof(forCheck, &forCheck); forCheck++;
					temp.LclRotation.y = strtof(forCheck, &forCheck); forCheck++;
					temp.LclRotation.z = strtof(forCheck, &forCheck); forCheck++;
				}


				pLimbNode[Limbidx++] = temp;


			}
		}


	}

	cout << "Make Geometry structure complete.\n\n";

//	//==========================================================================
//	// for Animation Data
//	// Model
//	//idx = 0;
//	//for (int i = 0; i < nModel; ++i) {
//	//	__int64 ID;
//	//	char forCheck[100];
//	//	LimbNode temp;
//
//	//	pch = strstr(pch, "Model: ");
//	//	pch += strlen("Model: ");
//	//	ID = _strtoi64(pch, &pch, 10);
//	//	strncat(forCheck, pch, 100);
//	//	if (NULL == strstr(forCheck, "LimbNode")) continue;
//
//	//	temp.ID = ID;
//
//	//	//P: "Lcl Translation", "Lcl Translation", "", "A",
//	//	pch = strstr(pch,	"P: \"Lcl Translation\", \"Lcl Translation\", \"\", \"A\",");
//	//	pch += strlen(		"P: \"Lcl Translation\", \"Lcl Translation\", \"\", \"A\",");
//	//	temp.LclTranslation.x = strtof(pch, &pch); pch++;
//	//	temp.LclTranslation.y = strtof(pch, &pch); pch++;
//	//	temp.LclTranslation.z = strtof(pch, &pch); pch++;
//
//	//	//P: "Lcl Rotation", "Lcl Rotation", "", "A+",
//	//	pch = strstr(pch,	"P: \"Lcl Rotation\", \"Lcl Rotation\", \"\", \"A+\",");
//	//	pch += strlen(		"P: \"Lcl Rotation\", \"Lcl Rotation\", \"\", \"A+\",");
//	//	temp.LclRotation.x = strtof(pch, &pch); pch++;
//	//	temp.LclRotation.y = strtof(pch, &pch); pch++;
//	//	temp.LclRotation.z = strtof(pch, &pch); pch++;
//
//	//	pLimbNode[idx++] = temp;
//	//}
//	//nModel = idx;
//	//for (int i = 0; i < nModel; ++i) {
//	//	pBone[boneIdx].ID = pLimbNode[i].ID;
//	//	pBone[boneIdx++].limbnodeData = &pLimbNode[i];
//
//	//	DB[DBIdx].ID = pLimbNode[i].ID;
//	//	DB[DBIdx].type = TYPE::MODEL;
//	//	DB[DBIdx++].data = &pLimbNode[i];
//	//}
//
//	//==========================================================================
//	// Cluster
//	int idx = 0;
//	for (int i = 0; i < nDeformer; ++i) {
//		__int64 ID;
//		char forCheck[100];
//		Cluster temp;
//		float tempM[16];
//
//		//Deformer: 
//		pch = strstr(pch, "Deformer: ");
//		pch += strlen("Deformer: ");
//		ID = _strtoi64(pch, &pch, 10);
//		strncat(forCheck, pch, 100);
//		if (NULL == strstr(forCheck, "Cluster")) continue;
//
//		temp.ID = ID;
//
//		//Indexes: *
//		pch = strstr(pch, "Indexes: *");
//		pch += strlen("Indexes: *");
//
//		temp.nIndexes = strtol(pch, &pch, 10);
//		temp.pIndexes = new UINT[temp.nIndexes];
//
//		for (int j = 0; j < temp.nIndexes; ++j) {
//			temp.pIndexes[j] = strtol(pch, &pch, 10); pch++;
//		}
//
//		//Weights: *
//		pch = strstr(pch, "Weights: *");
//		pch += strlen("Weights: *");
//
//		temp.nWeights = strtol(pch, &pch, 10);
//		temp.pWeights = new float[temp.nIndexes];
//
//		for (int j = 0; j < temp.nIndexes; ++j) {
//			temp.pWeights[j] = strtof(pch, &pch); pch++;
//		}
//
//
//		//Transform: *
//		pch = strstr(pch, "Transform: *");
//		pch = strstr(pch, "a:");
//		pch += strlen("a:");
//
//		for (int j = 0; j < 16; ++j) {
//			tempM[j] = strtof(pch, &pch); pch++;
//		}
//
//		temp.Transform = {
//			tempM[0],tempM[1],tempM[2],tempM[3],
//			tempM[4],tempM[5],tempM[6],tempM[7],
//			tempM[8],tempM[9],tempM[10],tempM[11],
//			tempM[12],tempM[13],tempM[14],tempM[15]
//		};
//
//		//TransformLink: *
//		pch = strstr(pch, "TransformLink: *");
//		pch = strstr(pch, "a:");
//		pch += strlen("a:");
//
//		for (int j = 0; j < 16; ++j) {
//			tempM[j] = strtof(pch, &pch); pch++;
//		}
//
//		temp.TransformLink = {
//			tempM[0],tempM[1],tempM[2],tempM[3],
//			tempM[4],tempM[5],tempM[6],tempM[7],
//			tempM[8],tempM[9],tempM[10],tempM[11],
//			tempM[12],tempM[13],tempM[14],tempM[15]
//		};
//
//		pCluster[idx++] = temp;
//	}
//	//nDeformer = idx;
//	//for (int i = 0; i < nDeformer; ++i) {
//	//	DB[DBIdx].ID = pCluster[i].ID;
//	//	DB[DBIdx].type = TYPE::DEFORMER;
//	//	DB[DBIdx++].data = &pCluster[i];
//	//}
//
//	//==========================================================================
//	// AnimationCurveNode
//	idx = 0;
//	for (int i = 0; i < nAnimationCurveNode; ++i) {
//		__int64 ID;
//		AnimationCurveNode temp;
//
//		pch = strstr(pch, "AnimationCurveNode: ");
//		pch += strlen("AnimationCurveNode: ");
//		ID = _strtoi64(pch, &pch, 10);
//
//		temp.ID = ID;
//
//		pch = strstr(pch,	"P: \"d|X\", \"Number\", \"\", \"A\",");
//		pch += strlen(		"P: \"d|X\", \"Number\", \"\", \"A\",");
//		temp.value.x = strtof(pch, &pch); pch++;
//
//		pch = strstr(pch,	"P: \"d|Y\", \"Number\", \"\", \"A\",");
//		pch += strlen(		"P: \"d|Y\", \"Number\", \"\", \"A\",");
//		temp.value.y = strtof(pch, &pch); pch++;
//
//		pch = strstr(pch,	"P: \"d|Z\", \"Number\", \"\", \"A\",");
//		pch += strlen(		"P: \"d|Z\", \"Number\", \"\", \"A\",");
//		temp.value.z = strtof(pch, &pch); pch++;
//
//		pAnimationCurveNode[idx++] = temp;
//	}
//	//nAnimationCurveNode = idx;
//	//for (int i = 0; i < nAnimationCurveNode; ++i) {
//	//	DB[DBIdx].ID = pAnimationCurveNode[i].ID;
//	//	DB[DBIdx].type = TYPE::ANIMCURVENODE;
//	//	DB[DBIdx++].data = &pAnimationCurveNode[i];
//	//}
//
//	//==========================================================================
//	// AnimationCurve
//	idx = 0;
//	for (int i = 0; i < nAnimationCurve; ++i) {
//		__int64 ID;
//		AnimationCurve temp;
//
//		//AnimationCurve: 
//		pch = strstr(pch, "AnimationCurve: ");
//		pch += strlen("AnimationCurve: ");
//		ID = _strtoi64(pch, &pch, 10);
//
//		temp.ID = ID;
//
//		pch = strstr(pch, "KeyTime: *");
//		pch += strlen("KeyTime: *");
//		temp.nKeyTime = strtol(pch, &pch, 10); pch++;
//		temp.pKeyTime = new __int64[temp.nKeyTime];
//
//		pch = strstr(pch, "a: ");
//		pch += strlen("a: ");
//
//		for (int j = 0; j < temp.nKeyTime; ++j) temp.pKeyTime[j] = _strtoi64(pch, &pch, 10); pch++;
//
//		pch = strstr(pch, "KeyValueFloat: *");
//		pch += strlen("KeyValueFloat: *");
//		temp.nKeyValueFloat = strtol(pch, &pch, 10); pch++;
//		temp.pKeyValueFloat = new float[temp.nKeyValueFloat];
//
//		pch = strstr(pch, "a: ");
//		pch += strlen("a: ");
//
//		for (int j = 0; j < temp.nKeyValueFloat; ++j) temp.pKeyValueFloat[j] = strtof(pch, &pch); pch++;
//
//		pAnimationCurve[idx++] = temp;
//	}
//	//nAnimationCurve = idx;
//	//for (int i = 0; i < nAnimationCurve; ++i) {
//	//	DB[DBIdx].ID = pAnimationCurve[i].ID;
//	//	DB[DBIdx].type = TYPE::ANIMCURVE;
//	//	DB[DBIdx++].data = &pAnimationCurve[i];
//	//}
//
//	cout << "Make Animation structure complete.\n\n";
//
//	//==========================================================================
//	// ; Object connections
//	//pch = strstr(pch, "; Object connections");
//
//	//while (true) {
//	//	char forCheck[9];
//
//	//	pch = strstr(pch, "C: ");
//	//	if (NULL == pch) break;
//	//	pch += strlen("\"OO\", \",");
//	//	CPDB[CPDBIdx].leftID = _strtoi64(pch, &pch, 10); pch++;
//	//	CPDB[CPDBIdx].rightID = _strtoi64(pch, &pch, 10); pch++;
//	//	for (int i = 0; i < DBIdx; ++i) {
//	//		if (DB[i].ID == CPDB[CPDBIdx].rightID) {
//	//			if (DB[i].type == TYPE::DEFORMER) {
//	//				DB[DBIdx].ID = CPDB[CPDBIdx].leftID;
//	//				DB[DBIdx++].type = TYPE::SUBDEFORMER;
//	//			}
//
//	//		}
//	//	}
//
//
//
//	//	//strncat(forCheck, pch, 8);
//	//	//if (0 == strcmp(forCheck, " \"d | X\"")) CPDB[CPDBIdx].subtype = SUBTYPE::DX;
//	//	//if (0 == strcmp(forCheck, " \"d | Y\"")) CPDB[CPDBIdx].subtype = SUBTYPE::DY;
//	//	//if (0 == strcmp(forCheck, " \"d | Z\"")) CPDB[CPDBIdx].subtype = SUBTYPE::DZ;
//	//	CPDBIdx++;
//	//}
///*
//	cout << "Make CPDB complete.\n\n";
//*/
//
}


void ExportCluster(char * pFile, const char * fileName) {
	char		*pch;
	int			nDeformer;

	pch = strstr(pFile, "ObjectType: \"Deformer\"");
	pch = strstr(pch, "Count: ");
	pch += strlen("Count: ");
	nDeformer = strtol(pch, &pch, 10);

	//char		filePath[_MAX_PATH];
	//char		additionalName[_MAX_PATH];
	//itoa(idx, additionalName, 10);
	//strcpy(filePath, fileName);
	//strcat(filePath, "_cluster_(");
	//strcat(filePath, additionalName);
	//strcat(filePath, ").dat");
	//ofstream out(filePath, ofstream::binary);

	for (int i = 0; i < nDeformer; ++i) {
		__int64		ID;
		char		*forCheck = nullptr;
		Cluster		*tempC = nullptr;
		Deformer	*tempD = nullptr;
		float		tempM[16];

		pch = strstr(pch, "Deformer: ");
		pch += strlen("Deformer: ");
		ID = _strtoi64(pch, &pch, 10);
		forCheck = strdup(pch);
		strtok(forCheck, "}");
		if (strstr(forCheck, "Cluster"))	goto Cluster;
		if (strstr(forCheck, "Skin"))		goto Skin;

	Cluster:
		tempC = new Cluster();
		tempC->ID = ID;

		//Indexes: *
		forCheck = strdup(pch);
		strtok(forCheck, "}");
		if (NULL == strstr(forCheck, "Indexes: *")) {
			tempC->nIndexes = 0;
			tempC->nWeights = 0;
			goto MATRIX;
		}
		pch = strstr(pch, "Indexes: *");
		pch += strlen("Indexes: *");

		tempC->nIndexes = strtol(pch, &pch, 10);
		tempC->pIndexes = new UINT[tempC->nIndexes];

		pch = strstr(pch, "a: ");
		pch += strlen("a: ");

		for (int j = 0; j < tempC->nIndexes; ++j) {
			tempC->pIndexes[j] = strtol(pch, &pch, 10); pch++;
		}

		//Weights: *
		pch = strstr(pch, "Weights: *");
		pch += strlen("Weights: *");

		pch = strstr(pch, "a: ");
		pch += strlen("a: ");

		tempC->nWeights = strtol(pch, &pch, 10);
		tempC->pWeights = new float[tempC->nIndexes];

		for (int j = 0; j < tempC->nIndexes; ++j) {
			tempC->pWeights[j] = strtof(pch, &pch); pch++;
		}


	MATRIX:
		//Transform: *
		pch = strstr(pch, "Transform: *");
		pch = strstr(pch, "a:");
		pch += strlen("a:");

		for (int j = 0; j < 16; ++j) {
			tempM[j] = strtof(pch, &pch); pch++;
		}

		tempC->Transform = {
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

		tempC->TransformLink = {
			tempM[0],tempM[1],tempM[2],tempM[3],
			tempM[4],tempM[5],tempM[6],tempM[7],
			tempM[8],tempM[9],tempM[10],tempM[11],
			tempM[12],tempM[13],tempM[14],tempM[15]
		};
		goto End;
	Skin:
		tempD = new Deformer();
		tempD->ID = ID;

		goto End;
	End:
		if (tempC) vCluster.push_back(tempC);
		if (tempD) vDeformer.push_back(tempD);




		//	//Deformer: 
		//NEXT:
		//	pch = strstr(pch, "Deformer: ");
		//	if (NULL == pch)
		//		break;
		//	pch += strlen("Deformer: ");
		//	if (NULL == pch) 
		//		break;
		//	ID = _strtoi64(pch, &pch, 10);
		//	forCheck = strdup(pch);
		//	strtok(forCheck, "}");
		//	if (NULL == strstr(forCheck, "Cluster")) goto NEXT;

		//	temp.ID = ID;

		//	//Indexes: *
		//	forCheck = strdup(pch);
		//	strtok(forCheck, "}");
		//	if (NULL == strstr(forCheck, "Indexes: *")) {
		//		temp.nIndexes = 0;
		//		temp.nWeights = 0;
		//		goto MATRIX;
		//	}
		//	pch = strstr(pch, "Indexes: *");
		//	pch += strlen("Indexes: *");

		//	temp.nIndexes = strtol(pch, &pch, 10);
		//	temp.pIndexes = new UINT[temp.nIndexes];

		//	pch = strstr(pch, "a: ");
		//	pch += strlen("a: ");

		//	for (int j = 0; j < temp.nIndexes; ++j) {
		//		temp.pIndexes[j] = strtol(pch, &pch, 10); pch++;
		//	}

		//	//Weights: *
		//	pch = strstr(pch, "Weights: *");
		//	pch += strlen("Weights: *");

		//	pch = strstr(pch, "a: ");
		//	pch += strlen("a: ");

		//	temp.nWeights = strtol(pch, &pch, 10);
		//	temp.pWeights = new float[temp.nIndexes];

		//	for (int j = 0; j < temp.nIndexes; ++j) {
		//		temp.pWeights[j] = strtof(pch, &pch); pch++;
		//	}


		//MATRIX:
		//	//Transform: *
		//	pch = strstr(pch, "Transform: *");
		//	pch = strstr(pch, "a:");
		//	pch += strlen("a:");

		//	for (int j = 0; j < 16; ++j) {
		//		tempM[j] = strtof(pch, &pch); pch++;
		//	}

		//	temp.Transform = {
		//		tempM[0],tempM[1],tempM[2],tempM[3],
		//		tempM[4],tempM[5],tempM[6],tempM[7],
		//		tempM[8],tempM[9],tempM[10],tempM[11],
		//		tempM[12],tempM[13],tempM[14],tempM[15]
		//	};

		//	//TransformLink: *
		//	pch = strstr(pch, "TransformLink: *");
		//	pch = strstr(pch, "a:");
		//	pch += strlen("a:");

		//	for (int j = 0; j < 16; ++j) {
		//		tempM[j] = strtof(pch, &pch); pch++;
		//	}

		//	temp.TransformLink = {
		//		tempM[0],tempM[1],tempM[2],tempM[3],
		//		tempM[4],tempM[5],tempM[6],tempM[7],
		//		tempM[8],tempM[9],tempM[10],tempM[11],
		//		tempM[12],tempM[13],tempM[14],tempM[15]
		//	};



		//	out.write((char*)&temp.ID, sizeof(__int64));
		//	out.write((char*)&temp.nIndexes, sizeof(UINT));
		//	out.write((char*)&temp.pIndexes, sizeof(UINT)*temp.nIndexes);
		//	out.write((char*)&temp.nWeights, sizeof(UINT));
		//	out.write((char*)&temp.pWeights, sizeof(float)*temp.nWeights);
		//	out.write((char*)&temp.Transform, sizeof(XMFLOAT4X4));
		//	out.write((char*)&temp.TransformLink, sizeof(XMFLOAT4X4));

		//	idx++;

		//}
		//__int64 end = -1;
		//out.write((char*)&end, sizeof(__int64));

		//out.close();
	}
}

void ExportConnection(char * pFile, const char * fileName)
{
	vector<T2> vT2;
	__int64 left, right;
	char * str = nullptr;
	str = new char[
		strlen("Connections:  {\
		; Model::Box001, Model::RootNode\
		C: \"OO\", 3428172848, 0\
		; Model::Box002, Model::RootNode\
		C: \"OO\", 3428170656, 0\
		; Model::Box003, Model::RootNode\
		C: \"OO\", 3428164080, 0\
		; Model::Bone001, Model::RootNode\
		C: \"OO\", 3428177232, 0\
		; AnimLayer::BaseLayer, AnimStack::Take 001\
		C: \"OO\", 1756864896, 3438785136\
		; AnimCurveNode::R, AnimLayer::BaseLayer\
		C: \"OO\", 3438774944, 1756864896\
		; Geometry::, Model::Box001\
		C: \"OO\", 2467101040, 3428172848\
		; Geometry::, Model::Box002\
		C: \"OO\", 2467096144, 3428170656\
		; Geometry::, Model::Box003\
		C: \"OO\", 2918101392, 3428164080\
		; NodeAttribute::, Model::Bone001\
		C: \"OO\", 1265861296, 3428177232\
		; AnimCurveNode::R, Model::Bone001\
		C: \"OP\", 3438774944, 3428177232, \"Lcl Rotation\"\
		; Deformer::, Geometry::\
		C: \"OO\", 1265864560, 2467101040\
		; Deformer::, Geometry::\
		C: \"OO\", 1265863408, 2467096144\
		; Deformer::, Geometry::\
		C: \"OO\", 1265859568, 2918101392\
		; AnimCurve::, AnimCurveNode::R\
		C: \"OP\", 3430266208, 3438774944, \"d|X\"\
		; AnimCurve::, AnimCurveNode::R\
		C: \"OP\", 3430267648, 3438774944, \"d|Y\"\
		; AnimCurve::, AnimCurveNode::R\
		C: \"OP\", 3430267968, 3438774944, \"d|Z\"\
		; SubDeformer::, Deformer::\
		C: \"OO\", 2478761280, 1265864560\
		; Model::Bone001, SubDeformer::\
		C: \"OO\", 3428177232, 2478761280\
		; SubDeformer::, Deformer::\
		C: \"OO\", 2478754512, 1265863408\
		; Model::Bone001, SubDeformer::\
		C: \"OO\", 3428177232, 2478754512\
		; SubDeformer::, Deformer::\
		C: \"OO\", 2478756768, 1265859568\
		; Model::Bone001, SubDeformer::\
		C: \"OO\", 3428177232, 2478756768\
}\
") ];
	strcpy(str, "Connections:  {\
		; Model::Box001, Model::RootNode\
		C: \"OO\", 3428172848, 0\
		; Model::Box002, Model::RootNode\
		C: \"OO\", 3428170656, 0\
		; Model::Box003, Model::RootNode\
		C: \"OO\", 3428164080, 0\
		; Model::Bone001, Model::RootNode\
		C: \"OO\", 3428177232, 0\
		; AnimLayer::BaseLayer, AnimStack::Take 001\
		C: \"OO\", 1756864896, 3438785136\
		; AnimCurveNode::R, AnimLayer::BaseLayer\
		C: \"OO\", 3438774944, 1756864896\
		; Geometry::, Model::Box001\
		C: \"OO\", 2467101040, 3428172848\
		; Geometry::, Model::Box002\
		C: \"OO\", 2467096144, 3428170656\
		; Geometry::, Model::Box003\
		C: \"OO\", 2918101392, 3428164080\
		; NodeAttribute::, Model::Bone001\
		C: \"OO\", 1265861296, 3428177232\
		; AnimCurveNode::R, Model::Bone001\
		C: \"OP\", 3438774944, 3428177232, \"Lcl Rotation\"\
		; Deformer::, Geometry::\
		C: \"OO\", 1265864560, 2467101040\
		; Deformer::, Geometry::\
		C: \"OO\", 1265863408, 2467096144\
		; Deformer::, Geometry::\
		C: \"OO\", 1265859568, 2918101392\
		; AnimCurve::, AnimCurveNode::R\
		C: \"OP\", 3430266208, 3438774944, \"d|X\"\
		; AnimCurve::, AnimCurveNode::R\
		C: \"OP\", 3430267648, 3438774944, \"d|Y\"\
		; AnimCurve::, AnimCurveNode::R\
		C: \"OP\", 3430267968, 3438774944, \"d|Z\"\
		; SubDeformer::, Deformer::\
		C: \"OO\", 2478761280, 1265864560\
		; Model::Bone001, SubDeformer::\
		C: \"OO\", 3428177232, 2478761280\
		; SubDeformer::, Deformer::\
		C: \"OO\", 2478754512, 1265863408\
		; Model::Bone001, SubDeformer::\
		C: \"OO\", 3428177232, 2478754512\
		; SubDeformer::, Deformer::\
		C: \"OO\", 2478756768, 1265859568\
		; Model::Bone001, SubDeformer::\
		C: \"OO\", 3428177232, 2478756768\
}\
");
	str = strstr(str, "Connections: ");
	str = strtok(str, "}");
	str = strstr(str, ";");
	while (true) {
		str = strstr(str, "C: ");
		if (nullptr == str) break;
		str = strstr(str, ", ");
		str += strlen(", ");
		left = _strtoi64(str, &str, 10); str++;
		right = _strtoi64(str, &str, 10); str++;
		vT2.emplace_back(left, right);
	}
	cout << "Total: " << vT2.size() << "\n\n";
	for (auto p = vT2.begin(); p != vT2.end(); ++p) cout << p->left << ", " << p->right << "\n";
/*
	list<T3> l;
	for (auto p = vT2.begin(); p != vT2.end(); ++p) {
		__int64 targetID = p->right;
		__int64 left = p->left;
		for (auto p2 = l.begin(); p2 != l.end(); ++p2) {
			if (targetID == p2->ID) {
				p2->AddChild(left);
				goto NEXT;
			}
		}

		l.emplace_back(targetID, left);
	NEXT:
		;
	}

	cout << "\n\n";


	for (auto p = l.begin(); p != l.end(); ++p) {
		cout << p->ID << "'s Child:\n";
		for (auto p2 = p->childList.begin(); p2 != p->childList.end(); ++p2) cout << *p2 << " ";
		cout << "\n\n";
	}


	for (auto p = l.begin(); p != l.end(); ++p) {
		for (auto p2 = p->childList.begin(); p2 != p->childList.end(); ++p2)
		{

		}
	}*/
}
