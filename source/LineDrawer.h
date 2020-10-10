#pragma once
//
#include <vector>
//
namespace Framework
{

class LineVertex{
public:
	D3DXVECTOR3 position;
	D3DXVECTOR4 color;
	inline LineVertex() {}
	inline LineVertex( const D3DXVECTOR3& _position, const D3DXVECTOR4& _color ) : position(_position), color(_color) {}
};

class LineDrawer{
public:
	std::vector<LineVertex> drawLineList;
	//void DrawLine(D3DXVECTOR3, D3DXVECTOR3, D3DXVECTOR4, const D3DMATRIX&, const D3DMATRIX&, const D3DMATRIX&);
	void DrawLineFromList(const D3DMATRIX&, const D3DMATRIX&, const D3DMATRIX&);
};

}