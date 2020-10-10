///////////////////////////////////////////////////////////////////////////////////////
///
///	\file Camera.h
///	Defines Camera Game Component
///	
///	Authors: 
///	Copyright 2010, Digipen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////////////
#pragma once //Makes sure this header is only included once

#include "Component.h"

namespace Framework
{	

	class Transform;
	///Simple orthographic camera for viewing the game world.
	///Depends on Transform.
	class Camera : public GameComponent
	{
	public:
		Camera();
		~Camera();
		virtual void Initialize();
		Transform * transform;
	};

}
