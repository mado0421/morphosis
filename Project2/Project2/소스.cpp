#include <iostream>
#include <fstream>
#include <vector>
#include <DirectXMath.h>

using namespace DirectX;
using namespace std;

struct LEVELDATA_DESC {
	std::string levelName		= "";
	int nCollisionMaps			= 0;
	int nTeam1SpawnPoint		= 0;
	int nTeam2SpawnPoint		= 0;
	bool isCapturePointExist	= false;

	XMFLOAT3 BoundaryBoxPosition;
	XMFLOAT3 BoundaryBoxScale;
	XMFLOAT4 BoundaryBoxRotation;

	std::vector<XMFLOAT3> CollisionPosition;
	std::vector<XMFLOAT3> CollisionScale;
	std::vector<XMFLOAT4> CollisionRotation;

	std::vector<XMFLOAT3> Team1SpawnPointPosition;
	std::vector<XMFLOAT4> Team1SpawnPointRotation;

	std::vector<XMFLOAT3> Team2SpawnPointPosition;
	std::vector<XMFLOAT4> Team2SpawnPointRotation;

	XMFLOAT3 CapturePointPosition;
	XMFLOAT3 CapturePointScale;
	XMFLOAT4 CapturePointRotation;
};

int main()
{
	ifstream in;
	LEVELDATA_DESC levelDataDesc;

	in.open("LevelData_TestMap.dat", std::ifstream::in);

	char line[256];

	in.getline(line, 256);
	levelDataDesc.levelName = line;

	in.getline(line, 256);
	levelDataDesc.nCollisionMaps = atoi(line);

	in.getline(line, 256);
	levelDataDesc.nTeam1SpawnPoint = atoi(line);

	in.getline(line, 256);
	levelDataDesc.nTeam2SpawnPoint = atoi(line);

	in.getline(line, 256);
	if (strcmp(line, "true"))	levelDataDesc.isCapturePointExist = true;
	else						levelDataDesc.isCapturePointExist = false;

	in.getline(line, 256); levelDataDesc.BoundaryBoxPosition.x	= atof(line);
	in.getline(line, 256); levelDataDesc.BoundaryBoxPosition.y	= atof(line);
	in.getline(line, 256); levelDataDesc.BoundaryBoxPosition.z	= atof(line);
	in.getline(line, 256); levelDataDesc.BoundaryBoxScale.x		= atof(line);
	in.getline(line, 256); levelDataDesc.BoundaryBoxScale.y		= atof(line);
	in.getline(line, 256); levelDataDesc.BoundaryBoxScale.z		= atof(line);
	in.getline(line, 256); levelDataDesc.BoundaryBoxRotation.x	= atof(line);
	in.getline(line, 256); levelDataDesc.BoundaryBoxRotation.y	= atof(line);
	in.getline(line, 256); levelDataDesc.BoundaryBoxRotation.z	= atof(line);
	in.getline(line, 256); levelDataDesc.BoundaryBoxRotation.w	= atof(line);

	for (int i = 0; i < levelDataDesc.nCollisionMaps; ++i) {
		XMFLOAT3 temp3;
		XMFLOAT4 temp4;

		in.getline(line, 256); temp3.x = atof(line);
		in.getline(line, 256); temp3.y = atof(line);
		in.getline(line, 256); temp3.z = atof(line);

		levelDataDesc.CollisionPosition.push_back(temp3);

		in.getline(line, 256); temp3.x = atof(line);
		in.getline(line, 256); temp3.y = atof(line);
		in.getline(line, 256); temp3.z = atof(line);

		levelDataDesc.CollisionScale.push_back(temp3);

		in.getline(line, 256); temp4.x = atof(line);
		in.getline(line, 256); temp4.y = atof(line);
		in.getline(line, 256); temp4.z = atof(line);
		in.getline(line, 256); temp4.w = atof(line);

		levelDataDesc.CollisionRotation.push_back(temp4);
	}

	for (int i = 0; i < levelDataDesc.nTeam1SpawnPoint; ++i) {
		XMFLOAT3 temp3;
		XMFLOAT4 temp4;

		in.getline(line, 256); temp3.x = atof(line);
		in.getline(line, 256); temp3.y = atof(line);
		in.getline(line, 256); temp3.z = atof(line);

		levelDataDesc.Team1SpawnPointPosition.push_back(temp3);

		in.getline(line, 256); temp4.x = atof(line);
		in.getline(line, 256); temp4.y = atof(line);
		in.getline(line, 256); temp4.z = atof(line);
		in.getline(line, 256); temp4.w = atof(line);

		levelDataDesc.Team1SpawnPointRotation.push_back(temp4);
	}

	for (int i = 0; i < levelDataDesc.nTeam2SpawnPoint; ++i) {
		XMFLOAT3 temp3;
		XMFLOAT4 temp4;

		in.getline(line, 256); temp3.x = atof(line);
		in.getline(line, 256); temp3.y = atof(line);
		in.getline(line, 256); temp3.z = atof(line);

		levelDataDesc.Team2SpawnPointPosition.push_back(temp3);

		in.getline(line, 256); temp4.x = atof(line);
		in.getline(line, 256); temp4.y = atof(line);
		in.getline(line, 256); temp4.z = atof(line);
		in.getline(line, 256); temp4.w = atof(line);

		levelDataDesc.Team2SpawnPointRotation.push_back(temp4);
	}

	in.getline(line, 256); levelDataDesc.CapturePointPosition.x = atof(line);
	in.getline(line, 256); levelDataDesc.CapturePointPosition.y = atof(line);
	in.getline(line, 256); levelDataDesc.CapturePointPosition.z = atof(line);
	in.getline(line, 256); levelDataDesc.CapturePointScale.x = atof(line);
	in.getline(line, 256); levelDataDesc.CapturePointScale.y = atof(line);
	in.getline(line, 256); levelDataDesc.CapturePointScale.z = atof(line);
	in.getline(line, 256); levelDataDesc.CapturePointRotation.x = atof(line);
	in.getline(line, 256); levelDataDesc.CapturePointRotation.y = atof(line);
	in.getline(line, 256); levelDataDesc.CapturePointRotation.z = atof(line);
	in.getline(line, 256); levelDataDesc.CapturePointRotation.w = atof(line);


	in.close();


	return 0;
}
