
struct VSInputP
{
	float4 Position : SV_Position;
};

//struct VSInputNmTx
//{
//	float4 Position : SV_Position;
//	float3 Normal   : NORMAL;
//	float2 TexCoord : TEXCOORD0;
//};

struct VSOutputEnvMap
{
	float4 PositionPS : SV_Position;
	float3 EnvCoord   : TEXCOORD0;
};

//-- Wave --//
struct VSInput
{
	float3 PosL		: SV_POSITION;
	float3 NormalL  : NORMAL;
	float3 TangentL : TANGENT;
	float2 TexCoord : TEXCOORD;
};

struct VSOutput
{
	float3 PosW		: POSITION;
	float3 NormalW	: NORMAL;
	float3 TangentW : TANGENT;
	float2 DispCoord0 : TEXCOORD0;
	float2 DispCoord1 : TEXCOORD1;
	float2 NormCoord0 : TEXCOORD2;
	float2 NormCoord1 : TEXCOORD3;
};

struct PatchTess
{
	float Edge[3]	: SV_TessFactor;
	float Inside	: SV_InsideTessFactor;
};

struct HullOut
{
	float3 PosW		: POSITION;
	float3 NormalW	: NORMAL;
	float3 TangentW : TANGENT;
	float2 DispCoord0 : TEXCOORD0;
	float2 DispCoord1 : TEXCOORD1;
	float2 NormCoord0 : TEXCOORD2;
	float2 NormCoord1 : TEXCOORD3;
};

struct DomainOut
{
	float4 PosH		: SV_Position;
	float3 PosW		: POSITION;
	float3 NormalW	: NORMAL;
	float3 TangentW : TANGENT;
	float2 NormCoord0 : TEXCOORD0;
	float2 NormCoord1 : TEXCOORD1;
};

struct DirectionalLight
{
	float3 ColorStrength;
	float3 Direction;
};

struct Material
{
	float4 DiffuseAlbedo;
	float3 FresnelR0;
	float Roughness;
};

cbuffer WaveBuffer : register(b0)
{
	float4x4 gWorld;
	float4x4 gViewProj;
	float4x4 gTessViewProj;
	float4x4 gDisplaceTexTransform0;
	float4x4 gDisplaceTexTransform1;
	float4x4 gNormalTexTransform0;
	float4x4 gNormalTexTransform1;
	float gProj11;
	float gEdgesPerScreenHeight;
	float gHeightScale;
	float pad0;
};