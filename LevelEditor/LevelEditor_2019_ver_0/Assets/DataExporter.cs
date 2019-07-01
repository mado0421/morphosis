using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;

public class DataExporter : MonoBehaviour
{
    public string   MapName             = "Default";
    public bool     IsCapturePointExist = false;

    private GameObject[]    CollisionMaps;
    private GameObject[]    Team1RespawnPoints;
    private GameObject[]    Team2RespawnPoints;
    private GameObject      CapturePoint;
    private GameObject      BoundaryBox;
    // Start is called before the first frame update
    void Start()
    {
        /*********************************************************************
        2019-07-01
        필요한 정보를 가진 태그는 CollisionMap, CapturePoint, 1TeamRespawnPoint, 2TeamRespawnPoint

        *********************************************************************/
        CollisionMaps = GameObject.FindGameObjectsWithTag("CollisionMap");
        foreach(GameObject c in CollisionMaps)
        {
            Debug.Log("위치 :" + c.transform.position + "\n크기 :" + c.transform.localScale + "\n회전 :" + c.transform.rotation);
        }
        Debug.Log("================================================================================");
        Team1RespawnPoints = GameObject.FindGameObjectsWithTag("1TeamRespawnPoint");
        foreach (GameObject c in Team1RespawnPoints)
        {
            Debug.Log("위치 :" + c.transform.position + "\n회전 :" + c.transform.rotation);
        }
        Debug.Log("================================================================================");
        Team2RespawnPoints = GameObject.FindGameObjectsWithTag("2TeamRespawnPoint");
        foreach (GameObject c in Team2RespawnPoints)
        {
            Debug.Log("위치 :" + c.transform.position + "\n회전 :" + c.transform.rotation);
        }
        Debug.Log("================================================================================");
        CapturePoint = GameObject.FindGameObjectWithTag("CapturePoint");
        Debug.Log("위치 :" + CapturePoint.transform.position + "\n크기 :" + CapturePoint.transform.localScale + "\n회전 :" + CapturePoint.transform.rotation);
        BoundaryBox = GameObject.FindGameObjectWithTag("BoundaryBox");
        Debug.Log("위치 :" + BoundaryBox.transform.position + "\n크기 :" + BoundaryBox.transform.localScale + "\n회전 :" + BoundaryBox.transform.rotation);

        /*********************************************************************
        2019-07-01
        양식은 다음과 같다.

        레벨 이름
        충돌맵 개수
        팀1 스폰 포인트 개수
        팀2 스폰 포인트 개수(둘이 개수가 다를 수도 있고 이거 하나 더 쓴다고 크게 차이도 안 나니까)
        캡쳐포인트 여부 (true/false)
        바운더리박스 위치
        바운더리박스 크기
        바운더리박스 회전
        충돌맵 위치
        충돌맵 크기
        충돌맵 회전
        팀1스폰포인트 위치
        팀1스폰포인트 회전
        팀2스폰포인트 위치
        팀2스폰포인트 회전
        if(만약 캡처포인트 여부가 true면){
            캡쳐포인트 위치
            캡쳐포인트 크기
            캡쳐포인트 회전
        }
        *********************************************************************/

        string path = "Assets/LevelData_";
        path += MapName;
        path += ".dat";
        StreamWriter w = new StreamWriter(path, false);

        w.WriteLine(MapName);

        w.WriteLine(CollisionMaps.Length);
        w.WriteLine(Team1RespawnPoints.Length);
        w.WriteLine(Team2RespawnPoints.Length);
        w.WriteLine(IsCapturePointExist);

        w.WriteLine((float)BoundaryBox.transform.position.x);
        w.WriteLine((float)BoundaryBox.transform.position.y);
        w.WriteLine((float)BoundaryBox.transform.position.z);
        w.WriteLine((float)BoundaryBox.transform.localScale.x);
        w.WriteLine((float)BoundaryBox.transform.localScale.y);
        w.WriteLine((float)BoundaryBox.transform.localScale.z);
        w.WriteLine((float)BoundaryBox.transform.rotation.x);
        w.WriteLine((float)BoundaryBox.transform.rotation.y);
        w.WriteLine((float)BoundaryBox.transform.rotation.z);
        w.WriteLine((float)BoundaryBox.transform.rotation.w);

        foreach (GameObject c in CollisionMaps)
        {
            w.WriteLine((float)c.transform.position.x);
            w.WriteLine((float)c.transform.position.y);
            w.WriteLine((float)c.transform.position.z);
            w.WriteLine((float)c.transform.localScale.x);
            w.WriteLine((float)c.transform.localScale.y);
            w.WriteLine((float)c.transform.localScale.z);
            w.WriteLine((float)c.transform.rotation.x);
            w.WriteLine((float)c.transform.rotation.y);
            w.WriteLine((float)c.transform.rotation.z);
            w.WriteLine((float)c.transform.rotation.w);
        }
        foreach (GameObject c in Team1RespawnPoints)
        {
            w.WriteLine((float)c.transform.position.x);
            w.WriteLine((float)c.transform.position.y);
            w.WriteLine((float)c.transform.position.z);
            w.WriteLine((float)c.transform.rotation.x);
            w.WriteLine((float)c.transform.rotation.y);
            w.WriteLine((float)c.transform.rotation.z);
            w.WriteLine((float)c.transform.rotation.w);
        }
        foreach (GameObject c in Team2RespawnPoints)
        {
            w.WriteLine((float)c.transform.position.x);
            w.WriteLine((float)c.transform.position.y);
            w.WriteLine((float)c.transform.position.z);
            w.WriteLine((float)c.transform.rotation.x);
            w.WriteLine((float)c.transform.rotation.y);
            w.WriteLine((float)c.transform.rotation.z);
            w.WriteLine((float)c.transform.rotation.w);
        }
        if (IsCapturePointExist)
        {
            w.WriteLine((float)CapturePoint.transform.position.x);
            w.WriteLine((float)CapturePoint.transform.position.y);
            w.WriteLine((float)CapturePoint.transform.position.z);
            w.WriteLine((float)CapturePoint.transform.localScale.x);
            w.WriteLine((float)CapturePoint.transform.localScale.y);
            w.WriteLine((float)CapturePoint.transform.localScale.z);
            w.WriteLine((float)CapturePoint.transform.rotation.x);
            w.WriteLine((float)CapturePoint.transform.rotation.y);
            w.WriteLine((float)CapturePoint.transform.rotation.z);
            w.WriteLine((float)CapturePoint.transform.rotation.w);
        }               
                        
        w.Close();      
                        
    }
}
