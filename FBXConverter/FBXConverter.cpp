#include "pch.h"		// default header
#include "헤더.h"
#define TEST

using namespace std;

vector<Geometry*>				vGeometry;
vector<Cluster*>				vCluster;
vector<Deformer*>				vDeformer;
vector<Model*>					vModel;
vector<Model*>					vMesh;
vector<Model*>					vLimbNode;
vector<AnimationCurve *>		vAnimCurve;
vector<AnimationCurveNode *>	vAnimCurveNode;

vector<Connection>				vConnection;

vector< EXGeometry*>			vEXGeometry;
vector< EXBone*>				vEXBone;

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
void ExportGeometry(char * pFile);
void ExportCluster(char * pFile);
void ExportConnection(char * pFile);
void ExportModel(char * pFile);
void ExportAnimCurveNode(char * pFile);
void ExportAnimCurve(char * pFile);

void ConnectingGeoToCluster();

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
		strcpy(inputFilePath, "Input\\animation_character_0");
		strcpy(outputFilePath, "Output\\animation_character_0");
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



		//fileBlock = FileRead(fileBlock, inputFilePath);
		ExportGeometry(fileBlock);
		ExportCluster(fileBlock);
		ExportConnection(fileBlock);
		ExportModel(fileBlock);
		ExportAnimCurveNode(fileBlock);
		ExportAnimCurve(fileBlock);

		ConnectingGeoToCluster();

		delete[] fileBlock;
	}
}

void ExportGeometry(char * pFile) {

	int nGeometry;
	char * pch;
	Geometry *temp = nullptr;

	pch = strstr(pFile, "; Object definitions");

	pch = strstr(pch, "ObjectType: \"Geometry\"");
	pch = strstr(pch, "Count: ");
	pch += strlen("Count: ");
	nGeometry = strtol(pch, &pch, 10);

	pch = strstr(pch, "; Object properties");
	for (int j = 0; j < nGeometry; ++j) {

		temp = new Geometry();

		pch = strstr(pch, "Geometry: ");
		pch += strlen("Geometry: ");
		temp->ID = _strtoi64(pch, &pch, 10);

		pch = strstr(pch, "Vertices: *");
		pch += strlen("Vertices: *");

		temp->nVertices = strtol(pch, &pch, 10) / 3;
		temp->pVertices = new XMFLOAT3[temp->nVertices];

		pch = strstr(pch, "a: ");
		pch += strlen("a: ");

		for (int i = 0; i < temp->nVertices; ++i) {
			temp->pVertices[i].x = strtof(pch, &pch); pch++;
			temp->pVertices[i].y = strtof(pch, &pch); pch++;
			temp->pVertices[i].z = strtof(pch, &pch); pch++;
		}

		pch = strstr(pch, "PolygonVertexIndex: *");
		pch += strlen("PolygonVertexIndex: *");

		temp->nPolygonVertexIndex = strtol(pch, &pch, 10);
		temp->pPolygonVertexIndex = new UINT[temp->nPolygonVertexIndex];

		pch = strstr(pch, "a: ");
		pch += strlen("a: ");

		for (int i = 0; i < temp->nPolygonVertexIndex; ++i) {
			temp->pPolygonVertexIndex[i] = strtol(pch, &pch, 10); pch++;
		}

		pch = strstr(pch, "Normals: *");
		pch += strlen("Normals: *");

		temp->nNormals = strtol(pch, &pch, 10) / 3;
		temp->pNormals = new XMFLOAT3[temp->nNormals];

		pch = strstr(pch, "a: ");
		pch += strlen("a: ");

		for (int i = 0; i < temp->nNormals; ++i) {
			temp->pNormals[i].x = strtof(pch, &pch); pch++;
			temp->pNormals[i].y = strtof(pch, &pch); pch++;
			temp->pNormals[i].z = strtof(pch, &pch); pch++;
		}

		pch = strstr(pch, "UV: *");
		pch += strlen("UV: *");

		temp->nUV = strtol(pch, &pch, 10) / 2;
		temp->pUV = new XMFLOAT2[temp->nUV];

		pch = strstr(pch, "a: ");
		pch += strlen("a: ");

		for (int i = 0; i < temp->nUV; ++i) {
			temp->pUV[i].x = strtof(pch, &pch); pch++;
			temp->pUV[i].y = 1 - strtof(pch, &pch); pch++;	// uv 의 v는 위아래를 뒤집기 위해 1 - val로 함
		}

		pch = strstr(pch, "UVIndex: *");
		pch += strlen("UVIndex: *");

		temp->nUVIndex = strtol(pch, &pch, 10);
		temp->pUVIndex = new UINT[temp->nUVIndex];

		pch = strstr(pch, "a: ");
		pch += strlen("a: ");

		for (int i = 0; i < temp->nUVIndex; ++i) {
			temp->pUVIndex[i] = strtol(pch, &pch, 10); pch++;
		}

		vGeometry.push_back(temp);
	}
	cout << "Make Geometry structure complete.\n\n";
}

