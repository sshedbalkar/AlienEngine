////////////////////////////////////////////////////////////////////////////////
// Filename: bumpmapshaderclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Precompiled.h"
#include "lightclass.h"
#include "bumpmapshaderclass.h"
#include "TimeMechanicManager.h"

namespace Framework {

BumpMapShaderClass::BumpMapShaderClass()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
	m_matrixBuffer2 = 0;
	m_cameraBuffer = 0;
	m_scaleBuffer = 0;
	m_sampleStateWrap = 0;
	m_shadowMapSampler = 0;
	m_lightBuffer = 0;
	m_attBuffer = 0;
	m_scBuffer = 0;
	m_lightPositionBuffer = 0;
	m_mttBuffer = 0;
	for(unsigned LightIndex = 0 ; LightIndex < MAX_NUM_LIGHTS ; ++LightIndex)
	{
		depthTextures[LightIndex] = 0;
	}
}


BumpMapShaderClass::BumpMapShaderClass(const BumpMapShaderClass& other)
{
}


BumpMapShaderClass::~BumpMapShaderClass()
{
}


bool BumpMapShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;


	// Initialize the vertex and pixel shaders.
	result = InitializeShader(device, hwnd, "Shaders//bumpmap.vs", "Shaders//bumpmap.ps");
	if(!result)
	{
		return false;
	}

	return true;
}


void BumpMapShaderClass::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	ShutdownShader();

	return;
}


bool BumpMapShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, const D3DXMATRIX& worldMatrix, const D3DXMATRIX& viewMatrix, const
			      D3DXMATRIX& projectionMatrix, ID3D11ShaderResourceView** textureArray, const D3DXVECTOR3& cameraPosition, const D3DXVECTOR3& Scale, unsigned mtt, const std::vector<LightClass>& lights, float Transparency, float TextMultiplier)
{
	bool result;


	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, textureArray, cameraPosition, Scale, mtt, lights, Transparency, TextMultiplier);
	if(!result)
	{
		return false;
	}

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);

	return true;
}


