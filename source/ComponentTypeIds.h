///////////////////////////////////////////////////////////////////////////////
//
//	ComponentTypeIds.h
//	Global Enumeration for component type ids.	
//
//	Authors: ,
//	Copyright 2010, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////


#pragma once

namespace Framework
{
	enum GraphicsState
	{
		STATE_SCENE=0,
		STATE_PREFREEZING,
		STATE_FREEZING,
		STATE_MENU,
		STATE_UNFREEZING
	};


	//A simple enumeration used to identify components quickly and efficiently.
	//There is alternative methods for generating ids automatically, 
	//(string hashing, static type ids).
	//But this is a clean, easy, understandable and stable way.
	/*
	enum ComponentTypeId
	{
		//Invalid component id
		CT_None = 0,
		//Basic Components
		CT_Transform,
		//Graphics
		CT_Camera,
		CT_Sprite,
		CT_VisualPlatform,
		CT_MeshInstance,
		CT_ParticleEmitter,
		CT_ScriptManager,
		//Physics
		CT_Body,
		//Game Logic
		CT_InteractiveObjProperties,
		CT_StoreMessage,
		CT_HeroProperties,
		CT_AI_Type1,
		CT_AI_Type2,
		//debug always last
		CT_DebugGeometry, 
		CT_ModelComponent, 
		//Max
		CT_MaxComponents
	};

	enum CompositionTypeId
	{
		//Invalid component id
		GOC_Type_Hero=0,
		GOC_Type_ButtonPlatform1,
		GOC_Type_ButtonPlatform2,
		GOC_Type_Platform1,
		GOC_Type_Platform_Death,
		GOC_Type_Mirror,
		GOC_Type_Pillar,
		GOC_Type_Enemy1,
		GOC_Type_Door1,
		GOC_Type_Door2,
		GOC_Type_MovingBox1,
		GOC_Type_InvisibleBox1,
		GOC_Type_InvisibleBoxOfDeath,
		GOC_Type_WaterPlatform,
		GOC_Type_Boss1,
		GOC_Type_SpecialMovingPlatform
	};
	*/
	//////////////////////////////////////////////////
	#define MCR_REGISTER_COMPID(x) x,
	typedef enum
	{
		CT_INVALID = 0,
		#include "ComponentTypeIdsName.h"
		CT_MAX
	} ComponentTypeId;
	#undef MCR_REGISTER_COMPID
	///////////////////////////////////////////////////
	#define MCR_REGISTER_COMPID(x) #x,
	static const char* ComponentIdText[] =
	{
		"CT_Invalid",
		#include "ComponentTypeIdsName.h"
		"CT_Max_Num"
	};
	#undef MCR_REGISTER_COMPID
	///////////////////////////////////////////////////
	//////////////////////////////////////////////////
	#define MCR_REGISTER_COMPSID(x) x,
	typedef enum
	{
		GOC_Type_INVALID = 0,
		#include "CompositionTypeIdsName.h"
		GOC_Type_MAX
	} CompositionTypeId;
	#undef MCR_REGISTER_COMPSID
	///////////////////////////////////////////////////
	#define MCR_REGISTER_COMPSID(x) #x,
	static const char* CompositionIdText[] =
	{
		"GOC_Type_Invalid",
		#include "CompositionTypeIdsName.h"
		"GOC_Type_Invalid_Max_Num"
	};
	#undef MCR_REGISTER_COMPSID
	///////////////////////////////////////////////////
}