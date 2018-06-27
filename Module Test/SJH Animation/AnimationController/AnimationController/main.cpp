#include<iostream>
#include<chrono>
#include"stdafx.h"
#include"FBXDataManager.h"
#include"AnimationController.h"

using namespace std;

FBXDataManager fbxmanager;
AnimationController ac;
int main()
{
	bool res;
	res = fbxmanager.FBXFileRead("BoxWithTwoBones5");


	float time=0;
	if (true == res) {
		cout << "FileRead Success\n";
		ac.Init(&fbxmanager);
		auto s = chrono::system_clock::time_point();
		s = chrono::system_clock::now();


		while (true)
		{
			float et = chrono::duration_cast<chrono::microseconds>(
				chrono::system_clock::now()
				- s).count()/1000.f;
			time += et;

			ac.Update(et);
			if (time > 5000)break;
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