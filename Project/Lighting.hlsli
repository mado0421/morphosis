float4 TestDirectionalLight(float3 vNormal, float3 vToCamera) {
	float4 fLightDiffuse = float4(0.9f, 0.8f, 0.8f, 1.0f);
	float4 fLightSpecular = float4(0.5f, 0.9f, 0.5f, 1.0f);
	float3 vLightDirection = normalize(float3(0.7f, -1.0f, 0.0f));
	// 빛의 방향
	float fSpecular = 16.0f;

	// 빛의 방향의 반대
	float3 vToLight = -vLightDirection;
	float fDiffuseFactor = dot(vToLight, vNormal);
	float fSpecularFactor = 0.0f;

	if (fDiffuseFactor > 0.0f) {
		float3 vReflect = reflect(-vToLight, vNormal);
		fSpecularFactor = pow(max(dot(vReflect, vToCamera), 0.0f), fSpecular);
	}

	return (
		(fLightDiffuse * fDiffuseFactor) +
		(fLightSpecular * fSpecularFactor)
		);
}

float4 TestLighting(float3 vPosition, float3 vNormal) {
	float4 cLightGlobalAmbient = float4(0.0f, 0.0f, 0.0f, 1.0f);

	float3 vCameraPosition = float3(gvCameraPosition.x, gvCameraPosition.y, gvCameraPosition.z);
	float3 vToCamera = normalize(vCameraPosition - vPosition);

	float4 cColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	cColor += TestDirectionalLight(vNormal, vToCamera);
	cColor += cLightGlobalAmbient;

	return cColor;
}