//=================================================================================================
//
//	Light Indexed Deferred Sample
//  by MJP
//  http://mynameismjp.wordpress.com/
//
//  All code and content licensed under Microsoft Public License (Ms-PL)
//
//=================================================================================================

//=================================================================================================
// Includes
//=================================================================================================
static const uint LightTileSize = 16;

struct Light
{
    float3 Position;
    float3 Color;
    float Falloff;
};


//=================================================================================================
// Constant buffers
//=================================================================================================
cbuffer VSConstants : register(cb0)
{
    float4x4 World;
	float4x4 View;
    float4x4 WorldViewProjection;
	float4 Scale;
	float TextMultiplier;
	bool Extendable;
}

cbuffer PSConstants : register(cb0)
{
    float3 SpecularAlbedo;
    float Balance;
    float Roughness;
    bool EnableNormalMapping;
	float4 ConstAmbient;
	float4 SkyColor;
	float4 GroundColor;
	float HemiIntensity;
	uint mtt;
	float3 LightColor;
	float timer;
}

//=================================================================================================
// Resources
//=================================================================================================
Texture2D ShaderTextures[3] : register(t0);

SamplerState AnisoSampler : register(s0);

//=================================================================================================
// Input/Output structs
//=================================================================================================
struct VSInput
{
    float4 PositionOS 		    : POSITION;
    float2 TexCoord 		    : TEXCOORD0;
	float3 NormalOS 		    : NORMAL;
	float3 TangentOS 		    : TANGENT;
    float3 BitangentOS		    : BINORMAL;
};

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

struct PSInput
{
    float4 PositionSS 		    : SV_Position;
    float3 PositionWS 		    : POSITIONWS;
    float2 TexCoord 		    : TEXCOORD;
	float3 NormalWS 		    : NORMALWS;
    float3 TangentWS 		    : TANGENTWS;
    float3 BitangentWS 		    : BINORMALWS;
	float DepthVS			    : DEPTHVS;
    uint Coverage               : SV_Coverage;
};

//=================================================================================================
// Vertex Shader
//=================================================================================================
VSOutput VS(in VSInput input, in uint VertexID : SV_VertexID)
{
    VSOutput output;

    // Calc the world-space position
    output.PositionWS = mul(input.PositionOS, World).xyz;

	// Calc the view-space depth
	output.DepthVS = mul(float4(output.PositionWS, 1.0f), View).z;

    // Calc the clip-space position
    output.PositionCS = mul(input.PositionOS, WorldViewProjection);

	// Store normalized normal for texture multiplication
	float3 n = normalize(input.NormalOS);

	// Rotate the normal into world space
    output.NormalWS = normalize(mul(input.NormalOS, (float3x3)World));

    // Rotate the rest of the tangent frame into world space
    output.TangentWS = normalize(mul(input.TangentOS, (float3x3)World));
    output.BitangentWS = normalize(mul(input.BitangentOS, (float3x3)World));

	input.TexCoord = input.TexCoord*TextMultiplier;

	if(Extendable)
	{
		if(abs(n.x) >= 0.9)
			input.TexCoord = input.TexCoord * Scale.zy;
		if(abs(n.y) >= 0.9)
			input.TexCoord = input.TexCoord * Scale.xz;
		if(abs(n.z) >= 0.9)
			input.TexCoord = input.TexCoord * Scale.xy;
	
		// Pass along the texture coordinate
		output.TexCoord = input.TexCoord*0.1;
	}
	else
		output.TexCoord = input.TexCoord;
	
    return output;
}

// G-Buffer output structure
struct GBuffer
{
    float4 Normal           : SV_Target0;
    float4 DiffuseAlbedo    : SV_Target1;
    float4 SpecularAlbedo   : SV_Target2;
    float4 Lighting         : SV_Target3;
};

