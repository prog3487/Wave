#include "structures.hlsli"

cbuffer SKY_VS_cbuffer : register(b0)
{
	float4x4 gWVP;
};

VSOutputEnvMap main(VSInputP vin)
{
	VSOutputEnvMap vout;
	vout.EnvCoord	= vin.Position.xyz;	// sky sphere
	vout.PositionPS = mul(gWVP, vin.Position).xyww;	// depth -> 1
	return vout;
}