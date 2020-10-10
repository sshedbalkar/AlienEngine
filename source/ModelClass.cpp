///////////////////////////////////////////////////////////////////////////////
///
///	 \file ModelClass.cpp
///  Defines the Virtual Model.
///
///	 Authors: Chia-Wei Wu
///  Copyright (c) 2012, Digipen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////
#include "Precompiled.h"
#include "ModelClass.h"
//#include "ModelInfo.h"
#include "Graphics.h"
#include "d3dclass.h"

namespace Framework {

ModelClass::ModelClass() : 
animationContent(NULL)
{
	//modelInfo1 = new ModelInfo ;
}
ModelClass::~ModelClass(){
	//SafeDelete( animationContent );
	if(animationContent){
		delete animationContent;
		animationContent = NULL;
	}
}

}

