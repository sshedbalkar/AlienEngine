#include "Precompiled.h"
#include "EditorFunctionality.h"
#include "Physics.h"
#include "Graphics.h"
#include "WindowsSystem.h"
#include "Factory.h"
#include "Core.h"
#include "VMath.h"
#include "LevelEditor.h"
#include "TweakBarManager.h"
#include "Axis.h"
#include "ModelComponent.h"
#include "Hero.h"
#include "GameLogic.h"
#include "LightComponent.h"

namespace Framework
{
	EditorFunctionality::EditorFunctionality()
	{
	}

	EditorFunctionality::~EditorFunctionality()
	{
	}


	void EditorFunctionality::SelectObjects()
	{
		//not done
		D3DXMATRIX matrix;
		D3DXMATRIX projectionMatrix;
		matrix=GRAPHICS->GetViewProjMatrix();
		
		GameObjectIdMapType mapOfGOCS = FACTORY->GetAllObjects() ;
		GameObjectIdMapType::iterator p;
		
		real x0 = (((2.0f * (float)EDITOR->clickedPosition.x ) / (float)GRAPHICS->screen_width ) - 1.0f);
		real y0 = ((((2.0f * (float)EDITOR->clickedPosition.y) / (float)GRAPHICS->screen_height) - 1.0f) * -1.0f);		
		real x1 = (((2.0f * (float)EDITOR->unClickedPosition.x ) / (float)GRAPHICS->screen_width ) - 1.0f);
		real y1 = ((((2.0f * (float)EDITOR->unClickedPosition.y) / (float)GRAPHICS->screen_height) - 1.0f) * -1.0f);
			
		if(x0>x1)
		{
			real temp=x0;
			x0=x1;
			x1=temp;
		}
		
		if(y0<y1)
		{
			real temp=y0;
			y0=y1;
			y1=temp;
		}
		Vec3 camPos=GRAPHICS->GetCameraPosition();
		for ( p = mapOfGOCS.begin(); p != mapOfGOCS.end(); ++p )
		{
			Transform* transform = p->second->has( Transform );
			Vec3 pos=p->second->has( Transform )->Position;
			if ( !transform ) continue;
			D3DXVECTOR4 positionWS( transform->Position[0], transform->Position[1], transform->Position[2], 1.0f );
			D3DXVECTOR4 positionPS;
			D3DXVec4Transform( &positionPS, &positionWS, &matrix );
			float x = positionPS.x / positionPS.w, y = positionPS.y / positionPS.w;
			if (  x > x0 && y > y1 && x < x1 && y < y0 ) 
			{
				if((squaredDistance(Vector3(camPos.x,camPos.y,camPos.z),Vector3(pos.x,pos.y,pos.z))<(EDITOR->maxSelectDistance*EDITOR->maxSelectDistance)) &&
					p->second->has(CollisionPrimitive) ){
					EDITOR->selectedObjects.push_back( p->second );
				}
			}
		}
	}

	GOC* EditorFunctionality::GetGOCFromIntersection(Vector3 & position,Vector3 & direction)
	{
		GOC * Object=NULL;
		Vector3 tempposition;
		Ray ray(position,direction);
		CollisionDetector detect;
		real distance=REAL_MAX;
		GameObjectIdMapType mapOfGOCS = FACTORY->GetAllObjects() ;

		GameObjectIdMapType::iterator p;
		for(p = mapOfGOCS.begin(); p != mapOfGOCS.end(); p++)	
		{
			if(p->second->CompositionTypeId==GOC_Type_LevelBox || p->second->CompositionTypeId==GOC_Type_TutorialBox)
			{
				if(! EDITOR->GetShowLevelBoxes())
				{
					continue;
				}
			}
			if(p->second->has(ComplexObject))
			{
				ComplexObject *object = p->second->has(ComplexObject);
				ObjectLinkList<CollisionPrimitive>::iterator it1;
				it1= object->collisionPrimitives.begin();
				int index=0;
				for(;it1!=object->collisionPrimitives.end();++it1,++index)
				{
					//if(!p->second->has(ComplexObject)->getAlive(index)) continue;
					//t1->GetOwner()->has(CollisionPrimitive)->calculateInternalsinEditor();
					if(it1->rayCast(ray,tempposition))
					{
						real a=squaredDistance(position,tempposition);
						if(a<=distance)
						{
							distance=a;
							Object=p->second;
						}
					}
				}
			}
			else if(p->second->has(CollisionPrimitive))
			{
				p->second->has(CollisionPrimitive)->calculateInternalsinEditor();
				if(p->second->has(CollisionPrimitive)->rayCast(ray,tempposition))
				{
					real a=squaredDistance(position,tempposition);
					if(a<distance)
					{
						distance=a;
						Object=p->second;
					}
				}
			}
		}
		return Object;
	}

	

