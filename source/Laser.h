#pragma once //Makes sure this header is only included once
#include "Composition.h"
#include <iostream>
#include "Physics.h"
#include "Transform.h"

namespace Framework
{
	
	class Laser : public GameComponent
	{
		friend class LoadLevelClass;
	public:
		Laser();
		~Laser();

		virtual void SendMessage(Message *);
		virtual void Initialize();
		virtual void Serialize(ISerializer& stream);
		virtual void RemoveComponentFromGame();
		void Update(const float &);
		void intersect();
		Vector3 GetDirection(){return Vector3(m_dir);}
	private:
		Vector3 m_dir,m_endpoint,m_position;
		GOC * m_CollidedWith;
		GOC *laserLight;
		//RigidBody * m_Body;
		Transform * m_Transform;
		//Vector3 m_InitialPos;
	};
}