#include "Common.hlsl"

float movingLine(float2 uv, float2 center, float radius, float speedfactor)
{
    //���̴� ���� ����, �ð��� ���� ������ ����
    // �� ������ �ٷ� ���� �׾��� ������ �ȴ�.
    float theta0 = 90.0 * gfTimer * speedfactor;

    // �������� uv�� ���ϴ� ����
    float2 d = uv - center;
    // ���� ���̸� ���Ѵ�.
    float r = sqrt(dot(d, d));

    // ����ũ�Ⱑ ������ ���������� ũ�� �� �߶���� ��.
    if (r < radius)
    {
        // ��Ÿ �������� ���ϴ�, ������ ������ ������ ���ϴ� ���͸� ���Ѵ�.
        // �� ���Ϳ����� ���� ���� ���̴� ���� ��ġ�� �ȴ�.
        float2 p = radius * float2(cos(theta0 * PI / 180.0), -sin(theta0 * PI / 180.0));

        // ��p ���� ���� ��ġ�� ���ϴ� ���͸� ���ϴµ�,
        // p�� ���̴� 0.0�� 1.0 �� ���� ������ (������ ���� �����ϱ�) 
        // ���� ��ġ����
        // d,p�� ������ �� / p�� ũ�⸸ŭ �� ���ư� ��ġ�� ���͸� ���Ѵ�.
        // �̰��� �ٷ� ���̴� ���� �׷��� ��ġ��� ���� �ȴ�.
        float l = length(d - p * clamp(dot(d, p) / dot(p, p), 0.0, 1.0));

        // �������� ���� uv ��ǥ�� ���ϴ� ������ ũ�⸦ 1�� ����ȭ�Ѵ�.
        d = normalize(d);
        //theta0 ���� �������� �̿��� �׶���Ʈ ������ ����Ѵ�
        // ���� d�� ��ũź��Ʈ�� ���� ���� ������
        // �̸� degree�� ��ȯ�� ��Ÿ���ο� ���� ��
        // 360���� ������. �̶� �ݵ�� fmod �� �ؾ��Ѵ�. modf�� �ƴ϶�.
        float theta = fmod(180.0 * atan2(d.y, d.x) / PI + theta0, 360.0);

        // ��Ÿ / �׶���Ʈ�� ������ ���� , ���� 180���� �����ٸ�
        // 180�� ������ �׶���Ʈ�� ĥ�ϰ� �ȴ�. 
        // �ּ� �׶���Ʈ ����, �ִ� �׶���Ʈ �������� Ŭ������Ų��.
        float gradient = clamp(1.0 - theta / 90.0, 0.0, 0.7);

        // gradient�� ���� ���� ������ ���� �� ������ ����������.
        // ���̽� ���� ���� * ��� ���� + �׶���Ʈ ���� * ��� ���� �� ��ȯ��Ų��.
        return Smooth(l, 0.05) * (0.6 + sin(gfTimer) * 0.4) * 3.0 + 4.0 * gradient;
        
    }
    else
        return 0.0;
}

Texture2D<float4> gtxtRader : register(t0);

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    float2x2 rotation : ROTATION;
};

VS_OUTPUT VS_MAIN(uint nVertexID : SV_VertexID)
{
    VS_OUTPUT output;

    float client_width = 1600.0f;
    float client_height = 900.0f;
    float rader_size = 500.0f;
    float VerticalSize = rader_size / client_width;
    float HorizonalSize = rader_size / client_height;
    float2 LeftTop = float2(1050.0f / client_width, ((50.0f + rader_size) / client_height) - 1.0f);
    float TopTightenFactor = 0.2f;

    if (nVertexID == 0)
    {
        output.position = float4(LeftTop.x, LeftTop.y, 0.0f, 1.0f);
        output.uv = float2(0.f, 0.f);
    }
    if (nVertexID == 1)
    {
        output.position = float4(LeftTop.x + VerticalSize, LeftTop.y, 0.0f, 1.0f);
        output.uv = float2(1.f, 0.f);
    }
    if (nVertexID == 2)
    {
        output.position = float4(LeftTop.x + VerticalSize, LeftTop.y - HorizonalSize, 0.0f, 1.0f);
        output.uv = float2(1.f, 1.f);
    }
    if (nVertexID == 3)
    {
        output.position = float4(LeftTop.x, LeftTop.y, 0.0f, 1.0f);
        output.uv = float2(0.f, 0.f);
    }
    if (nVertexID == 4)
    {
        output.position = float4(LeftTop.x + VerticalSize, LeftTop.y - HorizonalSize, 0.0f, 1.0f);
        output.uv = float2(1.f, 1.f);
    }
    if (nVertexID == 5)
    {
        output.position = float4(LeftTop.x, LeftTop.y - HorizonalSize, 0.0f, 1.0f);
        output.uv = float2(0.f, 1.f);
    }

    float theta = atan2(-PlayerLookVector.x, -PlayerLookVector.z);

    output.rotation =
    float2x2(cos(theta), -sin(theta),
             sin(theta), cos(theta));

    return output;
}