	void EditorFunctionality::CopySelectedObjects()
	{
		for(unsigned int i=0;i<EDITOR->m_gocsToCopy.size();++i) 
		{
			GOC *m_gocToCopy=EDITOR->m_gocsToCopy[i];
			GOC * newGOC = FACTORY->BuildFromArchetype(m_gocToCopy->archId.c_str() ); 
			Vector3 temp=EDITOR->CenterPos + EDITOR->CenterDir*10;
			newGOC->has(Transform)->Position=Vec3( (float)temp.x, (float)temp.y, (float)temp.z);
			newGOC->has(Transform)->Scale= m_gocToCopy->has(Transform)->Scale;
			newGOC->has(Transform)->QUATERNION=m_gocToCopy->has(Transform)->QUATERNION;
			if(m_gocToCopy->has(CollisionPrimitive)->Id == CollisionPrimitive::ShapeId::Sphere)
			{
				static_cast<CollisionSphere*>(newGOC->has(CollisionPrimitive))->radius=static_cast<CollisionSphere*>(m_gocToCopy->has(CollisionPrimitive))->radius;
			}
			else if(m_gocToCopy->has(CollisionPrimitive)->Id == CollisionPrimitive::ShapeId::Box )
			{
				static_cast<CollisionBox*>(newGOC->has(CollisionPrimitive))->halfSize=static_cast<CollisionBox*>(m_gocToCopy->has(CollisionPrimitive))->halfSize;
			}
			newGOC->Initialize();
			if(newGOC->has(LightComponent))
			{

				newGOC->has(LightComponent)->Color=m_gocToCopy->has(LightComponent)->Color;
				newGOC->has(LightComponent)->fallout=m_gocToCopy->has(LightComponent)->fallout;
				newGOC->has(LightComponent)->CanMove =m_gocToCopy->has(LightComponent)->CanMove;

				EDITOR->AddLightToGame(newGOC);
			}

		}
		

	}

	void EditorFunctionality::DeleteSelectedObjects()
	{
		for(unsigned int i=0;i<EDITOR->selectedObjects.size();++i)
		{
			if( EDITOR->selectedObjects[i]->has(LightComponent))
				EDITOR->RemoveLightFromGame(EDITOR->selectedObjects[i]);
			if (EDITOR->selectedObjects[i]->CompositionTypeId!=GOC_Type_NOTHING && EDITOR->selectedObjects[i]->CompositionTypeId!=GOC_Type_Hero)
				EDITOR->selectedObjects[i]->Destroy();
			
		}
		EDITOR->selectedObjects.clear();
		EDITOR->twbmanager->SetSelectedGOCs(EDITOR->selectedObjects);
		EDITOR->hoveredGOC=0;
			
	}

