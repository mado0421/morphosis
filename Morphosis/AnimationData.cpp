#include "stdafx.h"
#include "AnimationData.h"

//Anim::Anim()
//{
//	nKeys = 7;
//	nBones = 3;
//	keyList = new CKey*[nKeys];
//
//	CBone ** boneList = new CBone*[nBones];
//
//#ifdef TEST_
//	{
//		CBone * pB;
//		pB = new CBone();
//		pB->Initialize(
//			XMFLOAT3(0, 0, 0),
//			XMFLOAT3(90, -90, 0),
//			NULL);
//		boneList[0] = pB;
//	}
//
//	{
//		CBone * pB;
//		pB = new CBone();
//		pB->Initialize(
//			XMFLOAT3(10,0,0),
//			XMFLOAT3(0,0,0),
//			boneList[0]);
//		boneList[1] = pB;
//	}
//
//	{
//		CBone * pB;
//		pB = new CBone();
//		pB->Initialize(
//			XMFLOAT3(10,0,0),
//			XMFLOAT3(0,0,0),
//			boneList[1]);
//		boneList[2] = pB;
//	}
//#else
//	{
//		CBone * pB;
//		pB = new CBone();
//		pB->Initialize(
//			XMFLOAT3(-0.0176298636943102, -4.30831335052062e-07, 9.85627174377441),
//			XMFLOAT3(90.0000193525796, -89.5907443911997, 0),
//			NULL);
//		boneList[0] = pB;
//	}
//
//	{
//		CBone * pB;
//		pB = new CBone();
//		pB->Initialize(
//			XMFLOAT3(10.2381744384766, 1.24424695968628e-06, 1.33681851366418e-07),
//			XMFLOAT3(-2.73207566987348e-05, -0, 0.40925563548),
//			boneList[0]);
//		boneList[1] = pB;
//	}
//
//	{
//		CBone * pB;
//		pB = new CBone();
//		pB->Initialize(
//			XMFLOAT3(9.87227439880371, 0, 7.21543983672746e-07),
//			XMFLOAT3(7.95118888487746e-06, -0, 0.418209901769637),
//			boneList[1]);
//		boneList[2] = pB;
//	}
//#endif
//
//
//	float time[7] = {
//	GetTime(0),
//	GetTime(7697693000),
//	GetTime(15395386000),
//	GetTime(23093079000),
//	GetTime(30790772000),
//	GetTime(38488465000),
//	GetTime(46186158000)
//	};
//	// Key마다 본의 값을 복사해서 넣는다.
//	for (int i = 0; i < nKeys; ++i) {
//		CKey * pk = new CKey();
//		pk->nBones = nBones;
//		pk->boneList = new CBone*[pk->nBones];
//		for (int j = 0; j < nBones; ++j) {
//			pk->boneList[j] = new CBone(*boneList[j]);
//		}
//		pk->keyTime = time[i];
//		keyList[i] = pk;
//	}
//
//	keyList[0]->boneList[0]->parent = NULL;
//	keyList[0]->boneList[1]->parent = keyList[0]->boneList[0];
//	keyList[0]->boneList[2]->parent = keyList[0]->boneList[1];
//
//
//	// Key마다 본들의 Lcl 값을 넣는다.
//#ifdef TEST_
//	keyList[0]->boneList[0]->Pos = XMFLOAT3(0, 0, 0);
//	keyList[1]->boneList[0]->Pos = XMFLOAT3(0, 0, 0);
//	keyList[2]->boneList[0]->Pos = XMFLOAT3(0, 0, 0);
//	keyList[3]->boneList[0]->Pos = XMFLOAT3(0, 0, 0);
//	keyList[4]->boneList[0]->Pos = XMFLOAT3(0, 0, 0);
//	keyList[5]->boneList[0]->Pos = XMFLOAT3(0, 0, 0);
//	keyList[6]->boneList[0]->Pos = XMFLOAT3(0, 0, 0);
//
//	keyList[0]->boneList[1]->Pos = XMFLOAT3(0, 0, 0);
//	keyList[1]->boneList[1]->Pos = XMFLOAT3(0, 0, 0);
//	keyList[2]->boneList[1]->Pos = XMFLOAT3(0, 0, 0);
//	keyList[3]->boneList[1]->Pos = XMFLOAT3(0, 0, 0);
//	keyList[4]->boneList[1]->Pos = XMFLOAT3(0, 0, 0);
//	keyList[5]->boneList[1]->Pos = XMFLOAT3(0, 0, 0);
//	keyList[6]->boneList[1]->Pos = XMFLOAT3(0, 0, 0);
//
//	keyList[0]->boneList[2]->Pos = XMFLOAT3(0, 0, 0);
//	keyList[1]->boneList[2]->Pos = XMFLOAT3(0, 0, 0);
//	keyList[2]->boneList[2]->Pos = XMFLOAT3(0, 0, 0);
//	keyList[3]->boneList[2]->Pos = XMFLOAT3(0, 0, 0);
//	keyList[4]->boneList[2]->Pos = XMFLOAT3(0, 0, 0);
//	keyList[5]->boneList[2]->Pos = XMFLOAT3(0, 0, 0);
//	keyList[6]->boneList[2]->Pos = XMFLOAT3(0, 0, 0);
//#else
//	keyList[0]->boneList[0]->Pos = XMFLOAT3(-0.01762986, -4.308313e-07, 9.856272);
//	keyList[1]->boneList[0]->Pos = XMFLOAT3(-0.01762986, -4.308313e-07, 9.856272);
//	keyList[2]->boneList[0]->Pos = XMFLOAT3(-0.01762986, -4.308313e-07, 9.856272);
//	keyList[3]->boneList[0]->Pos = XMFLOAT3(-0.01762986, -4.308313e-07, 9.856272);
//	keyList[4]->boneList[0]->Pos = XMFLOAT3(-0.01762986, -4.308313e-07, 9.856272);
//	keyList[5]->boneList[0]->Pos = XMFLOAT3(-0.01762986, -4.308313e-07, 9.856272);
//	keyList[6]->boneList[0]->Pos = XMFLOAT3(-0.01762986, -4.308313e-07, 9.856272);
//
//	keyList[0]->boneList[1]->Pos = XMFLOAT3(10.23817, 1.244247e-06, 1.336819e-07);
//	keyList[1]->boneList[1]->Pos = XMFLOAT3(10.23817, 1.244247e-06, 1.336819e-07);
//	keyList[2]->boneList[1]->Pos = XMFLOAT3(10.23817, 1.244247e-06, 1.336819e-07);
//	keyList[3]->boneList[1]->Pos = XMFLOAT3(10.23817, 1.244247e-06, 1.336819e-07);
//	keyList[4]->boneList[1]->Pos = XMFLOAT3(10.23817, 1.244247e-06, 1.336819e-07);
//	keyList[5]->boneList[1]->Pos = XMFLOAT3(10.23817, 1.244247e-06, 1.336819e-07);
//	keyList[6]->boneList[1]->Pos = XMFLOAT3(10.23817, 1.244247e-06, 1.336819e-07);
//
//	keyList[0]->boneList[2]->Pos = XMFLOAT3(9.872274, 0, 7.21544e-07);
//	keyList[1]->boneList[2]->Pos = XMFLOAT3(9.872274, 0, 7.21544e-07);
//	keyList[2]->boneList[2]->Pos = XMFLOAT3(9.872274, 0, 7.21544e-07);
//	keyList[3]->boneList[2]->Pos = XMFLOAT3(9.872274, 0, 7.21544e-07);
//	keyList[4]->boneList[2]->Pos = XMFLOAT3(9.872274, 0, 7.21544e-07);
//	keyList[5]->boneList[2]->Pos = XMFLOAT3(9.872274, 0, 7.21544e-07);
//	keyList[6]->boneList[2]->Pos = XMFLOAT3(9.872274, 0, 7.21544e-07);
//#endif
//
//	keyList[0]->boneList[0]->Rot = XMFLOAT3(450, -89.59074, 0);
//	keyList[1]->boneList[0]->Rot = XMFLOAT3(450, -89.59074, 0);
//	keyList[2]->boneList[0]->Rot = XMFLOAT3(450, -89.59074, 0);
//	keyList[3]->boneList[0]->Rot = XMFLOAT3(450, 0, 0);
//	keyList[4]->boneList[0]->Rot = XMFLOAT3(450, -89.59074, 0);
//	keyList[5]->boneList[0]->Rot = XMFLOAT3(450, -89.59074, 0);
//	keyList[6]->boneList[0]->Rot = XMFLOAT3(450, -89.59074, 0);
//
//	keyList[0]->boneList[1]->Rot = XMFLOAT3(-2.732076e-05, 0, 0.4092556);
//	keyList[1]->boneList[1]->Rot = XMFLOAT3(-2.732076e-05, 0, 0.4092556);
//	keyList[2]->boneList[1]->Rot = XMFLOAT3(-2.732076e-05, 0, 45.40925);//각도? 기준점이 이상한 것 같은데? 피봇때문에 이동하려고 길어지는 것 같음
//	keyList[3]->boneList[1]->Rot = XMFLOAT3(-2.732076e-05, 0, 45.40925);
//	keyList[4]->boneList[1]->Rot = XMFLOAT3(-2.732076e-05, 0, 45.40925);
//	keyList[5]->boneList[1]->Rot = XMFLOAT3(-2.732076e-05, 0, 0.4092556);
//	keyList[6]->boneList[1]->Rot = XMFLOAT3(-2.732076e-05, 0, 0.4092556);
//
//	keyList[0]->boneList[2]->Rot = XMFLOAT3(7.951189e-06, 0,  0.4182099);
//	keyList[1]->boneList[2]->Rot = XMFLOAT3(7.951189e-06, 30, 0.4182099);
//	keyList[2]->boneList[2]->Rot = XMFLOAT3(7.951189e-06, 30, 0.4182099);
//	keyList[3]->boneList[2]->Rot = XMFLOAT3(7.951189e-06, 30, 0.4182099);
//	keyList[4]->boneList[2]->Rot = XMFLOAT3(7.951189e-06, 30, 0.4182099);
//	keyList[5]->boneList[2]->Rot = XMFLOAT3(7.951189e-06, 30, 0.4182099);
//	keyList[6]->boneList[2]->Rot = XMFLOAT3(7.951189e-06, 0,  0.4182099);
//}
//
//Anim::Anim(int Keys, int Bones, vector<CBone>& b, vector<CKey>& k)
//{
//	
//	nKeys = k.size();
//	nBones = b.size();
//	keyList = new CKey*[nKeys];
//	CBone ** boneList = new CBone*[nBones];
//
//	for (int i = 0; i < nBones; ++i) {
//		CBone * pB;
//		pB = new CBone();
//		
//		if(-1 == b[i].parentIdx)
//			pB->Initialize(
//				XMFLOAT3(0, 0, 0),
//				XMFLOAT3(0, 0, 0),
//				NULL);
//		else
//			pB->Initialize(
//				XMFLOAT3(0, 0, 0),
//				XMFLOAT3(0, 0, 0),
//				boneList[b[i].parentIdx]);
//		boneList[i] = pB;
//	}
//
//	float* time = new float[nKeys];
//	for (int i = 0; i < nKeys; ++i) {
//		time[i] = k[i].keyTime;
//	}
//
//	for (int i = 0; i < nKeys; ++i) {
//		CKey* pk = new CKey();
//		pk->nBones = k[i].nBones;
//		pk->boneList = new CBone*[pk->nBones];
//		for (int j = 0; j < pk->nBones; ++j) {
//			pk->boneList[j] = new CBone(*boneList[j]);
//
//		}
//		pk->keyTime = time[i];
//		keyList[i] = pk;
//		for (int j = 0; j < pk->nBones; ++j)
//			keyList[i]->boneList[j]->parent = boneList[boneList[j]->parentIdx];
//	}
//
//	for (int i = 0; i < nKeys; ++i) {
//		for (int j = 0; j < keyList[i]->nBones; ++j) {
//			keyList[i]->boneList[j]->Pos = k[i].bones[j].Pos;
//			keyList[i]->boneList[j]->Rot = k[i].bones[j].Rot;
//		}
//	}
//}
//
//Anim::~Anim()
//{
//
//}