void ExportCluster(char * pFile) {
	char		*pch;
	int			nDeformer;

	pch = strstr(pFile, "ObjectType: \"Deformer\"");
	pch = strstr(pch, "Count: ");
	pch += strlen("Count: ");
	nDeformer = strtol(pch, &pch, 10);

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

		tempC->nWeights = strtol(pch, &pch, 10);
		tempC->pWeights = new float[tempC->nIndexes];

		pch = strstr(pch, "a: ");
		pch += strlen("a: ");


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
	}
}

void ExportConnection(char * pFile)
{
	char * pch = nullptr;
	char *forCheck = nullptr;

	pch = strstr(pFile, "; Object connections");
	pch = strstr(pch, "Connections:");
	pch = strstr(pch, "{");
	pch = strtok(pch, "}");

	__int64 left, right;

	pch = strstr(pch, ";");
	while (true) {
		pch = strstr(pch, "C: ");
		if (nullptr == pch) break;
		pch = strstr(pch, ",");
		pch += strlen(",");
		left = _strtoi64(pch, &pch, 10); pch++;
		right = _strtoi64(pch, &pch, 10); pch++;

		forCheck = strdup(pch);
		strtok(forCheck, ";");
		if (strstr(forCheck, "Lcl Translation")) { vConnection.emplace_back(left, right, C::t); continue; }

		forCheck = strdup(pch);
		strtok(forCheck, ";");
		if (strstr(forCheck, "Lcl Rotation")) { vConnection.emplace_back(left, right, C::r); continue; }

		forCheck = strdup(pch);
		strtok(forCheck, ";");
		if (strstr(forCheck, "Lcl Scaling")) { vConnection.emplace_back(left, right, C::s); continue; }

		vConnection.emplace_back(left, right);
	}
	//cout << "Total: " << vConnection.size() << "\n\n";
	//for (auto p = vConnection.begin(); p != vConnection.end(); ++p) cout << p->left << ", " << p->right << "\n";

}

