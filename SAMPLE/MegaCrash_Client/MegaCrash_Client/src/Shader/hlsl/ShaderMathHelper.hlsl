
#define PI 3.1415926535897932384626433832795

#define blue1 float3(0.74,0.95,1.00)
#define blue2 float3(0.87,0.98,1.00)
#define blue3 float3(0.35,0.76,0.83)
#define blue4 float3(0.953,0.969,0.89)
#define blue5 float3(0.35,0.76,0.83)*0.25 // Ǫ���� :: �ణ ��Ӱ�
#define red   float3(1.00,0.38,0.227)

// �����
//#define mechtheme_color float3(0.91, 0.85, 0.3f) // ��ī�׸�
//#define mechtheme_darker_color float3(0.91, 0.85, 0.3f)*0.25 // ��ī�׸� , ��Ӱ�

// Ǫ����
#define mechtheme_color float3(0.74,0.95,1.00) // ��ī�׸�
#define mechtheme_darker_color float3(0.74,0.95,1.00)*0.25 // ��ī�׸� , ��Ӱ�

#define timeDiv 5.5

#define tau 6.28318
#define rot(a) mat2(cos(a), -sin(a), sin(a), cos(a)) // col1a col1b col2a col2b

float2 random2(float2 p)
{
    float2 result =
        frac(
    float2(
            sin(p.x * 591.32 + p.y * 154.077),
            cos(p.x * 391.32 + p.y * 49.077)
        )
    );

    return result;

}

float random(float n)
{
    return frac(sin(n) * 43758.5453123);
}


float noise1D(float p)
{
    float fl = floor(p);
    float fc = frac(p);
    return lerp(random(fl), random(fl + 1.0), fc);
}


// ���γ��� ����
// iquilezles �� ���γ��� ���� ������ ������
float voronoi(float2 x)
{
    float2 p = floor(x);
    float2 f = frac(x);
	
    float2 res = float2(16.0, 16.0);
    for (int j = -1; j <= 1; j++)
    {
        for (int i = -1; i <= 1; i++)
        {
            float2 b = float2(i, j);
            float2 r = float2(b.x, b.y) - f + random2(p+b);
			
			// chebyshev �Ÿ� ����.
            // ü���ǿ��� �Ÿ� ��� �Ͱ� ������ ����̴�.
            // float d = max(abs(r.x), abs(r.y));
            // ����ź ����
            float d = abs(r.x) + abs(r.y);
            // ��Ŭ���� ����
            //float d = sqrt(pow(r.x, 2) + pow(r.y , 2));

            if (d < res.x)
            {
                res.y = res.x;
                res.x = d;
            }
            else if (d < res.y)
            {
                res.y = d;
            }
        }
    }
    return res.y - res.x;
}

float SumThreeElem (float3 v)
{
    return v.x + v.y + v.z;
}

float SumFourElem(float4 v)
{
    return v.x + v.y + v.z + v.w;
}

float4 textureNoTile(float2 x, float v, Texture2D tex, SamplerState _sampler_state)
{
    float k = random(0.0005 * (x.x));
    float2 duvdx = ddx(x);
    float2 duvdy = ddx(x);
    
    float l = k * 8.0;
    float i = floor(l);
    float f = frac(l);
    
    float2 offa = sin(float2(3.0, 7.0) * (i + 0.0)); 
    float2 offb = sin(float2(3.0, 7.0) * (i + 1.0));   

    float4 cola = tex.SampleGrad(_sampler_state, x + v * offa, duvdx, duvdy).xyzw;
    float4 colb = tex.SampleGrad(_sampler_state, x + v * offb, duvdx, duvdy).xyzw;
    
    return lerp(cola, colb, smoothstep(0.2, 0.8, f - 0.1 * SumFourElem(cola - colb)));
}


/// �ּҰ��� �ִ밪 �׸��� ���� �� ���� �ѱ�� t ���� ��ȯ���ش�.
float InverseLerp(float min, float max, float value)
{
    return ((value - min) / (max - min));
}

float sphere(float a, float b, float x, float y, float radius)
{
    float d = pow(a - x, 2.) + pow(b - y, 2.) - radius;
    return d < 0. ? pow(smoothstep(-0., -radius, d), .2) : 0.;
}

// ����
float Drawbox(float2 st, float2 size)
{
    size = float2(0.5, 0.5) - size * 0.5;
    float2 uv = smoothstep(size, size + float2(0.001, 0.001), st);
    uv *= smoothstep(size, size + float2(0.001, 0.001), float2(1.0, 1.0) - st);
    return uv.x * uv.y;
}

// ���ڰ�
float Drawcross(float2 st, float size)
{
    return
		Drawbox(st, float2(size, size / 4.)) +
		Drawbox(st, float2(size / 4., size));
}

float Smooth(float r,float R)
{
    return 1.0 - smoothstep(R - 1.0, R + 1.0, r);
}

float Range(float a, float b , float x)
{
    return step(a, x) * (1.0 - step(b, x));
}

float Drawline(float2 p, float2 a, float2 b)
{
    float a2b = b - a;
    float a2p = p - a;
          
    float h = clamp(dot(a2p, a2b) / dot(a2b, a2b), 0.0, 1.0);
    float2 p1 = lerp(a, b, h);
    return length(p1 - p);
}