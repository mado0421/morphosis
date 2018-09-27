#include "Common.hlsl"
#include "Shadow.hlsl"

Texture2D gtxtModelTexture[10] : register(t2);
Texture2D gtxtModelNormalMap[10] : register(t12);
Texture2D gtxtSpecularMap[10] : register(t22);
Texture2D gtxtGlossinessMap[10] : register(t32);
Texture2D gtxtEmissiveMap[10] : register(t42);

struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 uv : TEXCOORD;
    uint material : MATERIAL;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float3 positionW : POSITION0;
    float4 ShadowPos : POSITION1;
    float3 normalW : NORMAL;
    float3 tangentW : TANGENT;
    float2 uv : TEXCOORD;
    uint material : MATERIAL;
};

VS_OUTPUT VS_MAIN(VS_INPUT input)
{
    VS_OUTPUT output;

    output.positionW = mul(float4(input.position, 1.0f), gmtxGameObject).xyz;
    output.ShadowPos = mul(float4(output.positionW, 1.0f), gmtxVPT);
    output.position = mul(float4(output.positionW, 1.0f), gmtxViewProjection);
    output.normalW = mul(input.normal, (float3x3) gmtxGameObject);
    output.tangentW = mul(input.tangent, (float3x3) gmtxGameObject);
    output.uv = input.uv;
    output.material = input.material;

    return (output);
}

float3 Circuit(float2 uv, float timer, float distance_delta, float color_distance_delta, bool isCircle)
{
    // ������ ȿ���� ���� ����
    float flicker = noise1D(timer * 2.0) * 0.8;

    // �Էµ� UV�� ����� �߰��� �� ���� ����������.

    uv *= (isCircle) ? 0.01 * (distance_delta ) : 2;

    float v = 0.0;
	
	// ȸ�θ��� ��⸦ �����Ѵ�.
    float a = 0.6;
    // ���� ���������� �������� �����Ѵ�. 
    //( ��ġ �̵�, ȸ�θ��� ���� ��ġ��, ���� �����Ϸ� �׷����� �ʰ� )
    float f = 3.0;
	
    // ������ ���������� �� ������ ������ ���������.
    // �߰��� ���� �������� ����
    int loop_factor = (isCircle) ? 5 : 2;


    for (int i = 0; i < loop_factor; i++) 
    {
        float v1 = voronoi( uv * f);
        float v2 = 0.0;
		
		// �߰� �������� ȸ�θ��� ���� �����̴� ����ȿ�� ����� 
        if (i > 0)
        {
			// ���γ��� �������� ������� ȸ�θ� ����, 
            //( ���ϸ� �Ͽ���, ��� ��Ӱ� �������� �����Ѵ� ���� �ȴ�)
            v2 = voronoi( uv * f * 0.5 + timer);
			
            float va = 0.0;
            float vb = 0.0;

            va = 1.0 - smoothstep(0.0, 0.1, v1);
            // �ִ� ���� Ŀ������ ���� �̵� ���� �β�����
            vb = 1.0 - smoothstep(0.0, 0.08, v2);
            // ���ڰ� �帣�� �κп� ������ �����Ѵ�.
            v += a * pow(va * (0.5 + vb), 2.0);
        }
		
		// ������ �����ڸ� �� �����
        v1 = 1.0 - smoothstep(0.0, 0.3, v1);
		
		// ȸ���� ���� �׾��� ��ġ���� ������ ���������� �Ѵ�.
        // v1 �� ����� ������ ���� ���� ���� ���̰� �� ���� ��������. ( ���� �ֱ⸦ ���� )
        // 3.14 �� ������ ���� ��� �����غ���.
        v2 = a * noise1D(v1 * 5.0 + 0.1);
		
		// 1�� ȸ�θ��� �����̰� ������
        if (i == 0)
            v += v2 * flicker;
        else
            v += v2;
		
        f *= 2.0;

        // ������ �ݺ��� ���� ���� ȸ�θ��� ��ο�����.
        a *= 0.6;
    }

    float3 cexp = float3(0,0,0);

    float3 color_01 = float3(6.0, 1.0, 1.0);
    float3 color_02 = float3(1.0, 1.0, 6.0);

    cexp = lerp(color_01, color_02, color_distance_delta);
	
    float3 col = float3(pow(v, cexp.x), pow(v, cexp.y), pow(v, cexp.z)) * 2.0;
	
    return col;


}

