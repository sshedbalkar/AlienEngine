#include "Precompiled.h"
#include "GameLogic.h"
#include "Graphics.h"
#include "Camera.h"
#include "DebugTools.h"
#include "XMLSerialization.h"
#include "Utility.h"
#include "LoadLevelClass.h"
#include "debugGeometry.h"
#include "XMLSerialization.h"
#include <fstream>
#include "FileRW.h"
#include "ActivationProperties.h"
#include "Laser.h"
#include "DominoComponent.h"
#include "ConstraintsComponent.h"
#include "AutoMovingComponent.h"
#include "DebugComponent.h"
#include "RopeComponent.h"
#include "ConnectorComponent.h"
#include "LightComponent.h"
#include "lightclass.h"
#include "ParticleComponent.h"
#include "Global.h"
#include "GameHistory.h"
#include "TimeMechanicManager.h"
#include "GlowingCylinder.h"
#include "Level_1.h"




namespace Framework
{

	LoadLevelClass* LevelClass=NULL;

	#define DrawDebug false

	void LoadLevelClass::Initialize()
	{
		
		loadGameSettings();
		g_gameHistory->Initialize();
		TIMEMANAGER->Initialize();
		LOGIC->m_PowerGun.Initialize(); 
		if(EDITOR)
			EDITOR->Initialize();
		
		
	}

	LoadLevelClass::LoadLevelClass()
	{
		LevelClass=this;
	}

	LoadLevelClass::~LoadLevelClass()
	{
		delete _gameData;
	}
	void LoadLevelClass::freeLogicData()
	{
		LOGIC->m_componentManager.Initialize();
		LOGIC->objectPointedAt=0;
		
		PHYSICS->makePhysicsNormal();
		PHYSICS->constraints.clear();
		PHYSICS->joints.clear();
		PHYSICS->SortedAABBArrayX.clear();
		
		//temp code will remove later -- harsha
		LOGIC->testObj=0;
		LOGIC->testObj1=0;
		LOGIC->temp123=true;
		//GRAPHICS->DestroyAllLights(); moved to graphics unload
		if(EDITOR)
			EDITOR->Free();
	}