void ExportModel(char * pFile)
{
	char * pch = nullptr;
	int nModel;

	pch = strstr(pFile, "ObjectType: \"Model\"");
	pch = strstr(pch, "Count: ");
	pch += strlen("Count: ");
	nModel = strtol(pch, &pch, 10);

	for (int i = 0; i < nModel; ++i) {
		__int64 ID;
		char *forCheck = nullptr;
		Model *temp = nullptr;

		temp = new Model();

		pch = strstr(pch, "Model: ");
		pch += strlen("Model: ");
		temp->ID = _strtoi64(pch, &pch, 10);
		forCheck = strdup(pch);
		strtok(forCheck, "}");
		if (strstr(forCheck, "Mesh"))		goto Mesh;
		else if (strstr(forCheck, "Root"))		goto Root;
		else if (strstr(forCheck, "LimbNode"))	goto LimbNode;
		else goto EndModel;
	Mesh:
		forCheck = strdup(pch);
		strtok(forCheck, "}");
		if (strstr(forCheck, "PreRotation")) {
			pch = strstr(pch, "PreRotation");
			pch = strstr(pch, ","); pch++;
			pch = strstr(pch, ","); pch++;
			pch = strstr(pch, ","); pch++;
			pch = strstr(pch, ","); pch++;
			temp->PreRotaion.x = strtof(pch, &pch); pch++;
			temp->PreRotaion.y = strtof(pch, &pch); pch++;
			temp->PreRotaion.z = strtof(pch, &pch); pch++;
		}
		forCheck = strdup(pch);
		strtok(forCheck, "}");
		if (strstr(forCheck, "Lcl Translation")) {
			pch = strstr(pch, "Lcl Translation");
			pch = strstr(pch, ","); pch++;
			pch = strstr(pch, ","); pch++;
			pch = strstr(pch, ","); pch++;
			pch = strstr(pch, ","); pch++;
			temp->LclTranslation.x = strtof(pch, &pch); pch++;
			temp->LclTranslation.y = strtof(pch, &pch); pch++;
			temp->LclTranslation.z = strtof(pch, &pch); pch++;
		}
		forCheck = strdup(pch);
		strtok(forCheck, "}");
		if (strstr(forCheck, "Lcl Rotation")) {
			pch = strstr(pch, "Lcl Rotation");
			pch = strstr(pch, ","); pch++;
			pch = strstr(pch, ","); pch++;
			pch = strstr(pch, ","); pch++;
			pch = strstr(pch, ","); pch++;
			temp->LclRotation.x = strtof(pch, &pch); pch++;
			temp->LclRotation.y = strtof(pch, &pch); pch++;
			temp->LclRotation.z = strtof(pch, &pch); pch++;
		}
		forCheck = strdup(pch);
		strtok(forCheck, "}");
		if (strstr(forCheck, "Lcl Scaling")) {
			pch = strstr(pch, "Lcl Scaling");
			pch = strstr(pch, ","); pch++;
			pch = strstr(pch, ","); pch++;
			pch = strstr(pch, ","); pch++;
			pch = strstr(pch, ","); pch++;
			temp->LclScale.x = strtof(pch, &pch); pch++;
			temp->LclScale.y = strtof(pch, &pch); pch++;
			temp->LclScale.z = strtof(pch, &pch); pch++;
		}

		if (temp) vMesh.push_back(temp);
		goto EndModel;
	Root:
		forCheck = strdup(pch);
		strtok(forCheck, "}");
		if (strstr(forCheck, "PreRotation")) {
			pch = strstr(pch, "PreRotation");
			pch = strstr(pch, ","); pch++;
			pch = strstr(pch, ","); pch++;
			pch = strstr(pch, ","); pch++;
			pch = strstr(pch, ","); pch++;
			temp->PreRotaion.x = strtof(pch, &pch); pch++;
			temp->PreRotaion.y = strtof(pch, &pch); pch++;
			temp->PreRotaion.z = strtof(pch, &pch); pch++;
		}
		forCheck = strdup(pch);
		strtok(forCheck, "}");
		if (strstr(forCheck, "Lcl Translation")) {
			pch = strstr(pch, "Lcl Translation");
			pch = strstr(pch, ","); pch++;
			pch = strstr(pch, ","); pch++;
			pch = strstr(pch, ","); pch++;
			pch = strstr(pch, ","); pch++;
			temp->LclTranslation.x = strtof(pch, &pch); pch++;
			temp->LclTranslation.y = strtof(pch, &pch); pch++;
			temp->LclTranslation.z = strtof(pch, &pch); pch++;
		}
		forCheck = strdup(pch);
		strtok(forCheck, "}");
		if (strstr(forCheck, "Lcl Rotation")) {
			pch = strstr(pch, "Lcl Rotation");
			pch = strstr(pch, ","); pch++;
			pch = strstr(pch, ","); pch++;
			pch = strstr(pch, ","); pch++;
			pch = strstr(pch, ","); pch++;
			temp->LclRotation.x = strtof(pch, &pch); pch++;
			temp->LclRotation.y = strtof(pch, &pch); pch++;
			temp->LclRotation.z = strtof(pch, &pch); pch++;
		}
		forCheck = strdup(pch);
		strtok(forCheck, "}");
		if (strstr(forCheck, "Lcl Scaling")) {
			pch = strstr(pch, "Lcl Scaling");
			pch = strstr(pch, ","); pch++;
			pch = strstr(pch, ","); pch++;
			pch = strstr(pch, ","); pch++;
			pch = strstr(pch, ","); pch++;
			temp->LclScale.x = strtof(pch, &pch); pch++;
			temp->LclScale.y = strtof(pch, &pch); pch++;
			temp->LclScale.z = strtof(pch, &pch); pch++;
		}
		goto EndModel;
	LimbNode:
		forCheck = strdup(pch);
		strtok(forCheck, "}");
		if (strstr(forCheck, "PreRotation")) {
			pch = strstr(pch, "PreRotation");
			pch = strstr(pch, ","); pch++;
			pch = strstr(pch, ","); pch++;
			pch = strstr(pch, ","); pch++;
			pch = strstr(pch, ","); pch++;
			temp->PreRotaion.x = strtof(pch, &pch); pch++;
			temp->PreRotaion.y = strtof(pch, &pch); pch++;
			temp->PreRotaion.z = strtof(pch, &pch); pch++;
		}
		forCheck = strdup(pch);
		strtok(forCheck, "}");
		if (strstr(forCheck, "Lcl Translation")) {
			pch = strstr(pch, "Lcl Translation");
			pch = strstr(pch, ","); pch++;
			pch = strstr(pch, ","); pch++;
			pch = strstr(pch, ","); pch++;
			pch = strstr(pch, ","); pch++;
			temp->LclTranslation.x = strtof(pch, &pch); pch++;
			temp->LclTranslation.y = strtof(pch, &pch); pch++;
			temp->LclTranslation.z = strtof(pch, &pch); pch++;
		}
		forCheck = strdup(pch);
		strtok(forCheck, "}");
		if (strstr(forCheck, "Lcl Rotation")) {
			pch = strstr(pch, "Lcl Rotation");
			pch = strstr(pch, ","); pch++;
			pch = strstr(pch, ","); pch++;
			pch = strstr(pch, ","); pch++;
			pch = strstr(pch, ","); pch++;
			temp->LclRotation.x = strtof(pch, &pch); pch++;
			temp->LclRotation.y = strtof(pch, &pch); pch++;
			temp->LclRotation.z = strtof(pch, &pch); pch++;
		}
		forCheck = strdup(pch);
		strtok(forCheck, "}");
		if (strstr(forCheck, "Lcl Scaling")) {
			pch = strstr(pch, "Lcl Scaling");
			pch = strstr(pch, ","); pch++;
			pch = strstr(pch, ","); pch++;
			pch = strstr(pch, ","); pch++;
			pch = strstr(pch, ","); pch++;
			temp->LclScale.x = strtof(pch, &pch); pch++;
			temp->LclScale.y = strtof(pch, &pch); pch++;
			temp->LclScale.z = strtof(pch, &pch); pch++;
		}

		if (temp) vLimbNode.push_back(temp);
		goto EndModel;
	EndModel:
		if (temp) vModel.push_back(temp);

	}

}

