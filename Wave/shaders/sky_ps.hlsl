#include "structures.hlsli"

TextureCube gCubeMap : register(t0);
sampler gCubeMapSampler : register(s0);

float4 main(VSOutputEnvMap vin) : SV_Target
{
	return gCubeMap.Sample(gCubeMapSampler, vin.EnvCoord);
}