struct DirectionalLight
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float3 Direction;
};

struct Material
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
};

cbuffer cbPerFrame
{
	float3 gEyePosW;
};

cbuffer cbPerObject
{
	float4x4 gWorld;
	float4x4 gWorldViewProj; 
};

DirectionalLight gSun = 
{
	{0.9f, 0.9f, 0.5f, 1.0f},			// Ambient
	{1.0f, 1.0f, 0.6f, 1.0f},			// Diffuse
	{1.0f, 1.0f, 1.0f, 2.0f},			// Specular
	{0.25, -0.57735f, 0.45f},			// Direction
};

Material gMaterial =
{
	{0.4f, 0.4f, 0.4f, 1.0f},			// Ambient
	{0.7f, 0.7f, 0.7f, 1.0f},			// Diffuse
	{1.0f, 1.0f, 1.0f, 2.0f},			// Specular
};

struct VertexIn
{
	float3 PosL  	: POSITION;
	float3 NormalL  : NORMAL;
};

struct VertexOut
{
	float4 PosH  	: SV_POSITION;
	float3 PosW  	: POSITION;
	float3 NormalW 	: NORMAL;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	vout.PosW = mul(float4(vin.PosL, 1.0f), gWorld).xyz;
	vout.NormalW = mul(normalize(vin.NormalL), (float3x3)gWorld);
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	return float4(0.0f, 0.0f, 1.0f, 1.0f);
}

technique11 MainTech
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}