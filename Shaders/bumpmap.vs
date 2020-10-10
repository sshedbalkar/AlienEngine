////////////////////////////////////////////////////////////////////////////////
// Filename: bumpmap.vs
////////////////////////////////////////////////////////////////////////////////

/////////////
// DEFINES //
/////////////
#define MAX_NUM_LIGHTS 9

/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer : register (b0)
{
	matrix World;
	matrix WorldView;
    matrix WorldViewProjection;
    matrix WorldViewInverseTranspose;
	matrix WorldLightProjectionTexture[MAX_NUM_LIGHTS][6];
};

cbuffer ScaleBuffer : register (b1)
{
    float3 Scale;
	float padding1;
};

cbuffer MultiTextureTypeBuffer : register (b2)
{
	uint mtt;
	float Transparency;
	float TextMultiplier;
	uint NUM_LIGHTS;
};

//////////////
// TYPEDEFS //
//////////////
struct VSInput
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;	
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
};

struct VSOutput
{
    float4 viewClipSpacePosition				: SV_Position;
	float4 position								: POSITION;
	float3 normal								: Attrib0;
	float3 view									: Attrib1;
	float3 vsp									: Attrib2;
	float2 tex									: TEXCOORD0;
	float3 tangent								: TANGENT;
	float3 binormal								: BINORMAL;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
VSOutput BumpMapVertexShader(VSInput input)
{
    input.position.w = 1.0f;
    float3 viewSpacePosition  = mul(WorldView, float4(input.position)).xyz;
	
	VSOutput output;

	output.vsp = viewSpacePosition;

	// Calculate the position of the vertex in the world.
    //output.worldPos = mul(input.position, World);

	float3 n = normalize(input.normal);

	output.viewClipSpacePosition  = mul(WorldViewProjection, float4(input.position));
	output.view   = -viewSpacePosition;
	output.normal = mul(WorldViewInverseTranspose, float4(input.normal, 0.0f)).xyz;

	input.tex = input.tex*TextMultiplier;

	// Store the texture coordinates for the pixel shader.
	if(mtt==8){		
		if(abs(n.x) >= 0.9)
			input.tex = input.tex * Scale.zy;
		if(abs(n.y) >= 0.9)
			input.tex = input.tex * Scale.xz;
		if(abs(n.z) >= 0.9)
			input.tex = input.tex * Scale.xy;
	
		output.tex = input.tex*0.1;
	}
	else output.tex = input.tex;
		
	output.position = input.position;

	// Calculate the tangent vector against the world matrix only and then normalize the final value.
    output.tangent = mul(WorldViewInverseTranspose, float4(input.tangent, 0.0f)).xyz;	

    // Calculate the binormal vector against the world matrix only and then normalize the final value.
    output.binormal = mul(WorldViewInverseTranspose, float4(input.binormal, 0.0f)).xyz;

    return output;
}