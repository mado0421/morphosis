#include "stdafx.h"
#include "Converter.h"


Converter::Converter()
{
}


Converter::~Converter()
{
}

void Converter::ReadFile(FBX_DATA format, const char * fileName)
{
	char filePath[256] = "resource/model/";
	__int64 lSize;
	size_t result;

	strcpy(m_fileName, fileName);
	strcat(filePath, m_fileName);
	strcat(filePath, ".txt");

	m_pFile = fopen(filePath, "r");
	assert(m_pFile && "파일을 찾지 못했습니다.");

	fseek(m_pFile, 0, SEEK_END);
	lSize = ftell(m_pFile);
	rewind(m_pFile);


	m_buffer = (char*)malloc(sizeof(char)*lSize);
	memset(m_buffer, 0, lSize);
	result = fread(m_buffer, 1, lSize, m_pFile);

	fclose(m_pFile);

	//FindAnimationCount();
	m_buffer = strtok(m_buffer, " ,\n\t");

	if (format == FBX_DATA::Mesh)
	{
		while (m_buffer != NULL)
		{
			FindMeshOnly();
			pass(&m_buffer, " ,\n\t");
		}
	}
	else if (format == FBX_DATA::FBX)
	{
		while (m_buffer != NULL)
		{
			Find();
			pass(&m_buffer, " ,\n\t");
		}
	}
	else if (format == FBX_DATA::Anim)
	{

	}

	MakeCNode();
	//	사용하지 않을 ScaleNode 제거
	ClearScaleNode();

	AssignBoneIndexToCNode();
	SetBindPose();
	UseDeformer();

	ConnectVertex();
	ConnectMesh();
	SetBoneHierarchy();


	if (format == FBX_DATA::Mesh)
	{
		WriteMeshOnly();
	}
	else if (format == FBX_DATA::FBX)
	{
		WriteFile();
	}
	else if(format == FBX_DATA::Anim)
	{

	}

	//Test();
}

void Converter::WriteFile()
{
	char filePath[256] = "ouput\\model\\";
	long lSize;
	size_t result;
	int num;

	strcat(filePath, m_fileName);
	strcat(filePath, ".dat");

	m_pFile = fopen(m_fileName, "wb");
	assert(m_pFile && "파일을 찾지 못했습니다.");

	num = m_vMaterial.size();
	fwrite(&num, sizeof(int), 1, m_pFile);
	for (int i = 0; i < num; ++i)
	{
		fwrite(&m_vMaterial[i], sizeof(Material), 1, m_pFile);
	}

	num = m_vTextureName.size();
	fwrite(&num, sizeof(int), 1, m_pFile);
	for (int i = 0; i < num; ++i)
	{
		fwrite(&m_vTextureName[i], sizeof(Name), 1, m_pFile);
	}

	//	Resource로 등록될 Vertex
	num = m_vVertex.size();
	fwrite(&num, sizeof(int), 1, m_pFile);
	for (int i = 0; i < num; ++i)
	{
		fwrite(&m_vVertex[i], sizeof(Vertex), 1, m_pFile);
	}

	//	Resource로 등록될 UV
	num = m_vUV.size();
	fwrite(&num, sizeof(int), 1, m_pFile);
	for (int i = 0; i < num; ++i)
	{
		fwrite(&m_vUV[i], sizeof(UV), 1, m_pFile);
	}

	//	IASet할 실제 버텍스, 버텍스 idx와 UV idx등을 갖고있음
	num = m_mesh.vertexIdx.size();
	IAVertex* iaVertex = new IAVertex[num];

	fwrite(&num, sizeof(int), 1, m_pFile);
	for (int i = 0; i < num; ++i)
	{
		iaVertex[i].vertexIdx = m_mesh.vertexIdx[i];
		iaVertex[i].uvIdx = m_mesh.uvIdx[i];
		iaVertex[i].normal = m_mesh.normal[i];
		if (m_mesh.biNormal.size() == 0)
			iaVertex[i].biNormal = float3(0, 0, 0);
		else
			iaVertex[i].biNormal = m_mesh.biNormal[i];

		if (m_mesh.tangent.size() == 0)
			iaVertex[i].tangent = float3(0, 0, 0);
		else
			iaVertex[i].tangent = m_mesh.tangent[i];
	}
	fwrite(iaVertex, sizeof(IAVertex)*num, 1, m_pFile);

	

	//	Bone 구조체를 쓴다.
	num = m_vBone.size();
	fwrite(&num, sizeof(int), 1, m_pFile);
	for (int i = 0; i < num; ++i)
	{
		fwrite(&m_vBone[i], sizeof(Bone), 1, m_pFile);
	}

	//	CurveNode를 쓴다.

	num = m_vNode.size();
	fwrite(&num, sizeof(int), 1, m_pFile);

	for (int i = 0; i < num; ++i)
	{
		//	연관되어 있는 BoneIndex
		fwrite(&m_vNode[i].bone_index, sizeof(int), 1, m_pFile);


		//	Translation
		int nt;
		nt = m_vNode[i].translation.size();
		fwrite(&nt, sizeof(int), 1, m_pFile);
		if (nt > 0){
			for (int j = 0; j < nt; ++j)
			{
				fwrite(&m_vNode[i].translation[j], sizeof(Keyframe), 1, m_pFile);
			}
		}

		//	Rotation
		int nr;
		nr = m_vNode[i].rotation.size();
		fwrite(&nr, sizeof(int), 1, m_pFile);
		if (nr > 0){
			for (int j = 0; j < nr; ++j)
			{
				fwrite(&m_vNode[i].rotation[j], sizeof(Keyframe), 1, m_pFile);
			}
		}

		//	Scale
		int ns;
		ns = m_vNode[i].scale.size();
		fwrite(&ns, sizeof(int), 1, m_pFile);
		if (ns > 0){
			for (int j = 0; j < ns; ++j)
			{
				fwrite(&m_vNode[i].scale[j], sizeof(Keyframe), 1, m_pFile);
			}
		}
	}
	fclose(m_pFile);
}

