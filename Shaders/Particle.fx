/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct GSInputType
{
    float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 scale : SCALE;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float4 color : COLOR;
};

[maxvertexcount(6) ]
void ParticleGS( point GSInputType input[1], inout TriangleStream<PixelInputType> ParticleOutputStream )
{
	PixelInputType output[6];
	float2 scale = input[0].scale * 0.5;

	// Bottom left 
	output[0].position = input[0].position + float4( -scale.x, -scale.y, 0, 0);
	output[0].tex = float2(0, 1);

	// Top left 
	output[1].position = input[0].position + float4( -scale.x, scale.y, 0, 0);
	output[1].tex = float2(0, 0);

	// Bottom right 
	output[2].position = input[0].position + float4( scale.x, -scale.y, 0, 0);
	output[2].tex = float2(1, 1);

	// Bottom right 
	output[3].position = input[0].position + float4( scale.x, -scale.y, 0, 0);
	output[3].tex = float2(1, 1);

	// Top left 
	output[4].position = input[0].position + float4( -scale.x, scale.y, 0, 0);
	output[4].tex = float2(0, 0);

	// Top right
	output[5].position = input[0].position + float4( scale.x, scale.y, 0, 0);
	output[5].tex = float2(1, 0);
	
	for(uint i = 0; i < 6; ++i){
		
		output[i].position= mul( output[i].position, projectionMatrix);
		output[i].color = input[0].color;
		
		ParticleOutputStream.Append( output[i] );
	}
}