	GOC* LoadLevelClass::CreateObjectAt(ObjectParameters parameters)
	{
		GOC * newObject = FACTORY->BuildFromArchetype(parameters.Arch_id.c_str() );
		newObject->archId=parameters.Arch_id;
		newObject->GlobalId=parameters.GlobalID;
		newObject->LevelBoxId=parameters.BoxLevelId;
		
		newObject->CompositionTypeId=newObject->GetIdFromName(const_cast<char*>(parameters.Arch_id.c_str()));
		
		Transform *t=newObject->has(Transform);
		t->Position=parameters.Position ;
		t->Scale=parameters.Scale ;

		Quaternion a=Quaternion(parameters.Quaternion.w,parameters.Quaternion.x,parameters.Quaternion.y,parameters.Quaternion.z);
		a.normalise();
		t->QUATERNION=a.QuaternionToD3DXQUATERNION();

		if(newObject->has(CollisionPrimitive))
		{
			if(newObject->has(CollisionPrimitive)->Id==CollisionPrimitive::ShapeId::Box)
			{
				static_cast<CollisionBox*>(newObject->has(CollisionPrimitive))->halfSize.x=parameters.Scale.x/2.0;
				static_cast<CollisionBox*>(newObject->has(CollisionPrimitive))->halfSize.y=parameters.Scale.y/2.0;
				static_cast<CollisionBox*>(newObject->has(CollisionPrimitive))->halfSize.z=parameters.Scale.z/2.0;
			}else if(newObject->has(CollisionPrimitive)->Id==CollisionPrimitive::ShapeId::Sphere)
			{
				static_cast<CollisionSphere*>(newObject->has(CollisionPrimitive))->radius=parameters.Scale.x/2.0;
			}else if(newObject->has(CollisionPrimitive)->Id==CollisionPrimitive::ShapeId::Cylinder)
			{
				static_cast<CollisionCylinder*>(newObject->has(CollisionPrimitive))->radius=parameters.Scale.x/2.0;
				static_cast<CollisionCylinder*>(newObject->has(CollisionPrimitive))->height=parameters.Scale.y;
			}else if(newObject->has(CollisionPrimitive)->Id==CollisionPrimitive::ShapeId::Cone)
			{
				static_cast<CollisionCone*>(newObject->has(CollisionPrimitive))->radius=parameters.Scale.x/2.0;
				static_cast<CollisionCone*>(newObject->has(CollisionPrimitive))->height=parameters.Scale.y;
				static_cast<CollisionCone*>(newObject->has(CollisionPrimitive))->CalculateSinTheta();
			}else if(newObject->has(CollisionPrimitive)->Id==CollisionPrimitive::ShapeId::Ellipsoid)
			{
				static_cast<CollisionEllipsoid*>(newObject->has(CollisionPrimitive))->halfSize.x=parameters.Scale.x/2.0;
				static_cast<CollisionEllipsoid*>(newObject->has(CollisionPrimitive))->halfSize.y=parameters.Scale.y/2.0;
				static_cast<CollisionEllipsoid*>(newObject->has(CollisionPrimitive))->halfSize.z=parameters.Scale.z/2.0;
			}
		}
		if(newObject->has(ActivationProperties))
		{
			newObject->has(ActivationProperties)->m_ActivationId = parameters.ActivationParameters.ObjectActivationId ;
			newObject->has(ActivationProperties)->m_ActivatedByVec = parameters.ActivationParameters.ActivatedBy ;
			newObject->has(ActivationProperties)->m_attachedToId  = parameters.ActivationParameters.AttachedTo ;
			newObject->has(ActivationProperties)->m_neverActivate  = parameters.ActivationParameters.NeverActivate ;
			newObject->has(ActivationProperties)->m_restrictedById   = parameters.ActivationParameters.RestrictedBy ;
			newObject->has(ActivationProperties)->m_oneTimeActivated   = !!parameters.ActivationParameters.OneTimeActivated ;
			newObject->has(ActivationProperties)->m_pointing   = parameters.ActivationParameters.PointingTo ;
			newObject->has(ActivationProperties)->m_timeToStayActivated   = parameters.ActivationParameters.TimeToStayActivated ;
			newObject->has(ActivationProperties)->m_shootingSpeed   = parameters.ActivationParameters.ShootingSpeed ;

		}
		if(newObject->has(ConstraintComponent))
		{
			newObject->has(ConstraintComponent)->Id = parameters.ConstraintParam.ConstraintID  ;
			newObject->has(ConstraintComponent)->AttachedToId = parameters.ConstraintParam.ConstraintAttachedTo  ;
			newObject->has(ConstraintComponent)->PosRelative   = parameters.ConstraintParam.Positionrelative  ;
			newObject->has(ConstraintComponent)->err  = parameters.ConstraintParam.err ;
			newObject->has(ConstraintComponent)->isInsideBody  = !!parameters.ConstraintParam.IsInsideBody ;
			newObject->has(ConstraintComponent)->multiplier  = parameters.ConstraintParam.Multiplier ;
		}
		if(newObject->has(AutoMovingComponent))
		{
			newObject->has(AutoMovingComponent)->m_MovingAxis  = parameters.AutoMovingPar.MovingAxis  ;
			newObject->has(AutoMovingComponent)->m_rotationAxis  = parameters.AutoMovingPar.RotationAxis  ;
			newObject->has(AutoMovingComponent)->m_movingSpeed   = parameters.AutoMovingPar.MovingSpeed  ;
			newObject->has(AutoMovingComponent)->m_rotatingSpeed  = parameters.AutoMovingPar.RotationSpeed ;
			newObject->has(AutoMovingComponent)->m_autoCanRotate   = !!parameters.AutoMovingPar.AutoCanRotate ;
			newObject->has(AutoMovingComponent)->m_autoCanMove   = !!parameters.AutoMovingPar.AutoCanMove;
			newObject->has(AutoMovingComponent)->m_RotationPoint   = parameters.AutoMovingPar.RotationPoint ;
		}
		if(newObject->has(ConnectorComponent))
		{
			newObject->has(ConnectorComponent)->m_isVertical  = parameters.ConnectorParams.IsVectical  ;
			newObject->has(ConnectorComponent)->m_canCollideWithStatic  = parameters.ConnectorParams.AllowStaticCollision  ;
		}

		if(newObject->has(LightComponent))
		{
			newObject->has(LightComponent)->Color  = parameters.LightParams.Color  ;
			newObject->has(LightComponent)->fallout  = parameters.LightParams.Fallout  ;
			newObject->has(LightComponent)->CanMove  = parameters.LightParams.CanMove  ;
		}

		if(newObject->has(RopeComponent))
		{
			newObject->has(RopeComponent)->Id  = parameters.RopePar.Id  ;
			newObject->has(RopeComponent)->AttachedId  = parameters.RopePar.AttachedTo  ;
			newObject->has(RopeComponent)->m_isStartPoint   = parameters.RopePar.isStart  ;
			if(EDITOR)
				EDITOR->twbmanager ->PushRopeId(parameters.RopePar.Id);
		}

		if(newObject->has(ParticleComponent))
		{
			//newObject->has(ParticleComponent)->t_emitPeriod = 1.0f;
		}

		if(newObject->has(GlowingCylinders))
		{
			newObject->has(GlowingCylinders)->m_parentActivationId = parameters.GlowCylindersParams.parentActicationId;
		}

		if(parameters.ConstraintParam.ConstraintID>-1)// add objects with constraints in this list, we'll take care of them later
		{
			ObjectsThatHaveConstraints[parameters.ConstraintParam.ConstraintID] = newObject;
			if(EDITOR) //otherwise we dont need that
			{
				ConstraintComponent * c = new ConstraintComponent();
				newObject->AddComponent(CT_ConstraintComponent,c);
				newObject->has(ConstraintComponent)->Id=parameters.ConstraintParam.ConstraintID;
			}
		}
		if(newObject->CompositionTypeId == GOC_Type_Constraint ) //add constraints here, later again
			ConstraintsObjects.push_back(newObject);

		if(newObject->CompositionTypeId == GOC_Type_RotatingSword )
		{
			static_cast<CollisionBox*>(newObject->has(CollisionPrimitive))->halfSize=Vector3(0.9,0.9,6.5);
		}
		/*if(EDITOR)
		{
			DebugComponent * c = new DebugComponent();
			newObject->AddComponent(CT_DebugComponent,c);
			if (newObject->has(CollisionPrimitive)->Id==CollisionPrimitive::ShapeId::Box)
			{
				newObject->has(DebugComponent)->SetIsCube(true);
			}

		}*/

		newObject->Initialize();
		return newObject;
	}

	
	void LoadLevelClass::loadGameSettings()
	{
		_gameData = new GameData();
		_gameData->initialize();
		//GAMEDATA->loadLevelData(filename.c_str());
		//
		utility::FileInfoList flist;
		utility::ListFilesInDirectory( "Objects/arch*.xml", flist );
		//
		for each( utility::FileInfo f in flist )
		{
			GAMEDATA->LoadArchetypeData( f.FullName.c_str() );
		}
	}
	void LoadLevelClass::LoadLevelFile(const std::string& filename)
	{
		std::cout<<"Opening Level file: "<<filename.c_str()<<"\n";
		XMLReader *reader = new XMLReader();
		if(reader->open(filename.c_str()))
		{
			ISerializer* str = reader->data().clone();
			std::vector<ISerializer*> comps;
			float arr[4];
			float y;
			ISerializer *str1=str->clone();
			if ( str1->readProperty( "constants", *str1 ) )
			{
				if(str1->readValue("BloomThreshold", y))
				{
					GRAPHICS->SetBloomThreshold(y);
				}
				if(str1->readValue("BloomMagnitude", y))
				{
					GRAPHICS->SetBloomMagnitude(y);
				}
				if(str1->readValue("BloomBlurSigma", y))
				{
					GRAPHICS->SetBloomBlurSigma(y);
				}
				if(str1->readValue("AdaptationRate", y))
				{
					GRAPHICS->SetAdaptationRate(y);
				}
				if(str1->readValue("ExposureKeyValue", y))
				{
					GRAPHICS->SetExposureKeyValue(y);
				}
				if(str1->readValue("SpecularIntensity", y))
				{
					GRAPHICS->SetSpecularIntensity(y);
				}
				if(str1->readValue("SpecularR", y))
				{
					GRAPHICS->SetSpecularR(y);
				}
				if(str1->readValue("SpecularG", y))
				{
					GRAPHICS->SetSpecularG(y);
				}
				if(str1->readValue("SpecularB", y))
				{
					GRAPHICS->SetSpecularB(y);
				}
				if(str1->readValue("Balance", y))
				{
					GRAPHICS->SetBalance(y);
				}
				if(str1->readValue("Roughness", y))
				{
					GRAPHICS->SetRoughness(y);
				}
				if(str1->readValue("ConstAmbient", arr, 4))
				{
					GRAPHICS->SetConstAmbient(arr);
				}
				if(str1->readValue("SkyColor", arr, 4))
				{
					GRAPHICS->SetSkyColor(arr);
				}
				if(str1->readValue("GroundColor", arr, 4))
				{
					GRAPHICS->SetGroundColor(arr);
				}

				if(str1->readValue("HemiIntensity", y))
				{
					GRAPHICS->SetHemiIntensity(y);
				}
					
			}

			if ( str->readProperty( "items", *str ) )
			{
				str->readProperty("item", comps);
				//
				for(std::vector<ISerializer*>::iterator it1 = comps.begin(), it2 = comps.end(); it1 != it2; ++it1)
				{
					ISerializer* stream = *it1;
					ObjectParameters objPar;
					if ( stream->readProperty("archetype", objPar.Arch_id ) )
					{
						GOC* obj;
						float arr[4];
						int x;
						float y;
						
						if(stream->readValue("GlobalID", x))
						{
							objPar.GlobalID =x;
						}
						if(stream->readValue("Position", arr, 3))
						{
							objPar.Position.x = arr[0];
							objPar.Position.y = arr[1];
							objPar.Position.z = arr[2];
						}if(stream->readValue("Scale", arr, 3))
						{
							objPar.Scale.x = arr[0];
							objPar.Scale.y = arr[1];
							objPar.Scale.z = arr[2];
						}
						if(stream->readValue("Orientation", arr, 4))
						{
							objPar.Quaternion.w = arr[3];
							objPar.Quaternion.x = arr[0];
							objPar.Quaternion.y = arr[1];
							objPar.Quaternion.z = arr[2];
						}
						//ACTIVATION PROPERTIES PARAMETERS
						if(stream->readValue("ObjectActivationId", x))
						{
							objPar.ActivationParameters.ObjectActivationId  =x;

						}

						if(stream->readValue("ActivatedBy",  arr, 4))
						{
							objPar.ActivationParameters.ActivatedBy.x =arr[0];
							objPar.ActivationParameters.ActivatedBy.y =arr[1];
							objPar.ActivationParameters.ActivatedBy.z =arr[2];
							objPar.ActivationParameters.ActivatedBy.w =arr[3];

						}
						if(stream->readValue("AttachedToId", x))
						{
							objPar.ActivationParameters.AttachedTo = x;

						}
						if(stream->readValue("NeverActivate", x))
						{
							objPar.ActivationParameters.NeverActivate = !!x;

						}
						if(stream->readValue("RestrictedBy", x))
						{
							objPar.ActivationParameters.RestrictedBy = x;

						}
						if(stream->readValue("TimeToStayActivated", y))
						{
							objPar.ActivationParameters.TimeToStayActivated  =y;

						}
						if(stream->readValue("OneTimeActivated", x))
						{
							objPar.ActivationParameters.OneTimeActivated  =x;

						}
						if(stream->readValue("PointingTo",  arr, 3))
						{
							objPar.ActivationParameters.PointingTo  = arr;

						}
						if(stream->readValue("ShootingSpeed",  y))
						{
							objPar.ActivationParameters.ShootingSpeed  = y;

						}

						//CONSTRAINT PARAMETERS
						if(stream->readValue("ConstraintId", x))
						{
							objPar.ConstraintParam.ConstraintID = x;
						}
						if(stream->readValue("ConstraintAttachedToId", x))
						{
							objPar.ConstraintParam.ConstraintAttachedTo = x;
						}
						if(stream->readValue("PositionRelative", arr,3))
						{
							objPar.ConstraintParam.Positionrelative = arr;
						}
						if(stream->readValue("Err", y))
						{
							objPar.ConstraintParam.err = y;
						}
						if(stream->readValue("Multiplier", arr,3))
						{
							objPar.ConstraintParam.Multiplier = arr;
						}
						if(stream->readValue("IsInsideBody", x))
						{
							objPar.ConstraintParam.IsInsideBody = x;
						}

						//AUTO MOVING PARAMETERS
						if(stream->readValue("MovingAxis", arr,3))
						{
							objPar.AutoMovingPar.MovingAxis  = arr;
						}
						if(stream->readValue("RotationAxis", arr,3))
						{
							objPar.AutoMovingPar.RotationAxis  = arr;
						}
						if(stream->readValue("RotationPoint", arr,3))
						{
							objPar.AutoMovingPar.RotationPoint  = arr;
						}
						if(stream->readValue("MovingSpeed", y))
						{
							objPar.AutoMovingPar.MovingSpeed  = y;
						}
						if(stream->readValue("RotationSpeed", y))
						{
							objPar.AutoMovingPar.RotationSpeed = y;
						}
						if(stream->readValue("AutoCanRotate", x))
						{
							objPar.AutoMovingPar.AutoCanRotate = x;
						}
						if(stream->readValue("AutoCanMove", x))
						{
							objPar.AutoMovingPar.AutoCanMove = x;
						}

						//ROPE PARAMETERS
						if(stream->readValue("RopeId", x))
						{
							objPar.RopePar.Id  = x;
						}
						if(stream->readValue("RopeAttachedToRopeId", x))
						{
							objPar.RopePar.AttachedTo = x;
						}
						if(stream->readValue("IsRopeStart", x))
						{
							objPar.RopePar.isStart = !!x;
						}

						//CONNECTOR PARAMS
						if(stream->readValue("IsBridgeVectical", x))
						{
							objPar.ConnectorParams.IsVectical  = !!x;
						}
						if(stream->readValue("AllowStaticCollide", x))
						{
							objPar.ConnectorParams.AllowStaticCollision = !!x;
						}

						//LIGHT PARAMETERS
						if(stream->readValue("Color",  arr,3))
						{
							objPar.LightParams.Color  = arr;
							//arr[3]=1.0f;
						}
						if(stream->readValue("Fallout", y))
						{
							objPar.LightParams.Fallout = y;
						}
						if(stream->readValue("LightCanMove", x))
						{
							objPar.LightParams.CanMove  = !!x;
						}


						//Box Level Id
						if(stream->readValue("LevelBoxId", x))
						{
							objPar.BoxLevelId=x;
						}

						//Glow Cylinder
						if(stream->readValue("GlowCylinderParentId", x))
						{
							objPar.GlowCylindersParams.parentActicationId=x;
						}

						obj=CreateObjectAt(objPar);
						AddGOCtoLogicContainer(obj);
					}	
						
				}
			}

			ActionsAfterAllObjectsLoaded();
			//
			utility::deletePointersVec(comps);
			delete str;
		}
		delete reader;
		//
		//SaveLevelLocal( "Testfile.xml" );
	}


