#include "common.h"

bool LoadScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename)
{
	int lFileMajor, lFileMinor, lFileRevision;
	int lSDKMajor, lSDKMinor, lSDKRevision;
	//int lFileFormat = -1;
	int i, lAnimStackCount;
	bool lStatus;
	char lPassword[1024];

	// Get the file version number generate by the FBX SDK.
	FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

	// Create an importer.
	FbxImporter* lImporter = FbxImporter::Create(pManager, "");

	// Initialize the importer by providing a filename.
	const bool lImportStatus = lImporter->Initialize(pFilename, -1, pManager->GetIOSettings());
	lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

	if (!lImportStatus)
	{
		FbxString error = lImporter->GetStatus().GetErrorString();
		FBXSDK_printf("Call to FbxImporter::Initialize() failed.\n");
		FBXSDK_printf("Error returned: %s\n\n", error.Buffer());

		if (lImporter->GetStatus().GetCode() == FbxStatus::eInvalidFileVersion)
		{
			FBXSDK_printf("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);
			FBXSDK_printf("FBX file format version for file '%s' is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);
		}

		return false;
	}

	FBXSDK_printf("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);

	if (lImporter->IsFBX())
	{
		FBXSDK_printf("FBX file format version for file '%s' is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);

		// From this point, it is possible to access animation stack information without
		// the expense of loading the entire file.

		FBXSDK_printf("Animation Stack Information\n");

		lAnimStackCount = lImporter->GetAnimStackCount();

		FBXSDK_printf("    Number of Animation Stacks: %d\n", lAnimStackCount);
		FBXSDK_printf("    Current Animation Stack: \"%s\"\n", lImporter->GetActiveAnimStackName().Buffer());
		FBXSDK_printf("\n");

		for (i = 0; i < lAnimStackCount; i++)
		{
			FbxTakeInfo* lTakeInfo = lImporter->GetTakeInfo(i);

			FBXSDK_printf("    Animation Stack %d\n", i);
			FBXSDK_printf("         Name: \"%s\"\n", lTakeInfo->mName.Buffer());
			FBXSDK_printf("         Description: \"%s\"\n", lTakeInfo->mDescription.Buffer());

			// Change the value of the import name if the animation stack should be imported 
			// under a different name.
			FBXSDK_printf("         Import Name: \"%s\"\n", lTakeInfo->mImportName.Buffer());

			// Set the value of the import state to false if the animation stack should be not
			// be imported. 
			FBXSDK_printf("         Import State: %s\n", lTakeInfo->mSelect ? "true" : "false");
			FBXSDK_printf("\n");
		}

		// Set the import states. By default, the import states are always set to 
		// true. The code below shows how to change these states.
		IOS_REF.SetBoolProp(IMP_FBX_MATERIAL, true);
		IOS_REF.SetBoolProp(IMP_FBX_TEXTURE, true);
		IOS_REF.SetBoolProp(IMP_FBX_LINK, true);
		IOS_REF.SetBoolProp(IMP_FBX_SHAPE, true);
		IOS_REF.SetBoolProp(IMP_FBX_GOBO, true);
		IOS_REF.SetBoolProp(IMP_FBX_ANIMATION, true);
		IOS_REF.SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
	}

	// Import the scene.
	lStatus = lImporter->Import(pScene);

	if (lStatus == false && lImporter->GetStatus().GetCode() == FbxStatus::ePasswordError)
	{
		FBXSDK_printf("Please enter password: ");

		lPassword[0] = '\0';

		FBXSDK_CRT_SECURE_NO_WARNING_BEGIN
			scanf("%s", lPassword);
		FBXSDK_CRT_SECURE_NO_WARNING_END

			FbxString lString(lPassword);

		IOS_REF.SetStringProp(IMP_FBX_PASSWORD, lString);
		IOS_REF.SetBoolProp(IMP_FBX_PASSWORD_ENABLE, true);

		lStatus = lImporter->Import(pScene);

		if (lStatus == false && lImporter->GetStatus().GetCode() == FbxStatus::ePasswordError)
		{
			FBXSDK_printf("\nPassword is wrong, import aborted.\n");
		}
	}

	// Destroy the importer.
	lImporter->Destroy();

	return lStatus;
}

void DestroySdkObjects(FbxManager* pManager, bool pExitStatus)
{
	//Delete the FBX Manager. All the objects that have been allocated using the FBX Manager and that haven't been explicitly destroyed are also automatically destroyed.
	if (pManager) pManager->Destroy();
	if (pExitStatus) FBXSDK_printf("Program Success!\n");
}

class FBXExporter {


private:
	int GetBoneIdxByName(const char* name) {
		for (auto p = bones.begin(); p != bones.end(); ++p) {
			/* stringÀÌ¶û const char*¶û ¹Ù·Î ºñ±³°¡ µÊ?? */
			if (name == p->name) return std::distance(bones.begin(), p);
		}
	}
	int GetMeshIdxByName(const char* name) {
		for (auto p = m_vecMesh.begin(); p != m_vecMesh.end(); ++p) {
			/* stringÀÌ¶û const char*¶û ¹Ù·Î ºñ±³°¡ µÊ?? */
			if (name == p->m_strName) return distance(m_vecMesh.begin(), p);
		}
	}

private:
	void MakeObjectData() {
		MakeBoneData();
		MakeMeshData();
		MakeAnimationData();
	}
	void MakeBoneData() {
		for (int i = 0; i < m_pScene->GetSrcObjectCount<FbxAnimStack>(); ++i) {
			FbxAnimStack* pAnimStack = m_pScene->GetSrcObject<FbxAnimStack>(i);
			int nAnimLayers = pAnimStack->GetMemberCount<FbxAnimLayer>();
			for (int j = 0; j < nAnimLayers; ++j) {
				FbxAnimLayer* pAnimLayer = pAnimStack->GetMember<FbxAnimLayer>(j);
				GetBoneDataRec(pAnimLayer, m_pScene->GetRootNode());
			}
		}
		ConnectBoneParent();
	}
	void MakeMeshData() {
		GetMeshDataRec(m_pScene->GetRootNode());
	}
	void MakeAnimationData() {
		for (int i = 0; i < m_pScene->GetSrcObjectCount<FbxAnimStack>(); i++)
		{
			FbxAnimStack* lAnimStack = m_pScene->GetSrcObject<FbxAnimStack>(i);
			int l;
			int nbAnimLayers = lAnimStack->GetMemberCount<FbxAnimLayer>();
			for (l = 0; l < nbAnimLayers; l++)
			{
				FbxAnimLayer* lAnimLayer = lAnimStack->GetMember<FbxAnimLayer>(l);
				GetAnimationDataRec(lAnimLayer, m_pScene->GetRootNode());
			}
		}
	}

private:
	void GetBoneDataRec(FbxAnimLayer* pAnimLayer, FbxNode* pNode) {
		int childCount;
		XMFLOAT3 t, r;

		FbxDouble3 lTranslation = pNode->LclTranslation.Get();
		FbxDouble3 lRotation = pNode->LclRotation.Get();

		t = XMFLOAT3(lTranslation[0], lTranslation[1], lTranslation[2]);
		r = XMFLOAT3(lRotation[0], lRotation[1], lRotation[2]);

		m_vecBone.emplace_back(pNode->GetName(), m_vecBone.size(), t, r, pNode);

		for (childCount = 0; childCount < pNode->GetChildCount(); ++childCount) {
			GetBoneDataRec(pAnimLayer, pNode->GetChild(childCount));
		}
	}
	void ConnectBoneParent() {
		for (auto p = m_vecBone.begin(); p != m_vecBone.end(); ++p) {
			for (auto pp = m_vecBone.begin(); pp != m_vecBone.end(); ++pp) {
				if (p->m_pNode->GetName() == pp->m_strName) {
					p->m_pParent = &(*pp);
					break;
				}
			}
		}
	}

	void GetMeshDataRec(FbxNode *pNode) {

		FbxMesh* pMesh = pNode->GetMesh();
		if (NULL != pMesh) {
			int nControlPoint = pMesh->GetControlPointsCount();
			FbxVector4 *pControlPoint = pMesh->GetControlPoints();

			m_vecMesh.emplace_back(pNode->GetName());
			int meshIdx = m_vecMesh.size();

			//make control point data
			// each CP
			// \ Position
			// \ boneIdx
			// \ boneWeight
			// 
			for (int i = 0; i < nControlPoint; ++i) {
				FbxVector4 fbxVec4Tmp = pControlPoint[i];
				XMFLOAT3 xmf3Tmp(fbxVec4Tmp.mData[0], fbxVec4Tmp.mData[1], fbxVec4Tmp.mData[2]);
				m_vecMesh[meshIdx].m_vecControlPoint.emplace_back(xmf3Tmp);
			}

			//make boneIdx and boneWeight
			FbxGeometry* pGeometry = pNode->GetGeometry();
			if (NULL != pGeometry) {
				int nSkinDeformer = pGeometry->GetDeformerCount(FbxDeformer::eSkin);
				for (int i = 0; i < nSkinDeformer; ++i) {
					FbxSkin* pSkinDeformer = (FbxSkin*)(pGeometry->GetDeformer(i, FbxDeformer::eSkin));
					int nCluster = pSkinDeformer->GetClusterCount();

					for (int j = 0; j < nCluster; ++j) {
						FbxCluster* pCluster = pSkinDeformer->GetCluster(j);
						FbxDouble* pWeight = pCluster->GetControlPointWeights();

					}
				}
			}




			//make polygon vertex index 
			int nPolygonVertexIdx = pMesh->GetPolygonCount();
			for (int i = 0; i < nPolygonVertexIdx; ++i) {
				int nPolygonSize = pMesh->GetPolygonSize(i);
				for (int j = 0; j < nPolygonSize; ++j) {
					int nControlPointIdx = pMesh->GetPolygonVertex(i, j);
					m_vecMesh[meshIdx].m_vecPolygonVertexIndex.push_back(nControlPointIdx);
				}
			}

			


		}
	}



private:
	FbxScene*		m_pScene = NULL;
	vector<MyMesh>	m_vecMesh;
	vector<MyBone>	m_vecBone;
	vector<MyKey>	m_vecKey;
};








/*
	¾Ö´Ï¸ÞÀÌ¼Ç ÆÄÀÏÀÌ¶û ¸Þ½¬ ÆÄÀÏÀ» µû·Î »©±â?
	µû·Î »©ÀÚ.
*/

void main() {

	FbxManager	*lSdkManager	= NULL;
	FbxScene	*lScene			= NULL;
	FbxString	lFileName("name");
	bool		lResult;

	/*
	Prepare FBX SDK
	*/
	//The first thing to do is to create the FBX Manager which is the object allocator for almost all the classes in the SDK
	lSdkManager = FbxManager::Create();
	if (!lSdkManager)
	{
		FBXSDK_printf("Error: Unable to create FBX Manager!\n");
		exit(1);
	}
	else FBXSDK_printf("Autodesk FBX SDK version %s\n", lSdkManager->GetVersion());

	//Create an IOSettings object. This object holds all import/export settings.
	FbxIOSettings* ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
	lSdkManager->SetIOSettings(ios);

	//Load plugins from the executable directory (optional)
	FbxString lPath = FbxGetApplicationDirectory();
	lSdkManager->LoadPluginsDirectory(lPath.Buffer());

	//Create an FBX scene. This object holds most objects imported/exported from/to files.
	lScene = FbxScene::Create(lSdkManager, "My Scene");
	if (!lScene)
	{
		FBXSDK_printf("Error: Unable to create FBX scene!\n");
		exit(1);
	}

	// The example can take a FBX file as an argument.
	if (lFileName.IsEmpty())
	{
		lResult = false;
		FBXSDK_printf("\n\nUsage: ImportScene <FBX file name>\n\n");
	}
	else
	{
		FBXSDK_printf("\n\nFile: %s\n\n", lFileName.Buffer());
		FbxString lFilePath(lFileName + ".fbx");
		lResult = LoadScene(lSdkManager, lScene, lFilePath.Buffer());
	}




	if (lResult == false)
	{
		FBXSDK_printf("\n\nAn error occurred while loading the scene...");
	}
	else
	{
		dataManager.Init(lScene);
		dataManager.ExportFile(lFileName);
	}















	// Destroy all objects created by the FBX SDK.
	DestroySdkObjects(lSdkManager, lResult);

}
