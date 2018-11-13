#include "stdafx.h"
#include "AnimationData.h"

//CAnimationData::CAnimationData()
//{
//	numOfBones = 3;
//	bones = new Bone*[numOfBones];
//
//	bones[0] = new Bone();
//	bones[1] = new Bone();
//	bones[2] = new Bone();
//
//
//	bones[0]->translation = XMFLOAT3(-0.0176298636943102, 9.85627174377441, 4.30831335052062e-07);
//	bones[0]->rotation = XMFLOAT3( 90.0000193525796,-89.5907443911997,0 );
//	bones[0]->numOfSibling = 0;
//	bones[0]->parent = NULL;	//it means root;
//	bones[0]->bindPose		= XMFLOAT4X4( 0.0071427971124649,0.999974489212036,0,0,-0.999974489212036,0.0071427971124649,3.37766238089898e-07,0,3.37757626311941e-07,-2.41259567879126e-09,1,0,-0.0176298636943102,9.85627174377441,4.30831335052062e-07,1 );
//	bones[0]->transform		= XMFLOAT4X4( 0.00714279712228283,-0.999974490586425,3.37757617137112e-07,0,6.12307775427002e-17,-3.37766233819447e-07,-0.999999999999886,0,0.999974490586539,0.00714279712228202,-2.41259561838444e-09,0,0.143850516341733,-0.0166027921920127,-4.25223472470179e-07,1 );
//	bones[0]->transformLink = XMFLOAT4X4( 0.00714279711246479,0.999974489212036,0,0,-0.999974489212036,0.00714279711246479,3.3776623335565e-07,0,3.3775761713715e-07,-2.41259567961705e-09,1,0,-0.0176298636943102,9.85627174377441,4.30831335052062e-07,1 );
//	bones[0]->numOfVertice = 8;
//	bones[0]->idx		= new int[bones[0]->numOfVertice];
//	bones[0]->weights = new float[bones[0]->numOfVertice];
//	{
//		bones[0]->idx[0] = 0;
//		bones[0]->idx[1] = 1;
//		bones[0]->idx[2] = 2;
//		bones[0]->idx[3] = 3;
//		bones[0]->idx[4] = 8;
//		bones[0]->idx[5] = 9;
//		bones[0]->idx[6] = 10;
//		bones[0]->idx[7] = 11;
//		for (int i = 0; i < bones[0]->numOfVertice; ++i) bones[0]->weights[i] = 1;
//	}
//	bones[0]->numOfKeys = 7;
//	bones[0]->keys = new Key*[bones[0]->numOfKeys];
//
//	bones[1]->translation = XMFLOAT3( 10.2381744384766,1.24424695968628e-06,1.33681851366418e-07 );
//	bones[1]->rotation = XMFLOAT3( -2.73207566987348e-05,-0,0.40925563548 );
//	bones[1]->numOfSibling = 0;
//	bones[1]->parent = bones[0];
//	bones[1]->bindPose		= XMFLOAT4X4( -4.65661287307739e-10,1,2.41259590083587e-09,0,-1,-4.65660149329139e-10,-1.39079531891184e-07,0,-1.39079531891184e-07,-2.41259590083587e-09,1,0,0.0554980933666229,20.0941848754883,5.64513584322412e-07,1 );
//	bones[1]->transform		= XMFLOAT4X4( -0.00729907543697492,-0.999973366064862,-3.08774294433234e-10,0,-1.39737971750286e-09,3.18982373045581e-10,-1,0,0.999973366064862,-0.00729907543697492,-1.39967076311125e-09,0,-19.9655224041678,0.201233303961158,-1.28191180835004e-06,1 );
//	bones[1]->transformLink = XMFLOAT4X4( -0.00729907536879188,0.999973356723785,1.39737976568019e-09,0,-0.999973356723785,-0.00729907536879187,-3.18982370512802e-10,0,-3.08774294433234e-10,-1.39967082434359e-09,1,0,0.0554980896413326,29.966459274292,1.30987541524519e-06,1 );
//	bones[1]->numOfVertice = 8;
//	bones[1]->idx		= new int[bones[1]->numOfVertice];
//	bones[1]->weights = new float[bones[1]->numOfVertice];
//	{
//		bones[1]->idx[0] = 4;
//		bones[1]->idx[1] = 5;
//		bones[1]->idx[2] = 6;
//		bones[1]->idx[3] = 7;
//		bones[1]->idx[4] = 12;
//		bones[1]->idx[5] = 13;
//		bones[1]->idx[6] = 14;
//		bones[1]->idx[7] = 15;
//		for (int i = 0; i < bones[1]->numOfVertice; ++i) bones[1]->weights[i] = 0.5;
//	}
//	bones[1]->numOfKeys = 7;
//	bones[1]->keys = new Key*[bones[1]->numOfKeys];
//
//	bones[2]->translation = XMFLOAT3( 9.87227439880371,0,7.21543983672746e-07 );
//	bones[2]->rotation = XMFLOAT3( 7.95118888487746e-06,-0,0.418209901769637 );
//	bones[2]->numOfSibling = 0;
//	bones[2]->parent = bones[1];
//	bones[2]->bindPose		= XMFLOAT4X4( -0.00729907536879182,0.999973356723785,1.3973797674538e-09,0,-0.999973356723785,-0.00729907536879182,-3.19005266646855e-10,0,-3.0880187296134e-10,-1.39967082368742e-09,1,0,0.0554980896413326,29.966459274292,1.30987541524519e-06,1 );
//	bones[2]->transform		= XMFLOAT4X4( 2.45712412100606e-09,-0.999999999999981,-1.3907953173037e-07,0,6.12323399573676e-17,1.3907953173037e-07,-0.999999999999981,0,1,2.457124121006e-09,4.02968012110286e-16,0,-10.0941848756246,0.0554980685640352,-5.56794938934586e-07,1 );
//	bones[2]->transformLink = XMFLOAT4X4( 2.45712412100606e-09,1,0,0,-1,2.45712412100606e-09,-1.39079531730373e-07,0,-1.39079531730373e-07,3.41735672152926e-16,1,0,0.0554980933666229,20.0941848754883,5.64513584322412e-07,1 );
//	bones[2]->numOfVertice = 8;
//	bones[2]->idx		= new int[bones[2]->numOfVertice];
//	bones[2]->weights = new float[bones[2]->numOfVertice];
//	{
//		bones[2]->idx[0] = 4;
//		bones[2]->idx[1] = 5;
//		bones[2]->idx[2] = 6;
//		bones[2]->idx[3] = 7;
//		bones[2]->idx[4] = 12;
//		bones[2]->idx[5] = 13;
//		bones[2]->idx[6] = 14;
//		bones[2]->idx[7] = 15;
//		for (int i = 0; i < bones[2]->numOfVertice; ++i) bones[2]->weights[i] = 0.5;
//	}
//	bones[2]->numOfKeys = 7;
//	bones[2]->keys = new Key*[bones[2]->numOfKeys];
//
//	for (int i = 0; i < bones[0]->numOfKeys; ++i) bones[0]->keys[i] = new Key();
//	for (int i = 0; i < bones[1]->numOfKeys; ++i) bones[1]->keys[i] = new Key();
//	for (int i = 0; i < bones[2]->numOfKeys; ++i) bones[2]->keys[i] = new Key();
//
//	bones[0]->keys[0]->time = GetTime(0);
//	bones[0]->keys[1]->time = GetTime(7697693000);
//	bones[0]->keys[2]->time = GetTime(15395386000);
//	bones[0]->keys[3]->time = GetTime(23093079000);
//	bones[0]->keys[4]->time = GetTime(30790772000);
//	bones[0]->keys[5]->time = GetTime(38488465000);
//	bones[0]->keys[6]->time = GetTime(46186158000);
//
//	bones[1]->keys[0]->time = GetTime(0);
//	bones[1]->keys[1]->time = GetTime(7697693000);
//	bones[1]->keys[2]->time = GetTime(15395386000);
//	bones[1]->keys[3]->time = GetTime(23093079000);
//	bones[1]->keys[4]->time = GetTime(30790772000);
//	bones[1]->keys[5]->time = GetTime(38488465000);
//	bones[1]->keys[6]->time = GetTime(46186158000);
//
//	bones[2]->keys[0]->time = GetTime(0);
//	bones[2]->keys[1]->time = GetTime(7697693000);
//	bones[2]->keys[2]->time = GetTime(15395386000);
//	bones[2]->keys[3]->time = GetTime(23093079000);
//	bones[2]->keys[4]->time = GetTime(30790772000);
//	bones[2]->keys[5]->time = GetTime(38488465000);
//	bones[2]->keys[6]->time = GetTime(46186158000);
//
//	bones[0]->keys[0]->bookmark = 0;
//	bones[0]->keys[1]->bookmark = 7697693000;
//	bones[0]->keys[2]->bookmark = 15395386000;
//	bones[0]->keys[3]->bookmark = 23093079000;
//	bones[0]->keys[4]->bookmark = 30790772000;
//	bones[0]->keys[5]->bookmark = 38488465000;
//	bones[0]->keys[6]->bookmark = 46186158000;
//
//	bones[1]->keys[0]->bookmark = 0;
//	bones[1]->keys[1]->bookmark = 7697693000;
//	bones[1]->keys[2]->bookmark = 15395386000;
//	bones[1]->keys[3]->bookmark = 23093079000;
//	bones[1]->keys[4]->bookmark = 30790772000;
//	bones[1]->keys[5]->bookmark = 38488465000;
//	bones[1]->keys[6]->bookmark = 46186158000;
//
//	bones[2]->keys[0]->bookmark = 0;
//	bones[2]->keys[1]->bookmark = 7697693000;
//	bones[2]->keys[2]->bookmark = 15395386000;
//	bones[2]->keys[3]->bookmark = 23093079000;
//	bones[2]->keys[4]->bookmark = 30790772000;
//	bones[2]->keys[5]->bookmark = 38488465000;
//	bones[2]->keys[6]->bookmark = 46186158000;
//
//	/*
//	해당 본을 움직이게 할 커브 노드를 찾고
//	그 노드의 키 타임별 값을 넣으면 됨
//	*/
//
//	bones[0]->keys[bones[0]->FindKeyIdxByBookmark(0)]->translation.x =				-0.01762986,4.308313e-07,9.856272;
//	bones[0]->keys[bones[0]->FindKeyIdxByBookmark(7697693000)]->translation.x =		-0.01762986,4.308313e-07,9.856272;
//	bones[0]->keys[bones[0]->FindKeyIdxByBookmark(15395386000)]->translation.x =	-0.01762986,4.308313e-07,9.856272;
//	bones[0]->keys[bones[0]->FindKeyIdxByBookmark(23093079000)]->translation.x =	-0.01762986,4.308313e-07,9.856272;
//	bones[0]->keys[bones[0]->FindKeyIdxByBookmark(30790772000)]->translation.x =	-0.01762986,4.308313e-07,9.856272;
//	bones[0]->keys[bones[0]->FindKeyIdxByBookmark(38488465000)]->translation.x =	-0.01762986,4.308313e-07,9.856272;
//	bones[0]->keys[bones[0]->FindKeyIdxByBookmark(46186158000)]->translation.x =	-0.01762986,4.308313e-07,9.856272;
//
//	bones[1]->keys[bones[1]->FindKeyIdxByBookmark(0)]->translation.x =				10.23817,1.244247e-06,1.336819e-07;
//	bones[1]->keys[bones[1]->FindKeyIdxByBookmark(7697693000)]->translation.x =		10.23817,1.244247e-06,1.336819e-07;
//	bones[1]->keys[bones[1]->FindKeyIdxByBookmark(15395386000)]->translation.x =	10.23817,1.244247e-06,1.336819e-07;
//	bones[1]->keys[bones[1]->FindKeyIdxByBookmark(23093079000)]->translation.x =	10.23817,1.244247e-06,1.336819e-07;
//	bones[1]->keys[bones[1]->FindKeyIdxByBookmark(30790772000)]->translation.x =	10.23817,1.244247e-06,1.336819e-07;
//	bones[1]->keys[bones[1]->FindKeyIdxByBookmark(38488465000)]->translation.x =	10.23817,1.244247e-06,1.336819e-07;
//	bones[1]->keys[bones[1]->FindKeyIdxByBookmark(46186158000)]->translation.x =	10.23817,1.244247e-06,1.336819e-07;
//
//	bones[2]->keys[bones[2]->FindKeyIdxByBookmark(0)]->translation.x =				9.872274,0,7.21544e-07;
//	bones[2]->keys[bones[2]->FindKeyIdxByBookmark(7697693000)]->translation.x =		9.872274,0,7.21544e-07;
//	bones[2]->keys[bones[2]->FindKeyIdxByBookmark(15395386000)]->translation.x =	9.872274,0,7.21544e-07;
//	bones[2]->keys[bones[2]->FindKeyIdxByBookmark(23093079000)]->translation.x =	9.872274,0,7.21544e-07;
//	bones[2]->keys[bones[2]->FindKeyIdxByBookmark(30790772000)]->translation.x =	9.872274,0,7.21544e-07;
//	bones[2]->keys[bones[2]->FindKeyIdxByBookmark(38488465000)]->translation.x =	9.872274,0,7.21544e-07;
//	bones[2]->keys[bones[2]->FindKeyIdxByBookmark(46186158000)]->translation.x =	9.872274,0,7.21544e-07;
//
//	bones[0]->keys[bones[0]->FindKeyIdxByBookmark(0)]->rotation.x =				450,-89.59074,0
//	bones[0]->keys[bones[0]->FindKeyIdxByBookmark(7697693000)]->rotation.x =	450,-89.59074,0
//	bones[0]->keys[bones[0]->FindKeyIdxByBookmark(15395386000)]->rotation.x =	450,-89.59074,0
//	bones[0]->keys[bones[0]->FindKeyIdxByBookmark(23093079000)]->rotation.x =	540.0001,0,-89.59074
//	bones[0]->keys[bones[0]->FindKeyIdxByBookmark(30790772000)]->rotation.x =	450,-89.59074,0
//	bones[0]->keys[bones[0]->FindKeyIdxByBookmark(38488465000)]->rotation.x =	450,-89.59074,0
//	bones[0]->keys[bones[0]->FindKeyIdxByBookmark(46186158000)]->rotation.x =	450,-89.59074,0
//
//	bones[1]->keys[bones[1]->FindKeyIdxByBookmark(0)]->rotation.x =				-2.732076e-05,0,0.4092556
//	bones[1]->keys[bones[1]->FindKeyIdxByBookmark(7697693000)]->rotation.x =	-2.732076e-05,0,0.4092556
//	bones[1]->keys[bones[1]->FindKeyIdxByBookmark(15395386000)]->rotation.x =	-2.732076e-05,0,45.40925
//	bones[1]->keys[bones[1]->FindKeyIdxByBookmark(23093079000)]->rotation.x =	-2.732076e-05,0,45.40925
//	bones[1]->keys[bones[1]->FindKeyIdxByBookmark(30790772000)]->rotation.x =	-2.732076e-05,0,45.40925
//	bones[1]->keys[bones[1]->FindKeyIdxByBookmark(38488465000)]->rotation.x =	-2.732076e-05,0,0.4092556
//	bones[1]->keys[bones[1]->FindKeyIdxByBookmark(46186158000)]->rotation.x =	-2.732076e-05,0,0.4092556
//
//	bones[2]->keys[bones[2]->FindKeyIdxByBookmark(0)]->rotation.x =				7.951189e-06,0,0.4182099
//	bones[2]->keys[bones[2]->FindKeyIdxByBookmark(7697693000)]->rotation.x =	7.951189e-06,30,0.4182099
//	bones[2]->keys[bones[2]->FindKeyIdxByBookmark(15395386000)]->rotation.x =	7.951189e-06,30,0.4182099
//	bones[2]->keys[bones[2]->FindKeyIdxByBookmark(23093079000)]->rotation.x =	7.951189e-06,30,0.4182099
//	bones[2]->keys[bones[2]->FindKeyIdxByBookmark(30790772000)]->rotation.x =	7.951189e-06,30,0.4182099
//	bones[2]->keys[bones[2]->FindKeyIdxByBookmark(38488465000)]->rotation.x =	7.951189e-06,30,0.4182099
//	bones[2]->keys[bones[2]->FindKeyIdxByBookmark(46186158000)]->rotation.x =	7.951189e-06,0,0.4182099
//
//}
//
//
//CAnimationData::~CAnimationData()
//{
//}

