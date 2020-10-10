////////////////////////////////////////////////////////////////////////////////
// Filename: depth.gs
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
cbuffer CubeMapViewMatrixBuffer : register (b0)
{
	matrix ViewProjection[6]; // View matrices for cube map rendering
};

//////////////
// TYPEDEFS //
//////////////
struct GSInput
{
    float4 position			: SV_POSITION;
};

struct GSOutput
{
    float4 position			: SV_POSITION;
	uint renderTargetIndex	: SV_RenderTargetArrayIndex;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
[maxvertexcount(18)]
void DepthGeometryShader( triangle GSInput input[3], inout TriangleStream<GSOutput> outputStream)
{
    for (int face = 0; face < 6; ++face)
	{
		GSOutput output;
		output.renderTargetIndex = face;
		for (int vertex = 0; vertex < 3; ++vertex)
		{
			output.position = mul(ViewProjection[face], input[vertex].position);
			outputStream.Append(output);
		}
		outputStream.RestartStrip();
	}
}