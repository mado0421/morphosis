#include <DirectXMath.h>
#include <d3d12.h>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace DirectX;

void Print(const XMFLOAT4X4& m) {
	std::cout << std::fixed << std::setprecision(5) <<
		std::setw(9) << m._11 << " " << std::setw(9) << m._12 << " " << std::setw(9) << m._13 << " " << std::setw(9) << m._14 << "\n" <<
		std::setw(9) << m._21 << " " << std::setw(9) << m._22 << " " << std::setw(9) << m._23 << " " << std::setw(9) << m._24 << "\n" <<
		std::setw(9) << m._31 << " " << std::setw(9) << m._32 << " " << std::setw(9) << m._33 << " " << std::setw(9) << m._34 << "\n" <<
		std::setw(9) << m._41 << " " << std::setw(9) << m._42 << " " << std::setw(9) << m._43 << " " << std::setw(9) << m._44 << "\n\n";
}

struct AnimBone {
	std::string	m_Name;
	XMFLOAT4X4	m_GlobalTransform;
	XMFLOAT4X4* m_pToRootTransforms;
};

struct AnimData {
public:
	void ImportFile(const char* fileName) {
		std::ifstream in;

		in.open(fileName, std::ios::in | std::ios::binary);
		char AnimName[32];
		in.read((char*)AnimName, sizeof(AnimName));
		m_AnimName = AnimName;

		m_nKeyTime = 0;
		in.read((char*)&m_nKeyTime, sizeof(int));
		m_KeyTime = new double[m_nKeyTime];
		in.read((char*)m_KeyTime, sizeof(double) * m_nKeyTime);

		in.read((char*)&m_nBoneList, sizeof(int));
		m_BoneList = new AnimBone[m_nBoneList];

		for (int i = 0; i < m_nBoneList; ++i) {
			char name[32];
			in.read((char*)name, sizeof(name));
			m_BoneList[i].m_Name = name;

			in.read((char*)&m_BoneList[i].m_GlobalTransform, sizeof(XMFLOAT4X4));
		}

		for (int i = 0; i < m_nBoneList; ++i) {
			m_BoneList[i].m_pToRootTransforms = new XMFLOAT4X4[m_nKeyTime];
			for (int j = 0; j < m_nKeyTime; ++j) {
				in.read((char*)&m_BoneList[i].m_pToRootTransforms[j], sizeof(XMFLOAT4X4));
			}
		}

		in.close();
	}
	void Display() {
		std::cout << m_AnimName << "\n\n";

		for (int i = 0; i < m_nBoneList; ++i) {
			std::cout << m_BoneList[i].m_Name << "\n\n";
		}
	}

private:
	std::string m_AnimName;
	int			m_nBoneList;
	int			m_nKeyTime;

	AnimBone*	m_BoneList;
	double*		m_KeyTime;
};


int main()
{
	AnimData a;

	a.ImportFile("test_0429_015_Character_anim.dat");
	a.Display();

	return 0;
}
