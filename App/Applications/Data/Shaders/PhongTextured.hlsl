//--------------------------------------------------------------------------------
// ImmediateGeometrySolid.hlsl
//
//
// Copyright (C) 2012 Jason Zink.  All rights reserved.
//--------------------------------------------------------------------------------

Texture2D ColorTexture : register(t0);
SamplerState LinearSampler : register(s0);

cbuffer WorldTransforms
{
	matrix WorldMatrix;
	matrix WorldViewProjMatrix;	
};

cbuffer ObjectMaterialInfo
{
	//float4 Ka;
	//float4 Kd;
	float4 Ks;
};

cbuffer PointLightInfo
{
	float4 LightPosition;
	float4 Ia;
	float4 Id;
	float4 Is;
};

cbuffer SceneInfo
{
	float4 ViewPosition;
};




struct VS_INPUT
{
	float3 position : POSITION;
	float3 normal   : NORMAL;
	float4 color    : COLOR;
	float2 tex		: TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 position : SV_Position;
	float4 worldPos : POSITION;
	float2 tex		: TEXCOORD0;
	float3 normal   : NORMAL;
};


VS_OUTPUT VSMAIN( in VS_INPUT input )
{
	VS_OUTPUT output;
	
	// Lighting calculations done in world space
	float4 P = mul( float4( input.position, 1.0f ), WorldMatrix );
	float3 N = mul( input.normal, (float3x3)WorldMatrix );

	output.position = mul( float4( input.position, 1.0f ), WorldViewProjMatrix );
	output.worldPos = P;	
	output.normal = N;
	output.tex = input.tex;
	
	return output;
}


float4 PSMAIN( in VS_OUTPUT input ) : SV_Target
{
	float3 P = input.worldPos.xyz;// / input.worldPos.w;
	float3 N = normalize( input.normal );
	float3 L = normalize( LightPosition.xyz - P.xyz );
	float3 V = normalize( ViewPosition.xyz - P.xyz );
	float3 H = normalize( L + V );

	float4 texcolour = ColorTexture.Sample(LinearSampler, input.tex);

	float NdotL = dot( L, N );
	float4 ambientIntensity = texcolour * Ia;
	float4 diffuseIntensity = texcolour * Id * clamp( NdotL, 0.0f, 1.0f );
	float4 specularIntensity = Ks * Is * clamp( pow( abs( dot( H, N ) ), 64.0f ), 0.0f, 1.0f );

	float4 output = ambientIntensity;

	if ( NdotL > 0.0f ) {
		output += diffuseIntensity;
		output += specularIntensity;
	}

	output.a = 1.0f;


	return( output );
}

