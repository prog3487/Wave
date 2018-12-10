#include "structures.hlsli"

VSOutput main(VSInput vin)
{
	VSOutput result;
	result.PosW	= mul(gWorld, float4(vin.PosL, 1.0f)).xyz;
	result.NormalW = mul((float3x3)gWorld, vin.NormalL);
	result.TangentW = mul((float3x3)gWorld, vin.TangentL);
	
	float4 uv = float4(vin.TexCoord, 0.0f, 1.0f);
	result.DispCoord0 = mul(gDisplaceTexTransform0, uv).xy;
	result.DispCoord1 = mul(gDisplaceTexTransform1, uv).xy;
	result.NormCoord0 = mul(gNormalTexTransform0, uv).xy;
	result.NormCoord1 = mul(gNormalTexTransform1, uv).xy;
	return result;
}