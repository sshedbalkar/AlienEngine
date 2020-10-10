////////////////////////////////////////////////////////////////////////////////
// Filename: animated.vs
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix WorldMatrixArray[80];
};

cbuffer CameraBuffer
{
    float3 cameraPosition;
	float padding;
};


//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 BlendWeights : BLENDWEIGHT;
    uint4 BlendIndices : BLENDINDICES;
};

/*
struct VSOutput
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	//float3 viewDirection : TEXCOORD1;
};*/

struct VSOutput
{
    float4 PositionCS 		    : SV_Position;
    float3 PositionWS 		    : POSITIONWS;
    float2 TexCoord 		    : TEXCOORD;
    float3 NormalWS 		    : NORMALWS;
    float3 TangentWS 		    : TANGENTWS;
    float3 BitangentWS 		    : BINORMALWS;
	float DepthVS			    : DEPTHVS;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
VSOutput AnimateVertexShader(VertexInputType input)
{
    VSOutput output;
	float3 Pos = float3(0,0,0);
    float3 Normal = float3(0,0,0);	
   
   // Change the position vector to be 4 units for proper matrix calculations.
    float BlendWeightsArray[4] = (float[4])input.BlendWeights;
    unsigned int   IndexArray[4]        = (unsigned int[4])input.BlendIndices;
	    
	for (int iBone = 0; iBone < 4 ; iBone++)
    {       
        Pos += mul( input.position , WorldMatrixArray[IndexArray[iBone]]).xyz * BlendWeightsArray[iBone];
        Normal += mul( float4( input.normal, 1) , WorldMatrixArray[IndexArray[iBone]]).xyz * BlendWeightsArray[iBone];
    }
	Normal = normalize(Normal);

	// Calculate the position of the vertex against the world, view, and projection matrices.
    float4 WorldPosition = mul( float4(Pos, 1), worldMatrix);
	output.PositionWS = WorldPosition.xyz;
	float4 ViewPosition = mul( WorldPosition, viewMatrix);
	output.DepthVS = ViewPosition.z;
    output.PositionCS = mul( ViewPosition, projectionMatrix);
    
	// Store the texture coordinates for the pixel shader.
	output.TexCoord = input.tex;
    
	// Calculate the normal vector against the world matrix only.
    output.NormalWS = mul(Normal, (float3x3)worldMatrix);
	
    // Normalize the normal vector.
    output.NormalWS = normalize(output.NormalWS);

	//garbage to fit the output structure
	/*output.TangentWS
	output.BitangentWS
	*/

    return output;
}