///////////////////////////////////////////////////////////////////////////////////////
///
///	\file GameLogic.h
///	Sample game logic for demo.
///	
///	Authors: , 
///	Copyright 2010, Digipen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////////////

#pragma once //Makes sure this header is only included once

#include "Physics.h"
#include "Engine.h"
#include "GameData.h"
#include "VMath.h"
#include "Graphics.h"
#include "Composition.h"
//
namespace Framework
{
	struct ConstantParameters
	{
		float BloomThreshold;
		float BloomMagnitude;
		float BloomBlurSigma;
		float AdaptationRate;
		float ExposureKeyValue;
		float SpecularIntensity;
		float SpecularR;
		float SpecularG;
		float SpecularB;
		float Balance;
		float Roughness;
		Vec4 ConstAmbient;
		Vec4 SkyColor;
		Vec4 GroundColor;
		float HemiIntensity;
	};

	struct GlowCylindersParameters
	{
		int parentActicationId;
		
		GlowCylindersParameters()
		{
			parentActicationId=-1;
		}

	};

	struct LightParameters
	{
		Vec3 Color;
		float Fallout;
		bool CanMove;
		
		LightParameters()
		{
			Color=Vec3(0,0,0);
			Fallout=0.0f;
			CanMove=false;
		}

	};

	struct ConnectorParameters
	{
		bool IsVectical;
		bool AllowStaticCollision;
		
		ConnectorParameters()
		{
			IsVectical=false;
			AllowStaticCollision=false;
		}

	};

	struct ConstraintParameters
	{
		int ConstraintID;
		int ConstraintAttachedTo;
		Vec3 Positionrelative;
		Vec3 Multiplier;
		int IsInsideBody;
		double err;

		ConstraintParameters()
		{
			ConstraintID=ConstraintAttachedTo=-1;
			Positionrelative=Vec3(0,0,0);
			err=0;
			IsInsideBody=0;
			Multiplier=Vec3(0,0,0);
		}

	};

	struct AutoMovingParameters
	{
		float MovingSpeed;
		float RotationSpeed;
		Vec3 RotationPoint;
		Vec3 RotationAxis;
		Vec3 MovingAxis;
		int AutoCanRotate;
		int AutoCanMove;
		AutoMovingParameters()
		{
			MovingSpeed=0.0f;
			RotationSpeed=0.0f;
			RotationPoint=Vec3(0,0,0);
			RotationAxis=Vec3(0,0,0);
			MovingAxis=Vec3(0,0,0);
			AutoCanMove=AutoCanRotate=1;
		}

	};

	struct ActivationPropertiesParameters
	{
		int AttachedTo;
		int RestrictedBy;
		bool NeverActivate;
		int ObjectActivationId;
		Vec4 ActivatedBy;
		Vec3 PointingTo;
		float ShootingSpeed;
		int OneTimeActivated;
		float TimeToStayActivated;
		bool CanMove;

		ActivationPropertiesParameters()
		{
			ObjectActivationId = 0;
			AttachedTo=0;
			NeverActivate=false;
			RestrictedBy=0;
			ActivatedBy=Vec4(0,0,0,0);
			PointingTo=Vec3(0,0,0);
			TimeToStayActivated=0.0f;
			ShootingSpeed=0.0f;


		}

	};
	struct RopeParameters
	{
		int AttachedTo;
		int Id;
		bool isStart;

		RopeParameters()
		{
			AttachedTo=0;
			Id=0;
			isStart=false;
		}

	};

	struct ObjectParameters
	{
		std::string Arch_id;
		Vec3 Position,Scale;
		D3DXQUATERNION Quaternion;
		int GlobalID;
		ConstraintParameters ConstraintParam;
		AutoMovingParameters AutoMovingPar;
		ActivationPropertiesParameters ActivationParameters;
		RopeParameters RopePar;
		ConnectorParameters ConnectorParams;
		LightParameters LightParams;
		GlowCylindersParameters GlowCylindersParams;
		int BoxLevelId;
		

		ObjectParameters()
		{
			GlobalID=-1;
			BoxLevelId=-1;
			Position = Vec3(0,0,0);
			Scale = Vec3(1,1,1);
			Quaternion = D3DXQUATERNION(0,0,0,1);
		}
	};


	
	class LoadLevelClass
	{
	public:	
		LoadLevelClass();
		~LoadLevelClass();
		void Initialize();
		void loadGameSettings();
		void LoadLevelFile(const std::string& file);
		GOC* CreateObjectAt(ObjectParameters parameters);
		void freeLogicData();
		void AddGOCtoLogicContainer(GOC*);
		void ActionsAfterAllObjectsLoaded();
		void CreateLight(GOC*);
		
	private:
		GameData* _gameData;
		std::map<int,GOC *> ObjectsThatHaveConstraints;
		std::vector<GOC *> ConstraintsObjects;

	};

	//A global pointer to our game so that it can be accessed from anywhere.
	extern LoadLevelClass* LevelClass;
}


