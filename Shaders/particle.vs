////////////////////////////////////////////////////////////////////////////////
// Filename: particle.vs
////////////////////////////////////////////////////////////////////////////////

/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
	float4 color : COLOR;
	float2 scale : SCALE;
};

struct GSInputType
{
    float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 scale : SCALE;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
GSInputType ParticleVertexShader(VertexInputType input)
{
    GSInputType output;

	// Change the position vector to be 4 units for proper matrix calculations.
	input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	//output.position = mul(output.position, projectionMatrix); GS
    
	// Store the particle color for the pixel shader. 
    output.color = input.color;

	output.scale = input.scale;

    return output;
}