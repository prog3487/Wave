#include "structures.hlsli"

float GetPostProjectionSphereExtent(float3 Origin, float Diameter)
{
	float4 ClipPos = mul(gTessViewProj, float4(Origin, 1.0));
	return abs(Diameter * gProj11 / ClipPos.w);
}

float CalculateTessellationFactor(float3 Control0, float3 Control1)
{
	float e0 = distance(Control0, Control1);
	float3 m0 = (Control0 + Control1) / 2;
	float ProjectedExtent = GetPostProjectionSphereExtent(m0, e0);
	return max(1, gEdgesPerScreenHeight * ProjectedExtent);
}

bool IsPointInsideFrustum(float4 p)
{
	float4 cp = mul(gTessViewProj, p);
	if (cp.w < cp.x || cp.x < -cp.w)
		return false;
	if (cp.w < cp.y || cp.y < -cp.w)
		return false;
	if (cp.w < cp.z || cp.z < 0.0f)
		return false;
	return true;
}

PatchTess constantFunc(InputPatch<VSOutput, 3> patch)
{
	PatchTess pt;

	if (gEnableFrustumCull > 0)
	{
		if (!IsPointInsideFrustum(float4(patch[0].PosW, 1.0f)) &&
			!IsPointInsideFrustum(float4(patch[1].PosW, 1.0f)) &&
			!IsPointInsideFrustum(float4(patch[1].PosW, 1.0f)))
		{
			pt.Edge[0] = 0.0f;
			pt.Edge[1] = 0.0f;
			pt.Edge[2] = 0.0f;
			pt.Inside = 0.0f;
			return pt;
		}
	}

	pt.Edge[0] = CalculateTessellationFactor(patch[1].PosW.xyz, patch[2].PosW.xyz); // v1-v2
	pt.Edge[1] = CalculateTessellationFactor(patch[2].PosW.xyz, patch[0].PosW.xyz); // v2-v0
	pt.Edge[2] = CalculateTessellationFactor(patch[0].PosW.xyz, patch[1].PosW.xyz); // v0-v1
	pt.Inside = pt.Edge[1];

	//pt.Edge[0] = 3;	// v1-v2
	//pt.Edge[1] = 3;	// v2-v0
	//pt.Edge[2] = 3;	// v0-v1
	//pt.Inside = 3;

	return pt;
}

[domain("tri")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("constantFunc")]
HullOut main(InputPatch<VSOutput, 3> inPoints, uint i : SV_OutputControlPointID)
{
	HullOut result;
	result.PosW		= inPoints[i].PosW;
	result.NormalW  = inPoints[i].NormalW;
	result.TangentW	= inPoints[i].TangentW;
	result.DispCoord0 = inPoints[i].DispCoord0;
	result.DispCoord1 = inPoints[i].DispCoord1;
	result.NormCoord0 = inPoints[i].NormCoord0;
	result.NormCoord1 = inPoints[i].NormCoord1;
	return result;
}