bool BumpMapShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, char* vsFilename, char* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[5];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC matrixBufferDesc2;
	D3D11_BUFFER_DESC scaleBufferDesc;
    D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC attBufferDesc;
	D3D11_BUFFER_DESC lightPositionBufferDesc;
	D3D11_BUFFER_DESC mttBufferDesc;
	D3D11_BUFFER_DESC scBufferDesc;

	int bw;

	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

    // Compile the vertex shader code.
	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "BumpMapVertexShader", "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 
								   0, NULL, &vertexShaderBuffer, &errorMessage, NULL);
	if(FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// If there was  nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(hwnd, vsFilename, "Missing Shader File", MB_OK);
		}

		return false;
	}

    // Compile the pixel shader code.
	result = D3DX11CompileFromFile(psFilename, NULL, NULL, "BumpMapPixelShader", "ps_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 
								   0, NULL, &pixelShaderBuffer, &errorMessage, NULL);
	if(FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// If there was  nothing in the error message then it simply could not find the file itself.
		else
		{
			MessageBox(hwnd, psFilename, "Missing Shader File", MB_OK);
		}

		return false;
	}

    // Create the vertex shader from the buffer.
    result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, 
										&m_vertexShader);
	if(FAILED(result))
	{
		return false;
	}

    // Create the vertex shader from the buffer.
    result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, 
									   &m_pixelShader);
	if(FAILED(result))
	{
		return false;
	}

	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the ModelClassTangent and in the shader.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;
	
	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	polygonLayout[3].SemanticName = "TANGENT";
	polygonLayout[3].SemanticIndex = 0;
	polygonLayout[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[3].InputSlot = 0;
	polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[3].InstanceDataStepRate = 0;

	polygonLayout[4].SemanticName = "BINORMAL";
	polygonLayout[4].SemanticIndex = 0;
	polygonLayout[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[4].InputSlot = 0;
	polygonLayout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[4].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
    numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), 
									   vertexShaderBuffer->GetBufferSize(), &m_layout);
	if(FAILED(result))
	{
		return false;
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// Create a texture sampler state description.
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	// Create the texture sampler state.
    result = device->CreateSamplerState(&samplerDesc, &m_sampleStateWrap);
	if(FAILED(result))
	{
		return false;
	}

	D3D11_SAMPLER_DESC descSampler;
	descSampler.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	descSampler.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;// D3D11_TEXTURE_ADDRESS_BORDER;
	descSampler.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;// D3D11_TEXTURE_ADDRESS_BORDER;
	descSampler.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;// D3D11_TEXTURE_ADDRESS_BORDER;
	descSampler.MipLODBias = 0;
	descSampler.MaxAnisotropy = 1;
	descSampler.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	descSampler.BorderColor[0] = 1.0f;
	descSampler.BorderColor[1] = 1.0f;
	descSampler.BorderColor[2] = 1.0f;
	descSampler.BorderColor[3] = 1.0f;
	descSampler.MinLOD = 0.0f;
	descSampler.MaxLOD = 0.0f;
	result = device->CreateSamplerState(&descSampler, &m_shadowMapSampler);
	if (FAILED(result))
		return result;

    // Setup the description of the matrix dynamic constant buffer that is in the vertex shader.
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	// Create the matrix constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if(FAILED(result)){return false;}

	// Setup the description of the matrix dynamic constant buffer that is in the vertex shader.
    matrixBufferDesc2.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc2.ByteWidth = sizeof(MatrixBufferType2);
    matrixBufferDesc2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc2.MiscFlags = 0;
	matrixBufferDesc2.StructureByteStride = 0;
	// Create the matrix constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&matrixBufferDesc2, NULL, &m_matrixBuffer2);
	if(FAILED(result)){return false;}

	// Setup the description of the scale dynamic constant buffer that is in the vertex shader.
	scaleBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	scaleBufferDesc.ByteWidth = sizeof(ScaleBufferType);
	scaleBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	scaleBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	scaleBufferDesc.MiscFlags = 0;
	scaleBufferDesc.StructureByteStride = 0;
	// Create the scale constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&scaleBufferDesc, NULL, &m_scaleBuffer);
	if(FAILED(result)){return false;}

	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bw = sizeof(LightBufferType);
	lightBufferDesc.ByteWidth = bw;
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&lightBufferDesc, NULL, &m_lightBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	attBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bw = sizeof(AttBufferType);
	attBufferDesc.ByteWidth = bw;
	attBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	attBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	attBufferDesc.MiscFlags = 0;
	attBufferDesc.StructureByteStride = 0;
	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&attBufferDesc, NULL, &m_attBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	scBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bw = sizeof(ScBufferType);
	scBufferDesc.ByteWidth = bw;
	scBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	scBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	scBufferDesc.MiscFlags = 0;
	scBufferDesc.StructureByteStride = 0;
	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&scBufferDesc, NULL, &m_scBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the description of the dynamic constant buffer that is in the vertex shader.
	lightPositionBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightPositionBufferDesc.ByteWidth = sizeof(LightPositionBufferType);
	lightPositionBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightPositionBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightPositionBufferDesc.MiscFlags = 0;
	lightPositionBufferDesc.StructureByteStride = 0;
	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&lightPositionBufferDesc, NULL, &m_lightPositionBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the description of the dynamic constant buffer that is in the pixel shader.
	mttBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	mttBufferDesc.ByteWidth = sizeof(MttBufferType);
	mttBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	mttBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	mttBufferDesc.MiscFlags = 0;
	mttBufferDesc.StructureByteStride = 0;
	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&mttBufferDesc, NULL, &m_mttBuffer);
	if(FAILED(result)){return false;}

	return true;
}