void Converter::Test()
{
	Bone p = m_vBone[0];
	Bone c = m_vBone[1];

	XMMATRIX pm = XMLoadFloat4x4(&p.offset);
	XMMATRIX cm = XMLoadFloat4x4(&c.offset);
	XMMATRIX rm;

	XMMATRIX res = cm;
	XMFLOAT4X4 t;
	XMFLOAT4X4 re;

	rm = XMMatrixInverse(nullptr, cm);
	if (!XMMatrixIsInfinite(rm))
	{
		rm = XMMatrixMultiply(rm, pm);
		XMStoreFloat4x4(&t, rm);

		res = XMMatrixMultiply(res, rm);
		XMStoreFloat4x4(&re, res);
	}
	else
	{
		printf("Infinite");
	}
	
}

void Converter::WriteMeshOnly()
{
	char filePath[256] = "ouput\\model\\";
	long lSize;
	size_t result;
	int num;

	strcat(filePath, m_fileName);
	strcat(filePath, ".dat");

	m_pFile = fopen(m_fileName, "wb");
	assert(m_pFile && "파일을 찾지 못했습니다.");

	num = m_vMaterial.size();
	fwrite(&num, sizeof(int), 1, m_pFile);
	for (int i = 0; i < num; ++i) 
	{
		fwrite(&m_vMaterial[i], sizeof(Material), 1, m_pFile);
	}

	num = m_vTextureName.size();
	fwrite(&num, sizeof(int), 1, m_pFile);
	for (int i = 0; i < num; ++i)
	{
		fwrite(&m_vTextureName[i], sizeof(Name), 1, m_pFile);
	}

	num = m_vVertex.size();
	fwrite(&num, sizeof(int), 1, m_pFile);
	for (int i = 0; i < num; ++i)
	{
		fwrite(&m_vVertex[i], sizeof(Vertex), 1, m_pFile);
	}

	num = m_vUV.size();
	fwrite(&num, sizeof(int), 1, m_pFile);
	for (int i = 0; i < num; ++i)
	{
		fwrite(&m_vUV[i], sizeof(UV), 1, m_pFile);
	}


	num = m_mesh.vertexIdx.size();
	IAVertex* iaVertex = new IAVertex[num];

	fwrite(&num, sizeof(int), 1, m_pFile);
	for (int i = 0; i < num; ++i)
	{
		iaVertex[i].vertexIdx = m_mesh.vertexIdx[i];
		iaVertex[i].uvIdx = m_mesh.uvIdx[i];
		iaVertex[i].normal = m_mesh.normal[i];
		if (m_mesh.biNormal.size() == 0)
			iaVertex[i].biNormal = float3(0, 0, 0);
		else
			iaVertex[i].biNormal = m_mesh.biNormal[i];

		if (m_mesh.tangent.size() == 0)
			iaVertex[i].tangent = float3(0, 0, 0);
		else
			iaVertex[i].tangent = m_mesh.tangent[i];
	}
	fwrite(iaVertex, sizeof(IAVertex)*num, 1, m_pFile);

	fclose(m_pFile);
}

void Converter::FindAnimationCount()
{
	/*
	지금 보니까 AnimationStack은 애니메이션이 있든 없든 카운트가 1이 있더라
	Object properties 나오기 전까지 AnimationCurve
	passOver(&m_buffer, "\"AnimationStack\"");
	m_nAnimationStack = getInt(&m_buffer);
	*/
}

