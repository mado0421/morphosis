
#define MAXBONE	40
struct BONE
{
	float4x4 m;
	float4x4 offset;
	float4x4 toParent;
};

StructuredBuffer<BONE> bone;
cbuffer cbone :register(b10)
{
	BONE b[MAXBONE];
};

struct WeightInfo
{
	int		idx;
	float	weight;
};

struct Vertex
{
	float3	pos;
	WeightInfo weights[4];

};

float4 Animation(Vertex v)
{
	float4 res;

	for (int i = 0; i < 4; ++i)
	{
		if (v.weights[i] == 0)break;


	}

	res=mul()


	return (res);
}


float4 main( float4 pos : POSITION ) : SV_POSITION
{
	return pos;
}