//=================================================================================================
// Pixel Shader
//=================================================================================================
GBuffer PS(in PSInput input)
{
	float3 vtxNormal = normalize(input.NormalWS);

    // Normalize after interpolation
    float3 normalWS = vtxNormal;

    if(EnableNormalMapping)
    {
        //float3 normalTS = float3(0, 0, 1);
        //float3 tangentWS = normalize(input.TangentWS);
        //float3 binormalWS = normalize(input.BitangentWS);
        //float3x3 tangentToWorld = float3x3(tangentWS, binormalWS, normalWS);

        // Sample the normal map, and convert the normal to world space
        //normalTS.xyz = ShaderTextures[1].Sample(AnisoSampler, input.TexCoord).xyz * 2.0f - 1.0f;
        //normalTS.z = sqrt(1.0f - ((normalTS.x * normalTS.x) + (normalTS.y * normalTS.y)));
        //normalWS = normalize(mul(normalTS, tangentToWorld));

		float4 bumpMap;
		float3 bumpNormal;

        float3 tangentWS = normalize(input.TangentWS);
        float3 binormalWS = normalize(input.BitangentWS);

        // Sample the normal map, and convert the normal to world space
        bumpMap = ShaderTextures[1].Sample(AnisoSampler, input.TexCoord) * 2.0f - 1.0f;
        bumpNormal = normalWS + bumpMap.x * tangentWS + bumpMap.y * binormalWS;
        normalWS = normalize(bumpNormal);
    }

	if( mtt == 13 ){
		float LavaHeight = ShaderTextures[2].Sample(AnisoSampler, input.TexCoord).x;
		//input.TexCoord.x += timer * 0.005;
		if( LavaHeight > 0.25 ) 
			input.TexCoord.y += 0.5 * timer * (1-LavaHeight);
		
	}

    float4 texSample = ShaderTextures[0].Sample(AnisoSampler, input.TexCoord);
    float3 diffuseAlbedo = texSample.xyz;
	float3 specularAlbedo = SpecularAlbedo;

    diffuseAlbedo *= 1.0f - Balance;
    specularAlbedo *= Balance;
	
	if(mtt == 11)
	{
		GBuffer gBuffer;
		gBuffer.Normal = float4(normalWS * 0.5f + 0.5f, 1.0f);
		gBuffer.DiffuseAlbedo = float4(diffuseAlbedo*LightColor, 1.0f);
		gBuffer.SpecularAlbedo = float4(0.0f, 0.0f, 0.0f, 1.0f);
		gBuffer.Lighting = float4(LightColor, 1.0f);
		return gBuffer;
	}

	if(mtt == 15)
	{
		GBuffer gBuffer;
		gBuffer.Normal = float4(normalWS * 0.5f + 0.5f, 1.0f);
		gBuffer.DiffuseAlbedo = float4(diffuseAlbedo, 1.0f);
		gBuffer.SpecularAlbedo = float4(diffuseAlbedo, 1.0f);
		gBuffer.Lighting = float4(diffuseAlbedo, 1.0f);
		return gBuffer;
	}

	// Set the direction to the sky
	float3 Sd = float3(0.0f,1.0f,0.0f);
	// Set ground color
	float4 Gc = GroundColor;
	// Set sky color
	float4 Sc = SkyColor;
	// Set the intensity of the hemisphere color
	float Hi = HemiIntensity;

	float VecHemi = (dot(normalWS, Sd) * 0.5f ) + 0.5f;
	float4 Hemi = Hi* lerp(Gc, Sc, VecHemi);
	float3 OldAmbient = ConstAmbient.xyz;
	float3 HemiFinal = OldAmbient * Hemi.xyz;
	
	float3 lighting = 0.0f;
    lighting +=  HemiFinal * (1.0f / 3.14159f) * diffuseAlbedo * 0.25f;	

    GBuffer gBuffer;
    gBuffer.Normal = float4(normalWS * 0.5f + 0.5f, 1.0f);
    gBuffer.DiffuseAlbedo = float4(diffuseAlbedo, input.Coverage / 255.0f);
    gBuffer.SpecularAlbedo = float4(specularAlbedo, Roughness);
    gBuffer.Lighting = float4(lighting, 1.0f);
    return gBuffer;
}