void ExportAnimCurveNode(char * pFile)
{
	char * pch = nullptr;
	int nAnimCurveNode;

	pch = strstr(pFile, "ObjectType: \"AnimationCurveNode\"");
	pch = strstr(pch, "Count: ");
	pch += strlen("Count: ");
	nAnimCurveNode = strtol(pch, &pch, 10);

	for (int i = 0; i < nAnimCurveNode; ++i) {
		__int64 ID;
		AnimationCurveNode *temp = nullptr;

		temp = new AnimationCurveNode();

		pch = strstr(pch, "AnimationCurveNode: ");
		pch += strlen("AnimationCurveNode: ");
		temp->ID = _strtoi64(pch, &pch, 10);

		pch = strstr(pch, "d|X");
		pch = strstr(pch, ","); pch++;
		pch = strstr(pch, ","); pch++;
		pch = strstr(pch, ","); pch++;
		pch = strstr(pch, ","); pch++;
		temp->animCurveValue.x = strtof(pch, &pch);

		pch = strstr(pch, "d|Y");
		pch = strstr(pch, ","); pch++;
		pch = strstr(pch, ","); pch++;
		pch = strstr(pch, ","); pch++;
		pch = strstr(pch, ","); pch++;
		temp->animCurveValue.y = strtof(pch, &pch);

		pch = strstr(pch, "d|Z");
		pch = strstr(pch, ","); pch++;
		pch = strstr(pch, ","); pch++;
		pch = strstr(pch, ","); pch++;
		pch = strstr(pch, ","); pch++;
		temp->animCurveValue.z = strtof(pch, &pch);

		vAnimCurveNode.push_back(temp);
	}
}

