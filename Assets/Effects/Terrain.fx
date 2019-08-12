struct DirectionalLight
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float3 Direction;
};

cbuffer cbPerFrame
{
	float3 gEyePosW;
	float3 gPlayerPosW;
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
	pin.NormalW = normalize(pin.NormalW);
	
	float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	
	float3 lightVec = -gSun.Direction;
	float diffuseFactor = dot(lightVec, pin.NormalW);
	
	ambient = float4(0.5f, 0.5f, 0.25f, 1.0f) * gSun.Ambient;
	
	[flatten]
	if (diffuseFactor > 0.0f) {
		diffuse = diffuseFactor * float4(0.6f, 0.6f, 0.3f, 1.0f) * gSun.Diffuse;
	}
	
	float4 highlightColor = float4(1.0f, 0.0f, 0.0f, 1.0f);
	float highlightFactor = 0.0f;
	float3 playerPos = gPlayerPosW;
	playerPos.y = pin.PosW.y;
	float distToPlayer = length(pin.PosW - playerPos);
	
	if (distToPlayer < 100.0f) {
		highlightFactor = 1.0f - (distToPlayer / 100);
	}
	
	return (ambient + diffuse + (highlightFactor * highlightColor));
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