float4 PS_MAIN(VS_OUTPUT input) : SV_Target
{
    // ó���� hp �� ������� 0 ~ 1 ���� ���� t ���� ã�Ƴ���.
    float first_hp_delta = InverseLerp(0.0f, 1.f, player_hp);
    // �� hp ��Ÿ���� �ٽ� ��ȣ ���� �׷����� �����Ѵ�.
    //float hp_delta = sqrt(1 - pow(1 - first_hp_delta, 2));
    float hp_delta = first_hp_delta;

    float4 result_color = 0;
    float2 centerpos = float2(0.5f, 0.5f);
    float distancefromcenter = distance(input.uv, centerpos);
    // HP �� �پ�� ���� �� �ֱ⸦ ������ ���� ���̴�.
    float time_based_hp_factor = lerp(1, 2, 1 - hp_delta);
    // �ð�, 0~1 �������� ��� �ֱ������� ���� ����.
    float time_factor = frac(gfTimer * time_based_hp_factor);
    float current_line_factor = sqrt(1 - pow(1 - time_factor, 2)) * 0.5;

    
    float green_line_thickness = 0.01;

    float2 new_directions[MAX_NUM_PLAYER_DOT];
    float2 new_disatance[MAX_NUM_PLAYER_DOT];

    for (int i = 0; i < num_of_players; ++i)
    {
        new_directions[i] = normalize(OtherPlayersPosition[i].xz - PlayerPosition.xz);
        new_directions[i].x *= -1;
        new_disatance[i] = InverseLerp(0, 10000, distance(PlayerPosition.xz, OtherPlayersPosition[i].xz));
    }

    // ���̴� �ؽ��ĸ� ���ø��Ͽ� ���̴��� �� ����� �׸���.
    float4 sampled_color = gtxtRader.Sample(gLinearWrapSampler, input.uv);
    result_color = (sampled_color) * float4(1.0, 1.0, 1.0, 0.95 + (sin(gfTimer * 55.f)) * 0.01);

     // ȸ���ϴ� ���̴� ���� �׸���.
    float4 moving_line_color = movingLine(input.uv, centerpos, 0.5f, time_based_hp_factor);

    moving_line_color *= lerp(float4(1, 0, 0, 0.25) * 0.25, float4(mechtheme_darker_color, 0.25) * 0.75, hp_delta);

    result_color += (length(moving_line_color.rgb) < 0.01) ? 1 : moving_line_color;
    result_color = clamp(result_color, 0, 1);
   

    result_color *= (current_line_factor - green_line_thickness <=
    distancefromcenter && distancefromcenter <= current_line_factor + green_line_thickness) ?
    float4(0.5, 1, 0.5, 1) : float4(1, 1, 1, 1);
    
    float4 sphereFact = sphere(input.uv.x, input.uv.y, centerpos.x, centerpos.y, 0.002 + sin(gfTimer * time_based_hp_factor) * 0.001);
    result_color += sphereFact;
    result_color = clamp(result_color, 0, 1);

    float theta0 = 90.0 * gfTimer * time_based_hp_factor;
    
    for (int i = 0; i < num_of_players; ++i)
    {
        float2 newpos = mul((new_directions[i] * new_disatance[i]), input.rotation) + 0.5;

        //////////< ���߿� ������ ��.>

        float angle = theta0 * PI / 180.0;
        float2 d = newpos - centerpos;
        float2 p = 0.5f * float2(cos(angle), -sin(angle));
        float l = length(d - p * clamp(dot(d, p) / dot(p, p), 0.0, 0.5));
        float r = sqrt(dot(d, d));
        d = normalize(d);
        float theta = fmod(180.0 * atan2(d.y, d.x) / PI + theta0, 360.0);
        float gradient = clamp(1.0 - theta / 90.0, -0.3, 1.0);


        ///////////

        sphereFact = float4(sphere(input.uv.x, input.uv.y, newpos.x, newpos.y, 0.0005 + max(0, gradient * 0.005)) * red * 2, 1.0);
        
        result_color += (length(sphereFact) > 0.001) ? sphereFact : 0;
        result_color = clamp(result_color, 0, 1);
    }
   

    // ���̴� �� �ۿ� �ִ°� ���� 0����
    result_color = (distance(input.uv, centerpos) < 0.5) ? result_color : 0;

    // �ణ �����ϰ�
    result_color.a = clamp(result_color.a, 0, 1);
    result_color *= float4(1, 1, 1, 0.80);

    return result_color;
}