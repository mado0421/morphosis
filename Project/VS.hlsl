#include "HLSLHeader.hlsli"

VS_ANIM_OUTPUT VSAnimModel(VS_ANIM_INPUT input) {
	VS_ANIM_OUTPUT output;

	float weights[4] = { input.weight[0], input.weight[1], input.weight[2], input.weight[3] };
	float3 posL = 0;
	float3 normalL = 0;
	float3 tangentL = 0;

	for (int i = 0; i < 4; ++i) {
		posL += weights[i] * mul(float4(input.position, 1.0f), AnimMatrix[input.boneIdx[i]]).xyz;
		normalL += weights[i] * mul(input.normal, (float3x3)AnimMatrix[input.boneIdx[i]]);
		tangentL += weights[i] * mul(input.tangent, (float3x3)AnimMatrix[input.boneIdx[i]]);
	}
	posL.x *= -1;
	normalL.x *= -1;
	tangentL.x *= -1;


	output.positionW = (float3)mul(float4(posL, 1.0f), gmtxGameObject);
	output.normalW = mul(normalL, (float3x3)gmtxGameObject);
	output.tangentW = mul(tangentL, (float3x3)gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return output;
}

VS_MODEL_OUTPUT VSModel(VS_MODEL_INPUT input) {
	VS_MODEL_OUTPUT output;

	output.normalW = normalize(mul(normalize(input.normal), (float3x3)gmtxGameObject));
	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return output;
}

VS_UI_OUTPUT VSFloatingUI(VS_UI_INPUT input, uint nVertexID : SV_VertexID) {
	VS_UI_OUTPUT output;

	if (0 == nVertexID % 6) {

	}
	else if (1 == nVertexID % 6) {
		input.position.x += gf2Size.x;
	}
	else if (2 == nVertexID % 6) {
		input.position.x += gf2Size.x;
		input.position.y -= gf2Size.y;
	}
	else if (3 == nVertexID % 6) {
	}
	else if (4 == nVertexID % 6) {
		input.position.x += gf2Size.x;
		input.position.y -= gf2Size.y;
	}
	else if (5 == nVertexID % 6) {
		input.position.y -= gf2Size.y;
	}

	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxUI);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);

	output.uv = input.uv;

	return output;
}

VS_UI_OUTPUT VSDefaultUI(VS_UI_INPUT input, uint nVertexID : SV_VertexID) {
	VS_UI_OUTPUT output;

	input.position.x += gmtxUI._m30 * 1920.0;
	input.position.y -= gmtxUI._m31 * 1080.0;

	if (0 == nVertexID % 6) {
		input.position.x -= gf2Size.x;
		input.position.y += gf2Size.y;
	}
	else if (1 == nVertexID % 6) {
		input.position.x += gf2Size.x;
		input.position.y += gf2Size.y;
	}
	else if (2 == nVertexID % 6) {
		input.position.x += gf2Size.x;
		input.position.y -= gf2Size.y;
	}
	else if (3 == nVertexID % 6) {
		input.position.x -= gf2Size.x;
		input.position.y += gf2Size.y;
	}
	else if (4 == nVertexID % 6) {
		input.position.x += gf2Size.x;
		input.position.y -= gf2Size.y;
	}
	else if (5 == nVertexID % 6) {
		input.position.x -= gf2Size.x;
		input.position.y -= gf2Size.y;
	}


	input.position.x = ((input.position.x * 2) - 1920) / 1920.0;
	input.position.y = ((input.position.y * 2) + 1080) / 1080.0;

	output.position = float4(input.position, 1.0f);
	output.positionW = float3(0, 0, 0);
	output.uv = input.uv;

	return output;
}