////////////////////////////////////////////////////////////////////////////////
// Filename: bitmapclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _BITMAPCLASS_H_
#define _BITMAPCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "TextureArrayClass.h"
namespace Framework {

////////////////////////////////////////////////////////////////////////////////
// Class name: BitmapClass
////////////////////////////////////////////////////////////////////////////////
class BitmapClass
{
private:
	/*struct VertexType
	{
		D3DXVECTOR3 position;
	    D3DXVECTOR2 texture;
	};*/

public:
	BitmapClass();
	BitmapClass(const BitmapClass&);
	~BitmapClass();

	bool Initialize(ID3D11Device*, float, float, char*, float, float);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, float, float);

	int GetIndexCount();
	ID3D11ShaderResourceView** GetTextureArray();
	void SetDimension( const float&, const float& );

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	bool UpdateBuffers(ID3D11DeviceContext*, float, float);
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, char*);
	void ReleaseTexture();

private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
	TextureArrayClass* m_TextureArray;
	float m_screenWidth, m_screenHeight;
	float m_bitmapWidth, m_bitmapHeight;
	float m_previousPosX, m_previousPosY;

};
}
#endif