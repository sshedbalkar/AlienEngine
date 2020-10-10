#pragma once

#include "Component.h"

namespace Framework
{
class Mesh;
class Texture;
class Animation;
class Transform;

class MeshInstance : public GameComponent
{
public:
	float dt;
	bool looping;
	Transform* transform;
	Animation* animation;
	Texture* texture;
	int flag;
	Mesh* mesh;
	bool is_paused;

	MeshInstance* Next;
	MeshInstance* Prev;

public:
	MeshInstance();
	virtual ~MeshInstance();

	virtual void Serialize( ISerializer& stream );
	virtual void Initialize();
	virtual void Load();
	virtual void Unload();
	virtual void Free();

	void SetAnimation( const char* id, bool _looping );
	void SetTexture( const char* id );

};

}
