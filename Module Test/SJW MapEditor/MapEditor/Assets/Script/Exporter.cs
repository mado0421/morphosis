using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;
using System.IO;

public struct Block
{
    public Vector3 _pos;
    public Vector3 _extent;
}

public class Exporter : MonoBehaviour {
	// Use this for initialization
	void Start () {
        GameObject[] LevelData = GameObject.FindGameObjectsWithTag("Level");
        GameObject[] SpawnData = GameObject.FindGameObjectsWithTag("Spawn");
        GameObject[] TargetPlaceData = GameObject.FindGameObjectsWithTag("TargetPlace");
        Block[] LevelBlocks = new Block[LevelData.Length];
        Vector3[] SpawnPoints = new Vector3[SpawnData.Length];
        Block[] TargetBlocks = new Block[TargetPlaceData.Length];

        string str ="";

        /*지형 데이터 빼기*/
        for (int i = 0; i < LevelData.Length; ++i)
        {
            LevelBlocks[i]._pos= LevelData[i].transform.position;
            LevelBlocks[i]._extent= LevelData[i].transform.lossyScale;

            str = String.Format("LevelBlock[{0}]'s Pos {1}", i, LevelBlocks[i]._pos);
            print(str);
            str = String.Format("LevelBlock[{0}]'s Extent {1}", i, LevelBlocks[i]._extent);
            print(str);
        }

        /*스폰 데이터 빼기*/
        for (int i = 0; i < SpawnData.Length; ++i)
        {
            SpawnPoints[i] = SpawnData[i].transform.position;
            str = String.Format("SpawnPoints[{0}]'s Pos {1}", i, SpawnPoints[i]);
            print(str);
        }

        /*지형 데이터 빼기*/
        for (int i = 0; i < TargetPlaceData.Length; ++i)
        {
            TargetBlocks[i]._pos = TargetPlaceData[i].transform.position;
            TargetBlocks[i]._extent = TargetPlaceData[i].transform.lossyScale;
            str = String.Format("TargetBlocks[{0}]'s Pos {1}", i, TargetBlocks[i]._pos);
            print(str);
            str = String.Format("TargetBlocks[{0}]'s Extent {1}", i, TargetBlocks[i]._extent);
            print(str);
        }

        string strFilePath = Application.dataPath + "/Level_0.dat";
        print(strFilePath);
        FileStream fs = new FileStream(strFilePath, FileMode.Create);
        //StreamWriter sw = new StreamWriter(fs);
        BinaryWriter sw = new BinaryWriter(fs);
        
        sw.Write(LevelBlocks.Length);
        for (int i = 0; i < LevelBlocks.Length; ++i)
        {
            sw.Write(LevelBlocks[i]._pos.x);
            sw.Write(LevelBlocks[i]._pos.y);
            sw.Write(LevelBlocks[i]._pos.z);
            sw.Write(LevelBlocks[i]._extent.x);
            sw.Write(LevelBlocks[i]._extent.y);
            sw.Write(LevelBlocks[i]._extent.z);
        }

        sw.Write(SpawnPoints.Length);
        for (int i = 0; i < SpawnPoints.Length; ++i)
        {
            sw.Write(SpawnPoints[i].x);
            sw.Write(SpawnPoints[i].y);
            sw.Write(SpawnPoints[i].z);
        }

        sw.Write(TargetBlocks.Length);
        for (int i = 0; i < TargetBlocks.Length; ++i)
        {
            sw.Write(TargetBlocks[i]._pos.x);
            sw.Write(TargetBlocks[i]._pos.y);
            sw.Write(TargetBlocks[i]._pos.z);
            sw.Write(TargetBlocks[i]._extent.x);
            sw.Write(TargetBlocks[i]._extent.y);
            sw.Write(TargetBlocks[i]._extent.z);
        }
        sw.Close();
        fs.Close();
    }
	
	// Update is called once per frame
	void Update () {
		
	}
}