float4 PS_MAIN(VS_OUTPUT input) : SV_TARGET
{
    float3 N = normalize(input.normalW);
    float3 T = normalize(input.tangentW - dot(input.tangentW, N) * N);
    float3 B = cross(N, T);
    float3x3 TBN = float3x3(T, B, N);
    
    float rim = saturate(dot(input.normalW, normalize(gvCameraPosition - input.positionW)));

    float3 normal = float3(gtxtModelNormalMap[NonUniformResourceIndex(input.material)].Sample(gLinearWrapSampler, input.uv).xyz);
    //float3 normal = float3(gtxtModelNormalMap[0].Sample(gLinearWrapSampler, input.uv).xyz);
    normal = 2.0f * normal - 1.0f;
    float3 normalW = normalize(mul(normal, TBN));
    
    float4 cDiffuseColor = gtxtModelTexture[NonUniformResourceIndex(input.material)].Sample(gLinearWrapSampler, input.uv);
    float4 cSpecularColor = gtxtSpecularMap[NonUniformResourceIndex(input.material)].Sample(gLinearWrapSampler, input.uv);
    float4 cGlossiness = gtxtGlossinessMap[NonUniformResourceIndex(input.material)].Sample(gLinearWrapSampler, input.uv);
    // ��Ŷ �ؽ��� �ϳ��� ���
    float4 cEmissiveColor = gtxtEmissiveMap[NonUniformResourceIndex(0)].Sample(gLinearWrapSampler, input.uv*8.0);
    // 0 ~ 50000 �Ÿ� �� �� ǥ�鿡 ȸ�ΰ� ��µ�
    float circuit_max_Distance = 50000;
    // ���� ȸ�� �ִ� �Ÿ�
    float unit_circle_max_Distance = 170;

    float circuit_delta = InverseLerp(0, circuit_max_Distance, clamp(distance(PlayerPosition.xyz, input.positionW), 0, circuit_max_Distance));
    float unit_circuit_dist_delta = InverseLerp(0, unit_circle_max_Distance, clamp(distance(PlayerPosition.xyz, input.positionW), 0, unit_circle_max_Distance));
    float circuit_Color_delta = InverseLerp(0, circuit_max_Distance * 0.1, clamp(distance(PlayerPosition.xyz, input.positionW), 0, circuit_max_Distance * 0.1));
    
    cEmissiveColor *= ( 1 - circuit_delta) * cSpecularColor;
    cEmissiveColor += cEmissiveColor * pow(1-rim,5);

    cEmissiveColor.rgb *= Circuit(input.uv, gfTimer, circuit_delta, circuit_Color_delta, false) * 2;

    cEmissiveColor.rgb +=
    Circuit(input.uv, gfTimer, unit_circuit_dist_delta, circuit_Color_delta, true)
    * (1 - unit_circuit_dist_delta)*2;

    cEmissiveColor.rgb = lerp(cEmissiveColor.rgb, CalcShadowFactor(input.ShadowPos) * cEmissiveColor.rgb, 0.5f);

    //----------------<Get Reflection Color>---------------------------------

    float3 vCameraPosition = gvCameraPosition.xyz;
    float3 vToCamera = normalize(vCameraPosition - input.positionW);
    float3 r = reflect(-vToCamera, normalW);
    float4 reflectionColor = gtxtSkyBox.Sample(gLinearWrapSampler, r);

    //-----------------------------------------------------------


    float4 ResultColor = BRDFLighting(
          input.positionW
        , normalW
        , cDiffuseColor
        , cSpecularColor + cEmissiveColor
        , 1
        , reflectionColor
        , CalcShadowFactor(input.ShadowPos));

    ResultColor = ResultColor * 0.5 + ResultColor * pow(rim, 7) * 0.5;
    ResultColor += reflectionColor * ( CalcShadowFactor(input.ShadowPos))*0.5;
    ResultColor.rgb += cEmissiveColor.rgb * cEmissiveColor.a ;
    return saturate(ResultColor);
}