Anim::Anim()
{
	nKeys = 7;
	nBones = 3;
	keyList = new CKey*[nKeys];

	CBone ** boneList = new CBone*[nBones];

	{
		CBone * pB;
		pB = new CBone();
		pB->Initialize(
			XMFLOAT3(0, 10, 0),
			XMFLOAT3(90, -90, 0),
			NULL);
		//pB->Initialize(
		//	XMFLOAT3(-0.0176298636943102, 9.85627174377441, 4.30831335052062e-07),
		//	XMFLOAT3(0, 0, 0),
		//	NULL);
		//pB->Initialize(
		//	XMFLOAT3(-0.0176298636943102,  4.30831335052062e-07, 9.85627174377441),
		//	XMFLOAT3(90.0000193525796, 0, -89.5907443911997),
		//	NULL);
		boneList[0] = pB;
	}

	{
		CBone * pB;
		pB = new CBone();
		pB->Initialize(
			XMFLOAT3(10, 0, 0),
			XMFLOAT3(0, 0, 0),
			boneList[0]);
		//pB->Initialize(
		//	XMFLOAT3( 1.24424695968628e-06, 10.2381744384766, 1.33681851366418e-07),
		//	XMFLOAT3(-2.73207566987348e-05, -0, 0.40925563548),
		//	boneList[0]);
		//pB->Initialize(
		//	XMFLOAT3(10.2381744384766, 1.33681851366418e-07, 1.24424695968628e-06),
		//	XMFLOAT3(-2.73207566987348e-05, 0.40925563548, -0),
		//	boneList[0]);
		boneList[1] = pB;
	}

	{
		CBone * pB;
		pB = new CBone();
		pB->Initialize(
			XMFLOAT3(10, 0, 0),
			XMFLOAT3(0, 0, 0),
			boneList[1]);
		//pB->Initialize(
		//	XMFLOAT3( 0, 9.87227439880371, 7.21543983672746e-07),
		//	XMFLOAT3(7.95118888487746e-06, -0, 0.418209901769637),
		//	boneList[1]);
		//pB->Initialize(
		//	XMFLOAT3(9.87227439880371, 7.21543983672746e-07, 0),
		//	XMFLOAT3(7.95118888487746e-06, 0.418209901769637, -0),
		//	boneList[1]);
		boneList[2] = pB;
	}

	

	

	float time[7] = {
	GetTime(0),
	GetTime(7697693000),
	GetTime(15395386000),
	GetTime(23093079000),
	GetTime(30790772000),
	GetTime(38488465000),
	GetTime(46186158000)
	};
	// Key마다 본의 값을 복사해서 넣는다.
	for (int i = 0; i < nKeys; ++i) {
		CKey * pk = new CKey();
		pk->nBones = nBones;
		pk->boneList = new CBone*[pk->nBones];
		for (int j = 0; j < nBones; ++j) {
			pk->boneList[j] = new CBone(*boneList[j]);
		}
		pk->keyTime = time[i];
		keyList[i] = pk;
	}

	keyList[0]->boneList[0]->parent = NULL;
	keyList[0]->boneList[1]->parent = keyList[0]->boneList[0];
	keyList[0]->boneList[2]->parent = keyList[0]->boneList[1];


	// Key마다 본들의 Lcl 값을 넣는다.
	keyList[0]->boneList[0]->Pos = XMFLOAT3(0, 0, 10);
	keyList[1]->boneList[0]->Pos = XMFLOAT3(0, 0, 10);
	keyList[2]->boneList[0]->Pos = XMFLOAT3(0, 0, 10);
	keyList[3]->boneList[0]->Pos = XMFLOAT3(0, 0, 10);
	keyList[4]->boneList[0]->Pos = XMFLOAT3(0, 0, 10);
	keyList[5]->boneList[0]->Pos = XMFLOAT3(0, 0, 10);
	keyList[6]->boneList[0]->Pos = XMFLOAT3(0, 0, 10);

	keyList[0]->boneList[1]->Pos = XMFLOAT3(10, 0, 0);
	keyList[1]->boneList[1]->Pos = XMFLOAT3(10, 0, 0);
	keyList[2]->boneList[1]->Pos = XMFLOAT3(10, 0, 0);
	keyList[3]->boneList[1]->Pos = XMFLOAT3(10, 0, 0);
	keyList[4]->boneList[1]->Pos = XMFLOAT3(10, 0, 0);
	keyList[5]->boneList[1]->Pos = XMFLOAT3(10, 0, 0);
	keyList[6]->boneList[1]->Pos = XMFLOAT3(10, 0, 0);

	keyList[0]->boneList[2]->Pos = XMFLOAT3(10, 0, 0);
	keyList[1]->boneList[2]->Pos = XMFLOAT3(10, 0, 0);
	keyList[2]->boneList[2]->Pos = XMFLOAT3(10, 0, 0);
	keyList[3]->boneList[2]->Pos = XMFLOAT3(10, 0, 0);
	keyList[4]->boneList[2]->Pos = XMFLOAT3(10, 0, 0);
	keyList[5]->boneList[2]->Pos = XMFLOAT3(10, 0, 0);
	keyList[6]->boneList[2]->Pos = XMFLOAT3(10, 0, 0);

	keyList[0]->boneList[0]->Rot = XMFLOAT3(90, -90, 0);
	keyList[1]->boneList[0]->Rot = XMFLOAT3(90, -90, 0);
	keyList[2]->boneList[0]->Rot = XMFLOAT3(90, -90, 0);
	keyList[3]->boneList[0]->Rot = XMFLOAT3(90, -90, 0);
	keyList[4]->boneList[0]->Rot = XMFLOAT3(90, -90, 0);
	keyList[5]->boneList[0]->Rot = XMFLOAT3(90, -90, 0);
	keyList[6]->boneList[0]->Rot = XMFLOAT3(90, -90, 0);

	keyList[0]->boneList[1]->Rot = XMFLOAT3(0, 0, 0);
	keyList[1]->boneList[1]->Rot = XMFLOAT3(0, 0, 0);
	keyList[2]->boneList[1]->Rot = XMFLOAT3(0, 90, 0);//각도? 기준점이 이상한 것 같은데? 피봇때문에 이동하려고 길어지는 것 같음
	keyList[3]->boneList[1]->Rot = XMFLOAT3(0, 90, 0);
	keyList[4]->boneList[1]->Rot = XMFLOAT3(0, 90, 0);
	keyList[5]->boneList[1]->Rot = XMFLOAT3(0, 0, 0);
	keyList[6]->boneList[1]->Rot = XMFLOAT3(0, 0, 0);

	keyList[0]->boneList[2]->Rot = XMFLOAT3(0, 0, 0);
	keyList[1]->boneList[2]->Rot = XMFLOAT3(0, 0, 0);
	keyList[2]->boneList[2]->Rot = XMFLOAT3(0, 90, 0);
	keyList[3]->boneList[2]->Rot = XMFLOAT3(0, 90, 0);
	keyList[4]->boneList[2]->Rot = XMFLOAT3(0, 90, 0);
	keyList[5]->boneList[2]->Rot = XMFLOAT3(0, 0, 0);
	keyList[6]->boneList[2]->Rot = XMFLOAT3(0, 0, 0);

	//keyList[0]->boneList[0]->Rot = XMFLOAT3(90, -90, 0);
	//keyList[1]->boneList[0]->Rot = XMFLOAT3(90, -90, 0);
	//keyList[2]->boneList[0]->Rot = XMFLOAT3(90, -90, 0);
	//keyList[3]->boneList[0]->Rot = XMFLOAT3(180, 0, -90);
	//keyList[4]->boneList[0]->Rot = XMFLOAT3(90, -90, 0);
	//keyList[5]->boneList[0]->Rot = XMFLOAT3(90, -90, 0);
	//keyList[6]->boneList[0]->Rot = XMFLOAT3(90, -90, 0);

	//keyList[0]->boneList[1]->Rot = XMFLOAT3(0, 0, 0);
	//keyList[1]->boneList[1]->Rot = XMFLOAT3(0, 0, 0);
	//keyList[2]->boneList[1]->Rot = XMFLOAT3(0, 0, 45);//각도? 기준점이 이상한 것 같은데? 피봇때문에 이동하려고 길어지는 것 같음
	//keyList[3]->boneList[1]->Rot = XMFLOAT3(0, 0, 45);
	//keyList[4]->boneList[1]->Rot = XMFLOAT3(0, 0, 45);
	//keyList[5]->boneList[1]->Rot = XMFLOAT3(0, 0, 0);
	//keyList[6]->boneList[1]->Rot = XMFLOAT3(0, 0, 0);

	//keyList[0]->boneList[2]->Rot = XMFLOAT3(0, 0, 0);
	//keyList[1]->boneList[2]->Rot = XMFLOAT3(0, 30, 0);
	//keyList[2]->boneList[2]->Rot = XMFLOAT3(0, 30, 0);
	//keyList[3]->boneList[2]->Rot = XMFLOAT3(0, 30, 0);
	//keyList[4]->boneList[2]->Rot = XMFLOAT3(0, 30, 0);
	//keyList[5]->boneList[2]->Rot = XMFLOAT3(0, 30, 0);
	//keyList[6]->boneList[2]->Rot = XMFLOAT3(0, 0, 0);

	for (int i = 0; i < 7; ++i) {
		for (int j = 0; j < 3; ++j) {
			swap(keyList[i]->boneList[j]->Pos.y, keyList[i]->boneList[j]->Pos.z);
			swap(keyList[i]->boneList[j]->Rot.y, keyList[i]->boneList[j]->Rot.z);
		}
	}
}

Anim::~Anim()
{
}
