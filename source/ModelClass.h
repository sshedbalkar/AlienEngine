///////////////////////////////////////////////////////////////////////////////
///
///	 \file ModelClass.h 
///  Defines the Virtual Model.
///
///	 Authors: Chia-Wei Wu
///  Copyright (c) 2012, Digipen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Animation.h"

namespace Framework {

//class ModelInfo;

class ModelClass{
public:
	friend class ModelComponent;
	friend class ModelComponent2;
	ModelClass();
	virtual ~ModelClass();
	virtual void SetAndDraw(ID3D11DeviceContext* deviceContext, D3DXVECTOR3 Scale) = 0;
	virtual unsigned int GetIndexCount() = 0;;
public:
	AnimationContent* animationContent;
	D3DXVECTOR3 Scale;
	//ModelInfo * modelInfo1;
};

}
