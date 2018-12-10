#include "structures.hlsli"

Texture2D TexWave0 : register(t0);
Texture2D TexWave1 : register(t1);
sampler WaveSampler : register(s0);

[domain("tri")]
DomainOut main(
	PatchTess patchTess,
	float3 uvw : SV_DomainLocation,
	const OutputPatch<HullOut, 3> tri)
{
	float3 PosW		= uvw.x * tri[0].PosW + uvw.y * tri[1].PosW + uvw.z * tri[2].PosW;
	float3 Normal	= uvw.x * tri[0].NormalW + uvw.y * tri[1].NormalW + uvw.z * tri[2].NormalW;
	float3 Tangent	= uvw.x * tri[0].TangentW + uvw.y * tri[1].TangentW + uvw.z * tri[2].TangentW;
	float2 DispUV0	= uvw.x * tri[0].DispCoord0 + uvw.y * tri[1].DispCoord0 + uvw.z * tri[2].DispCoord0;
	float2 DispUV1	= uvw.x * tri[0].DispCoord1 + uvw.y * tri[1].DispCoord1 + uvw.z * tri[2].DispCoord1;
	float2 NormUV0	= uvw.x * tri[0].NormCoord0 + uvw.y * tri[1].NormCoord0 + uvw.z * tri[2].NormCoord0;
	float2 NormUV1	= uvw.x * tri[0].NormCoord1 + uvw.y * tri[1].NormCoord1 + uvw.z * tri[2].NormCoord1;
	
	float4 wave0 = TexWave0.SampleLevel(WaveSampler, DispUV0, 0);
	float4 wave1 = TexWave1.SampleLevel(WaveSampler, DispUV1, 0);

	PosW.y += (wave0.a + wave1.a) * gHeightScale;

	DomainOut result;
	result.PosH = mul(gViewProj, float4(PosW, 1.0f));
	result.PosW = PosW;
	result.NormalW = Normal;
	result.TangentW = Tangent;
	result.NormCoord0 = NormUV0;
	result.NormCoord1 = NormUV1;
	return result;
}