void Converter::FindMeshOnly()
{
	FindMaterials();
	FindTextures();
	FindPosition();
	FindVertexIdx();
	FindUV();
	FindUVIdx();
	FindNormal();
	FindBiNormal();
	FindTangent();
}

void Converter::Find()
{
	FindMaterials();
	FindTextures();
	FindPosition();
	FindVertexIdx();
	FindUV();
	FindUVIdx();
	FindBiNormal();
	FindNormal();
	FindTangent();
	FindBone();

	FindCurve();
	FindCurveNode();
	FindDeformer();
	FindOffsetMatrix();
	Bone_SubDeformer_Hierarchy();
	AnimCurve_AnimationCurveNode();
	CurveNode_Bone();
}

void Converter::FindMaterials()
{
	if (isSame(m_buffer, "Material:"))
	{
		Material tempMaterial;
		pass(&m_buffer, " ,");
		tempMaterial.materialIdx = get64int(&m_buffer);

		passTo(&m_buffer, "\"EmissiveFactor\"");
		passOver(&m_buffer, "\"A\"");
		tempMaterial.EmissiveFactor = getFloat(&m_buffer);

		passTo(&m_buffer, "\"AmbientColor\"");
		passOver(&m_buffer, "\"A\"");
		tempMaterial.AmbientColor = getFloat3(&m_buffer);

		passTo(&m_buffer, "\"DiffuseColor\"");
		passOver(&m_buffer, "\"A\"");
		tempMaterial.DiffuseColor = getFloat3(&m_buffer);

		passTo(&m_buffer, "\"TransparentColor\"");
		passOver(&m_buffer, "\"A\"");
		tempMaterial.TransparentColor = getFloat3(&m_buffer);

		passTo(&m_buffer, "\"SpecularColor\"");
		passOver(&m_buffer, "\"A\"");
		tempMaterial.SpecularColor = getFloat3(&m_buffer);

		passTo(&m_buffer, "\"SpecularFactor\"");
		passOver(&m_buffer, "\"A\"");
		tempMaterial.SpecularFactor = getFloat(&m_buffer);

		passTo(&m_buffer, "\"ShininessExponent\"");
		passOver(&m_buffer, "\"A\"");
		tempMaterial.ShininessExponent = getFloat(&m_buffer);

		passTo(&m_buffer, "\"Emissive\"");
		passOver(&m_buffer, "\"\"");
		tempMaterial.Emissive = getFloat3(&m_buffer);

		passTo(&m_buffer, "\"Ambient\"");
		passOver(&m_buffer, "\"\"");
		tempMaterial.Ambient = getFloat3(&m_buffer);

		passTo(&m_buffer, "\"Diffuse\"");
		passOver(&m_buffer, "\"\"");
		tempMaterial.Diffuse = getFloat3(&m_buffer);

		passTo(&m_buffer, "\"Specular\"");
		passOver(&m_buffer, "\"\"");
		tempMaterial.Specular = getFloat3(&m_buffer);

		passTo(&m_buffer, "\"Shininess\"");
		passOver(&m_buffer, "\"\"");
		tempMaterial.Shininess = getDouble(&m_buffer);

		passTo(&m_buffer, "\"Opacity\"");
		passOver(&m_buffer, "\"\"");
		tempMaterial.Opacity = getDouble(&m_buffer);

		passTo(&m_buffer, "\"Reflectivity\"");
		passOver(&m_buffer, "\"\"");
		tempMaterial.Reflectivity = getDouble(&m_buffer);

		m_vMaterial.push_back(tempMaterial);
	}
}

void Converter::FindCurve()
{
	/*시간을 갯수만큼 읽고 float 값을 갯수만큼 읽음*/
	if (isSame(m_buffer, "AnimationCurve:"))
	{
		Curve tempCurve;
		int numOfKey;

		pass(&m_buffer, " ,");
		tempCurve.curveIdx = get64int(&m_buffer);
		//	KeyTime: 띄어쓰기 뒤에 붙이지 말것...
		passOver(&m_buffer, "KeyTime:");
		numOfKey = getInt(&m_buffer);
		//	a: 띄어쓰기 뒤에 붙이지 말것....
		passOver(&m_buffer, "a:");

		tempCurve.key.resize(numOfKey);

		for (int i = 0; i < numOfKey; ++i)
		{
			__int64 i64 = get64int(&m_buffer) * 0.01 / 30790772;
			float timePos = i64 / 30.0f;

			tempCurve.key[i].timePos = timePos;
			pass(&m_buffer, " ,\n\t");
		}

		passOver(&m_buffer, "KeyValueFloat:");
		passOver(&m_buffer, "a:");

		for (int i = 0; i < numOfKey; ++i)
		{
			tempCurve.key[i].value = getFloat(&m_buffer);
			pass(&m_buffer, " ,\n\t");
		}

		m_vCurve.push_back(tempCurve);
	}
}