void ExportAnimCurve(char * pFile)
{
	char * pch = nullptr;
	int nAnimCurve;

	pch = strstr(pFile, "ObjectType: \"AnimationCurve\"");
	pch = strstr(pch, "Count: ");
	pch += strlen("Count: ");
	nAnimCurve = strtol(pch, &pch, 10);

	for (int i = 0; i < nAnimCurve; ++i) {
		__int64 ID;
		AnimationCurve *temp = nullptr;

		temp = new AnimationCurve();

		pch = strstr(pch, "AnimationCurve: ");
		pch += strlen("AnimationCurve: ");
		temp->ID = _strtoi64(pch, &pch, 10);

		pch = strstr(pch, "KeyTime: *");
		pch += strlen("KeyTime: *");
		temp->nKeyTime = strtol(pch, &pch, 10);

		temp->pKeyTime = new __int64[temp->nKeyTime];

		pch = strstr(pch, "a: ");
		pch += strlen("a: ");
		for (int j = 0; j < temp->nKeyTime; ++j) {
			temp->pKeyTime[j] = _strtoi64(pch, &pch, 10); pch++;
		}

		pch = strstr(pch, "KeyValueFloat: *");
		pch += strlen("KeyValueFloat: *");
		temp->nKeyValueFloat = strtol(pch, &pch, 10);

		temp->pKeyValueFloat = new float[temp->nKeyValueFloat];

		pch = strstr(pch, "a: ");
		pch += strlen("a: ");
		for (int j = 0; j < temp->nKeyValueFloat; ++j) {
			temp->pKeyTime[j] = strtof(pch, &pch); pch++;
		}

		vAnimCurve.push_back(temp);
	}
}