PS_DEFFERED_OUTPUT PS_DEFFERED(VS_OUTPUT input)
{
    PS_DEFFERED_OUTPUT output;
    float3 N = normalize(input.normalW);
    float3 T = normalize(input.tangentW - dot(input.tangentW, N) * N);
    float3 B = cross(N, T);
    float3x3 TBN = float3x3(T, B, N);
    
    float rim = saturate(dot(input.normalW, normalize(gvCameraPosition - input.positionW)));

    float3 normal = float3(gtxtModelNormalMap[NonUniformResourceIndex(input.material)].Sample(gLinearWrapSampler, input.uv).xyz);
    //float3 normal = float3(gtxtModelNormalMap[0].Sample(gLinearWrapSampler, input.uv).xyz);
    normal = 2.0f * normal - 1.0f;
    float3 normalW = normalize(mul(normal, TBN));
    
    float4 cDiffuseColor = gtxtModelTexture[NonUniformResourceIndex(input.material)].Sample(gLinearWrapSampler, input.uv);
    float4 cSpecularColor = gtxtSpecularMap[NonUniformResourceIndex(input.material)].Sample(gLinearWrapSampler, input.uv);
    // ��Ŷ �ؽ��� �ϳ��� ���
    float4 cEmissiveColor = gtxtEmissiveMap[NonUniformResourceIndex(0)].Sample(gLinearWrapSampler, input.uv * 8.0);
    // 0 ~ 50000 �Ÿ� �� �� ǥ�鿡 ȸ�ΰ� ��µ�
    float circuit_max_Distance = 50000;
    // ���� ȸ�� �ִ� �Ÿ�
    float unit_circle_max_Distance = 170;

    float circuit_delta = InverseLerp(0, circuit_max_Distance, clamp(distance(PlayerPosition.xyz, input.positionW), 0, circuit_max_Distance));
    float unit_circuit_dist_delta = InverseLerp(0, unit_circle_max_Distance, clamp(distance(PlayerPosition.xyz, input.positionW), 0, unit_circle_max_Distance));
    float circuit_Color_delta = InverseLerp(0, circuit_max_Distance * 0.1, clamp(distance(PlayerPosition.xyz, input.positionW), 0, circuit_max_Distance * 0.1));
    
    cEmissiveColor *= (1 - circuit_delta) * cSpecularColor;
    cEmissiveColor += cEmissiveColor * pow(1 - rim, 5);

    cEmissiveColor.rgb *= Circuit(input.uv, gfTimer, circuit_delta, circuit_Color_delta, false) * 2;

    cEmissiveColor.rgb +=
    Circuit(input.uv, gfTimer, unit_circuit_dist_delta, circuit_Color_delta, true)
    * (1 - unit_circuit_dist_delta) * 2;

    cEmissiveColor.rgb = lerp(cEmissiveColor.rgb, CalcShadowFactor(input.ShadowPos) * cEmissiveColor.rgb, 0.5f);

    output.posW_bActive = float4(input.positionW, 1);
    output.normW_PostRimType.xyz = normalW;
    output.normW_PostRimType.w = POST_RIM_TYPE_STAGE;
    output.ShadowposW = input.ShadowPos;
    output.diffuse = cDiffuseColor;
    output.SpecC_GlossF.rgb = (cSpecularColor + cEmissiveColor).rgb;
    output.SpecC_GlossF.w = 1;
    output.emissive = cEmissiveColor;

    return output;
}