void Converter::FindCurveNode()
{
	if (isSame(m_buffer, "AnimationCurveNode:"))
	{
		CurveNode tempNode;

		pass(&m_buffer, " ,");
		tempNode.nodeIdx = get64int(&m_buffer);
		m_vNode.push_back(tempNode);
	}
}

void Converter::FindDeformer()
{
	if (isSame(m_buffer, "Deformer:"))
	{
		pass(&m_buffer, " ,");
		__int64 deformerIdx = get64int(&m_buffer);
		pass(&m_buffer, " ,");
		if (isSame(m_buffer, "\"SubDeformer::\""))
		{
			Deformer tempDeformer;
			int numOfAffectVertices;
			std::vector<int> affectedVertexIdx;
			std::vector<float> weight;

			tempDeformer.deformerIdx = deformerIdx;

			passOver(&m_buffer, "Indexes:");
			numOfAffectVertices = getInt(&m_buffer);

			affectedVertexIdx.resize(numOfAffectVertices);
			weight.resize(numOfAffectVertices);
			tempDeformer.weight.resize(numOfAffectVertices);

			passOver(&m_buffer, "a:");
			
			for (int i = 0; i < numOfAffectVertices; ++i)
			{
				affectedVertexIdx[i] = getInt(&m_buffer);
				pass(&m_buffer, " ,");
			}

			//	이렇게 하지 않으면 에러남
			passOver(&m_buffer, "\n\t\tWeights:");
			passOver(&m_buffer, "a:");

			for (int i = 0; i < numOfAffectVertices; ++i)
			{
				weight[i] = getFloat(&m_buffer);
				pass(&m_buffer, " ,");
			}

			for (int i = 0; i < numOfAffectVertices; ++i)
			{
				tempDeformer.weight[i].idx = affectedVertexIdx[i];
				tempDeformer.weight[i].weight = weight[i];
			}

			m_vDeformer.push_back(tempDeformer);
		}
	}
}

void Converter::FindTextures()
{
	if (isSame(m_buffer, "Texture:"))
	{
		passOver(&m_buffer, "FileName:");
		char * pTexturePath;
		char * pTextureName = NULL;
		pTexturePath = new char[_MAX_PATH];
		memcpy(pTexturePath, m_buffer, _MAX_PATH);

		while (*pTexturePath)
		{
			if (*pTexturePath == '\\' && (pTexturePath + 1) != NULL)
			{
				pTextureName = pTexturePath + 1;
			}
			else
			{
			}

			pTexturePath++; //mv pointer

		}
		Name name;
		strcpy(name.string, pTextureName);
		m_vTextureName.push_back(name);
		m_buffer = strtok(NULL, " ,\n\t");
		m_buffer = strtok(NULL, " ,\n\t");
//		pass(&m_buffer, " \t\n");
	}
}

void Converter::FindPosition()
{
	if (isSame(m_buffer, "Vertices:"))
	{
		int numOfVertex;
		pass(&m_buffer, " ,*");
		numOfVertex = getInt(&m_buffer);
		passOver(&m_buffer, "a:");

		m_vPos.resize(numOfVertex/3);

		for (auto i = 0; i < numOfVertex/3; ++i)
		{
			float3 tempFloat3;
			tempFloat3 = getFloat3(&m_buffer);
			m_vPos[i] = tempFloat3;
		}
		printf("TEST\n");
	}
}

void Converter::FindVertexIdx()
{
	if (isSame(m_buffer, "PolygonVertexIndex:"))
	{
		int numOfIdx;
		pass(&m_buffer, " ,*");
		numOfIdx = getInt(&m_buffer);
		passOver(&m_buffer, "a:");

		m_vVertexIdx.resize(numOfIdx);

		for (auto i = 0; i < numOfIdx; ++i)
		{
			m_vVertexIdx[i] = getInt(&m_buffer);
			pass(&m_buffer, " ,\n\t");
		}
	}
}

void Converter::FindUV()
{
	if (isSame(m_buffer, "UV:"))
	{
		int numOfUV;
		pass(&m_buffer, " ,*");
		numOfUV = getInt(&m_buffer);
		passOver(&m_buffer, "a:");

		m_vUV.resize(numOfUV/2);

		for (auto i = 0; i < numOfUV/2; ++i)
		{
			UV tempUV;
			tempUV.u = getFloat(&m_buffer);
			pass(&m_buffer, " ,\n\t");
			tempUV.v = getFloat(&m_buffer);
			pass(&m_buffer, " ,\n\t");
			m_vUV[i] = tempUV;
		}
	}
}

