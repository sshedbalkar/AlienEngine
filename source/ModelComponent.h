#pragma once
#include "Composition.h"
#include "VMath.h"
#include "lightclass.h"

namespace Framework
{

class Transform;
class ModelClass;
class TextureArrayClass;
class AnimationController;

class ModelComponent : public GameComponent
{	
public:
	ModelComponent * Next;
	ModelComponent * Prev;
	ModelComponent();
	virtual ~ModelComponent();

	virtual void Serialize( ISerializer& stream );
	virtual void Initialize();
	virtual void Free();
	virtual void RemoveComponentFromGame();
	virtual void RestoreComponentToGame();
	void Draw(ID3D11DeviceContext* deviceContext, float dt, const D3DXMATRIX&, const D3DXMATRIX&, const D3DXMATRIX&, ID3D11ShaderResourceView** textureArray, const D3DXVECTOR3& cameraPosition, const std::vector<LightClass>&);//Draw the sprite to the screen
	void SwapTexture();
	void ChangeToTexture(std::string & s);
	void DefaultTexture();
	void ChangeTextureDebug( );
	void ChangeTextureBack( );
	
///private:
	Transform* transform;
	std::string ModelName;
	std::string TextureName;
	ModelClass* pModel;
	TextureArrayClass* pTextureArray;
	
	std::string SwapTextureName;
	std::string InitialSwapTextureName;
	std::string InitialTextureName;
	float Transparency;
	float TextMultiplier;
	
	float timer;//lava
	
	AnimationController* animationController;
};

}