	void LoadLevelClass::ActionsAfterAllObjectsLoaded()
	{
		std::vector<GOC *>::iterator consIt;
		std::map<int,GOC *> ::iterator p;
		for(p = ObjectsThatHaveConstraints.begin(); p != ObjectsThatHaveConstraints.end(); p++){
			p->second->has(RigidBody)->setHasConstraint(true);
			if (EDITOR){
				EDITOR->twbmanager->PushContraintId(p->first); 
			}
			
		}

		for(consIt = ConstraintsObjects.begin(); consIt != ConstraintsObjects.end(); consIt++)
		{
			if((*consIt)->has(ConstraintComponent)->err>1)
			{
				GOC * g= FACTORY->BuildFromArchetype("GOC_Type_RopePart");
				LOGIC->RolesList.push_back(g);
				g->Initialize();
			}
			Constraint c;
			int id=(*consIt)->has(ConstraintComponent)->AttachedToId ;
			c.set(ObjectsThatHaveConstraints[id]->has(RigidBody),(*consIt)->has(ConstraintComponent)->PosRelative ,(*consIt)->has(Transform)->Position,(*consIt)->has(ConstraintComponent)->err) ;
			PHYSICS->constraints.push_back(c);
		}

		if(!EDITOR) //in the final version destroy these constraints, we dont need them anymore
		{
			for(consIt = ConstraintsObjects.begin(); consIt != ConstraintsObjects.end(); consIt++)
			{
				(*consIt)->Destroy();
			}

		}
		//
		//LOGIC->bulbHack=FACTORY->BuildFromArchetype("GOC_Type_LightBulb");
		//LOGIC->bulbHack->Initialize();
		for(p = LOGIC->RopePointsList.begin(); p != LOGIC->RopePointsList.end(); p++)
		{
			Vector3 a,b,atob;
			if (p->second->has(RopeComponent)->m_isStartPoint )
			{
				a=p->second->has(Transform)->Position;
				b=LOGIC->RopePointsList[p->second->has(RopeComponent)->AttachedId]->has(Transform)->Position;
			}
			else{
				continue;
			}
			GOC *g;
				
			atob=b-a;
			real dist=atob.magnitude();
			
			dist/=0.5;
			atob.normalise();
			std::vector<GOC*> rope;
			int i=0,k;
			for(;i<dist;i++)
			{
				g=FACTORY->BuildFromArchetype("GOC_Type_RopeKnot");
				g->has(Transform)->Position= Vector3ToVec3(a+atob*0.5*i);
				g->has(Transform)->Scale=Vec3(0.1f,0.1f,0.1f);
				static_cast<CollisionSphere*>(g->has(CollisionPrimitive))->radius=0.05;
				g->Initialize();
				g->has(RigidBody)->setAwake(false);
				rope.push_back(g);
				
				
			}
			//LOGIC->BulbList.push_back(g);
			
			
			//LOGIC->bulbHack->has(Transform)->Scale=Vec3(1.2,1.2,1.2);
			//LOGIC->bulbHack->has(Transform)->Position=LOGIC->hackPointForLight->has(Transform)->Position;
			k=i;

			Constraint c;
			c.set(rope[0]->has(RigidBody),Vector3(0,0,0),rope[0]->has(Transform)->Position,0.01);
			PHYSICS->constraints.push_back(c);

			for(unsigned int i=0;i<rope.size()-1;i++)
			{
				Joint j;
				j.set(rope[i]->has(RigidBody),Vector3(0,0,0),rope[i+1]->has(RigidBody),Vector3(0,0,0),0.5);
				PHYSICS->joints.push_back(j);

				g=FACTORY->BuildFromArchetype("GOC_Type_RopePart");
				g->Initialize();
				LOGIC->RopePartsList.push_back(g);
			}

		/*	g=FACTORY->BuildFromArchetype("GOC_Type_HangingBall");
			g->has(Transform)->Position= Vector3ToVec3(a+atob*0.2*k) + Vector3ToVec3(atob*1.0);
			g->has(Transform)->Scale=Vec3(2.0,2.0,2.0);
			static_cast<CollisionSphere*>(g->has(CollisionPrimitive))->radius=1.0;
			g->Initialize();

			Joint j;
			j.set(rope[rope.size()-1]->has(RigidBody),Vector3(0,0,0),g->has(RigidBody),Vector3(0,0,0),1.2);
			PHYSICS->joints.push_back(j);
*/
			/*GOC* g1=FACTORY->BuildFromArchetype("GOC_Type_RopePart");
			g1->Initialize();
			LOGIC->RopePartsList.push_back(g1);*/
			
		}



		LOGIC->m_ObjectActivator.InitializeAttachedObjects();
		ObjectsThatHaveConstraints.clear();
		ConstraintsObjects.clear();
		GRAPHICS->CreateLightBuffer();


		//determine in which box object is
		GameObjectIdMapType mapOfGOCS = FACTORY->GetAllObjects() ;
		GameObjectIdMapType::iterator it;
		float posX,posZ;
		for(it = mapOfGOCS.begin(); it != mapOfGOCS.end(); it++)
		{
			//THIS CODE IS FOR THE GLOWING CYLINDERS
			if(it->second->has(GlowingCylinders))
			{
				int x=it->second->has(GlowingCylinders)->m_parentActivationId;
				std::map<int,GOC*> ::iterator p;
				p=LOGIC->ActivatableObjectsList.find(x);
				if ( p != LOGIC->ActivatableObjectsList.end( ) )
				{
					p->second->has(ActivationProperties)->glowingCylList.push_back(it->second);
				}
			}
			//END OF CODE FOR GLOWING CYLINDERS


			if(it->second->has(RigidBody) && (it->second->has(RigidBody)-> hasFiniteMass()) && !(it->second->has(RigidBody)-> getIsStatic()) && it->second->CompositionTypeId!=GOC_Type_Domino && it->second->CompositionTypeId!=GOC_Type_Hero)
				continue;
			if(it->second->CompositionTypeId==GOC_Type_RopePart || it->second->CompositionTypeId==GOC_Type_LaserLight)
				continue;
			posX=it->second->has(Transform)->Position.x;
			posZ=it->second->has(Transform)->Position.z;
			for (unsigned int i=0;i<LOGIC->LevelBoxes.size();i++)
			{
				if(it->second->CompositionTypeId==GOC_Type_TutorialBox)
					continue;
				GOC *goc=LOGIC->LevelBoxes[i];
				float minX,maxX,minZ,maxZ;
				minX=goc->has(Transform)->Position.x-goc->has(Transform)->Scale.x*0.5f;
				maxX=goc->has(Transform)->Position.x+goc->has(Transform)->Scale.x*0.5f;
				minZ=goc->has(Transform)->Position.z-goc->has(Transform)->Scale.z*0.5f;
				maxZ=goc->has(Transform)->Position.z+goc->has(Transform)->Scale.z*0.5f;
				if(posX>minX && posX<maxX && posZ>minZ && posZ<maxZ)
				{
					it->second->LevelBoxId = goc->LevelBoxId;
					break;
				}
			}
		}

	}
	