void Converter::FindUVIdx()
{
	if (isSame(m_buffer, "UVIndex:"))
	{
		int numOfIdx;
		pass(&m_buffer, " ,*");
		numOfIdx = getInt(&m_buffer);
		passOver(&m_buffer, "a:");

		m_vUVIdx.resize(numOfIdx);

		for (auto i = 0; i < numOfIdx; ++i)
		{
			m_vUVIdx[i] = getInt(&m_buffer);
			pass(&m_buffer, " ,\n\t");
		}
	}
}

void Converter::FindNormal()
{
	if (isSame(m_buffer, "Normals:"))
	{
		int numOfElements;
		pass(&m_buffer, " ,*");
		numOfElements = getInt(&m_buffer);
		passOver(&m_buffer, "a:");

		m_vNormal.resize(numOfElements/3);

		for (auto i = 0; i < numOfElements/3; ++i)
		{
			float3 tempFloat3;
			tempFloat3 = getFloat3(&m_buffer);
			m_vNormal[i] = tempFloat3;
		}
	}
}

void Converter::FindBiNormal()
{
	if (isSame(m_buffer, "Binormals:"))
	{
		int numOfElements;
		pass(&m_buffer, " ,*");
		numOfElements = getInt(&m_buffer);
		passOver(&m_buffer, "a:");

		m_vBiNormal.resize(numOfElements/3);

		for (auto i = 0; i < numOfElements / 3; ++i)
		{
			float3 tempFloat3;
			tempFloat3 = getFloat3(&m_buffer);
			m_vBiNormal[i] = tempFloat3;
		}
	}
}

void Converter::FindTangent()
{
	if (isSame(m_buffer, "Tangents:"))
	{
		int numOfElements;
		pass(&m_buffer, " ,*");
		numOfElements = getInt(&m_buffer);
		passOver(&m_buffer, "a:");

		m_vTangent.resize(numOfElements/3);

		for (auto i = 0; i < numOfElements / 3; ++i)
		{
			float3 tempFloat3;
			tempFloat3 = getFloat3(&m_buffer);
			m_vTangent[i] = tempFloat3;
		}
	}
}

void Converter::FindOffsetMatrix()
{
	if (isSame(m_buffer, "PoseNode:"))
	{
		PoseNode offset;
		passOver(&m_buffer, "Node:");
		offset.poseIdx = get64int(&m_buffer);
		passOver(&m_buffer, "a:");
		for (int i = 0; i < 16; ++i)
		{
			offset.mat.m[i / 4][i % 4] = getFloat(&m_buffer);
			pass(&m_buffer, " ,\n\t");
		}
		m_vPoseNode.push_back(offset);
	}
}

void Converter::FindConnectionPoseNodeToBone()
{

}



void Converter::ConnectVertex()
{
	m_vVertex.resize(m_vPos.size());

	for (auto i = 0; i < m_vPos.size(); ++i)
	{
		m_vVertex[i].pos = m_vPos[i];
	}

	/*
		Deformer 구조: 본 인덱스, weight(영향을 줄 버텍스, 해당 버텍스에 줄 가중치)
		Vertex 구조: 영향받을 본 인덱스 4개, 가중치 4개, 위치

		따라서 디포머를 순회하면서 버텍스[영향을 줄 인덱스].가중치에 가중치를 주고
		그 버텍스[영향을 줄 인덱스].본 인덱스에 현재 디포머의 본 인덱스를 주면 됨

		단, 해당 버텍스에 정보를 넣어줄 땐 별도의 받는 함수가 있어야 함.
		따라서 inputWeight() 함수를 만들 것.
	
	*/


	for (auto i = 0; i < m_vDeformer.size(); ++i)
	{
		for (auto j = 0; j < m_vDeformer[i].weight.size(); ++j)
		{
			/*
			이게 무슨 소린가 싶겠지만
			버텍스[현재 디포머의 가중치 정보에 있는 '영향을 줄 버텍스 인덱스'].가중치 정보를 갱신하는 내용이다..
			깔끔하지 못한 코드 죄악 그 자체
			*/
			m_vVertex[m_vDeformer[i].weight[j].idx].inputWeight(m_vDeformer[i].boneIdx, m_vDeformer[i].weight[j].weight);
		}
	}
}

void Converter::ConnectUV()
{
}

void Converter::ConnectMesh()
{
	for (auto i = 0; i < m_vVertexIdx.size(); ++i)
	{
		if (i % 3 == 2) m_mesh.vertexIdx.push_back(m_vVertexIdx[i] * -1 - 1);
		else m_mesh.vertexIdx.push_back(m_vVertexIdx[i]);
	}

	for (auto i = 0; i < m_vUVIdx.size(); ++i)
	{
		m_mesh.uvIdx.push_back(m_vUVIdx[i]);
	}

	for (auto i = 0; i < m_vNormal.size(); ++i)
	{
		m_mesh.normal.push_back(m_vNormal[i]);
	}

	for (auto i = 0; i < m_vBiNormal.size(); ++i)
	{
		m_mesh.biNormal.push_back(m_vBiNormal[i]);
	}

	for (auto i = 0; i < m_vTangent.size(); ++i)
	{
		m_mesh.tangent.push_back(m_vTangent[i]);
	}
}

