// FBXConverter.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include "Converter.h"
#include"FBXDataManager.h"
#include<chrono>
using namespace std::chrono;
Converter g_Converter;

FBXDataManager manager;
int main()
{
	/*char name[200];
	printf("변환 할 파일 이름을 입력하시오: ");
	scanf("%d", &name);*/

	g_Converter.ReadFile("BoxWithTwoBones5");

	manager.FBXFileRead("BoxWithTwoBones5");

	for (int i = 0; i < 4; ++i)
	{
		printf("%d", i);
	}
    return 0;
}