void ConnectingGeoToCluster()
{
	EXBone		*pTempBone = nullptr;
	EXGeometry	*pTempGeo = nullptr;


	// LimbNode가 Model이라 이미 모델 정보는 갖고 있을 것.
	// 해줘야 하는 것은 부모-자식 관계.
	// 어짜피 저장은 vector<Model*>에 들어간 순서대로 할 것.
	// 따라서 몇 번째가 부모인지 정하기만 하면 될 듯?
	// left가 LimbNode일 때, right도 LimbNode면 right를 부모로.
	// right의 distance를 활용하여 순번을 알아내고 해당 idx를 쓴다.
	// 나중에 실제로 사용할 때는 idx가 아니라 idx를 이용하여 찾아낸 포인터 주소를 쓰는 것이 좋을 듯

	// LimbNode 
	for (auto c = vConnection.begin(); c != vConnection.end(); ++c) {
		const __int64		left	= c->left;
		const __int64		right	= c->right;

		for (auto p1 = vLimbNode.begin(); p1 != vLimbNode.end(); ++p1) {
			if (right == (*p1)->ID) {

				for (auto p2 = vLimbNode.begin(); p2 != vLimbNode.end(); ++p2) {
					if (left == (*p2)->ID) {

						(*p2)->parent = (*p1);
						goto 본_부모_첫_루프;
					}
				}
			}
		}
	본_부모_첫_루프:;

	}


	// AnimCurveNode를 채우기 위한 부분
	for (auto c = vConnection.begin(); c != vConnection.end(); ++c) {
		const __int64		left		= c->left;
		const __int64		right		= c->right;

		for (auto pACN = vAnimCurveNode.begin(); pACN != vAnimCurveNode.end(); ++pACN) {
			// 만약 지금 보고 있는 ConnectionData가 이 ACN라면
			if (right == (*pACN)->ID) {

				// 이제는 left로 AC를 찾아야 할 차례
				for (auto pAC = vAnimCurve.begin(); pAC != vAnimCurve.end(); ++pAC) {
					if (left == (*pAC)->ID) {

						// 찾았으면 그 내용을 가지고 ACN에 넣어줘야 한다.
						// 빈 칸인지 확인하고 ACN에 넣어주기

						for (int i = 0; i < 3; ++i) 
							if (nullptr == (*pACN)->animCurve[i]) {
								(*pACN)->animCurve[i] = (*pAC);
								goto 애니메이션커브노드_첫_루프;
						}
					}
				}
			}
		}
	애니메이션커브노드_첫_루프:;
	}

	// LimbNode를 채우기 위한 부분
	for (auto c = vConnection.begin(); c != vConnection.end(); ++c) {
		const __int64		left	= c->left;
		const __int64		right	= c->right;
		const int			type	= c->type;

		for (auto pLN = vLimbNode.begin(); pLN != vLimbNode.end(); ++pLN) {
			// 만약 지금 보고 있는 ConnectionData가 이 LimbNode라면
			if (right == (*pLN)->ID) {

				// 이제는 left로 ACN를 찾아야 할 차례
				for (auto pACN = vAnimCurveNode.begin(); pACN != vAnimCurveNode.end(); ++pACN) {
					if (left == (*pACN)->ID) {

						// 찾았으면 그 내용을 가지고 LimbNode에 넣어줘야 한다.
						// 빈 칸인지 확인하고 LimbNode에 넣어주기

						(*pLN)->ACN[type] = (*pACN);
						goto 림노드_첫_루프;
					}
				}
			}
		}
	림노드_첫_루프:;
	}

	// Cluster를 채우기 위한 부분
	for (auto c = vConnection.begin(); c != vConnection.end(); ++c) {
		const __int64		left = c->left;
		const __int64		right = c->right;

		for (auto pC = vCluster.begin(); pC != vCluster.end(); ++pC) {
			if (right == (*pC)->ID) {

				for (auto pLN = vLimbNode.begin(); pLN != vLimbNode.end(); ++pLN) {
					if (left == (*pLN)->ID) {
						(*pC)->boneIdx = distance(vLimbNode.begin(), pLN);
						goto 클러스터_첫_루프;
					}
				}
			}
		}
	클러스터_첫_루프:;
	}

	// Deformer를 채우기 위한 부분
	for (auto c = vConnection.begin(); c != vConnection.end(); ++c) {
		const __int64		left = c->left;
		const __int64		right = c->right;

		for (auto pD = vDeformer.begin(); pD != vDeformer.end(); ++pD) {
			if (right == (*pD)->ID) {

				for (auto pC = vCluster.begin(); pC != vCluster.end(); ++pC) {
					if (left == (*pC)->ID) {
						(*pD)->cluster = (*pC);
						goto 디포머_첫_루프;
					}
				}
			}
		}
	디포머_첫_루프:;
	}


	// 일단 EXGeometry를 만들어줌
	for (auto pGeo = vGeometry.begin(); pGeo != vGeometry.end(); ++pGeo) {
		pTempGeo = new EXGeometry();
		pTempGeo->geoData		= (*pGeo);
		pTempGeo->pBoneIdx		= new XMINT4[pTempGeo->geoData->nPolygonVertexIndex];
		pTempGeo->EffectedCount = new UINT[pTempGeo->geoData->nPolygonVertexIndex];
		pTempGeo->pWeight		= new XMFLOAT4[pTempGeo->geoData->nPolygonVertexIndex];
		for (int i = 0; i < pTempGeo->geoData->nPolygonVertexIndex; ++i) pTempGeo->EffectedCount[i] = 0;
		vEXGeometry.push_back(pTempGeo);
	}

	// Geometry를 채우기 위한 부분
	for (auto c = vConnection.begin(); c != vConnection.end(); ++c) {
		const __int64		left = c->left;
		const __int64		right = c->right;

		for (auto pGeo = vEXGeometry.begin(); pGeo != vEXGeometry.end(); ++pGeo) {
			if (right == (*pGeo)->geoData->ID) {

				for (auto pD = vDeformer.begin(); pD != vDeformer.end(); ++pD) {
					if (left == (*pD)->ID) {
						UINT nEffect = (*pD)->cluster->nIndexes;
						for (int i = 0; i < nEffect; ++i) {
							switch ((*pGeo)->EffectedCount[(*pD)->cluster->pIndexes[i]])
							{
							case 0:
								(*pGeo)->pBoneIdx[(*pD)->cluster->pIndexes[i]].x = (*pD)->cluster->boneIdx;
								(*pGeo)->pWeight[(*pD)->cluster->pIndexes[i]].x = (*pD)->cluster->pWeights[i];
								break;
							case 1:
								(*pGeo)->pBoneIdx[(*pD)->cluster->pIndexes[i]].y = (*pD)->cluster->boneIdx;
								(*pGeo)->pWeight[(*pD)->cluster->pIndexes[i]].y = (*pD)->cluster->pWeights[i];
								break;
							case 2:
								(*pGeo)->pBoneIdx[(*pD)->cluster->pIndexes[i]].z = (*pD)->cluster->boneIdx;
								(*pGeo)->pWeight[(*pD)->cluster->pIndexes[i]].z = (*pD)->cluster->pWeights[i];
								break;
							case 3:
								(*pGeo)->pBoneIdx[(*pD)->cluster->pIndexes[i]].w = (*pD)->cluster->boneIdx;
								(*pGeo)->pWeight[(*pD)->cluster->pIndexes[i]].w = (*pD)->cluster->pWeights[i];
								break;
							default: break;
							}
							(*pGeo)->EffectedCount[(*pD)->cluster->pIndexes[i]]++;
						}
						goto EX지오_첫_루프;
					}
				}
			}
		}
	EX지오_첫_루프:;
	}

	// Mesh를 채우기 위한 부분
	for (auto c = vConnection.begin(); c != vConnection.end(); ++c) {
		const __int64		left = c->left;
		const __int64		right = c->right;

		// 여기선 반대로 geo가 mesh에 들어가는 꼴이므로 left로 geo를 찾아야 함.
		for (auto pGeo = vEXGeometry.begin(); pGeo != vEXGeometry.end(); ++pGeo) {
			if (left == (*pGeo)->geoData->ID) {
				for (auto pM = vMesh.begin(); pM != vMesh.end(); ++pM) {
					if (right == (*pM)->ID) {
						(*pGeo)->modelData = (*pM);
						goto 메쉬_첫_루프;
					}
				}
			}
		}
	메쉬_첫_루프:;
	}

	cout << "진짜 끝?\n";

}
