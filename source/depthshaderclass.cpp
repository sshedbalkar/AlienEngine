////////////////////////////////////////////////////////////////////////////////
// Filename: depthshaderclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Precompiled.h"
#include "depthshaderclass.h"
#include "lightclass.h"

namespace Framework {

DepthShaderClass::DepthShaderClass()
{
	m_vertexShader = 0;
	m_geometryShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
	m_CMVMBuffer = 0;
}


DepthShaderClass::DepthShaderClass(const DepthShaderClass& other)
{
}


DepthShaderClass::~DepthShaderClass()
{
}


bool DepthShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;


	// Initialize the vertex and pixel shaders.
	result = InitializeShader(device, hwnd, "Shaders//depth.vs", "Shaders//depth.gs");
	if(!result)
	{
		return false;
	}

	return true;
}


void DepthShaderClass::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	ShutdownShader();

	return;
}


bool DepthShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, 
							  D3DXMATRIX projectionMatrix, const std::vector<LightClass>& lights, float LightIndex)
{
	bool result;


	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, lights, LightIndex);
	if(!result)
	{
		return false;
	}

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);

	return true;
}


bool DepthShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, char* vsFilename, char* gsFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* geometryShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[1];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC CMVMBufferDesc;


	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	geometryShaderBuffer = 0;

    // Compile the vertex shader code.
	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "DepthVertexShader", "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
								   &vertexShaderBuffer, &errorMessage, NULL);
	if(FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(hwnd, vsFilename, "Missing Shader File", MB_OK);
		}

		return false;
	}

	// Compile the geometry shader code.
	result = D3DX11CompileFromFile(gsFilename, NULL, NULL, "DepthGeometryShader", "gs_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
								   &geometryShaderBuffer, &errorMessage, NULL);
	if(FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, gsFilename);
		}
		// If there was nothing in the error message then it simply could not find the file itself.
		else
		{
			MessageBox(hwnd, gsFilename, "Missing Shader File", MB_OK);
		}

		return false;
	}

 
    // Create the vertex shader from the buffer.
    result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if(FAILED(result))
	{
		return false;
	}

	// Create the geometry shader from the buffer.
	result = device->CreateGeometryShader(geometryShaderBuffer->GetBufferPointer(), geometryShaderBuffer->GetBufferSize(), NULL, &m_geometryShader);
	if(FAILED(result))
	{
		return false;
	}


	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;	

	// Get a count of the elements in the layout.
    numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
	if(FAILED(result))
	{
		return false;
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	geometryShaderBuffer->Release();
	geometryShaderBuffer = 0;

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
    CMVMBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	CMVMBufferDesc.ByteWidth = sizeof(CMVMBufferType);
    CMVMBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    CMVMBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    CMVMBufferDesc.MiscFlags = 0;
	CMVMBufferDesc.StructureByteStride = 0;
	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&CMVMBufferDesc, NULL, &m_CMVMBuffer);
	if(FAILED(result))
	{
		return false;
	}

	return true;
}


void DepthShaderClass::ShutdownShader()
{
	// Release the matrix constant buffer.
	if(m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	// Release the cube map view matrix constant buffer.
	if(m_CMVMBuffer)
	{
		m_CMVMBuffer->Release();
		m_CMVMBuffer = 0;
	}

	// Release the layout.
	if(m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// Release the geometry shader.
	if(m_geometryShader)
	{
		m_geometryShader->Release();
		m_geometryShader = 0;
	}

	// Release the vertex shader.
	if(m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	return;
}


void DepthShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, char* shaderFilename)
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


bool DepthShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, const std::vector<LightClass>& lights, float LightIndex)
{
	HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;
	MatrixBufferType* dataPtr;
	CMVMBufferType* dataPtr2;

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}
	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	// Copy the matrices into the constant buffer.
	dataPtr->World = worldMatrix;
	// Unlock the constant buffer.
    deviceContext->Unmap(m_matrixBuffer, 0);
	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;
	// Now set the constant buffer in the vertex shader with the updated values.
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(m_CMVMBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}
	// Get a pointer to the data in the constant buffer.
	dataPtr2 = (CMVMBufferType*)mappedResource.pData;

	D3DXMATRIX CubeMapViewMatrix[6];
	lights[LightIndex].GetCubeMapViewMatrix(CubeMapViewMatrix);

	// Copy the matrices into the constant buffer.
	for (int index = 0; index < 6; ++index)
	{
		dataPtr2->viewM[index] = CubeMapViewMatrix[index];
	}
	// Unlock the constant buffer.
    deviceContext->Unmap(m_CMVMBuffer, 0);
	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;
	// Now set the constant buffer in the vertex shader with the updated values.
    deviceContext->GSSetConstantBuffers(bufferNumber, 1, &m_CMVMBuffer);



	return true;
}


void DepthShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(m_layout);

    // Set the vertex and pixel shaders that will be used to render this triangle.
    deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->GSSetShader(m_geometryShader, NULL, 0);
    deviceContext->PSSetShader(NULL, NULL, 0);
	deviceContext->DSSetShader(NULL, NULL, 0);
    deviceContext->HSSetShader(NULL, NULL, 0);
	deviceContext->CSSetShader(NULL, NULL, 0);
	
	// Render the triangle.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}
}