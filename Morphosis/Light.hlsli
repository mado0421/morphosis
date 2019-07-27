////--------------------------------------------------------------------------------------
//#define MAX_LIGHTS			8 
//#define MAX_MATERIALS		64 
//
//#define POINT_LIGHT			1
//#define SPOT_LIGHT			2
//#define DIRECTIONAL_LIGHT	3
//
//#define _WITH_LOCAL_VIEWER_HIGHLIGHTING
//#define _WITH_THETA_PHI_CONES
////#define _WITH_REFLECT

//struct MATERIAL
//{
//	float4				m_cAmbient;
//	float4				m_cDiffuse;
//	float4				m_cSpecular; //a = power
//	float4				m_cEmissive;
//};
//
//struct LIGHT
//{
//	float4				m_cAmbient;
//	float4				m_cDiffuse;
//	float4				m_cSpecular;
//	float3				m_vPosition;
//	float 				m_fFalloff;
//	float3				m_vDirection;
//	float 				m_fTheta; //cos(m_fTheta)
//	float3				m_vAttenuation;
//	float				m_fPhi; //cos(m_fPhi)
//	bool				m_bEnable;
//	int 				m_nType;
//	float				m_fRange;
//	float				padding;
//};
//
//
//
//cbuffer cbMaterial : register(b2)
//{
//	MATERIAL			gMaterials[MAX_MATERIALS];
//};
//
//cbuffer cbLights : register(b3)
//{
//	LIGHT				gLights[MAX_LIGHTS];
//	float4				gcGlobalAmbientLight;
//};
//
//float4 DirectionalLight(int nIndex, float3 vNormal, float3 vToCamera)
//{
//	float3 vToLight = -gLights[nIndex].m_vDirection;
//	float fDiffuseFactor = dot(vToLight, vNormal);
//	float fSpecularFactor = 0.0f;
//	if (fDiffuseFactor > 0.0f)
//	{
//		if (gMaterials[gnMaterial].m_cSpecular.a != 0.0f)
//		{
//#ifdef _WITH_REFLECT
//			float3 vReflect = reflect(-vToLight, vNormal);
//			fSpecularFactor = pow(max(dot(vReflect, vToCamera), 0.0f), gMaterials[gnMaterial].m_cSpecular.a);
//#else
//#ifdef _WITH_LOCAL_VIEWER_HIGHLIGHTING
//			float3 vHalf = normalize(vToCamera + vToLight);
//#else
//			float3 vHalf = float3(0.0f, 1.0f, 0.0f);
//#endif
//			fSpecularFactor = pow(max(dot(vHalf, vNormal), 0.0f), gMaterials[gnMaterial].m_cSpecular.a);
//#endif
//		}
//	}
//
//	return(
//		(gLights[nIndex].m_cAmbient * gMaterials[gnMaterial].m_cAmbient) 
//		+ (gLights[nIndex].m_cDiffuse * fDiffuseFactor * gMaterials[gnMaterial].m_cDiffuse) 
//		+ (gLights[nIndex].m_cSpecular * fSpecularFactor * gMaterials[gnMaterial].m_cSpecular)
//		);
//}



float4 TestDirectionalLight(float3 vNormal, float3 vToCamera) {
	/*********************************************************************
	2019-07-20
	*********************************************************************/
	float4 fLightDiffuse	= float4(0.5f, 0.4f, 0.4f, 1.0f);
	float4 fLightSpecular	= float4(1.0f, 0.9f, 0.9f, 1.0f);
	float3 vLightDirection	= normalize(float3(0.7f, -1.0f, 0.0f));
	// 빛의 방향
	float fSpecular = 16.0f;

	// 빛의 방향의 반대
	float3 vToLight = -vLightDirection;
	float fDiffuseFactor = dot(vToLight, vNormal);
	float fSpecularFactor = 0.0f;

	if (fDiffuseFactor > 0.0f){
		float3 vReflect = reflect(-vToLight, vNormal);
		fSpecularFactor = pow(max(dot(vReflect, vToCamera), 0.0f), fSpecular);
	}

	return (
		(fLightDiffuse * fDiffuseFactor) +
		(fLightSpecular * fSpecularFactor)
		);
}

float4 TestLighting(float3 vPosition, float3 vNormal) {
	/*********************************************************************
	2019-07-20
	*********************************************************************/
	float4 cLightGlobalAmbient = float4(0.0f, 0.0f, 0.0f, 1.0f);

	float3 vCameraPosition = float3(gvCameraPosition.x, gvCameraPosition.y, gvCameraPosition.z);
	float3 vToCamera = normalize(vCameraPosition - vPosition);

	float4 cColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	cColor += TestDirectionalLight(vNormal, vToCamera);
	cColor += cLightGlobalAmbient;

	return cColor;
}







