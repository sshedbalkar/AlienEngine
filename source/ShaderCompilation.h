////////////////////////////////////////////////////////////////////////////////
// Filename: postprocessorbase.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>
#define _XM_NO_INTRINSICS_
#include <xnamath.h>

namespace Framework
{

// Compiles a shader from file and returns the compiled bytecode
ID3D10Blob* CompileShader(LPCWSTR path,
                            LPCSTR functionName,
                            LPCSTR profile,
                            CONST D3D_SHADER_MACRO* defines = NULL,
                            ID3DInclude* includes = NULL);

void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, char* shaderFilename);

// Compiles a shader from file and creates the appropriate shader instance
//ID3D11VertexShader* CompileVSFromFile(ID3D11Device* device,
//									  char* vsFilename, 
//									  char* fnName, 
//									  char* version, 
//									  HWND hwnd,
//									  ID3D10Blob** vsBuffer,
//									  CONST D3D_SHADER_MACRO* defines = NULL);

ID3D11VertexShader* CompileVSFromFile(ID3D11Device* device,
                                        LPCWSTR path,
                                        LPCSTR functionName = "VS",
                                        LPCSTR profile = "vs_5_0",
                                        CONST D3D_SHADER_MACRO* defines = NULL,
                                        ID3DInclude* includes = NULL,
                                        ID3DBlob** byteCode = NULL);

ID3D11PixelShader* CompilePSFromFile(ID3D11Device* device,
                                        LPCWSTR path,
                                        LPCSTR functionName = "PS",
                                        LPCSTR profile = "ps_5_0",
                                        CONST D3D_SHADER_MACRO* defines = NULL,
                                        ID3DInclude* includes = NULL);

ID3D11GeometryShader* CompileGSFromFile(ID3D11Device* device,
                                        LPCWSTR path,
                                        LPCSTR functionName = "GS",
                                        LPCSTR profile = "gs_5_0",
                                        CONST D3D_SHADER_MACRO* defines = NULL,
                                        ID3DInclude* includes = NULL);

ID3D11HullShader* CompileHSFromFile(ID3D11Device* device,
                                    LPCWSTR path,
                                    LPCSTR functionName = "HS",
                                    LPCSTR profile = "hs_5_0",
                                    CONST D3D_SHADER_MACRO* defines = NULL,
                                    ID3DInclude* includes = NULL);

ID3D11DomainShader* CompileDSFromFile(ID3D11Device* device,
                                        LPCWSTR path,
                                        LPCSTR functionName = "DS",
                                        LPCSTR profile = "ds_5_0",
                                        CONST D3D_SHADER_MACRO* defines = NULL,
                                        ID3DInclude* includes = NULL);

ID3D11ComputeShader* CompileCSFromFile(ID3D11Device* device,
                                        LPCWSTR path,
                                        LPCSTR functionName = "CS",
                                        LPCSTR profile = "cs_5_0",
                                        CONST D3D_SHADER_MACRO* defines = NULL,
                                        ID3DInclude* includes = NULL);


class CompileOptions
{
public:

    // constants
    static const UINT32 MaxDefines = 16;
    static const UINT32 BufferSize = 1024;

    CompileOptions();

    void Add(const std::string& name, UINT32 value);
    void Reset();

    const D3D10_SHADER_MACRO* Defines() const;

protected:

    D3D_SHADER_MACRO defines[MaxDefines + 1];
    CHAR buffer[BufferSize];
    UINT32 numDefines;
    UINT32 bufferIdx;
};

}