void Converter::ConnectAnimation()
{
}

void Converter::ConnectBone()
{
}

void Converter::Connect()
{
	ConnectVertex();
	ConnectUV();
	ConnectMesh();
	ConnectAnimation();
	ConnectBone();
}

void Converter::AnimCurve_AnimationCurveNode()
{
	if (isSame(m_buffer, ";AnimCurve::"))
	{
		__int64 curveIdx;
		__int64 curveNodeIdx;
		pass(&m_buffer, " ,\n\t*");
		if (isIn(m_buffer, "AnimCurveNode::T"))
		{
			passOver(&m_buffer, "\"OP\"");
			curveIdx = get64int(&m_buffer);
			pass(&m_buffer, " ,\n\t*");
			curveNodeIdx = get64int(&m_buffer);
			pass(&m_buffer, " ,\n\t*");

			if (strcmp(m_buffer, "\"d|X\"") == 0)
			{
				m_vCurve_CNode_T[0].push_back(std::make_pair(curveIdx, curveNodeIdx));
			}
			else if (strcmp(m_buffer, "\"d|Y\"") == 0)
			{
				m_vCurve_CNode_T[1].push_back(std::make_pair(curveIdx, curveNodeIdx));
			}
			else if (strcmp(m_buffer, "\"d|Z\"") == 0)
			{
				m_vCurve_CNode_T[2].push_back(std::make_pair(curveIdx, curveNodeIdx));
			}
		}
		else if (isIn(m_buffer, "AnimCurveNode::R"))
		{
			passOver(&m_buffer, "\"OP\"");
			curveIdx = get64int(&m_buffer);
			pass(&m_buffer, " ,\n\t*");
			curveNodeIdx = get64int(&m_buffer);
			pass(&m_buffer, " ,\n\t*");

			if (strcmp(m_buffer, "\"d|X\"") == 0)
			{
				m_vCurve_CNode_R[0].push_back(std::make_pair(curveIdx, curveNodeIdx));
			}
			else if (strcmp(m_buffer, "\"d|Y\"") == 0)
			{
				m_vCurve_CNode_R[1].push_back(std::make_pair(curveIdx, curveNodeIdx));
			}
			else if (strcmp(m_buffer, "\"d|Z\"") == 0)
			{
				m_vCurve_CNode_R[2].push_back(std::make_pair(curveIdx, curveNodeIdx));
			}
		}
	}
}
void Converter::CurveNode_Bone()
{
	__int64 curveIdx;
	__int64 boneIdx;
	if (isIn(m_buffer, "AnimCurveNode::T"))
	{
		pass(&m_buffer, " ,\n\t*");
		if (isIn(m_buffer, "Model::"))
		{
			passOver(&m_buffer, "\"OP\"");
			curveIdx = get64int(&m_buffer);
			pass(&m_buffer, " ,\n\t*");
			boneIdx = get64int(&m_buffer);
			m_vCNode_Bone_T.push_back(std::make_pair(curveIdx, boneIdx));
		}
	}
	else if(isIn(m_buffer, "AnimCurveNode::R"))
	{
		pass(&m_buffer, " ,\n\t*");
		if (isIn(m_buffer, "Model::"))
		{
			passOver(&m_buffer, "\"OP\"");
			curveIdx = get64int(&m_buffer);
			pass(&m_buffer, " ,\n\t*");
			boneIdx = get64int(&m_buffer);
			m_vCNode_Bone_R.push_back(std::make_pair(curveIdx, boneIdx));
		}
	}
}
void Converter::Bone_SubDeformer_Hierarchy()
{
	__int64 boneIdx;
	if (isIn(m_buffer, ";Model::"))
	{
		passToIn(&m_buffer, "Model::");
		if (isIn(m_buffer, "SubDeformer::"))
		{
			__int64 SubDeformerIdx;
			passOver(&m_buffer, "\"OO\"");
			boneIdx = get64int(&m_buffer);
			pass(&m_buffer, " ,\n\t*");
			SubDeformerIdx = get64int(&m_buffer);

			auto p = find_if(m_vDeformer.begin(), m_vDeformer.end(), 
				[&SubDeformerIdx](const auto& t) {
				return (t.deformerIdx == SubDeformerIdx); }
			);
			if (p != m_vDeformer.end()){
				auto bi = find(m_vBone.begin(), m_vBone.end(), boneIdx);
				if (bi != m_vBone.end())
				{
					p->boneIdx = bi - m_vBone.begin();
				}
			}

		}
		else if (isIn(m_buffer, "Model::"))
		{
			__int64 parentBoneIdx;
			passOver(&m_buffer, "\"OO\"");
			boneIdx = get64int(&m_buffer);
			pass(&m_buffer, " ,\n\t*");
			parentBoneIdx = get64int(&m_buffer);

			m_vPaBone_chBone.push_back(std::make_pair(parentBoneIdx, boneIdx));
		}
	}
}
void Converter::FindBone()
{
	__int64 boneIdx;

	if (isSame(m_buffer, "Model:"))
	{
		pass(&m_buffer, " ,\n\t*");
		boneIdx = get64int(&m_buffer);
		Bone tmpBone;
		tmpBone.boneIdx = boneIdx;
		m_vBone.push_back(tmpBone);
		printf("야호 본이다\n");
	}
}
void Converter::SetBindPose()
{
	for (const auto& p : m_vPoseNode)
	{
		auto bi = find(m_vBone.begin(), m_vBone.end(), p.poseIdx);

		if (bi != m_vBone.end())
		{
			bi->offset = p.mat;
		}
		else
		{
			printf("Bone이랑 연결되어 있지 않은 PoseNode.\n1개여야 정상.\n");
		}
	}
}
void Converter::MakeCNode()
{
	for (auto& node : m_vNode)
	{
		//	Translation
		for (auto p = m_vCurve_CNode_T[0].begin(); p != m_vCurve_CNode_T[0].end(); ++p)
		{
			if (p->second == node.nodeIdx)
			{
				//	m_vCurve_CNode_T 에서 찾은 index를 기록
				int idx;
				idx = p - m_vCurve_CNode_T[0].begin();

				//	m_vCurve_CNode_T에 각각 기록된 x,y,z에 해당하는 Curve의 idx를 기록
				__int64 curve_x = (m_vCurve_CNode_T[0].begin() + idx)->first;
				__int64 curve_y = (m_vCurve_CNode_T[1].begin() + idx)->first;
				__int64 curve_z = (m_vCurve_CNode_T[2].begin() + idx)->first;
				
				//	Curve idx를 이용해 m_vCurve에서 어디에 있는지 iterator(pointer)를 찾아둔다.
				auto i_x = find(m_vCurve.begin(), m_vCurve.end(), curve_x);
				auto i_y = find(m_vCurve.begin(), m_vCurve.end(), curve_y);
				auto i_z = find(m_vCurve.begin(), m_vCurve.end(), curve_z);
				//	3개를 찾아 뒀지만 실제로는 x만 찾아도 될 것 같음
				//	x,y,z가 일반적으로 벡터에 순서대로 들어오고 있기 때문에
				//	x다음 원소가 y일 경우가 높지만 만약을 위해 전부 찾는다.

				//	찾지 못한 경우 breadk;
				if (i_x == m_vCurve.end())break;
				if (i_y == m_vCurve.end())break;
				if (i_z == m_vCurve.end())break;

				Keyframe tmpKey;

				{
					//	x,y,z는 전부 한 쌍으로 묶여서 기록되는 것을 전제로 만든 코드
					//	만약 x만 변환할때 y와 z값은 생성되지 않는다면 전부 수정해야함

					auto p_x = i_x->key.begin();
					auto p_y = i_y->key.begin();
					auto p_z = i_z->key.begin();

					while (true)
					{
						if ((p_x == i_x->key.end()) ||
							(p_y == i_y->key.end()) ||
							(p_z == i_z->key.end()))
							break;

						tmpKey.timePos = p_x->timePos;
						tmpKey.value = float3(p_x->value, p_y->value, p_z->value);
						node.translation.push_back(tmpKey);

						p_x++; p_y++; p_z++;
					}
				}
			
				//tmpKey.value.float3=float3()

				//printf("Make Cnode : curve idx=%lld, node Idx=%lld, p.second=%lld\n", p->first, node.nodeIdx, p->second);
				break;
			}
		}

		//	Rotation
		for (auto p = m_vCurve_CNode_R[0].begin(); p != m_vCurve_CNode_R[0].end(); ++p)
		{
			if (p->second == node.nodeIdx)
			{
				int idx;
				idx = p - m_vCurve_CNode_R[0].begin();

				__int64 curve_x = (m_vCurve_CNode_R[0].begin() + idx)->first;
				__int64 curve_y = (m_vCurve_CNode_R[1].begin() + idx)->first;
				__int64 curve_z = (m_vCurve_CNode_R[2].begin() + idx)->first;

				auto i_x = find(m_vCurve.begin(), m_vCurve.end(), curve_x);
				auto i_y = find(m_vCurve.begin(), m_vCurve.end(), curve_y);
				auto i_z = find(m_vCurve.begin(), m_vCurve.end(), curve_z);

				if (i_x == m_vCurve.end())break;
				if (i_y == m_vCurve.end())break;
				if (i_z == m_vCurve.end())break;

				Keyframe tmpKey;
				{
					auto p_x = i_x->key.begin();
					auto p_y = i_y->key.begin();
					auto p_z = i_z->key.begin();

					while (true)
					{
						if ((p_x == i_x->key.end()) ||
							(p_y == i_y->key.end()) ||
							(p_z == i_z->key.end()))
							break;

						tmpKey.timePos = p_x->timePos;
						tmpKey.value = float3(p_x->value, p_y->value, p_z->value);
						node.rotation.push_back(tmpKey);

						p_x++; p_y++; p_z++;
					}
				}
				break;
			}
		}
	}
}