	void LoadLevelClass::AddGOCtoLogicContainer(GOC* g)
	{
		if(LOGIC->m_levelId==0)
		{
			if(g->CompositionTypeId==GOC_Type_Gun)
			{
				Level_1* l=dynamic_cast<Level_1*>(LOGIC->level);
				l->GUN=g;
			}
			if(g->CompositionTypeId==GOC_Type_TutorialBox)
			{
				Level_1* l=dynamic_cast<Level_1*>(LOGIC->level);
				l->TurorialBoxes.push_back(g);
			}
		}
		

		LOGIC->m_componentManager.AddGOCtoLogicContainer(g);
		if(g->has(LightComponent)  )
		{
			LOGIC->BulbList.push_back(g);
			CreateLight(g);

		}
		
	}

	void LoadLevelClass::CreateLight(GOC* g)
	{
		//return;
		int index=GRAPHICS->GetNoOfLights();
		Light  & light=GRAPHICS->DeferredLights[index];

		light.Position = g->has(Transform)->Position;
		light.Color.x  = g->has(LightComponent)->Color.x;
		light.Color.y  = g->has(LightComponent)->Color.y;
		light.Color.z  = g->has(LightComponent)->Color.z;
		light.Falloff  = g->has(LightComponent)->fallout;
		g->has(LightComponent)->SetIndex(index);
		GRAPHICS->SetNumOfLights(index+1);
	}


	

}



