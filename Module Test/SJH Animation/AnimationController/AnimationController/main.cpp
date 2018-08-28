#include<iostream>
#include<chrono>
#include"stdafx.h"
#include"FBXDataManager.h"
#include"AnimationController.h"

using namespace std;

FBXDataManager fbxmanager;
AnimationController ac;

void TestAffineTransFormation()
{
#include<time.h>
	srand(time(NULL));
	XMFLOAT3 Scaling{ 1,1,1 };
	XMFLOAT4 zero{ 0,0,0,1};
	XMFLOAT3 Trans{ 0,0,0 };
	XMFLOAT3 Rotate{ 0,0,0 };

	XMFLOAT4X4 res_matrix;
	int cnt = -1;
	const int sampling = 1;
	cout.width(10);



	while (cnt++ < sampling)
	{
		Trans.x += rand() % 5 == 0 ? 1 : 0;
		Trans.y += rand() % 5 == 0 ? 1 : 0;
		Trans.z += rand() % 5 == 0 ? 1 : 0;

		Rotate.x += rand() % 5 == 0 ? 10 : 0;
		Rotate.y += rand() % 5 == 0 ? 10 : 0;
		Rotate.z += rand() % 5 == 0 ? 10 : 0;
		
		Trans.x = 100;
		Trans.y = 200;
		Trans.z = 300;

		Rotate.x = XMConvertToRadians(90);
		Rotate.y = XMConvertToRadians(90);
		Rotate.z = XMConvertToRadians(0);

		XMFLOAT4 Quat, tmp;
		XMStoreFloat4(&Quat, XMQuaternionRotationRollPitchYaw(
			XMConvertToRadians(90) ,
			XMConvertToRadians(90) ,
			XMConvertToRadians(0)));
		//XMQuaternionRotationRollPitchYaw

		res_matrix = Matrix4x4::AffineTransformation(Scaling, zero, Rotate, Trans);

		XMStoreFloat4x4(&res_matrix,
			XMMatrixRotationQuaternion(XMLoadFloat4(&Quat))
		);

		XMStoreFloat4(&tmp,
			XMQuaternionRotationMatrix(XMLoadFloat4x4(&res_matrix))
		);
		

		cout << "count: " << cnt << "\n\n";
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				cout << res_matrix.m[i][j] << "\t";
			}
			cout << "\n";
		}
		cout << "Quaternion: " << Quat.x << ", " << Quat.y << ", " << Quat.z << ", " << Quat.w << "\n";
		cout << "tmp: " << tmp.x << ", " << tmp.y << ", " << tmp.z << ", " << tmp.w << "\n";
		cout << "==========================\n";

	}

}

int main()
{
	bool res;
	res = fbxmanager.FBXFileRead("char_run_test");
	

	float local_time=0;
	if (true == res) {
		cout << "FileRead Success\n";
		ac.Init(nullptr,nullptr,&fbxmanager);
		auto s = chrono::system_clock::time_point();
		s = chrono::system_clock::now();


		while (true)
		{
			float et = chrono::duration_cast<chrono::microseconds>(
				chrono::system_clock::now()
				- s).count()/1000.f;
			local_time += et;

			ac.Update(et,nullptr);
			if (local_time > 5000)break;
			Sleep(1);
			s = chrono::system_clock::now();

		}
	}
	else
	{
		cout << "FileRead failed\n";
	}

	system("pause");
}