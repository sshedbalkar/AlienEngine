///////////////////////////////////////////////////////////////////////////////////////
//
//	Camera.cpp
//	Authors: 
//	Copyright 2010, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////////////
#include "Precompiled.h"
#include "Camera.h"
#include "Graphics.h"
#include <iostream>

namespace Framework
{
	Camera::Camera()
	{
		transform = NULL;
	}

	Camera::~Camera()
	{
		GRAPHICS->PopCamera();
	}

	void Camera::Initialize()
	{
		transform = GetOwner()->has(Transform);
		GRAPHICS->PushCamera(this);
	}


}