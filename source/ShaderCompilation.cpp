////////////////////////////////////////////////////////////////////////////////
// Filename: postprocessor.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Precompiled.h"
#include "ShaderCompilation.h"
#include <fstream>

using namespace std;

namespace Framework
{

ID3D10Blob* CompileShader(LPCWSTR path,
                            LPCSTR functionName,
                            LPCSTR profile,
                            CONST D3D_SHADER_MACRO* defines,
                            ID3DInclude* includes)
{
    // Loop until we succeed, or an exception is thrown
    while (true)
    {

        UINT flags = 0;
        #ifdef _DEBUG
            flags |= D3D10_SHADER_DEBUG|D3D10_SHADER_SKIP_OPTIMIZATION|D3D10_SHADER_WARNINGS_ARE_ERRORS;
        #endif

        ID3D10Blob* compiledShader;
        ID3D10Blob* errorMessages;
       
		if (FAILED(D3DX11CompileFromFileW(path, defines, includes, functionName, profile,
            flags, 0, NULL, &compiledShader, &errorMessages, NULL)))
        {
            if (errorMessages)
            {
                WCHAR message[1024];
                message[0] = NULL;
                char* blobdata = reinterpret_cast<char*>(errorMessages->GetBufferPointer());

                MultiByteToWideChar(CP_ACP, 0, blobdata, static_cast<int>(errorMessages->GetBufferSize()), message, 1024);
                std::wstring fullMessage = L"Error compiling shader file \"";
                fullMessage += path;
                fullMessage += L"\" - ";
                fullMessage += message;

                #ifdef _DEBUG
                    // Pop up a message box allowing user to retry compilation
                    int retVal = MessageBoxW(NULL, fullMessage.c_str(), L"Shader Compilation Error", MB_RETRYCANCEL);
                    if(retVal != IDRETRY)
                        __debugbreak();
                #else
                    __debugbreak();
                #endif
            }
            else
            {
                _ASSERT(false);
                __debugbreak();
            }
        }
        else
            return compiledShader;
    }
}

void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, char* shaderFilename)
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

ID3D11VertexShader* CompileVSFromFile(ID3D11Device* device,
                                        LPCWSTR path,
                                        LPCSTR functionName,
                                        LPCSTR profile,
                                        CONST D3D_SHADER_MACRO* defines,
                                        ID3DInclude* includes,
                                        ID3DBlob** byteCode)
{
	ID3DBlob* compiledShader = CompileShader(path, functionName, profile, defines, includes);
    ID3D11VertexShader* shader = NULL;
    if(FAILED(device->CreateVertexShader(compiledShader->GetBufferPointer(),
        compiledShader->GetBufferSize(),
        NULL,
        &shader)))
		__debugbreak();

    if (byteCode != NULL)
        *byteCode = compiledShader;

    return shader;
}

ID3D11PixelShader* CompilePSFromFile(ID3D11Device* device,
                                        LPCWSTR path,
                                        LPCSTR functionName,
                                        LPCSTR profile,
                                        CONST D3D_SHADER_MACRO* defines,
                                        ID3DInclude* includes)
{
    ID3DBlob* compiledShader = CompileShader(path, functionName, profile, defines, includes);
    ID3D11PixelShader* shader = NULL;
    if(FAILED((device->CreatePixelShader(compiledShader->GetBufferPointer(),
        compiledShader->GetBufferSize(),
        NULL,
        &shader))))
		__debugbreak();

    return shader;
}

ID3D11GeometryShader* CompileGSFromFile(ID3D11Device* device,
                                        LPCWSTR path,
                                        LPCSTR functionName,
                                        LPCSTR profile,
                                        CONST D3D_SHADER_MACRO* defines,
                                        ID3DInclude* includes)
{
    ID3DBlob* compiledShader = CompileShader(path, functionName, profile, defines, includes);
    ID3D11GeometryShader* shader = NULL;
    
	if(FAILED((device->CreateGeometryShader(compiledShader->GetBufferPointer(),
        compiledShader->GetBufferSize(),
        NULL,
        &shader))))
		__debugbreak();

    return shader;
}

ID3D11HullShader* CompileHSFromFile(ID3D11Device* device,
                                    LPCWSTR path,
                                    LPCSTR functionName,
                                    LPCSTR profile,
                                    CONST D3D_SHADER_MACRO* defines,
                                    ID3DInclude* includes)
{
    ID3DBlob* compiledShader = CompileShader(path, functionName, profile, defines, includes);
    ID3D11HullShader* shader = NULL;
    if(FAILED((device->CreateHullShader(compiledShader->GetBufferPointer(),
        compiledShader->GetBufferSize(),
        NULL,
        &shader))))
		__debugbreak();

    return shader;
}

ID3D11DomainShader* CompileDSFromFile(ID3D11Device* device,
                                        LPCWSTR path,
                                        LPCSTR functionName,
                                        LPCSTR profile,
                                        CONST D3D_SHADER_MACRO* defines,
                                        ID3DInclude* includes)
{
    ID3DBlob* compiledShader = CompileShader(path, functionName, profile, defines, includes);
    ID3D11DomainShader* shader = NULL;
   
	if(FAILED((device->CreateDomainShader(compiledShader->GetBufferPointer(),
        compiledShader->GetBufferSize(),
        NULL,
        &shader))))
		__debugbreak();

    return shader;
}

ID3D11ComputeShader* CompileCSFromFile(ID3D11Device* device,
                                        LPCWSTR path,
                                        LPCSTR functionName,
                                        LPCSTR profile,
                                        CONST D3D_SHADER_MACRO* defines,
                                        ID3DInclude* includes)
{
    ID3DBlob* compiledShader = CompileShader(path, functionName, profile, defines, includes);
    ID3D11ComputeShader* shader = NULL;
    if(FAILED((device->CreateComputeShader(compiledShader->GetBufferPointer(),
        compiledShader->GetBufferSize(),
        NULL,
        &shader))))
		__debugbreak();

    return shader;
}

// == CompileOptions ==============================================================================

CompileOptions::CompileOptions()
{
    Reset();
}

void CompileOptions::Add(const std::string& name, UINT32 value)
{
    _ASSERT(numDefines < MaxDefines);

    defines[numDefines].Name = buffer + bufferIdx;
    for(UINT32 i = 0; i < name.length(); ++i)
        buffer[bufferIdx++] = name[i];
    ++bufferIdx;

    std::string stringVal;

	std::ostringstream stream;
    if (!(stream << value))
        __debugbreak();
    stringVal = stream.str();

    defines[numDefines].Definition = buffer + bufferIdx;
    for(UINT32 i = 0; i < stringVal.length(); ++i)
        buffer[bufferIdx++] = stringVal[i];
    ++bufferIdx;
    
    ++numDefines;
}

void CompileOptions::Reset()
{
    numDefines = 0;
    bufferIdx = 0;

    for(UINT32 i = 0; i < MaxDefines; ++i)
    {
        defines[i].Name = NULL;
        defines[i].Definition = NULL;
    }

    ZeroMemory(buffer, BufferSize);
}

const D3D_SHADER_MACRO* CompileOptions::Defines() const
{
    return defines;
}

}