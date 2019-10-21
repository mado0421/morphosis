#include "Header.hlsli"
#include "Light.hlsli"

float4 PSAnimModelShader(VS_ANIM_OUTPUT input) : SV_TARGET {
	input.normalW = normalize(input.normalW);

	//float4 cColor = gtxtTexture[0].Sample(gSamplerState, input.uv);
	float4 cColor = gtxtDiffuseTexture.Sample(gSamplerState, input.uv);
	float4 cLightResult = TestLighting(input.positionW, input.normalW);

	float3 toEyeW = gvCameraPosition - input.positionW;
	float distToEye = length(toEyeW);

	cColor = lerp(cColor, cLightResult, 0.5f);
	cColor = lerp(cColor, float4(0.1, 0.1, 0.1, 1.0), distToEye / 512);

	return cColor;
}

float4 PSModelShader(VS_MODEL_OUTPUT input) : SV_TARGET {
	float4 cAlbedo = gtxtDiffuseTexture.Sample(gSamplerState, input.uv);

	float3 toEyeW = normalize( gvCameraPosition - input.positionW );
	float4 ambient = float4(0.8, 0.8, 0.8, 1);

	float4 normalMapSample = gtxtNormalTexture.Sample(gSamplerState, input.uv);
	float3 bumpedNomalW = NormalSampleToWorldSpace(normalMapSample.rgb, input.normalW, input.tangentW);
	float4 cLightResult = TestLighting(input.positionW, bumpedNomalW);

	return (cAlbedo * ambient) + cLightResult;
}

float4 PSDefaultUI(VS_UI_OUTPUT input) : SV_TARGET {
	float4 cColor = gtxtDiffuseTexture.Sample(gSamplerState, input.uv);
	if (cColor.a < 0.3) discard;
	return cColor;
}

float4 PSDebug(VS_DEBUG_OUTPUT input) : SV_TARGET {
	return float4(1.0f, 0.0f, 0.0f, 1.0f);
}

float4 PSTestShader(VS_MODEL_OUTPUT input) : SV_TARGET{
	float dist = distance(gvCameraPosition, input.positionW);
	float normalizedDist = clamp(dist, 0, 5000)/5000.0f;
	return float4(normalizedDist, normalizedDist, normalizedDist,1);
}