////////////////////////////////////////////////////////////////////////////////
// Filename: depth.vs
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer : register (b0)
{
	matrix World;
	matrix WorldView;
    matrix WorldViewProjection;
    matrix WorldViewInverseTranspose;
	matrix WorldLightProjectionTexture[6];
};


//////////////
// TYPEDEFS //
//////////////
struct VSInput
{
    float4 position : POSITION;
};

struct VSOutput
{
    float4 position : SV_POSITION;	
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
VSOutput DepthVertexShader(VSInput input)
{
	VSOutput output;

	// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

	// Calculate the position of the vertex against the world matrix.
    output.position = mul(World, input.position);

    return output;
}