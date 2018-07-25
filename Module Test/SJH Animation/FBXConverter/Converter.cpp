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
	assert(m_pFile && "������ ã�� ���߽��ϴ�.");

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
	//	������� ���� ScaleNode ����
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
	assert(m_pFile && "������ ã�� ���߽��ϴ�.");

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

	//	Resource�� ��ϵ� Vertex
	num = m_vVertex.size();
	fwrite(&num, sizeof(int), 1, m_pFile);
	for (int i = 0; i < num; ++i)
	{
		fwrite(&m_vVertex[i], sizeof(Vertex), 1, m_pFile);
	}

	//	Resource�� ��ϵ� UV
	num = m_vUV.size();
	fwrite(&num, sizeof(int), 1, m_pFile);
	for (int i = 0; i < num; ++i)
	{
		fwrite(&m_vUV[i], sizeof(UV), 1, m_pFile);
	}

	//	IASet�� ���� ���ؽ�, ���ؽ� idx�� UV idx���� ��������
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

	

	//	Bone ����ü�� ����.
	num = m_vBone.size();
	fwrite(&num, sizeof(int), 1, m_pFile);
	for (int i = 0; i < num; ++i)
	{
		fwrite(&m_vBone[i], sizeof(Bone), 1, m_pFile);
	}

	//	CurveNode�� ����.

	num = m_vNode.size();
	fwrite(&num, sizeof(int), 1, m_pFile);

	for (int i = 0; i < num; ++i)
	{
		//	�����Ǿ� �ִ� BoneIndex
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
	assert(m_pFile && "������ ã�� ���߽��ϴ�.");

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
	���� ���ϱ� AnimationStack�� �ִϸ��̼��� �ֵ� ���� ī��Ʈ�� 1�� �ִ���
	Object properties ������ ������ AnimationCurve
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
	/*�ð��� ������ŭ �а� float ���� ������ŭ ����*/
	if (isSame(m_buffer, "AnimationCurve:"))
	{
		Curve tempCurve;
		int numOfKey;

		pass(&m_buffer, " ,");
		tempCurve.curveIdx = get64int(&m_buffer);
		//	KeyTime: ���� �ڿ� ������ ����...
		passOver(&m_buffer, "KeyTime:");
		numOfKey = getInt(&m_buffer);
		//	a: ���� �ڿ� ������ ����....
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

			//	�̷��� ���� ������ ������
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
		Deformer ����: �� �ε���, weight(������ �� ���ؽ�, �ش� ���ؽ��� �� ����ġ)
		Vertex ����: ������� �� �ε��� 4��, ����ġ 4��, ��ġ

		���� �����Ӹ� ��ȸ�ϸ鼭 ���ؽ�[������ �� �ε���].����ġ�� ����ġ�� �ְ�
		�� ���ؽ�[������ �� �ε���].�� �ε����� ���� �������� �� �ε����� �ָ� ��

		��, �ش� ���ؽ��� ������ �־��� �� ������ �޴� �Լ��� �־�� ��.
		���� inputWeight() �Լ��� ���� ��.
	
	*/


	for (auto i = 0; i < m_vDeformer.size(); ++i)
	{
		for (auto j = 0; j < m_vDeformer[i].weight.size(); ++j)
		{
			/*
			�̰� ���� �Ҹ��� �Ͱ�����
			���ؽ�[���� �������� ����ġ ������ �ִ� '������ �� ���ؽ� �ε���'].����ġ ������ �����ϴ� �����̴�..
			������� ���� �ڵ� �˾� �� ��ü
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
		printf("��ȣ ���̴�\n");
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
			printf("Bone�̶� ����Ǿ� ���� ���� PoseNode.\n1������ ����.\n");
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
				//	m_vCurve_CNode_T ���� ã�� index�� ���
				int idx;
				idx = p - m_vCurve_CNode_T[0].begin();

				//	m_vCurve_CNode_T�� ���� ��ϵ� x,y,z�� �ش��ϴ� Curve�� idx�� ���
				__int64 curve_x = (m_vCurve_CNode_T[0].begin() + idx)->first;
				__int64 curve_y = (m_vCurve_CNode_T[1].begin() + idx)->first;
				__int64 curve_z = (m_vCurve_CNode_T[2].begin() + idx)->first;
				
				//	Curve idx�� �̿��� m_vCurve���� ��� �ִ��� iterator(pointer)�� ã�Ƶд�.
				auto i_x = find(m_vCurve.begin(), m_vCurve.end(), curve_x);
				auto i_y = find(m_vCurve.begin(), m_vCurve.end(), curve_y);
				auto i_z = find(m_vCurve.begin(), m_vCurve.end(), curve_z);
				//	3���� ã�� ������ �����δ� x�� ã�Ƶ� �� �� ����
				//	x,y,z�� �Ϲ������� ���Ϳ� ������� ������ �ֱ� ������
				//	x���� ���Ұ� y�� ��찡 ������ ������ ���� ���� ã�´�.

				//	ã�� ���� ��� breadk;
				if (i_x == m_vCurve.end())break;
				if (i_y == m_vCurve.end())break;
				if (i_z == m_vCurve.end())break;

				Keyframe tmpKey;

				{
					//	x,y,z�� ���� �� ������ ������ ��ϵǴ� ���� ������ ���� �ڵ�
					//	���� x�� ��ȯ�Ҷ� y�� z���� �������� �ʴ´ٸ� ���� �����ؾ���

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
					� ��ȯ�� ���� ���� CNode�� ������.\n");
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

���� manager���� ��� �� ���� �����ΰ�?
Bone
�� �ӿ� ���? ���� �־�� �ϰ��� ��ȯ�� ���� ����
Scale�� �����ϰ� Translation�̶� Rotation�� ���� �������� �־����
�� �������� Time�� �������� ������������ �� �־���ϰ�
�� Timepos�� matrix ������ ����Ʈ�� ���� �ִ� ������
�׸��� Bind ��� ���� ���� ����

CPU���� Animation�� ��Ʈ���� Ŭ������ �����
State�� ���� � ����� ����� ������ Ȯ�� �ؾ���
time�� ����
���� �ִϸ��̼��� Time�� ����
startŸ�Ӱ� end Ÿ���� ����ȭ�� ��� ��ġ�� �ִ��� ã��
�� ��ġ�� �հ� ���� ��ĵ��� ����Ѵ�.

���̴� �ڵ�󿡼� prev�� next�� ��İ� ����ȭ�� time����
���� lerp�ؼ� ���� �ð��� �ʿ��� �ִϸ��̼� ����� ����
�ش� ���� ������ Vertex�� ����ġ�� ��ŭ ��ȯ ����� ����� ���̰�

mul(mul(mul(input.pos,Bone.Trans),world),camera)


Converter���� �ؾ��� ��

�������� Bone �Ѱ��ֱ�
Bone���� 



*/
