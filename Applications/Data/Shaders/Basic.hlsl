cbuffer Transforms
{
	matrix WorldMatrix;
	matrix ViewMatrix;
	matrix ProjMatrix;
};

struct VS_INPUT
{
	float4 position : SV_Position;
	float4 color : COLOR;
};

struct VS_OUTPUT
{
	float4 position : SV_Position;
	float4 color : COLOR;
};

VS_OUTPUT VSMain(in VS_INPUT v)
{
	VS_OUTPUT o = (VS_OUTPUT)0;

	float WorldSpace = mul(v.position, WorldMatrix);
	float ViewSpace = mul(WorldSpace, ViewMatrix);
	float ScreenSpace = mul(ViewSpace, ProjMatrix);

	o.position = ScreenSpace;
	o.color = v.color;

	return o;
}

float PSMain(in VS_OUTPUT input) : SV_Target
{
	float color = input.color;

	return(color);
}