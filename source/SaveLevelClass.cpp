#include "Precompiled.h"
#include "XMLSerialization.h"
#include "Utility.h"
#include "SaveLevelClass.h"
#include "XMLSerialization.h"
#include <fstream>
#include "ActivationProperties.h"
#include "Laser.h"
#include "ConstraintsComponent.h"
#include "AutoMovingComponent.h"
#include "RopeComponent.h"
#include "ConnectorComponent.h"
#include "LightComponent.h"
#include "Graphics.h"
#include "GlowingCylinder.h"


namespace Framework
{

	SaveLevelClass* SAVECLASS=NULL;


	void SaveLevelClass::Initialize()
	{
	}

	SaveLevelClass::SaveLevelClass()
	{
		ErrorIf(SAVECLASS!=NULL,"SAVECLASS already initialized");
		SAVECLASS=this;
	}

	SaveLevelClass::~SaveLevelClass()
	{
		delete _gameData;
	}

	void SaveLevelClass::SaveLevel (int l)
	{
		const int len = 256;
		char file[len];
		::sprintf_s( file, len, "Levels\\Level_%d.xml", l );
		const std::string& fileName=file;
		Save(fileName);
	}

	void SaveLevelClass::SaveLevel (const std::string& fileName)
	{
		Save(fileName);
	}