void Converter::ClearScaleNode()
{
	for (auto p = m_vNode.begin(); p != m_vNode.end();)
	{
		if (p->translation.size() == 0 && p->rotation.size() == 0)
		{
			p = m_vNode.erase(p);
		}
		else ++p;
	}
}

void Converter::AssignBoneIndexToCNode()
{
	for (auto a = m_vNode.begin(); a != m_vNode.end(); ++a)
	{
		if (a->bone_index == -1)
		{
			if (a->translation.size() != 0)
			{
				auto p = std::find_if(m_vCNode_Bone_T.begin(), m_vCNode_Bone_T.end(),
					[&a](const std::pair<__int64, __int64>& t) {return (t.first == a->nodeIdx); });

				if (p == m_vCNode_Bone_T.end())
				{
					continue;
				}
				auto bone_iter = find(m_vBone.begin(), m_vBone.end(), p->second);
				if (bone_iter != m_vBone.end())
				{
					int bone_index = bone_iter - m_vBone.begin();
					a->bone_index = bone_index;
				}
			}
			else if(a->rotation.size() != 0)
			{
				auto p = std::find_if(m_vCNode_Bone_R.begin(), m_vCNode_Bone_R.end(),
					[&a](const std::pair<__int64, __int64>& t) {return (t.first == a->nodeIdx); });

				if (p == m_vCNode_Bone_R.end())
				{
					continue;
				}

				auto bone_iter = find(m_vBone.begin(), m_vBone.end(), p->second);
				if (bone_iter != m_vBone.end())
				{
					int bone_index = bone_iter - m_vBone.begin();
					a->bone_index = bone_index;
				}
			}
			else
			{
				printf("Error_AssignBoneIndexToCNode()\n\
					어떤 변환도 갖지 않은 CNode가 존재함.\n");
			}
		}
	}

}
void Converter::UseDeformer()
{

}
void Converter::SetBoneHierarchy()
{
	for (const auto& p : m_vPaBone_chBone)
	{
		__int64 paBone = p.first;
		__int64 chBone = p.second;

		auto p = find(m_vBone.begin(), m_vBone.end(), paBone);
		auto c = find(m_vBone.begin(), m_vBone.end(), chBone);

		if (paBone == 0)
		{
			if (c != m_vBone.end())
			{
				c->parentIdx = -1;
			}
		}
		else
		{
			if (p != m_vBone.end() && c != m_vBone.end())
			{
				c->parentIdx = p - m_vBone.begin();
			}
		}
	}
}
/*

지금 manager에게 줘야 할 것이 무엇인가?
Bone
그 속에 행렬? 들이 있어야 하겠지 변환에 관한 정보
Scale은 제외하고 Translation이랑 Rotation에 관한 정보들이 있어야해
그 정보들은 Time을 기준으로 오름차순으로 들어가 있어야하고
각 Timepos와 matrix 쌍으로 리스트를 갖고 있는 것이지
그리고 Bind 포즈를 갖고 있을 꺼야

CPU에서 Animation을 컨트롤할 클래스를 만들고
State에 따라 어떤 행렬을 등록할 것인지 확인 해야지
time을 갖고
현재 애니메이션의 Time을 갖고
start타임과 end 타임의 정규화중 어느 위치에 있는지 찾고
그 위치의 앞과 뒤의 행렬들을 등록한다.

쉐이더 코드상에서 prev와 next의 행렬과 정규화된 time으로
둘을 lerp해서 현재 시간에 필요한 애니메이션 행렬을 생성
해당 본에 연관된 Vertex는 가중치량 만큼 변환 행렬을 사용할 것이고

mul(mul(mul(input.pos,Bone.Trans),world),camera)


Converter에서 해야할 일

계층구조 Bone 넘겨주기
Bone마다 



*/