//
//float4 PointLight(int nIndex, float3 vPosition, float3 vNormal, float3 vToCamera)
//{
//	float3 vToLight = gLights[nIndex].m_vPosition - vPosition;
//	float fDistance = length(vToLight);
//	if (fDistance <= gLights[nIndex].m_fRange)
//	{
//		float fSpecularFactor = 0.0f;
//		vToLight /= fDistance;
//		float fDiffuseFactor = dot(vToLight, vNormal);
//		if (fDiffuseFactor > 0.0f)
//		{
//			if (gMaterials[gnMaterial].m_cSpecular.a != 0.0f)
//			{
//#ifdef _WITH_REFLECT
//				float3 vReflect = reflect(-vToLight, vNormal);
//				fSpecularFactor = pow(max(dot(vReflect, vToCamera), 0.0f), gMaterials[gnMaterial].m_cSpecular.a);
//#else
//#ifdef _WITH_LOCAL_VIEWER_HIGHLIGHTING
//				float3 vHalf = normalize(vToCamera + vToLight);
//#else
//				float3 vHalf = float3(0.0f, 1.0f, 0.0f);
//#endif
//				fSpecularFactor = pow(max(dot(vHalf, vNormal), 0.0f), gMaterials[gnMaterial].m_cSpecular.a);
//#endif
//			}
//		}
//		float fAttenuationFactor = 1.0f / dot(gLights[nIndex].m_vAttenuation, float3(1.0f, fDistance, fDistance*fDistance));
//
//		return(((gLights[nIndex].m_cAmbient * gMaterials[gnMaterial].m_cAmbient) + (gLights[nIndex].m_cDiffuse * fDiffuseFactor * gMaterials[gnMaterial].m_cDiffuse) + (gLights[nIndex].m_cSpecular * fSpecularFactor * gMaterials[gnMaterial].m_cSpecular)) * fAttenuationFactor);
//	}
//	return(float4(0.0f, 0.0f, 0.0f, 0.0f));
//}
//
//float4 SpotLight(int nIndex, float3 vPosition, float3 vNormal, float3 vToCamera)
//{
//	float3 vToLight = gLights[nIndex].m_vPosition - vPosition;
//	float fDistance = length(vToLight);
//	if (fDistance <= gLights[nIndex].m_fRange)
//	{
//		float fSpecularFactor = 0.0f;
//		vToLight /= fDistance;
//		float fDiffuseFactor = dot(vToLight, vNormal);
//		if (fDiffuseFactor > 0.0f)
//		{
//			if (gMaterials[gnMaterial].m_cSpecular.a != 0.0f)
//			{
//#ifdef _WITH_REFLECT
//				float3 vReflect = reflect(-vToLight, vNormal);
//				fSpecularFactor = pow(max(dot(vReflect, vToCamera), 0.0f), gMaterials[gnMaterial].m_cSpecular.a);
//#else
//#ifdef _WITH_LOCAL_VIEWER_HIGHLIGHTING
//				float3 vHalf = normalize(vToCamera + vToLight);
//#else
//				float3 vHalf = float3(0.0f, 1.0f, 0.0f);
//#endif
//				fSpecularFactor = pow(max(dot(vHalf, vNormal), 0.0f), gMaterials[gnMaterial].m_cSpecular.a);
//#endif
//			}
//		}
//#ifdef _WITH_THETA_PHI_CONES
//		float fAlpha = max(dot(-vToLight, gLights[nIndex].m_vDirection), 0.0f);
//		float fSpotFactor = pow(max(((fAlpha - gLights[nIndex].m_fPhi) / (gLights[nIndex].m_fTheta - gLights[nIndex].m_fPhi)), 0.0f), gLights[nIndex].m_fFalloff);
//#else
//		float fSpotFactor = pow(max(dot(-vToLight, gLights[i].m_vDirection), 0.0f), gLights[i].m_fFalloff);
//#endif
//		float fAttenuationFactor = 1.0f / dot(gLights[nIndex].m_vAttenuation, float3(1.0f, fDistance, fDistance*fDistance));
//
//		return(((gLights[nIndex].m_cAmbient * gMaterials[gnMaterial].m_cAmbient) + (gLights[nIndex].m_cDiffuse * fDiffuseFactor * gMaterials[gnMaterial].m_cDiffuse) + (gLights[nIndex].m_cSpecular * fSpecularFactor * gMaterials[gnMaterial].m_cSpecular)) * fAttenuationFactor * fSpotFactor);
//	}
//	return(float4(0.0f, 0.0f, 0.0f, 0.0f));
//}
//
//bool Test()
//{
//	//if (gLights[0].m_cAmbient.x != 0.1f) return false;
//	//if (gLights[0].m_cDiffuse.x != 0.8f) return false;
//	//if (gLights[0].m_cSpecular.x != 0.1f) return false;
//	//if (gLights[0].m_vPosition.x != 130.0f) return false;
//	//if (gLights[0].m_vDirection.x != 0.0f) return false;
//	if (gvCameraPosition.x == 0.0f && gvCameraPosition.y == 40.0f && gvCameraPosition.z == -120.0f) return false;
//	return true;
//}
//
//bool Test2(float a, float b, float range) {
//	return (a - b) * (a - b) <= (range * range);
//}
//
//float4 Lighting(float3 vPosition, float3 vNormal)
//{
//	float3 vCameraPosition = float3(gvCameraPosition.x, gvCameraPosition.y, gvCameraPosition.z);
//	float3 vToCamera = normalize(vCameraPosition - vPosition);
//
//	float4 cColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
//	//	if(Test2(vNormal.y, 1.0f, 0.3f)) cColor = float4(1.0f, 0.0f, 0.0f, 1.0f);
//	for (int i = 0; i < MAX_LIGHTS; i++)
//	{
//		if (gLights[i].m_bEnable)
//		{
//			if (gLights[i].m_nType == DIRECTIONAL_LIGHT)
//			{
//				cColor += DirectionalLight(i, vNormal, vToCamera);
//			}
//			else if (gLights[i].m_nType == POINT_LIGHT)
//			{
//				cColor += PointLight(i, vPosition, vNormal, vToCamera);
//			}
//			else if (gLights[i].m_nType == SPOT_LIGHT)
//			{
//				cColor += SpotLight(i, vPosition, vNormal, vToCamera);
//			}
//		}
//	}
//	cColor += (gcGlobalAmbientLight * gMaterials[gnMaterial].m_cAmbient);
//	cColor.a = gMaterials[gnMaterial].m_cDiffuse.a;
//
//	return(cColor);
//}