#include "Header.hlsli"
#include "Light.hlsli"

float4 PSAnimModelShader(VS_ANIM_OUTPUT input) : SV_TARGET {
	//float3 N = input.normalW;
	//float3 T = normalize(input.tangentW - dot(input.tangentW, N) * N);
	//float3 B = cross(N, T);
	//float3x3 TBN = float3x3(T, B, N);


	//float3 normal		= mul(input.normalW, TBN);
	////float4 cColor		= gtxtTexture.Sample(gSamplerState, input.uv);
	////float4 cLightResult = TestLighting(input.positionW, normal);
	//////return (lerp(cColor, cLightResult, 0.5f));

	////return cLightResult;

	input.normalW = normalize(input.normalW);

	float4 cColor = gtxtTexture[0].Sample(gSamplerState, input.uv);
	float4 cLightResult = TestLighting(input.positionW, input.normalW);

	float3 toEyeW = gvCameraPosition - input.positionW;
	float distToEye = length(toEyeW);

	cColor = lerp(cColor, cLightResult, 0.5f);
	cColor = lerp(cColor, float4(0.1, 0.1, 0.1, 1.0), distToEye / 512);
	//return  float4(1, 1, 1, 1.0);

	return cColor;
	//return (lerp(cColor, cLightResult, 0.5f));
}

float4 PSModelShader(VS_MODEL_OUTPUT input) : SV_TARGET {
	//return  float4(1, 1, 1, 1.0);
	float4 cAlbedo = gtxtTexture[0].Sample(gSamplerState, input.uv);

	float3 toEyeW = normalize( gvCameraPosition - input.positionW );
	float4 ambient = float4(0.8, 0.8, 0.8, 1);
	//float distToEye = length(toEyeW);

	//cColor = lerp(cColor, cLightResult, 0.5f);
	//cColor = lerp(cColor, float4(0.1, 0.1, 0.1, 1.0), distToEye / 512);

	float4 normalMapSample = gtxtTexture[1].Sample(gSamplerState, input.uv);
	float3 bumpedNomalW = NormalSampleToWorldSpace(normalMapSample.rgb, input.normalW, input.tangentW);
	float4 cLightResult = TestLighting(input.positionW, bumpedNomalW);









	return (cAlbedo * ambient) + cLightResult;

	//return (lerp(cColor, cLightResult, 0.5f));
}

float4 PSDefaultUI(VS_UI_OUTPUT input) : SV_TARGET {
	float4 cColor = gtxtTexture[0].Sample(gSamplerState, input.uv);
	//cColor.a *= 1.0f;
	if (cColor.a < 0.3) discard;
	return cColor;
}

float4 PSDebug(VS_DEBUG_OUTPUT input) : SV_TARGET {
	return float4(1.0f, 0.0f, 0.0f, 1.0f);
}