	void EditorFunctionality::InitializeLightsIfDeleted()
	{
		LOGIC->BulbList.clear();
		GRAPHICS->DestroyAllLights();
		GameObjectIdMapType mapOfGOCS = FACTORY->GetAllObjects() ;
		GOC * g;
		GameObjectIdMapType::iterator p;
		for(p = mapOfGOCS.begin(); p != mapOfGOCS.end(); p++)	
		{
			g=p->second;
			if(g->has(LightComponent) && !g->MarkedAsDestroyed )
			{
				LOGIC->BulbList.push_back(g);
				LightClass Directionlight;
				Directionlight.SetLightProperties(g->has(LightComponent));
				Directionlight.SetPosition(g->has(Transform)->Position);
				if(Directionlight.GetShadow())
					Directionlight.InitializeRenderTarget();
				GRAPHICS->PushLightToList(Directionlight);
			}
			
		}
	}

	
	void EditorFunctionality::UpdatePosition()
	{
		Vector3 lookingAt=GRAPHICS->GetCameraLookAt();
		switch(EDITOR->axisObject->selectedAxis)
			{
			case(EDITOR->axisObject->AxisSelection::x ):
			{
				real x=(EDITOR->screenMousePosition.x-EDITOR->prevScreenMousePosition.x);
				x/=GRAPHICS->screen_width;
				for(unsigned int i=0;i<EDITOR->selectedObjects.size();++i)
				{
					if(lookingAt.z>0)
						EDITOR->selectedObjects[i]->has(Transform)->Position.x+= (float)(x*EDITOR->axisObject->magicNumber*5);
					else
						EDITOR->selectedObjects[i]->has(Transform)->Position.x-= (float)(x*EDITOR->axisObject->magicNumber*5);
				}
				break;
			}
			case(EDITOR->axisObject->AxisSelection::z):
			{
				real z=(EDITOR->screenMousePosition.x-EDITOR->prevScreenMousePosition.x);
				z/=GRAPHICS->screen_width;
				for(unsigned int i=0;i<EDITOR->selectedObjects.size();++i)
				{
					if(lookingAt.x<0)
						EDITOR->selectedObjects[i]->has(Transform)->Position.z+= (float)(z*EDITOR->axisObject->magicNumber*5);
					else
						EDITOR->selectedObjects[i]->has(Transform)->Position.z-= (float)(z*EDITOR->axisObject->magicNumber*5);
				}
				break;
			}
			case(EDITOR->axisObject->AxisSelection::y):
			{
				real y=(EDITOR->screenMousePosition.y-EDITOR->prevScreenMousePosition.y);
				y/=GRAPHICS->screen_height;
				for(unsigned int i=0;i<EDITOR->selectedObjects.size();++i)
				{
					EDITOR->selectedObjects[i]->has(Transform)->Position.y-= (float)(y*EDITOR->axisObject->magicNumber*5);
				}
				break;
			}
		}
	}


	void EditorFunctionality::UpdateRotation()
	{
		if(EDITOR->axisObject->selectedRotationAxis==EDITOR->axisObject->AxisSelection::none)
			return;
		real dist;
		D3DXQUATERNION q;
		if(EDITOR->axisObject->selectedRotationAxis==EDITOR->axisObject->AxisSelection::y)
		{
			dist=EDITOR->MousePosition.x-EDITOR->prevMousePosition.x;

			q=calculateQuaterion(-(float)dist,Vec3(0,1,0));

		}else if(EDITOR->axisObject->selectedRotationAxis==EDITOR->axisObject->AxisSelection::x)
		{
			dist=EDITOR->MousePosition.y-EDITOR->prevMousePosition.y;

			q=calculateQuaterion(-(float)dist,Vec3(1,0,0));

		}else if(EDITOR->axisObject->selectedRotationAxis==EDITOR->axisObject->AxisSelection::z)
		{
			dist=EDITOR->MousePosition.x-EDITOR->prevMousePosition.x;

			q=calculateQuaterion(-(float)dist,Vec3(0,0,1));

		}
		for(unsigned int i=0;i<EDITOR->selectedObjects.size();++i)
		{
			EDITOR->selectedObjects[i]->has(Transform)->QUATERNION*=q;
			Quaternion localQ=Quaternion(q.w,q.x,q.y,q.z);
			Vec3 temp= EDITOR->selectedObjects[i]->has(Transform)->Position-EDITOR->center;
			Vector3 dir=temp;
			Vector3 newPos=((localQ*dir)*dir.magnitude())+EDITOR->center;
			EDITOR->selectedObjects[i]->has(Transform)->Position=Vector3ToVec3(newPos);
		}
	}



	

}