	void SaveLevelClass::Save(const std::string& fileName)
	{
		const int len=256;
		char str[len];
		//const char* fileName = "test.xml";
		ConstraintsObjects.clear();
		ObjectsThatHaveConstraints.clear();

		std::cout<<"Saving Level file..\n";

		std::fstream os;
		os.open ( fileName.c_str(), std::ios::binary | std::fstream::in | std::fstream::out | std::fstream::trunc );
		////
		if( os.is_open() )
		{
			ISerializer *str1, *str2;
			XMLSerializer root;
			root.allocateNew();
			ISerializer* stream = root.clone();

			stream->writeNode( "constants" );
			str1=stream->clone();
			str1->readProperty( "constants", *str1 );
			sprintf_s( str, len, "%f",  GRAPHICS->GetBloomThreshold() );
			str1->writeNodeAndValue( "BloomThreshold", str );
			sprintf_s( str, len, "%f",  GRAPHICS->GetBloomMagnitude() );
			str1->writeNodeAndValue( "BloomMagnitude", str );
			sprintf_s( str, len, "%f",  GRAPHICS->GetBloomBlurSigma() );
			str1->writeNodeAndValue( "BloomBlurSigma", str );
			sprintf_s( str, len, "%f",  GRAPHICS->GetAdaptationRate() );
			str1->writeNodeAndValue( "AdaptationRate", str );
			sprintf_s( str, len, "%f",  GRAPHICS->GetExposureKeyValue() );
			str1->writeNodeAndValue( "ExposureKeyValue", str );
			sprintf_s( str, len, "%f",  GRAPHICS->GetSpecularIntensity() );
			str1->writeNodeAndValue( "SpecularIntensity", str );
			sprintf_s( str, len, "%f",  GRAPHICS->GetSpecularR() );
			str1->writeNodeAndValue( "SpecularR", str );
			sprintf_s( str, len, "%f",  GRAPHICS->GetSpecularG() );
			str1->writeNodeAndValue( "SpecularG", str );
			sprintf_s( str, len, "%f",  GRAPHICS->GetSpecularB() );
			str1->writeNodeAndValue( "SpecularB", str );
			sprintf_s( str, len, "%f",  GRAPHICS->GetBalance() );
			str1->writeNodeAndValue( "Balance", str );
			sprintf_s( str, len, "%f",  GRAPHICS->GetRoughness() );
			str1->writeNodeAndValue( "Roughness", str );
			sprintf_s( str, len, "%4.4f,%4.4f,%4.4f,%4.4f", GRAPHICS->GetConstAmbient().x,  GRAPHICS->GetConstAmbient().y, GRAPHICS->GetConstAmbient().z,GRAPHICS->GetConstAmbient().w);
			str1->writeNodeAndValue( "ConstAmbient", str );
			sprintf_s( str, len, "%4.4f,%4.4f,%4.4f,%4.4f", GRAPHICS->GetSkyColor().x,  GRAPHICS->GetSkyColor().y, GRAPHICS->GetSkyColor().z,GRAPHICS->GetSkyColor().w);
			str1->writeNodeAndValue( "SkyColor", str );
			sprintf_s( str, len, "%4.4f,%4.4f,%4.4f,%4.4f", GRAPHICS->GetGroundColor().x,  GRAPHICS->GetGroundColor().y, GRAPHICS->GetGroundColor().z,GRAPHICS->GetGroundColor().w);
			str1->writeNodeAndValue( "GroundColor", str );
			sprintf_s( str, len, "%f",  GRAPHICS->GetHemiIntensity() );
			str1->writeNodeAndValue( "HemiIntensity", str );

			stream->writeNode( "items" );
			//
			GameObjectIdMapType mapOfGOCS = FACTORY->GetAllObjects() ;
			
			GameObjectIdMapType::iterator p;
			int id=0;
			for(p = mapOfGOCS.begin(); p != mapOfGOCS.end(); p++)	
			{
				if (p->second->archId =="" || p->second->CompositionTypeId ==GOC_Type_DebugCube  || p->second->CompositionTypeId ==GOC_Type_DebugSphere || p->second->CompositionTypeId == GOC_Type_RopeKnot|| p->second->CompositionTypeId == GOC_Type_LaserLight || p->second->CompositionTypeId == GOC_Type_RopePart )
					continue;//dont save	

				if(p->second->CompositionTypeId ==GOC_Type_Constraint )//handle later
				{
					ConstraintsObjects.push_back(p->second);
					continue;
				}
									
				str1 = stream->clone();
				str1->writeNode( "item" );
				str1->readProperty( "item", *str1 );
				str1->writeProperty( "archetype", p->second->archId.c_str() ) ;
				
				sprintf_s( str, len, "%d", ++id);
				str1->writeNodeAndValue( "GlobalID", str );
				if (p->second->has(Transform))
				{
					sprintf_s( str, len, "%4.4f,%4.4f,%4.4f", p->second->has(Transform)->Position.x,  p->second->has(Transform)->Position.y, p->second->has(Transform)->Position.z);
					str1->writeNodeAndValue( "Position", str );
					sprintf_s( str, len, "%4.4f,%4.4f,%4.4f", p->second->has(Transform)->Scale.x,  p->second->has(Transform)->Scale.y, p->second->has(Transform)->Scale.z);
					str1->writeNodeAndValue( "Scale", str );
					sprintf_s( str, len, "%4.4f,%4.4f,%4.4f,%4.4f",  p->second->has(Transform)->QUATERNION.x , p->second->has(Transform)->QUATERNION.y ,p->second->has(Transform)->QUATERNION.z ,p->second->has(Transform)->QUATERNION.w);
					str1->writeNodeAndValue( "Orientation", str );
				}

				if (p->second->has(ActivationProperties))
				{
					sprintf_s( str, len, "%d",  p->second->has(ActivationProperties)->GetActivationId());
					str1->writeNodeAndValue( "ObjectActivationId", str );
					sprintf_s( str, len, "%4.0f,%4.0f,%4.0f,%4.0f",  p->second->has(ActivationProperties)->GetObjectActivatedByVec().x ,   p->second->has(ActivationProperties)->GetObjectActivatedByVec().y, p->second->has(ActivationProperties)->GetObjectActivatedByVec().z, p->second->has(ActivationProperties)->GetObjectActivatedByVec().w);
					str1->writeNodeAndValue( "ActivatedBy", str );
					sprintf_s( str, len, "%4.2f,%4.2f,%4.2f",  p->second->has(ActivationProperties)->m_pointing.x ,   p->second->has(ActivationProperties)->m_pointing.y, p->second->has(ActivationProperties)->m_pointing.z);
					str1->writeNodeAndValue( "PointingTo", str );
					sprintf_s( str, len, "%d",  p->second->has(ActivationProperties)->GetAttachedToId() );
					str1->writeNodeAndValue( "AttachedToId", str );
					sprintf_s( str, len, "%d",  p->second->has(ActivationProperties)->m_neverActivate );
					str1->writeNodeAndValue( "NeverActivate", str );
					sprintf_s( str, len, "%d",  p->second->has(ActivationProperties)->m_restrictedById );
					str1->writeNodeAndValue( "RestrictedBy", str );
					sprintf_s( str, len, "%f",  p->second->has(ActivationProperties)->m_timeToStayActivated );
					str1->writeNodeAndValue( "TimeToStayActivated", str );
					sprintf_s( str, len, "%d",  p->second->has(ActivationProperties)->m_oneTimeActivated );
					str1->writeNodeAndValue( "OneTimeActivated", str );
					sprintf_s( str, len, "%f",  p->second->has(ActivationProperties)->m_shootingSpeed );
					str1->writeNodeAndValue( "ShootingSpeed", str );
				}

				if (p->second->has(LightComponent))
				{
					sprintf_s( str, len, "%4.4f,%4.4f,%4.4f",  p->second->has(LightComponent)->Color.x ,   p->second->has(LightComponent)->Color.y, p->second->has(LightComponent)->Color.z);
					str1->writeNodeAndValue( "Color", str );
					sprintf_s( str, len, "%f",  p->second->has(LightComponent)->fallout );
					str1->writeNodeAndValue( "Fallout", str );
					sprintf_s( str, len, "%d",  p->second->has(LightComponent)->CanMove);
					str1->writeNodeAndValue( "LightCanMove", str );

				}

				if(p->second->has(AutoMovingComponent))
				{
					sprintf_s( str, len, "%d",  p->second->has(AutoMovingComponent)->m_autoCanRotate);
					str1->writeNodeAndValue( "AutoCanRotate", str );
					sprintf_s( str, len, "%d",  p->second->has(AutoMovingComponent)->m_autoCanMove);
					str1->writeNodeAndValue( "AutoCanMove", str );
					sprintf_s( str, len, "%4.0f,%4.0f,%4.0f",  p->second->has(AutoMovingComponent)->m_MovingAxis.x ,   p->second->has(AutoMovingComponent)->m_MovingAxis.y,  p->second->has(AutoMovingComponent)->m_MovingAxis.z);
					str1->writeNodeAndValue( "MovingAxis", str );
					sprintf_s( str, len, "%4.0f,%4.0f,%4.0f",  p->second->has(AutoMovingComponent)->m_RotationPoint.x ,   p->second->has(AutoMovingComponent)->m_RotationPoint.y,  p->second->has(AutoMovingComponent)->m_RotationPoint.z);
					str1->writeNodeAndValue( "RotationPoint", str );
					sprintf_s( str, len, "%4.0f,%4.0f,%4.0f",  p->second->has(AutoMovingComponent)->m_rotationAxis.x ,   p->second->has(AutoMovingComponent)->m_rotationAxis.y,  p->second->has(AutoMovingComponent)->m_rotationAxis.z);
					str1->writeNodeAndValue( "RotationAxis", str );
					sprintf_s( str, len, "%f",  p->second->has(AutoMovingComponent)->m_movingSpeed );
					str1->writeNodeAndValue( "MovingSpeed", str );
					sprintf_s( str, len, "%f",  p->second->has(AutoMovingComponent)->m_rotatingSpeed );
					str1->writeNodeAndValue( "RotationSpeed", str );
				}

				if(p->second->has(RopeComponent))
				{
					sprintf_s( str, len, "%d",  p->second->has(RopeComponent)->Id);
					str1->writeNodeAndValue( "RopeId", str );
					sprintf_s( str, len, "%d",  p->second->has(RopeComponent)->AttachedId);
					str1->writeNodeAndValue( "RopeAttachedToRopeId", str );
					sprintf_s( str, len, "%d",  p->second->has(RopeComponent)->m_isStartPoint );
					str1->writeNodeAndValue( "IsRopeStart", str );
				}


				if (p->second->has(ConstraintComponent))
				{
					if(p->second->CompositionTypeId != GOC_Type_Constraint )
					{
						if(p->second->has(RigidBody)->getHasConstraint())
						{
							ObjectsThatHaveConstraints[p->second->has(ConstraintComponent)->Id ]=p->second;
							sprintf_s( str, len, "%d",  p->second->has(ConstraintComponent)->Id);
							str1->writeNodeAndValue( "ConstraintId", str );
						}
					}
				}

				if (p->second->has(ConnectorComponent))
				{
					sprintf_s( str, len, "%d",  p->second->has(ConnectorComponent)->m_isVertical );
					str1->writeNodeAndValue( "IsBridgeVectical", str );
					sprintf_s( str, len, "%d",  p->second->has(ConnectorComponent)->m_canCollideWithStatic );
					str1->writeNodeAndValue( "AllowStaticCollide", str );
				}

				if (p->second->has(GlowingCylinders))
				{
					sprintf_s( str, len, "%d",  p->second->has(GlowingCylinders)->m_parentActivationId );
					str1->writeNodeAndValue( "GlowCylinderParentId", str );
				}

				if (p->second->CompositionTypeId==GOC_Type_LevelBox||p->second->CompositionTypeId==GOC_Type_TutorialBox )
				{
					sprintf_s( str, len, "%d",  p->second->LevelBoxId );
					str1->writeNodeAndValue( "LevelBoxId", str );
				}


				str2 = str1->clone();
				str1->goToParent();
				str1->readProperty( "items", *str1 );
				stream->removeNode( "item" );
				str1->appendNode( *str2 );
			}		
			
			//write constraints in the end of the file
			std::vector<GOC *>::iterator consIt;
			for(consIt = ConstraintsObjects.begin(); consIt != ConstraintsObjects.end(); consIt++)
			{
				if(ValidateConstraint(*consIt))
				{
					str1 = stream->clone();
					str1->writeNode( "item" );
					str1->readProperty( "item", *str1 );
					str1->writeProperty( "archetype",(*consIt)->archId.c_str() ) ;
				
					sprintf_s( str, len, "%d", ++id);
					str1->writeNodeAndValue( "GlobalID", str );

					if ((*consIt)->has(Transform))
					{
						sprintf_s( str, len, "%4.4f,%4.4f,%4.4f", (*consIt)->has(Transform)->Position.x,  (*consIt)->has(Transform)->Position.y, (*consIt)->has(Transform)->Position.z);
						str1->writeNodeAndValue( "Position", str );
						sprintf_s( str, len, "%4.4f,%4.4f,%4.4f", (*consIt)->has(Transform)->Scale.x,  (*consIt)->has(Transform)->Scale.y, (*consIt)->has(Transform)->Scale.z);
						str1->writeNodeAndValue( "Scale", str );
						sprintf_s( str, len, "%4.4f,%4.4f,%4.4f,%4.4f",  (*consIt)->has(Transform)->QUATERNION.x , (*consIt)->has(Transform)->QUATERNION.y ,(*consIt)->has(Transform)->QUATERNION.z ,(*consIt)->has(Transform)->QUATERNION.w);
						str1->writeNodeAndValue( "Orientation", str );
					}
					sprintf_s( str, len, "%d", (*consIt)->has(ConstraintComponent)->AttachedToId );
					str1->writeNodeAndValue( "ConstraintAttachedToId", str );
					sprintf_s( str, len, "%4.4f,%4.4f,%4.4f",  (*consIt)->has(ConstraintComponent)->PosRelative.x ,(*consIt)->has(ConstraintComponent)->PosRelative.y ,(*consIt)->has(ConstraintComponent)->PosRelative.z );
					str1->writeNodeAndValue( "PositionRelative", str );
					//
				
					//
					sprintf_s( str, len, "%1.0f,%1.0f,%1.0f",  (*consIt)->has(ConstraintComponent)->multiplier.x ,(*consIt)->has(ConstraintComponent)->multiplier.y ,(*consIt)->has(ConstraintComponent)->multiplier.z );
					str1->writeNodeAndValue( "Multiplier", str );
					sprintf_s( str, len, "%d", (*consIt)->has(ConstraintComponent)->isInsideBody );
					str1->writeNodeAndValue( "IsInsideBody", str );
					sprintf_s( str, len, "%f", (*consIt)->has(ConstraintComponent)->err );
					str1->writeNodeAndValue( "Err", str );
				
					str2 = str1->clone();
					str1->goToParent();
					str1->readProperty( "items", *str1 );
					stream->removeNode( "item" );
					str1->appendNode( *str2 );
				}
			}


			XMLSerializer::saveNodeToStream( root, &os );
			os.close();
			std::cout<<"Level file save as: "<<fileName<<"\n";
		}
		else
		{
			std::cout<<"Can't save file: " <<fileName<<"\n";
		}
	}



	bool SaveLevelClass::ValidateConstraint(GOC * c)
	{
		int id= c->has(ConstraintComponent)->AttachedToId;
		std::map<int,GOC *> ::iterator p=ObjectsThatHaveConstraints.begin();
		p=ObjectsThatHaveConstraints.find(id);

		if (p==ObjectsThatHaveConstraints.end())
			return false;
		else 
			return true;
	}


}
