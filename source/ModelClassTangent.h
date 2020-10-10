////////////////////////////////////////////////////////////////////////////////
// Filename: ModelClassTangent.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MODELCLASSTANGENT_H_
#define _MODELCLASSTANGENT_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>
#include <fstream>
using namespace std;


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "ModelClass.h"
#include "Component.h"

namespace Framework {

const int TEXTURE_REPEAT = 8;

////////////////////////////////////////////////////////////////////////////////
// Class name: ModelClassTangent
////////////////////////////////////////////////////////////////////////////////
class ModelClassTangent : public ModelClass
{
private:

	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
		D3DXVECTOR3 tangent;
		D3DXVECTOR3 binormal;
	};

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
		float tx, ty, tz;
		float bx, by, bz;
	};

	struct TempVertexType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	struct VectorType
	{
		float x, y, z;
	};

public:

	ModelClassTangent();
	ModelClassTangent(const ModelClassTangent&);
	~ModelClassTangent();
	
	bool Initialize(ID3D11Device*, char*);
	
	void SetAndDraw(ID3D11DeviceContext*, D3DXVECTOR3 Scale);
	unsigned int GetIndexCount();

private:	
	void Shutdown();
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadModel(char*);
	void ReleaseModel();

	void CalculateTextureCoordinates();
	void CalculateModelVectors();
	void CalculateTangentBinormal(TempVertexType, TempVertexType, TempVertexType, VectorType&, VectorType&);
	void CalculateNormal(VectorType, VectorType, VectorType&);

private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	unsigned m_vertexCount, m_indexCount;
	ModelType* m_model;
};
}
#endif