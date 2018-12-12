#include "structures.hlsli"

Texture2D TexWave0 : register(t0);
Texture2D TexWave1 : register(t1);
TextureCube TexCube : register(t2);
sampler NormalSampler : register(s0);
sampler CubeSampler : register(s1);

cbuffer LightBuffer : register(b0)
{
	DirectionalLight gDirLight;
	float3 gEyePosW;
};

cbuffer MaterialBuffer : register(b1)
{
	Material gMaterial;
};

//
float3 SchlickFresnel(float3 R0, float3 normal, float3 lightVec)
{
	float cosIncidentAngle = saturate(dot(normal, lightVec));
	float f0 = 1.0f - cosIncidentAngle;
	float3 reflectPercent = R0 + (1.0f - R0) * pow(f0, 5);
	return reflectPercent;
}

//
float3 BlinnPhong(float3 lightStrength, float3 lightVec, float3 normal, float3 toEye, float3 color, Material mat)
{
	float Shininess = 1.0f - mat.Roughness;
	const float m = Shininess * 256.0f;
	float3 halfVec = normalize(toEye + lightVec);

	float roughnessFactor = (m + 8.0f) * pow(max(dot(halfVec, normal), 0.0f), m) / 8.0f;
	float3 fresnelFactor = SchlickFresnel(mat.FresnelR0, halfVec, lightVec);
	float3 specAlbedo = fresnelFactor * roughnessFactor;
	specAlbedo = specAlbedo / (specAlbedo + 1.0f);
	return (color + specAlbedo) * lightStrength;
}

//
float3 ComputeDirectionalLight(DirectionalLight Light, Material mat, float3 color, float3 normal, float3 toEye)
{
	float3 lightVec = -Light.Direction;
	float ndotl = max(dot(lightVec, normal), 0.0f);
	float3 lightStrength = Light.ColorStrength * ndotl;
	return BlinnPhong(lightStrength, lightVec, normal, toEye, color, mat);
}

//
float3 NormalSampleToWorldSpace(float3 normalMapSample, float3 NormalW, float3 TangentW)
{
	// To [-1, 1]
	float3 normalT = 2.0f * normalMapSample - 1.0f;
	
	// build basis
	float3 N = NormalW;
	float3 T = normalize(TangentW - (dot(TangentW, N) * N));
	float3 B = cross(N, T);
	float3x3 TBN = float3x3(T, B, N);
	float3 bumpedNormalW = mul(normalT, TBN);
	return float3(bumpedNormalW.x, bumpedNormalW.y, -bumpedNormalW.z); // example LH texture -> RH
}

//
float4 main(DomainOut pin) : SV_Target
{
	pin.NormalW = normalize(pin.NormalW);

	float3 ToEye = gEyePosW - pin.PosW;
	ToEye = normalize(ToEye);

	float3 n0 = TexWave0.Sample(NormalSampler, pin.NormCoord0).rgb;
	float3 n1 = TexWave1.Sample(NormalSampler, pin.NormCoord1).rgb;
	
	float3 bn0 = NormalSampleToWorldSpace(n0, pin.NormalW, pin.TangentW);
	float3 bn1 = NormalSampleToWorldSpace(n1, pin.NormalW, pin.TangentW);
	
	float3 bn = normalize(bn0 + bn1);

	float3 r = reflect(-ToEye, bn);
	float4 diffuse = TexCube.Sample(CubeSampler, r);
	//float4 diffuse = gMaterial.DiffuseAlbedo;

	float3 LitColor = ComputeDirectionalLight(gDirLight, gMaterial, diffuse.rgb, bn, ToEye);
	return float4(LitColor, diffuse.a);
}