void BumpMapShaderClass::ShutdownShader()
{
	//Release the kraken!
	for(unsigned LightIndex = 0 ; LightIndex < MAX_NUM_LIGHTS ; ++LightIndex)
	{
		if(depthTextures[LightIndex])
		{
			depthTextures[LightIndex]->Release();
			depthTextures[LightIndex] = 0;
		}
	}
	// Release the light constant buffers.
	if(m_lightBuffer)
	{
		m_lightBuffer->Release();
		m_lightBuffer = 0;
	}

	if(m_attBuffer)
	{
		m_attBuffer->Release();
		m_attBuffer = 0;
	}

	if(m_scBuffer)
	{
		m_scBuffer->Release();
		m_scBuffer = 0;
	}

	if(m_lightPositionBuffer)
	{
		m_lightPositionBuffer->Release();
		m_lightPositionBuffer = 0;
	}

	// Release the scale constant buffer.
	if(m_scaleBuffer)
	{
		m_scaleBuffer->Release();
		m_scaleBuffer = 0;
	}

	if(m_mttBuffer)
	{
		m_mttBuffer->Release();
		m_mttBuffer = 0;
	}

	// Release the sampler states.
	if(m_sampleStateWrap)
	{
		m_sampleStateWrap->Release();
		m_sampleStateWrap = 0;
	}

	if(m_shadowMapSampler)
	{
		m_shadowMapSampler->Release();
		m_shadowMapSampler = 0;
	}

	// Release the matrix constant buffer.
	if(m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	if(m_matrixBuffer2)
	{
		m_matrixBuffer2->Release();
		m_matrixBuffer2 = 0;
	}

	// Release the layout.
	if(m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// Release the pixel shader.
	if(m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// Release the vertex shader.
	if(m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	return;
}


void BumpMapShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, char* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;


	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for(i=0; i<bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, "Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}


bool BumpMapShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, 
					   D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView** textureArray, const D3DXVECTOR3& cameraPosition, const D3DXVECTOR3& Scale, unsigned mtt, const std::vector<LightClass>& lights, float Transparency, float TextMultiplier)
{
	HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int VSBufferNumber = 0;
	unsigned int PSBufferNumber = 0;
	MatrixBufferType* dataPtr;
	LightPositionBufferType* dataPtr1;
	ScaleBufferType* dataPtr2;
	LightBufferType* dataPtr3;	
	MttBufferType*  dataPtr4;
	MatrixBufferType2* dataPtr5;
	AttBufferType* dataPtr6;
	ScBufferType* dataPtr7;

	D3DXMATRIX ModelWorldView, ModelWorldViewInverse, ModelWorldViewInverseTranspose;
	ModelWorldView = worldMatrix * viewMatrix;
	D3DXVECTOR4 det = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
	D3DXMatrixInverse(&ModelWorldViewInverse, det, &ModelWorldView);
	D3DXMatrixTranspose(&ModelWorldViewInverseTranspose, &ModelWorldViewInverse);

	D3DXMATRIX g_LightTextureMatrix = D3DXMATRIX(0.5f, 0.0f, 0.0f, 0.f, 
												   0.0f, 0.5f, 0.0f, 0.0f,
												   0.0f, 0.0f, 1.0f, 0.0f,
												   0.5f, 0.5f, 0.0f, 1.0f);

	D3DXMATRIX CubeMapViewMatrix[6];

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result)){return false;}
	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	// Copy the matrices into the constant buffer.
	dataPtr->World = worldMatrix;
	dataPtr->WorldView = ModelWorldView;
	dataPtr->WorldViewProjection= ModelWorldView * projectionMatrix;
	dataPtr->WorldViewInverseTranspose = ModelWorldViewInverseTranspose;	
	// Unlock the constant buffer.
    deviceContext->Unmap(m_matrixBuffer, 0);	
	// Now set the matrix constant buffer in the vertex shader with the updated values.
    deviceContext->VSSetConstantBuffers(VSBufferNumber, 1, &m_matrixBuffer);
	// Set the position of the matrix constant buffer in the vertex shader.
	++VSBufferNumber;



	// Lock the scale constant buffer so it can be written to.
	result = deviceContext->Map(m_scaleBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result)){return false;}
	// Get a pointer to the data in the constant buffer.
	dataPtr2 = (ScaleBufferType*)mappedResource.pData;
	// Copy the scale position into the constant buffer.
	dataPtr2->Scale = Scale;
	dataPtr2->padding = 1.0f;
	// Unlock the scale constant buffer.
	deviceContext->Unmap(m_scaleBuffer, 0);
	// Now set the scale constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(VSBufferNumber, 1, &m_scaleBuffer);
	++VSBufferNumber;
	
	// Disregard females, acquire depth maps
	for(unsigned LightIndex = 0 ; LightIndex < lights.size() ; ++LightIndex)
	{
		depthTextures[LightIndex] = lights[LightIndex].GetDepthTexture();
	}

	// Set shader texture array resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 3, textureArray);
	//deviceContext->PSSetShaderResources(2, MAX_NUM_LIGHTS, depthTextures);
	
	// Lock the light constant buffer so it can be written to.
	result = deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result)){return false;}
	// Get a pointer to the data in the constant buffer.
	dataPtr3 = (LightBufferType*)mappedResource.pData;
	// Copy the lighting variables into the constant buffer.
	for(unsigned i = 0 ; i <lights.size() ; i++)
	{
		dataPtr3->diffuseColor[i] = lights[i].GetDiffuseColor();
		dataPtr3->ambientColor = lights[i].GetAmbientColor();
		dataPtr3->specularPower = lights[i].GetSpecularPower();				
	}
	// Unlock the constant buffer.
	deviceContext->Unmap(m_lightBuffer, 0);
	// Finally set the light constant buffer in the pixel shader with the updated values.
	deviceContext->PSSetConstantBuffers(PSBufferNumber, 1, &m_lightBuffer);
	++PSBufferNumber;


	// Lock the light constant buffer so it can be written to.
	result = deviceContext->Map(m_attBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result)){return false;}
	// Get a pointer to the data in the constant buffer.
	dataPtr6 = (AttBufferType*)mappedResource.pData;
	// Copy the lighting variables into the constant buffer.
	for(unsigned i = 0 ; i <lights.size() ; i++)
	{
		D3DXVECTOR4 tempAtt = D3DXVECTOR4(lights[i].GetAttenuation0(), lights[i].GetAttenuation1(), lights[i].GetAttenuation2(), 0.0f); 	
		dataPtr6->attenuation[i] = tempAtt;
	}
	// Unlock the constant buffer.
	deviceContext->Unmap(m_attBuffer, 0);
	// Finally set the light constant buffer in the pixel shader with the updated values.
	deviceContext->PSSetConstantBuffers(PSBufferNumber, 1, &m_attBuffer);
	++PSBufferNumber;

	// Lock the light position constant buffer so it can be written to.
	result = deviceContext->Map(m_mttBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result)){return false;}
	// Get a pointer to the data in the constant buffer.
	dataPtr4 = (MttBufferType*)mappedResource.pData;
	// Copy the light position variables into the constant buffer.
	dataPtr4->mtt = mtt;
	dataPtr4->Transparency = Transparency;
	dataPtr4->TextMultiplier = TextMultiplier;
	dataPtr4->NUM_LIGHTS = lights.size();
	/*if(TIMEMANAGER->GetTimeState() != TIME_NORMAL)
		dataPtr4->TimePaused = true;
	else
		dataPtr4->TimePaused = false;*/

	// Unlock the constant buffer.
	deviceContext->Unmap(m_mttBuffer, 0);
	// Finally set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(VSBufferNumber, 1, &m_mttBuffer);
	++VSBufferNumber;
	deviceContext->PSSetConstantBuffers(PSBufferNumber, 1, &m_mttBuffer);
	++PSBufferNumber;

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(m_matrixBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result)){return false;}
	// Get a pointer to the data in the constant buffer.
	dataPtr5 = (MatrixBufferType2*)mappedResource.pData;
	// Copy the matrices into the constant buffer.
	dataPtr5->World = worldMatrix;
	dataPtr5->WorldView = ModelWorldView;
	dataPtr5->WorldViewProjection= ModelWorldView * projectionMatrix;
	dataPtr5->WorldViewInverseTranspose = ModelWorldViewInverseTranspose;	
	for(unsigned LightIndex = 0; LightIndex < lights.size() ; ++LightIndex)
	{
		lights[LightIndex].GetCubeMapViewMatrix(CubeMapViewMatrix);
		for (int face = 0; face < 6; ++face)
		{
			dataPtr5->WorldLightProjectionTexture[LightIndex][face] = worldMatrix * CubeMapViewMatrix[face] * g_LightTextureMatrix;
		}
	}
	// Unlock the constant buffer.
    deviceContext->Unmap(m_matrixBuffer2, 0);	
	// Now set the matrix constant buffer in the vertex shader with the updated values.
    deviceContext->PSSetConstantBuffers(PSBufferNumber, 1, &m_matrixBuffer2);
	// Set the position of the matrix constant buffer in the vertex shader.
	++PSBufferNumber;

	// Lock the light position constant buffer so it can be written to.
	result = deviceContext->Map(m_lightPositionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result)){return false;}
	// Get a pointer to the data in the constant buffer.
	dataPtr1 = (LightPositionBufferType*)mappedResource.pData;
	// Copy the light position variables into the constant buffer.
	for(unsigned i = 0 ; i <lights.size() ; i++)
	{
		D3DXVECTOR4 temp;
		D3DXVec4Transform(&temp, &lights[i].GetPosition(), &viewMatrix);
		dataPtr1->lightPosition[i] = temp;
	}
	// Unlock the constant buffer.
	deviceContext->Unmap(m_lightPositionBuffer, 0);
	// Finally set the constant buffer in the vertex shader with the updated values.
	deviceContext->PSSetConstantBuffers(PSBufferNumber, 1, &m_lightPositionBuffer);
	++PSBufferNumber;


	// Lock the light constant buffer so it can be written to.
	result = deviceContext->Map(m_scBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result)){return false;}
	// Get a pointer to the data in the constant buffer.
	dataPtr7 = (ScBufferType*)mappedResource.pData;
	// Copy the lighting variables into the constant buffer.
	for(unsigned i = 0 ; i <lights.size() ; i++)
	{
		dataPtr7->specularColor = lights[i].GetSpecularColor();
	}
	// Unlock the constant buffer.
	deviceContext->Unmap(m_scBuffer, 0);
	// Finally set the light constant buffer in the pixel shader with the updated values.
	deviceContext->PSSetConstantBuffers(PSBufferNumber, 1, &m_scBuffer);
	++PSBufferNumber;
	
	return true;
}


void BumpMapShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(m_layout);

    // Set the vertex and pixel shaders that will be used to render this triangle.
    deviceContext->VSSetShader(m_vertexShader, NULL, 0);
    deviceContext->PSSetShader(m_pixelShader, NULL, 0);
	deviceContext->DSSetShader(NULL, NULL, 0);
    deviceContext->HSSetShader(NULL, NULL, 0);
    deviceContext->GSSetShader(NULL, NULL, 0);
	deviceContext->CSSetShader(NULL, NULL, 0);

	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &m_sampleStateWrap);
	deviceContext->PSSetSamplers(1, 1, &m_shadowMapSampler);

	// Render the triangles.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}
}